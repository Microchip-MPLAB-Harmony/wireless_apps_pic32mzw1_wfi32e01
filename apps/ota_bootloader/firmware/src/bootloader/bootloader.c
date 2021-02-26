/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    bootloader.c

  Summary:
    Interface for the Bootloader library.

  Description:
    This file contains the interface definition for the Bootloader library.
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "configuration.h"
#include "definitions.h"  
#include "ota_config.h"
#include "ota_image.h"
#include "bootloader.h"
#include "ext_flash.h"
#include "int_flash.h"
#include "imagestore.h"
#include "sha256.h"

//---------------------------------------------------------------------------
typedef uint32_t BOOTLOADER_STATUS;
#define BOOTLOADER_STATUS_SUCCESS                  0
#define BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED 1
#define BOOTLOADER_STATUS_ERROR                   -1
#define BOOTLOADER_STATUS_ERROR_NO_IMAGE_FOUND    -2

#define BOOTLOADER_DEBUG_LEVEL 0

#define APP_IMG_BOOT_CTL_SIGNATURE \
    (((volatile uint8_t*)APP_IMG_BOOT_CTL)[FIRMWARE_IMAGE_HEADER_SIGNATURE_BYTE])

#define __woraround_unused_variable(x) ((void)x)

typedef enum
{
    BOOTLOADER_TASK_INIT = 0,
    BOOTLOADER_TASK_CHECK_IMAGE,
    BOOTLOADER_TASK_SELECT_IMAGE,
    BOOTLOADER_TASK_PROGRAM_IMAGE,
    BOOTLOADER_TASK_VERIFY_IMAGE,
    BOOTLOADER_TASK_SIGN_BOOT_CTL,
    BOOTLOADER_TASK_INVALIDATE_IMAGE,
    BOOTLOADER_TASK_FLASH_PROGRAMMER,
    BOOTLOADER_TASK_JUMP_TO_APP,
    BOOTLOADER_TASK_CLEANER,
    BOOTLOADER_TASK_FACTORY_RESET
} BOOTLOADER_TASK_ID;

typedef struct
{
    FIRMWARE_IMAGE_HEADER img;
    uint8_t               guard[32];
} BOOTLOADER_TASK_PARAM;


typedef struct
{
    uint8_t buf[FLASH_SECTOR_SIZE];
    
    struct {
        uint8_t                 context[1024];
        int                     state;
        int                     state_dbg;
        BOOTLOADER_TASK_PARAM   param;
    } task;
    
    BOOTLOADER_TASK_ID current_task;
    uint32_t           debug_level;

} BOOTLOADER_DATA;

static BOOTLOADER_DATA  __attribute__ ((coherent, aligned(128))) bootloader;

#ifndef SYS_ASSERT
#define SYS_ASSERT(test, msg)\
do {\
    if (!(test))\
    {\
        printf(msg);\
        while(1);\
    }\
} while(0)
#endif

#ifndef SYS_INT_Disable
    #define SYS_INT_Disable()\
    do {\
        __builtin_disable_interrupts();\
    } while(0);
#endif

#ifndef SYS_RESET_SoftwareReset
    #define SYS_RESET_SoftwareReset()\
    do {\
        SYSKEY = 0x00000000;\
        SYSKEY = 0xAA996655;\
        SYSKEY = 0x556699AA;\
        RSWRSTSET = _RSWRST_SWRST_MASK;\
        RSWRST;\
        Nop();\
        Nop();\
        Nop();\
        Nop();\
    } while(0)
#endif

#ifndef EVIC_Deinitialize
    #define EVIC_Deinitialize()\
    do {\
        SYS_INT_Disable();\
        EVIC_SourceDisable(INT_SOURCE_FLASH_CONTROL);\
        EVIC_SourceStatusClear(INT_SOURCE_FLASH_CONTROL);\
        EVIC_SourceDisable(INT_SOURCE_SPI1_RX);\
        EVIC_SourceStatusClear(INT_SOURCE_FLASH_CONTROL);\
        EVIC_SourceDisable(INT_SOURCE_SPI1_TX);\
        EVIC_SourceStatusClear(INT_SOURCE_FLASH_CONTROL);\
        PRISS = 0;\
    } while(0)
#endif

//---------------------------------------------------------------------------
void Bootloader_TraceHeader(void *addr)
{
    volatile FIRMWARE_IMAGE_HEADER *hdr = (volatile FIRMWARE_IMAGE_HEADER *)addr;
    printf("status    :%02X\n", hdr->status);
    printf("version   :%02X\n", hdr->version);
    printf("order     :%02X\n", hdr->order);
    printf("type      :%02X\n", hdr->type);
    printf("sz        :%08X\n", hdr->sz);
    printf("slot      :%08X\n", hdr->slot);
    printf("boot_addr :%08X\n", hdr->boot_addr);
    printf("digest    :%02X %02X %02X %02X %02X %02X %02X %02X\n",
            hdr->digest[0],hdr->digest[1],hdr->digest[2],hdr->digest[3],
            hdr->digest[4],hdr->digest[5],hdr->digest[6],hdr->digest[7]);
    printf("          :%02X %02X %02X %02X %02X %02X %02X %02X\n",
            hdr->digest[8],hdr->digest[9],hdr->digest[10],hdr->digest[11],
            hdr->digest[12],hdr->digest[13],hdr->digest[14],hdr->digest[15]);
    
    printf("          :%02X %02X %02X %02X %02X %02X %02X %02X\n",
            hdr->digest[16],hdr->digest[17],hdr->digest[18],hdr->digest[19],
            hdr->digest[20],hdr->digest[21],hdr->digest[22],hdr->digest[23]);
    printf("          :%02X %02X %02X %02X %02X %02X %02X %02X\n",
            hdr->digest[24],hdr->digest[25],hdr->digest[26],hdr->digest[27],
            hdr->digest[28],hdr->digest[29],hdr->digest[30],hdr->digest[31]);
    printf("\n");
}


//---------------------------------------------------------------------------
void  Bootloader_Initialize(void)
{
    memset(&bootloader, 0, sizeof(bootloader));
    bootloader.current_task = BOOTLOADER_TASK_INIT;
    bootloader.debug_level  = BOOTLOADER_DEBUG_LEVEL;
    printf("\n");
    printf("***********************************************************\n");
    printf("BOOTLOADER Build %s %s\n", __DATE__, __TIME__);
    printf("OTA VERSION %s\n", OTA_VERSION);
    printf("***********************************************************\n");

    INT_Flash_Initialize();
}

//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_CheckImage(void)

  Description:
    Return BOOTLOADER_STATUS_SUCCESS if the images on INT/EXT are the same.
    DOWNLOADED or VALID. Otherwise the function invalidates image and returns
    BOOTLOADER_STATUS_ERROR

  Task Parameters:
    None

  Return:
    A BOOTLOADER_STATUS code describing the current status.
*/
//---------------------------------------------------------------------------
typedef struct {
    uint8_t *buf;
} BOOTLOADER_CHECK_IMAGE_TASK_CONTEXT;

typedef enum
{
    TASK_STATE_C_INIT = 0,
    TASK_STATE_C_PARSE_HEADER,
    TASK_STATE_C_INVALIDATE_BOOT_CTL,
    TASK_STATE_C_INVALIDATE_IMAGESTORE,
    TASK_STATE_C_DONE
} BOOTLOADER_CHECK_IMAGE_TASK_STATE;

static BOOTLOADER_STATUS Bootloader_Task_CheckImage(void)
{
    BOOTLOADER_CHECK_IMAGE_TASK_CONTEXT *ctx = (void *)bootloader.task.context;

    if (bootloader.debug_level  >= 1)
    {
        static const char * s[] __attribute__((unused)) = {
            "TASK_STATE_C_INIT",
            "TASK_STATE_C_PARSE_HEADER",
            "TASK_STATE_C_INVALIDATE_BOOT_CTL",
            "TASK_STATE_C_INVALIDATE_IMAGESTORE",
            "TASK_STATE_C_DONE" }; 

        if (bootloader.task.state_dbg != bootloader.task.state)
        {
            printf("%s\n", &s[bootloader.task.state][11]);
            bootloader.task.state_dbg = bootloader.task.state;
        }
        
    }
    
    SYS_ASSERT(sizeof(*ctx)<sizeof(bootloader.task.context), "Buffer Overflow");
    
    switch ( bootloader.task.state )
    {
        case TASK_STATE_C_INIT:
        {
            uint32_t *p = (uint32_t*)APP_IMG_BOOT_CTL;
            
            
            if (p[0] == 0xffffffff && p[1] == 0xffffffff 
            &&  p[2] == 0xffffffff && p[3] == 0xffffffff )
            {
                //APP_IMG_BOOT_CTL is uninitialized.
                printf("Uninitialized sector\n");
                return BOOTLOADER_STATUS_ERROR;
            }

            ctx->buf = bootloader.buf;
            if (APP_IMG_BOOT_CTL->slot >= IMAGESTORE_NUM_SLOT)
            {
                printf("Invalid slot #%d\n", APP_IMG_BOOT_CTL->slot);
                bootloader.task.state = TASK_STATE_C_INVALIDATE_BOOT_CTL;
                break;
            }
            
            IMAGESTORE_Read(APP_IMG_BOOT_CTL->slot, 0, ctx->buf, FLASH_SECTOR_SIZE);
            bootloader.task.state = TASK_STATE_C_PARSE_HEADER;
            break;
        }
        case TASK_STATE_C_PARSE_HEADER:
        {
            FIRMWARE_IMAGE_HEADER *img;
    
            if (IMAGESTORE_Busy())
            {
                break;
            }
            
            img = (FIRMWARE_IMAGE_HEADER *)ctx->buf;
            
            if (APP_IMG_BOOT_CTL->status == IMG_STATUS_VALID)
            {
                if (img->version == FIRMWARE_IMAGE_HEADER_VERSION
                &&  img->status == ctx->buf[FIRMWARE_IMAGE_HEADER_SIGNATURE_BYTE]
                && (memcmp(img->digest, (void*)APP_IMG_BOOT_CTL->digest, sizeof(APP_IMG_BOOT_CTL->digest)) == 0))
                {
                    if (img->status == IMG_STATUS_VALID)
                    {
                        return BOOTLOADER_STATUS_SUCCESS;
                    }
                }
            }

            if (APP_IMG_BOOT_CTL->slot == 0)
            {
                bootloader.task.state = TASK_STATE_C_DONE;
            } else {
                bootloader.task.state = TASK_STATE_C_INVALIDATE_BOOT_CTL;
            }
            break;
        }
        case TASK_STATE_C_INVALIDATE_BOOT_CTL:
        {
            ctx->buf[FIRMWARE_IMAGE_HEADER_SIGNATURE_BYTE] = IMG_STATUS_DISABLED;
            ctx->buf[FIRMWARE_IMAGE_HEADER_STATUS_BYTE] = IMG_STATUS_DISABLED;

            INT_Flash_Write(APP_IMG_SLOT_ADDR, ctx->buf, FLASH_SECTOR_SIZE);
            bootloader.task.state = TASK_STATE_C_INVALIDATE_IMAGESTORE;
            break;
        }
        case TASK_STATE_C_INVALIDATE_IMAGESTORE:
        {
            if (INT_Flash_Busy())
            {
                break;
            }

            IMAGESTORE_Write(APP_IMG_BOOT_CTL->slot, 0, ctx->buf, FLASH_SECTOR_SIZE);
            bootloader.task.state = TASK_STATE_C_DONE;
            break;
        }
        case TASK_STATE_C_DONE:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }
            
            return BOOTLOADER_STATUS_ERROR;
        }
        default:
        {
            SYS_ASSERT(false, "Unknown task state");
            return BOOTLOADER_STATUS_ERROR;
        }
    }

    return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
}
//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_SelectImage(void)

  Description:
    Select the higest ranked image from the Image Store

  Task Parameters:
    (OUT) img   - A copy of the selected images' FIRMWARE_IMAGE_HEADER

  Return:
    BOOTLOADER_STATUS_ERROR_NO_IMAGE_FOUND if no image found,
    BOOTLOADER_STATUS_SUCCESS otherwise.
*/
//---------------------------------------------------------------------------
typedef struct {
    uint8_t *buf;
    uint32_t slot;
    uint32_t selected;
    uint8_t  order;
} BOOTLOADER_SELECT_IMAGE_TASK_CONTEXT;

typedef enum
{
    TASK_STATE_S_INIT = 0,
    TASK_STATE_S_READ_HEADER,
    TASK_STATE_S_CHECK_IMAGE,
    TASK_STATE_S_SELECT_IMAGE,
    TASK_STATE_S_DONE
} BOOTLOADER_SELECT_IMAGE_TASK_STATE;

static BOOTLOADER_STATUS Bootloader_Task_SelectImage(void)
{
    BOOTLOADER_SELECT_IMAGE_TASK_CONTEXT *ctx = (void*)bootloader.task.context;
    BOOTLOADER_TASK_PARAM *param = &bootloader.task.param;

    if (bootloader.debug_level  >= 1)
    {
        static const char * s[] __attribute__((unused)) = {
            "TASK_STATE_S_INIT",
            "TASK_STATE_S_READ_HEADER",
            "TASK_STATE_S_CHECK_IMAGE",
            "TASK_STATE_S_SELECT_IMAGE",
            "TASK_STATE_S_DONE" };

        if (bootloader.task.state_dbg != bootloader.task.state)
        {
            printf("%s\n", s[bootloader.task.state]);
            bootloader.task.state_dbg = bootloader.task.state;
        }
    }
    SYS_ASSERT(sizeof(*ctx)<sizeof(bootloader.task.context), "Buffer Overflow");
    
    switch ( bootloader.task.state )
    {
        case TASK_STATE_S_INIT:
        {
            bootloader.task.state = TASK_STATE_S_READ_HEADER;
            ctx->slot     = 0;
            ctx->selected = 0;
            ctx->order    = 0;
            ctx->buf      = bootloader.buf;
            break;
        }
        case TASK_STATE_S_READ_HEADER:
        {
            if (ctx->slot < IMAGESTORE_NUM_SLOT)
            {
                IMAGESTORE_Read(ctx->slot, 0, ctx->buf, FLASH_SECTOR_SIZE);
                
                bootloader.task.state = TASK_STATE_S_CHECK_IMAGE;
            }
            else
            {
                bootloader.task.state = TASK_STATE_S_SELECT_IMAGE;
            }
            break;
        }
        case TASK_STATE_S_CHECK_IMAGE:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }
            
            memcpy(&param->img, ctx->buf, sizeof(FIRMWARE_IMAGE_HEADER));
            if (bootloader.debug_level  >= 1)
            {
               Bootloader_TraceHeader((void*)ctx->buf);
            }
            
            if ( param->img.status == ctx->buf[FIRMWARE_IMAGE_HEADER_SIGNATURE_BYTE]
            &&   param->img.version == FIRMWARE_IMAGE_HEADER_VERSION)
            {
                if (param->img.status == IMG_STATUS_DOWNLOADED)
                {
                    ctx->selected = ctx->slot;
                    ctx->order = param->img.order;
                    bootloader.task.state = TASK_STATE_S_SELECT_IMAGE;
                    break;
                }

                if (param->img.status == IMG_STATUS_VALID)
                {
                    if ( ctx->order == 0
                    || ((ctx->order - param->img.order) & 0x08))
                    {
                        ctx->selected = ctx->slot;
                        ctx->order = param->img.order;
                    }
                }
            }
            
            ctx->slot++;
            bootloader.task.state = TASK_STATE_S_READ_HEADER;
            break;
        }
        case TASK_STATE_S_SELECT_IMAGE:
        {
            IMAGESTORE_Read(ctx->selected, 0,
                    &param->img, sizeof(FIRMWARE_IMAGE_HEADER));

            bootloader.task.state = TASK_STATE_S_DONE;
            break;
        }
        case TASK_STATE_S_DONE:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }
            
            if (param->img.status != IMG_STATUS_DOWNLOADED
            &&  param->img.status != IMG_STATUS_VALID)
            {
                printf("No image found\n");
                return BOOTLOADER_STATUS_ERROR_NO_IMAGE_FOUND;
            }
            else
            {
                printf("Select Image @ %d\n", ctx->selected);
            }

            return BOOTLOADER_STATUS_SUCCESS;
        }
        default:
        {
            SYS_ASSERT(false, "Unknown task state");
            return BOOTLOADER_STATUS_ERROR;
        }
    }

    return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
}
//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_ProgramImage(void)

  Description:
    Copy selected image to PFM.
    NB: BOOT_CTG is erased, but not programmed by this function

  Task Parameters:
    (IN) img   - A copy of the selected images' FIRMWARE_IMAGE_HEADER

  Return:
    A BOOTLOADER_STATUS code describing the current status.
*/
//---------------------------------------------------------------------------
typedef struct {
    uint8_t *buf;
    uint32_t offset;
    uint32_t len;
    uint32_t copy_len;
} BOOTLOADER_PROGRAM_IMAGE_TASK_CONTEXT;

typedef enum
{
    TASK_STATE_P_INIT = 0,
    TASK_STATE_P_ERASE_SLOT,
    TASK_STATE_P_READ_IMAGE,
    TASK_STATE_P_PROGRAM_IMAGE,
    TASK_STATE_P_READ_HEADER,
    TASK_STATE_P_PROGRAM_BOOT_CTL,
    TASK_STATE_P_DONE
} BOOTLOADER_PROGRAM_IMAGE_TASK_STATE;

static BOOTLOADER_STATUS Bootloader_Task_ProgramImage(void)
{
    BOOTLOADER_PROGRAM_IMAGE_TASK_CONTEXT *ctx = (void*)bootloader.task.context;
    BOOTLOADER_TASK_PARAM *param = &bootloader.task.param;

    if (bootloader.debug_level  >= 1)
    {
        static const char * s[] __attribute__((unused)) = {
            "TASK_STATE_P_INIT",
            "TASK_STATE_P_ERASE_SLOT",
            "TASK_STATE_P_READ_IMAGE",
            "TASK_STATE_P_PROGRAM_IMAGE",
            "TASK_STATE_P_READ_HEADER",
            "TASK_STATE_P_PROGRAM_BOOT_CTL",
            "TASK_STATE_P_DONE" };

        if (bootloader.task.state_dbg != bootloader.task.state)
        {
            printf("%s\n", s[bootloader.task.state]);
            bootloader.task.state_dbg = bootloader.task.state;
        }
    }
    SYS_ASSERT(sizeof(*ctx)<sizeof(bootloader.task.context), "Buffer Overflow");
    
    switch ( bootloader.task.state )
    {
        case TASK_STATE_P_INIT:
        {
            ctx->buf = bootloader.buf;
            ctx->len = FLASH_SECTOR_SIZE;
            ctx->offset = FLASH_SECTOR_SIZE;
            ctx->copy_len = (param->img.sz + FLASH_SECTOR_SIZE-1)/FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE;
            bootloader.task.state = TASK_STATE_P_ERASE_SLOT;
            break;
        }
        case TASK_STATE_P_ERASE_SLOT:
        {
            if (INT_Flash_Erase(APP_IMG_SLOT_ADDR, IMAGESTORE_SLOT_SIZE) == false)
            {
                ctx->buf = NULL;
                return BOOTLOADER_STATUS_ERROR;
            }
            bootloader.task.state = TASK_STATE_P_READ_IMAGE;
            break;
        }
        case TASK_STATE_P_READ_IMAGE:
        {
            if (IMAGESTORE_Busy() || INT_Flash_Busy())
            {
                break;
            }

            IMAGESTORE_Read(param->img.slot, ctx->offset, ctx->buf, ctx->len);

            bootloader.task.state = TASK_STATE_P_PROGRAM_IMAGE;
            break;
        }
        case TASK_STATE_P_PROGRAM_IMAGE:
        {
            if (IMAGESTORE_Busy() || INT_Flash_Busy())
            {
                break;
            }
                       
            INT_Flash_Write(APP_IMG_SLOT_ADDR + ctx->offset, ctx->buf, FLASH_SECTOR_SIZE);
            
            ctx->offset += ctx->len;

            if (ctx->offset >= ctx->copy_len)
            {
                bootloader.task.state = TASK_STATE_P_READ_HEADER;
            }
            else
            {
                bootloader.task.state = TASK_STATE_P_READ_IMAGE;                
            }
            break;
        }
        case TASK_STATE_P_READ_HEADER:
        {
            if (IMAGESTORE_Busy() || INT_Flash_Busy())
            {
                break;
            }
            IMAGESTORE_Read(param->img.slot, 0, ctx->buf, FLASH_SECTOR_SIZE);
            bootloader.task.state = TASK_STATE_P_PROGRAM_BOOT_CTL;
            break;    
        }
        case TASK_STATE_P_PROGRAM_BOOT_CTL:
        {
            if (IMAGESTORE_Busy() || INT_Flash_Busy()){
                break;
            }
            if (INT_Flash_Write(APP_IMG_SLOT_ADDR, ctx->buf, FLASH_SECTOR_SIZE) == false)
            {
                ctx->buf = NULL;
                return BOOTLOADER_STATUS_ERROR;
            }
            
            bootloader.task.state = TASK_STATE_P_DONE;
            break;
        }
        case TASK_STATE_P_DONE:
        {
            if (IMAGESTORE_Busy() || INT_Flash_Busy())
            {
                break;
            }

            ctx->buf = NULL;
            return BOOTLOADER_STATUS_SUCCESS;
        }
        default:
        {
            SYS_ASSERT(false, "Unknown task state");
            return BOOTLOADER_STATUS_ERROR;
        }
    }

    return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
}
//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_VerifyImage(void)

  Description:
    Verify the copied image in INT Flash..
    This example code uses SHA256 for image validation.
    NB: BOOT_CFG has not been programmed!

  Task Parameters:
    None

  Return:
    A BOOTLOADER_STATUS code describing the current status.
*/
//---------------------------------------------------------------------------
typedef struct
{
    CRYPT_SHA256_CTX  sha256; // This has to be 128 bytes aligned !!
    uint8_t *buf;
    uint32_t offset;
    uint32_t len;
    uint32_t img_sz;
} BOOTLOADER_VERIFY_IMAGE_TASK_CONTEXT;

typedef enum
{
    TASK_STATE_V_INIT = 0,
    TASK_STATE_V_READ_IMG,
    TASK_STATE_V_DONE,
} BOOTLOADER_V_IMAGE_TASK_STATE;

static BOOTLOADER_STATUS Bootloader_Task_VerifyImage(void)
{
    BOOTLOADER_VERIFY_IMAGE_TASK_CONTEXT *ctx = (void*)&bootloader.task.context;
    BOOTLOADER_TASK_PARAM *param = &bootloader.task.param;

    if (bootloader.debug_level  >= 1)
    {
        static const char * s[] __attribute__((unused))= {
            "TASK_STATE_V_INIT",
            "TASK_STATE_V_READ_IMG",
            "TASK_STATE_V_DONE" };

        if (bootloader.task.state_dbg != bootloader.task.state)
        {
            printf("%s\n", s[bootloader.task.state]);
            bootloader.task.state_dbg = bootloader.task.state;
        }
    }
    SYS_ASSERT(sizeof(*ctx)<sizeof(bootloader.task.context), "Buffer Overflow");
    
    switch ( bootloader.task.state )
    {
        case TASK_STATE_V_INIT:
        {
            ctx->offset = 0;
            ctx->len = FLASH_SECTOR_SIZE;
            ctx->buf = bootloader.buf;
            ctx->img_sz = param->img.sz;
            
            CRYPT_SHA256_Initialize(&ctx->sha256);
            CRYPT_SHA256_DataSizeSet(&ctx->sha256, param->img.sz);

            INT_Flash_Read(APP_IMG_SLOT_ADDR, ctx->buf, ctx->len);
            bootloader.task.state = TASK_STATE_V_READ_IMG;
            break;
        }
        case TASK_STATE_V_READ_IMG:
        {
            if (INT_Flash_Busy())
            {
                break;
            }
            
            if (ctx->offset == 0)
            {
                FIRMWARE_IMAGE_HEADER * img = (FIRMWARE_IMAGE_HEADER *)ctx->buf;
                memset(ctx->buf, 0xFF, sizeof(FIRMWARE_IMAGE_HEADER));
                img->version   = param->img.version;
                img->sz        = param->img.sz;
                img->boot_addr = param->img.boot_addr;
                ctx->buf[FIRMWARE_IMAGE_HEADER_SIGNATURE_BYTE] = 0xFF;
            }
            
            CRYPT_SHA256_DataAdd(&ctx->sha256, ctx->buf, ctx->len);
            ctx->offset += ctx->len;

            if (ctx->offset < ctx->img_sz)
            {
                if ((ctx->offset + ctx->len) > ctx->img_sz)
                {
                    ctx->len = ctx->img_sz - ctx->offset;
                }
                INT_Flash_Read(APP_IMG_SLOT_ADDR + ctx->offset, ctx->buf, ctx->len);
            }
            else
            {
                bootloader.task.state = TASK_STATE_V_DONE;
            }
            break;
        }
        case TASK_STATE_V_DONE:
        {
            uint8_t digest[CRYPT_SHA256_DIGEST_SIZE];

            if (INT_Flash_Busy())
            {
                break;
            }
            
            CRYPT_SHA256_Finalize(&ctx->sha256, digest);

            if (memcmp(digest, param->img.digest, CRYPT_SHA256_DIGEST_SIZE) != 0)
            {
                printf("Broken Image\n");
                return BOOTLOADER_STATUS_ERROR;
            }
            return BOOTLOADER_STATUS_SUCCESS;
        }
        default:
        {
            SYS_ASSERT(false, "Unknown task state");
            return BOOTLOADER_STATUS_ERROR;
        }
    }

    return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
}
//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_SignImage(void)

  Description:
    Mark INT flash image to VALID.
    
  Task Parameters:
    None

  Return:
    A BOOTLOADER_STATUS code describing the current status.
*/
//---------------------------------------------------------------------------
typedef struct
{
    uint8_t *buf;
} BOOTLOADER_SIGN_IMAGE_TASK_CONTEXT;

typedef enum
{
    TASK_STATE_SI_INIT = 0,
    TASK_STATE_SI_SIGN,
    TASK_STATE_SI_DONE
} BOOTLOADER_SIGN_IMAGE_TASK_STATE;

static BOOTLOADER_STATUS Bootloader_Task_SignImage(void)
{
    BOOTLOADER_SIGN_IMAGE_TASK_CONTEXT *ctx = (void*)bootloader.task.context;
    BOOTLOADER_TASK_PARAM *param = &bootloader.task.param;
    
    switch ( bootloader.task.state )
    {
        case TASK_STATE_SI_INIT:
        {
            ctx->buf = bootloader.buf;
            
            INT_Flash_Read(APP_IMG_SLOT_ADDR, ctx->buf, FLASH_SECTOR_SIZE);
            bootloader.task.state = TASK_STATE_SI_SIGN;
            break;
        }
        case TASK_STATE_SI_SIGN:
        {
            if (INT_Flash_Busy())
            {
                break;
            }
            
            //param->img.status = IMG_STATUS_VALID;
            param->img.status &= IMG_STATUS_UNBOOTED;
            memcpy(ctx->buf, &param->img, sizeof(FIRMWARE_IMAGE_HEADER));
            ctx->buf[FIRMWARE_IMAGE_HEADER_SIGNATURE_BYTE] = param->img.status;
            INT_Flash_Write(APP_IMG_SLOT_ADDR, ctx->buf, FLASH_SECTOR_SIZE);
            bootloader.task.state = TASK_STATE_SI_DONE;
            break;
        }
        case TASK_STATE_SI_DONE:
        {
            if (INT_Flash_Busy())
            {
                break;
            }

            return BOOTLOADER_STATUS_SUCCESS;
        }
        default:
        {
            SYS_ASSERT(false, "Unknown task state");
            return BOOTLOADER_STATUS_ERROR;
        }
    }

    return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
}
//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_InvalidateImage(void)

  Description:
    Invaldate image in Image Store to DISABLED.


  Task Parameters:
    (IN) img   - A copy of the selected images' FIRMWARE_IMAGE_HEADER

  Return:
    A BOOTLOADER_STATUS code describing the current status.
*/
//---------------------------------------------------------------------------
typedef enum
{
    TASK_STATE_I_INIT = 0,
    TASK_STATE_I_INVALIDATE,
    TASK_STATE_I_DONE
} BOOTLOADER_INVALIDATE_IMAGE_TASK_STATE;

static BOOTLOADER_STATUS Bootloader_Task_InvalidateImage(void)
{
    BOOTLOADER_TASK_PARAM *param = &bootloader.task.param;

    switch ( bootloader.task.state )
    {
        case TASK_STATE_I_INIT:
        {
            bootloader.task.state = TASK_STATE_I_INVALIDATE;
        }
        case TASK_STATE_I_INVALIDATE:
        {
            param->img.status = IMG_STATUS_DISABLED;
            if (param->img.order == 0)
            {
                printf("The golden image is broken!!\n");
                bootloader.task.state = TASK_STATE_I_DONE;
                break;
            }

            IMAGESTORE_Write(param->img.slot, 0,
                    &param->img, sizeof(FIRMWARE_IMAGE_HEADER));
            
            bootloader.task.state = TASK_STATE_I_DONE;
            break;
        }
        case TASK_STATE_I_DONE:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }
            
            return BOOTLOADER_STATUS_SUCCESS;
        }
    }
    return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
}

//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_JumpApplication(void)

  Description:


  Task Parameters:
    None


  Return:
    A BOOTLOADER_STATUS code describing the current status.
*/
//---------------------------------------------------------------------------
static BOOTLOADER_STATUS Bootloader_Task_JumpApplication(void)
{
    void(*fptr)(void);

	EVIC_Deinitialize();
    fptr = (void(*)(void))(APP_IMG_BOOT_CTL->boot_addr);

    fptr();

    return BOOTLOADER_STATUS_SUCCESS;
}

//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_FlashProgrammer(void)

  Description:


  Task Parameters:
    None

  Return:
    A BOOTLOADER_STATUS code describing the current status.
*/
//---------------------------------------------------------------------------
/*
typedef enum
{
    TASK_STATE_FP_INIT = 0,
    TASK_STATE_FP_SERVICE_TASK,
} BOOTLOADER_FINVALIDATE_IMAGE_TASK_STATE;

static BOOTLOADER_STATUS Bootloader_Task_FlashProgrammer(void)
{
    switch (bootloader.task.state)
    {
        case TASK_STATE_FP_INIT:
        {
            INT_Flash_Close();
            IMAGESTORE_Close();
        
            FlashProgrammer_Initialize();
            FlashProgrammer_Start();
            bootloader.task.state = TASK_STATE_FP_SERVICE_TASK;
            break;
        }       
        case TASK_STATE_FP_SERVICE_TASK:
        {
            FlashProgrammer_Tasks();
            break;
        }
    }
    return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
}
*/
//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_Cleaner(void)

  Description:


  Task Parameters:
    None

  Return:
    A BOOTLOADER_STATUS code describing the current status.
*/
//---------------------------------------------------------------------------
typedef enum
{
    TASK_STATE_CLEANER_INIT = 0,
    TASK_STATE_CLEANER_ERASE_EXT_FLASH,
    TASK_STATE_CLEANER_DONE,
} BOOTLOADER_CLEANER_STATE;

static BOOTLOADER_STATUS Bootloader_Task_Cleaner(void)
{
    switch (bootloader.task.state)
    {
        case TASK_STATE_CLEANER_INIT:
        {
            EXT_Flash_Initialize();
            if (EXT_Flash_Open() == false)
            {
                printf("Fail to open EXT_Flash\n");
                break;
            }

            EXT_Flash_Erase(0, EXT_Flash_Capacity());
            bootloader.task.state = TASK_STATE_CLEANER_ERASE_EXT_FLASH;
            
            break;
        }
        case TASK_STATE_CLEANER_ERASE_EXT_FLASH:
        {
            if (EXT_Flash_Busy())
            {
                break;
            }
            
            printf("External Flash is erased\n");
            bootloader.task.state = TASK_STATE_CLEANER_DONE;
            break;         
        }
        default:
        {
            break;
        }
    }
    return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
}

//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_FactoryReset(void)

  Description:


  Task Parameters:
    None

  Return:
    A BOOTLOADER_STATUS code describing the current status.
*/
//---------------------------------------------------------------------------
typedef struct
{
    uint8_t *buf;
    uint32_t offset;
    uint32_t len;
    uint32_t copy_len;
    uint32_t dbg;
} BOOTLOADER_FACTORY_RESET_TASK_CONTEXT;

typedef enum
{
    TASK_STATE_FR_INIT = 0,
    TASK_STATE_FR_ERASE_IMAGESTORE,
    TASK_STATE_FR_READ_IMAGE,
    TASK_STATE_FR_PROGRAM_IMAGE,
    TASK_STATE_FR_ERASE_BOOT_CTL,
    TASK_STATE_FR_DONE,
} BOOTLOADER_FR_STATE;

static BOOTLOADER_STATUS Bootloader_Task_FactoryReset(void)
{
    BOOTLOADER_FACTORY_RESET_TASK_CONTEXT *ctx = (void*)&bootloader.task.context;
            
    if (bootloader.debug_level >= 1)
    {
        static const char * s[] __attribute__((unused))= {
            "TASK_STATE_FR_INIT",
            "TASK_STATE_FR_ERASE_IMAGESTORE",
            "TASK_STATE_FR_READ_IMAGE",
            "TASK_STATE_FR_PROGRAM_IMAGE",
            "TASK_STATE_FR_ERASE_BOOT_CTL",
            "TASK_STATE_FR_DONE" };

        if (bootloader.task.state_dbg != bootloader.task.state)
        {
            printf("%s\n", s[bootloader.task.state]);
            bootloader.task.state_dbg = bootloader.task.state;
        }
    }
    
    switch (bootloader.task.state)
    {
        case TASK_STATE_FR_INIT:
        {
            ctx->buf = bootloader.buf;
            ctx->copy_len = (APP_IMG_BOOT_CTL->sz + FLASH_SECTOR_SIZE-1) / FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE;
            ctx->offset = 0;
            ctx->dbg = 0;
            bootloader.task.state = TASK_STATE_FR_ERASE_IMAGESTORE;
            IMAGESTORE_EraseAll();
            break;
        }
        case TASK_STATE_FR_ERASE_IMAGESTORE:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }
            
            bootloader.task.state = TASK_STATE_FR_READ_IMAGE;
            break;         
        }
        case TASK_STATE_FR_READ_IMAGE:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }
            INT_Flash_Read(APP_IMG_SLOT_ADDR + ctx->offset, ctx->buf, FLASH_SECTOR_SIZE);
            bootloader.task.state = TASK_STATE_FR_PROGRAM_IMAGE;
            break;
        }
        case TASK_STATE_FR_PROGRAM_IMAGE:
        {
            if (INT_Flash_Busy())
            {
                break;
            }
            
            if (ctx->offset == 0)
            {
                FIRMWARE_IMAGE_HEADER *img;
                img = (FIRMWARE_IMAGE_HEADER *)ctx->buf;
                img->type = IMG_TYPE_PRODUCTION;
                img->order = 0;
                ctx->buf[FIRMWARE_IMAGE_HEADER_SIGNATURE_BYTE] = img->status;
            }

            IMAGESTORE_Write(APP_IMG_BOOT_CTL->slot, ctx->offset, ctx->buf, FLASH_SECTOR_SIZE);

            bootloader.task.state = TASK_STATE_FR_READ_IMAGE;
            ctx->offset += FLASH_SECTOR_SIZE;
            if (ctx->offset >= ctx->copy_len)
            {
                bootloader.task.state = TASK_STATE_FR_ERASE_BOOT_CTL;
            }
            break;
        }
        case TASK_STATE_FR_ERASE_BOOT_CTL:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }
                        
            INT_Flash_Erase(APP_IMG_SLOT_ADDR, FLASH_SECTOR_SIZE);
            bootloader.task.state = TASK_STATE_FR_DONE;
            break;
        }
        case TASK_STATE_FR_DONE:
        {
            if (INT_Flash_Busy())
            {
                break;
            }
            printf("\n");
            SYS_RESET_SoftwareReset();
            break;
        }
        default:
        {
            break;
        }
    }
    return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
}
//---------------------------------------------------------------------------
//
// void Bootloader_Tasks(void)
//
//---------------------------------------------------------------------------
void Bootloader_Tasks(void)
{
    BOOTLOADER_TASK_ID next = bootloader.current_task;
    BOOTLOADER_STATUS status;

    switch ( bootloader.current_task )
    {
        case BOOTLOADER_TASK_INIT:
        {
            // This needs IRQ, so can not be called from Bootloader_Initialize()
            IMAGESTORE_Initialize();

            INT_Flash_Open(); 
            IMAGESTORE_Open();
             
            next = BOOTLOADER_TASK_CHECK_IMAGE;

	        Bootloader_TraceHeader((void*)APP_IMG_BOOT_CTL);
            if (APP_IMG_BOOT_CTL->status == IMG_STATUS_VALID
            &&  APP_IMG_BOOT_CTL_SIGNATURE == IMG_STATUS_VALID)
            {
                switch (APP_IMG_BOOT_CTL->type)
                {
                case IMG_TYPE_PRODUCTION:
                    next = BOOTLOADER_TASK_CHECK_IMAGE;
                    break;

                case IMG_TYPE_DEBUG:
                    next = BOOTLOADER_TASK_JUMP_TO_APP;
                    break;

                case IMG_TYPE_FLASH_PROGRAMMER:
                    next = BOOTLOADER_TASK_FLASH_PROGRAMMER;
                    break;

                case IMG_TYPE_FACTORY_RESET:
                    next = BOOTLOADER_TASK_FACTORY_RESET;
                    break;
 
                default:
                    //next = BOOTLOADER_TASK_SELECT_IMAGE;
                    break;
                }
            }
            break;
        }
        case BOOTLOADER_TASK_CHECK_IMAGE:
        {
            status = Bootloader_Task_CheckImage();
            if (status == BOOTLOADER_STATUS_SUCCESS)
            {
                next = BOOTLOADER_TASK_JUMP_TO_APP;
            }
            if (status == BOOTLOADER_STATUS_ERROR)
            {
                next = BOOTLOADER_TASK_SELECT_IMAGE;
            }
            break;
        }
        case BOOTLOADER_TASK_SELECT_IMAGE:
        {
            status =  Bootloader_Task_SelectImage();
            if (status == BOOTLOADER_STATUS_SUCCESS)
            {
                next = BOOTLOADER_TASK_PROGRAM_IMAGE;
            }
            if (status == BOOTLOADER_STATUS_ERROR_NO_IMAGE_FOUND)
            {
                next = BOOTLOADER_TASK_FLASH_PROGRAMMER;
            }
            if (status == BOOTLOADER_STATUS_ERROR)
            {
                bootloader.task.state = 0;
            }
            break;
        }
        case BOOTLOADER_TASK_PROGRAM_IMAGE:
        {
            status = Bootloader_Task_ProgramImage();
            if (status == BOOTLOADER_STATUS_SUCCESS)
            {
                next = BOOTLOADER_TASK_VERIFY_IMAGE;
            }
            if (status == BOOTLOADER_STATUS_ERROR)
            {
                next = BOOTLOADER_TASK_SELECT_IMAGE;
            }
            break;
        }
        case BOOTLOADER_TASK_VERIFY_IMAGE:
        {
            status = Bootloader_Task_VerifyImage();
            if (status == BOOTLOADER_STATUS_SUCCESS)
            {
                next = BOOTLOADER_TASK_SIGN_BOOT_CTL;
            }
            if (status == BOOTLOADER_STATUS_ERROR)
            {
                next = BOOTLOADER_TASK_INVALIDATE_IMAGE;
            }
            break;
        }
        case BOOTLOADER_TASK_SIGN_BOOT_CTL:
        {
            status = Bootloader_Task_SignImage();
            if (status == BOOTLOADER_STATUS_SUCCESS)
            {
                next = BOOTLOADER_TASK_JUMP_TO_APP;
            }
            if (status == BOOTLOADER_STATUS_ERROR)
            {
                next = BOOTLOADER_TASK_INVALIDATE_IMAGE;
            }
            break;
        }
        case BOOTLOADER_TASK_INVALIDATE_IMAGE:
        {
            status = Bootloader_Task_InvalidateImage();
            if (status == BOOTLOADER_STATUS_SUCCESS)
            {
                next = BOOTLOADER_TASK_SELECT_IMAGE;
            }
            break;
        }
        case BOOTLOADER_TASK_JUMP_TO_APP:
        {
            /*GREEN LED OFF*/
            //SYS_PORTS_PinClear (PORTS_ID_0, PORT_CHANNEL_K, PORTS_BIT_POS_3);
            Bootloader_Task_JumpApplication();
            break;
        }
        case BOOTLOADER_TASK_FLASH_PROGRAMMER:
        {
            //Bootloader_Task_FlashProgrammer();
            SYS_ASSERT(false, "Unexpected state");
            break;
        }
        case BOOTLOADER_TASK_CLEANER:
        {
            Bootloader_Task_Cleaner();
            break;
        }
        case BOOTLOADER_TASK_FACTORY_RESET:
        {
            Bootloader_Task_FactoryReset();
            break;
        }
        default:
        {
            SYS_ASSERT(false, "Unknown Task");
            break;
        }
    }

    if (next != bootloader.current_task)
    {
        {
            static const char * s[] __attribute__((unused)) = {
                "BOOTLOADER_TASK_INIT",
                "BOOTLOADER_TASK_CHECK_IMAGE",
                "BOOTLOADER_TASK_SELECT_IMAGE",
                "BOOTLOADER_TASK_PROGRAM_IMAGE",
                "BOOTLOADER_TASK_VERIFY_IMAGE",
                "BOOTLOADER_TASK_SIGN_BOOT_CTL",
                "BOOTLOADER_TASK_INVALIDATE_IMAGE",
                "BOOTLOADER_TASK_FLASH_PROGRAMMER",
                "BOOTLOADER_TASK_JUMP_TO_APP",
                "BOOTLOADER_TASK_CLEANER",
                "BOOTLOADER_TASK_FACTORY_RESET"
            };
            
            printf("%s\n", s[next]);
        
            if (bootloader.current_task != BOOTLOADER_TASK_INIT
            &&  next == BOOTLOADER_TASK_JUMP_TO_APP)
            {
                Bootloader_TraceHeader((void*)APP_IMG_BOOT_CTL);
                printf("***********************************************************\n");
            }
        }
        bootloader.current_task = next;
        bootloader.task.state = 0;
        bootloader.task.state_dbg = -1;
    }
}
