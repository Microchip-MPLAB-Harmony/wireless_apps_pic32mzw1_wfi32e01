/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    ota.c
    
  Summary:
    Interface for the Bootloader library.

  Description:
    This file contains the interface definition for the OTA library.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "system_config.h"
#include "system_definitions.h"
#include "driver/driver_common.h"
#include "osal/osal.h"
#include "crypto/crypto.h"

#include "ota.h"
#include "ota_config.h"
#include "ota_image.h"
#include "downloader.h"
#include "imagestore.h"
#include "int_flash.h"

#define APP_IMG_BOOT_CTL_BLANK      { 0xFF,             0xFF, 0xFF, 0xFF,                     0xFFFFFFFF,        0xFFFFFFFF, 0xFFFFFFFF }
#define APP_IMG_BOOT_CTL_DEBUG      { IMG_STATUS_VALID, 0x01, 0xFF, IMG_TYPE_DEBUG,           APP_IMG_SLOT_ADDR, 0,          APP_IMG_BOOT_ADDR }
#define APP_IMG_BOOT_CTL_PROGRAMMER { IMG_STATUS_VALID, 0x01, 0xFF, IMG_TYPE_FLASH_PROGRAMMER,0xFFFFFFFF,        0xFFFFFFFF, 0xFFFFFFFF }

#define OTA_DEBUG_LEVEL        0
#define OTA_DOWNLOADER_TIMEOUT 100

#define __woraround_unused_variable(x) ((void)x)


typedef struct {
    FIRMWARE_IMAGE_HEADER header;
    uint8_t rsvd[4096-sizeof(FIRMWARE_IMAGE_HEADER)-1];
    uint8_t signature;
} OTA_BOOT_CONTROL;


const OTA_BOOT_CONTROL BOOT_CONTROL_INSTANCE\
 __attribute__((section(".ota_boot_control"))) = {\
    APP_IMG_BOOT_CTL_DEBUG, {}, IMG_STATUS_VALID };

typedef enum {
    OTA_TASK_INIT = 0,
    OTA_TASK_IDLE,
    OTA_TASK_ALLOCATE_SLOT,
    OTA_TASK_DOWNLOAD_IMAGE,
    OTA_TASK_VERIFY_IMAGE,
    OTA_TASK_SIGN_IMAGE,
    OTA_TASK_FACTORY_RESET,
    OTA_TASK_UPDATE_USER
} OTA_TASK_ID;

typedef struct 
{
    uint32_t        slot;
    uint8_t         order;
    uint8_t         abort;
    uint8_t         img_status;
    uint8_t         pfm_status;
} OTA_TASK_PARAM;

typedef struct
{
    struct {
        uint8_t             context[256 + 64+1024];
        int                 state;
        int                 dbg;
        OTA_TASK_PARAM      param;
    } task;

    OTA_TASK_ID             current_task;
    SYS_STATUS              status;
    OTA_COMPLETION_CALLBACK callback;
    DRV_HANDLE              downloader;
    OSAL_MUTEX_HANDLE_TYPE  mutex;
    
    uint32_t                debug_level;
    
} OTA_DATA;

static OTA_DATA __attribute__((coherent, aligned(128))) ota;


//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_Task_AllocateSlot(void)

  Description:
    Allocate a slot in IMAGE store. If the all slots are used, the function 
    uses the lowest ranked image's slot.

  Task Parameters:
    (OUT) slot   - slot to save image in IMAGESTORE
    (OUT) order  - order number for the downloading image

  Return:
    A SYS_STATUS code describing the current status.
*/
//---------------------------------------------------------------------------

typedef struct
{
    FIRMWARE_IMAGE_HEADER img;
    uint8_t  order;
    uint32_t slot;
    uint32_t selected;
    uint32_t *ptr;
} OTA_ALLOCATE_SLOT_TASK_CONTEXT;

typedef enum
{
    TASK_STATE_A_INIT = 0,
    TASK_STATE_A_READ_HEADER,
    TASK_STATE_A_CHECK_IMAGE,
    TASK_STATE_A_SELECT_SLOT,
    TASK_STATE_A_ERASE_SLOT,
    TASK_STATE_A_DONE
} OTA_ALLOCATE_SLOT_TASK_STATE;

static SYS_STATUS OTA_Task_AllocateSlot(void)
{
    OTA_ALLOCATE_SLOT_TASK_CONTEXT *ctx = (void*)ota.task.context;
    OTA_TASK_PARAM *param = &ota.task.param;

    SYS_ASSERT(sizeof(*ctx)<ota.task.context, "Buffer Overflow");

    

    switch (ota.task.state)
    {
        case TASK_STATE_A_INIT:
        {
            if (IMAGESTORE_Open() == false)
            {
                return SYS_STATUS_ERROR;
            }
            
            ctx->slot = 1;
            ctx->order = APP_IMG_BOOT_CTL->order;
            param->order = APP_IMG_BOOT_CTL->order + 1;

            if (param->order > 0x0F)
            {
                param->order = 0x01;
            }

            
            if (IMAGESTORE_NUM_SLOT == 2)
            {
                ctx->selected = 1;
                ota.task.state = TASK_STATE_A_SELECT_SLOT;
                break;
            }
            
#ifdef __DEBUG
            ctx->selected = ctx->slot;
            ota.task.state = TASK_STATE_A_SELECT_SLOT;
#else
            ota.task.state = TASK_STATE_A_READ_HEADER;
#endif
            break;
        }
        case TASK_STATE_A_READ_HEADER:
        {
            IMAGESTORE_Read(ctx->slot, 0, &ctx->img, sizeof(FIRMWARE_IMAGE_HEADER));

            ota.task.state = TASK_STATE_A_CHECK_IMAGE;
            break;
        }
        case TASK_STATE_A_CHECK_IMAGE:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }

            // Uninitialized slot
            if (*((uint32_t*)&ctx->img) == 0xFFFFFFFF
            || ctx->img.version != FIRMWARE_IMAGE_HEADER_VERSION
            || ctx->img.order ==  0x00
            || ctx->img.order > 0x0F)
            {
                ctx->selected = ctx->slot;
                ota.task.state = TASK_STATE_A_SELECT_SLOT;
                break;
            }

            if (ctx->img.status != IMG_STATUS_VALID)
            {
                ctx->selected = ctx->slot;
                ota.task.state = TASK_STATE_A_SELECT_SLOT;
                break;
            }
            
            if ((ctx->img.order - ctx->order) & 0x08)
            {
                ctx->selected = ctx->slot;
                ota.task.state = TASK_STATE_A_SELECT_SLOT;
                break;
            }

            ctx->slot++;
            ota.task.state = TASK_STATE_A_READ_HEADER;
            if (ctx->slot == IMAGESTORE_NUM_SLOT)
            {
                ota.task.state = TASK_STATE_A_SELECT_SLOT;
            }
            break;
        }
        case TASK_STATE_A_SELECT_SLOT:
        {
            if (ctx->selected == 0 || ctx->selected >= IMAGESTORE_NUM_SLOT)
            {
                return SYS_STATUS_ERROR;
            }
            IMAGESTORE_Read(ctx->selected, 0, &ctx->img, sizeof(FIRMWARE_IMAGE_HEADER));

            ota.task.state = TASK_STATE_A_ERASE_SLOT;
            break;
        }
        case TASK_STATE_A_ERASE_SLOT:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }

            IMAGESTORE_Erase(ctx->selected);
            ota.task.state = TASK_STATE_A_DONE;
            break;
        }
        case TASK_STATE_A_DONE:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }
            
            param->slot = ctx->selected;
            
            
            IMAGESTORE_Close();
            
            return SYS_STATUS_READY;
        }
        default:
        {
            SYS_ASSERT(false, "Unknown task state");
            return SYS_STATUS_ERROR;
        }
    }
    return SYS_STATUS_BUSY;
}
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_Task_DownloadImage(void)

  Description:
    Download image. The image is copied to the slot (passed by Task Param)
    in Image Store.

  Task Parameters:
    (IN) slot   - slot to save image in IMAGESTORE
    (IN) order  - order number for the downloading image

  Return:
    A SYS_STATUS code describing the current status.
*/
//---------------------------------------------------------------------------
typedef struct
{
    uint8_t *buf;
    uint32_t buf_len;
    uint32_t copied_len;
    uint32_t total_len;
    uint32_t timeout;
} OTA_DOWNLOAD_TASK_CONTEXT;

typedef enum
{
    TASK_STATE_D_INIT = 0,
    TASK_STATE_D_DOWNLOAD_HEADER,
    TASK_STATE_D_DOWNLOAD,
    TASK_STATE_D_PROGRAM,
    TASK_STATE_D_DONE,   
    TASK_STATE_D_ERROR
} OTA_DOWNLOAD_TASK_STATE;

static SYS_STATUS OTA_Task_DownloadImage(void)
{
    OTA_DOWNLOAD_TASK_CONTEXT * ctx = (void*)ota.task.context;
    OTA_TASK_PARAM *param = &ota.task.param;
    DRV_CLIENT_STATUS status = DRV_CLIENT_STATUS_BUSY;

    SYS_ASSERT(sizeof(*ctx)<ota.task.context, "Buffer Overflow");
    
    
    if ( param->abort )
    {
        if (ota.task.state == TASK_STATE_D_INIT)
        {
            return SYS_STATUS_ERROR;
        }
        ota.task.state = TASK_STATE_D_DONE;
    }
    else
    {
        if (ota.task.state != TASK_STATE_D_INIT
        &&  ota.task.state != TASK_STATE_D_DONE)
        {
            if ((ctx->timeout - SYS_TMR_TickCountGet()) & 0x80000000)
            {
                status = DRV_CLIENT_STATUS_ERROR;
                ota.task.state = TASK_STATE_D_DONE;
            }
        }
    }
    
    switch ( ota.task.state )
    {
        case TASK_STATE_D_INIT:
        {
            ctx->total_len = 0;
            ctx->copied_len = 0;
            ctx->buf_len = 0;
            ctx->buf = (uint8_t*)OSAL_Malloc(FLASH_SECTOR_SIZE);
            ctx->timeout = SYS_TMR_TickCountGet() + (OTA_DOWNLOADER_TIMEOUT * SYS_TMR_TickCounterFrequencyGet());

            ota.task.state = TASK_STATE_D_DOWNLOAD_HEADER;

            if (IMAGESTORE_Open() == false)
            {
                ota.task.state = TASK_STATE_D_DONE;
                status = DRV_CLIENT_STATUS_ERROR;
                break;
            }
            
            if (ctx->buf == NULL)
            {
                ota.task.state = TASK_STATE_D_DONE;
                status = DRV_CLIENT_STATUS_ERROR;
            }
            break;
        }
        case TASK_STATE_D_DOWNLOAD_HEADER:
        {
            FIRMWARE_IMAGE_HEADER *img;
            int rx_len;

            rx_len = DOWNLOADER_Read(ota.downloader, &ctx->buf[ctx->buf_len],(FLASH_SECTOR_SIZE- ctx->buf_len));

            if (rx_len <= 0)
            {
                if (rx_len < 0)
                {
                    ota.task.state = TASK_STATE_D_DONE;
                    status = DRV_CLIENT_STATUS_ERROR;
                }
                break;
            }

            ctx->buf_len += rx_len;
            if (ctx->buf_len != FLASH_SECTOR_SIZE)
            {
                break;
            }
            
            img = (FIRMWARE_IMAGE_HEADER *)ctx->buf;
            
            if (img->sz > IMAGESTORE_SLOT_SIZE
            ||  img->version != FIRMWARE_IMAGE_HEADER_VERSION)
            {
                status = DRV_CLIENT_STATUS_ERROR;
                ota.task.state = TASK_STATE_D_DONE;
                break;
            }
            
            img->status = 0xFF;
            img->order = param->order;
            img->type = IMG_TYPE_PRODUCTION;
            img->slot = param->slot;
            ctx->total_len = img->sz;
            ctx->buf[FIRMWARE_IMAGE_HEADER_SIGNATURE_BYTE] = 0xFF;
            ota.task.state = TASK_STATE_D_PROGRAM;

            break;
        }
        case TASK_STATE_D_DOWNLOAD:
        {
            int rx_len;
            int req_len = ctx->total_len - ctx->copied_len;
            
            if (req_len > FLASH_SECTOR_SIZE)
            {
                req_len = FLASH_SECTOR_SIZE;
            }
            
            rx_len = DOWNLOADER_Read(ota.downloader, &ctx->buf[ctx->buf_len], (req_len - ctx->buf_len));

            if (rx_len <= 0)
            {
                if (rx_len < 0)
                {
                    ota.task.state = TASK_STATE_D_DONE;
                    status = DRV_CLIENT_STATUS_ERROR;
                }
                break;
            }

            ctx->buf_len += rx_len;
            if (ctx->buf_len == req_len)
            {
                ota.task.state = TASK_STATE_D_PROGRAM;
            }
            break;
        }
        case TASK_STATE_D_PROGRAM:
        {
            if (ctx->buf_len)
            {
                if (ctx->buf_len <= FLASH_SECTOR_SIZE)
                {
                    memset(&ctx->buf[ctx->buf_len], 0, FLASH_SECTOR_SIZE-ctx->buf_len);
                }

                if (IMAGESTORE_Write(param->slot, ctx->copied_len, ctx->buf, FLASH_SECTOR_SIZE) == false)
                {
                    ota.task.state = TASK_STATE_D_DONE;
                    status = DRV_CLIENT_STATUS_ERROR;
                    break;
                }

                ctx->copied_len += ctx->buf_len;
                ctx->buf_len = 0;
                break;
            }
            
            if (IMAGESTORE_Busy())
            {
                break;
            }
            
            if (ctx->copied_len < ctx->total_len)
            {
                ota.task.state = TASK_STATE_D_DOWNLOAD;
            }
            else
            {
                ota.task.state = TASK_STATE_D_DONE;
            }
            break;
        }
        case TASK_STATE_D_DONE:
        {
            break;
        }
        default:
        {
            SYS_ASSERT(false, "Unknown task state");
            return SYS_STATUS_ERROR;
        }
    }

    if (ota.task.state == TASK_STATE_D_DONE)
    {
        if (ctx->buf != NULL)
        {
            OSAL_Free(ctx->buf);
            ctx->buf = NULL;
        }
        if (ota.downloader != DRV_HANDLE_INVALID)
        {
            DOWNLOADER_Close(ota.downloader);
            ota.downloader = DRV_HANDLE_INVALID;
        }
        return status == DRV_CLIENT_STATUS_ERROR?
                SYS_STATUS_ERROR : SYS_STATUS_READY;

    }
    return SYS_STATUS_BUSY;
}
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_Task_VerifyImage(void)
  
  Description:
    Verify the downloaded image in Image Store.
    (This example code uses SHA256 for image validation)
  
  Task Parameters:
    (IN) slot   - slot to save image in IMAGESTORE
    (IN) order  - order number for the downloading image
    (IN) abort  - 1 if OTA_Abort() is called.
  
  Return:
     A SYS_STATUS code describing the current status.
*/
//---------------------------------------------------------------------------
typedef struct
{
    CRYPT_SHA256_CTX  *sha256;
    uint8_t *buf;
    uint32_t len;
    uint32_t offset;
    uint32_t img_sz;
} OTA_VERIFY_TASK_CONTEXT;

typedef enum
{
    TASK_STATE_V_INIT = 0,
    TASK_STATE_V_READ_HEADER,
    TASK_STATE_V_READ,
    TASK_STATE_V_DONE
} OTA_VERIFY_TASK_STATE;

static SYS_STATUS OTA_Task_VerifyImage(void)
{
    OTA_VERIFY_TASK_CONTEXT * ctx = (void*)ota.task.context;
    OTA_TASK_PARAM *param = &ota.task.param;
    DRV_CLIENT_STATUS status;

    SYS_ASSERT(sizeof(*ctx) < ota.task.context, "Buffer Overflow");
    if ( param->abort )
    {
        if (ota.task.state == TASK_STATE_V_INIT)
        {
            return SYS_STATUS_ERROR;
        }
        ota.task.state = TASK_STATE_V_DONE;
    }

    switch ( ota.task.state )
    {
        case TASK_STATE_V_INIT:
        {
            ctx->offset = 0;
            ctx->len = FLASH_SECTOR_SIZE;
            ctx->buf = (uint8_t*)OSAL_Malloc(
                    FLASH_SECTOR_SIZE + sizeof(CRYPT_SHA256_CTX) + 128);
            
            if (ctx->buf == NULL)
            {
                return SYS_STATUS_ERROR;
            }
            
            if (IMAGESTORE_Open() == false)
            {
                return SYS_STATUS_ERROR;
            }
            
            ctx->sha256 = (CRYPT_SHA256_CTX *)( ((unsigned long)ctx->buf + FLASH_SECTOR_SIZE + 128) & 0xFFFFFF80);

            memset(ctx->sha256, 0, sizeof(CRYPT_SHA256_CTX));
            CRYPT_SHA256_Initialize(ctx->sha256);
            
            IMAGESTORE_Read(param->slot, ctx->offset, ctx->buf, ctx->len);
            
            ota.task.state = TASK_STATE_V_READ_HEADER;
            break;
        }
        case TASK_STATE_V_READ_HEADER:
        {
            unsigned char tmp[16];
            FIRMWARE_IMAGE_HEADER *img;
            
            if (IMAGESTORE_Busy())
            {
                break;
            }
            
            memcpy(tmp, ctx->buf, sizeof(tmp));
            memset(ctx->buf, 0xFF, sizeof(FIRMWARE_IMAGE_HEADER));
            
            img = (FIRMWARE_IMAGE_HEADER *)tmp;
            
            ((FIRMWARE_IMAGE_HEADER*)ctx->buf)->version   = img->version;
            ((FIRMWARE_IMAGE_HEADER*)ctx->buf)->sz        = img->sz;
            ((FIRMWARE_IMAGE_HEADER*)ctx->buf)->boot_addr = img->boot_addr;

            ctx->buf[FIRMWARE_IMAGE_HEADER_SIGNATURE_BYTE] = 0xFF;
            ctx->img_sz = img->sz;
            
            if (img->version != 1 || img->sz > IMAGESTORE_SLOT_SIZE)
            {
                param->abort = 1;
                ota.task.state = TASK_STATE_V_DONE;
                break;
            }
            CRYPT_SHA256_DataSizeSet(ctx->sha256, ctx->img_sz);

            ota.task.state = TASK_STATE_V_READ;
            break;
        }
        case TASK_STATE_V_READ:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }
            
            CRYPT_SHA256_DataAdd(ctx->sha256, ctx->buf, ctx->len);
            ctx->offset += ctx->len;

            if (ctx->offset == ctx->img_sz)
            {
                IMAGESTORE_Read(param->slot, 0, ctx->buf, FLASH_SECTOR_SIZE);
                
                ota.task.state = TASK_STATE_V_DONE;
                break;
            }
            
            if ((ctx->offset + ctx->len) > ctx->img_sz)
            {
                ctx->len = ctx->img_sz - ctx->offset;
            }
            
            IMAGESTORE_Read(param->slot, ctx->offset, ctx->buf, ctx->len);
            break;
        }
        case TASK_STATE_V_DONE:
        {
            uint8_t digest[CRYPT_SHA256_DIGEST_SIZE];
            FIRMWARE_IMAGE_HEADER *img;
            
            if (IMAGESTORE_Busy())
            {
                break;
            }
            
            if (param->abort == 0)
            {
                CRYPT_SHA256_Finalize(ctx->sha256, digest);
                img = (FIRMWARE_IMAGE_HEADER*)ctx->buf;
                if (memcmp(digest, img->digest, CRYPT_SHA256_DIGEST_SIZE) != 0)
                {
                    status = DRV_CLIENT_STATUS_ERROR;
                }
                else
                {
                     status = DRV_CLIENT_STATUS_READY;
                }
            } else {
                status = DRV_CLIENT_STATUS_ERROR;
            }

            IMAGESTORE_Close();

            if (ctx->buf != NULL)
            {
                OSAL_Free(ctx->buf);
            }
            ctx->buf = NULL;
            return status == DRV_CLIENT_STATUS_READY?
                    SYS_STATUS_READY : SYS_STATUS_ERROR;
        }
        default:
        {
            SYS_ASSERT(false, "Unknown task state");
            return SYS_STATUS_ERROR;
        }
    }

    return SYS_STATUS_BUSY;
}
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_Task_SignImage(void)
  
  Description:
    Mark downloaded image to DOWNLOADED and mark PFM to VALID.
    This task is called from OTA_Start() and OTA_SignImage()

    
  Task Parameters:
    (IN) slot      - slot for the target image in Image Store
    (IN) img_status - New state value to set Image Store
    (IN) pfm_status - New state value to set PFM
  
  Return:
    A SYS_STATUS code describing the current status.
*/
//---------------------------------------------------------------------------
typedef struct
{
    FIRMWARE_IMAGE_HEADER img;
    uint8_t *buf;
} OTA_SIGN_IMAGE_TASK_CONTEXT;

typedef enum
{
    TASK_STATE_S_INIT = 0,
    TASK_STATE_S_READ_HEADER,
    TASK_STATE_S_WRITE_HEADER,
    TASK_STATE_S_VERIFY_HEADER,
    TASK_STATE_S_ERASE_BOOT_CONTROL,
    TASK_STATE_S_WRITE_BOOT_CONTROL,
    TASK_STATE_S_DONE,
} OTA_SIGN_IMAGE_TASK_STATE;

static SYS_STATUS OTA_Task_SignImage(void)
{
    OTA_SIGN_IMAGE_TASK_CONTEXT *ctx = (void*)ota.task.context;
    OTA_TASK_PARAM *param = &ota.task.param;
 
    SYS_ASSERT(sizeof(*ctx)<ota.task.context, "Buffer Overflow");
    
    if ( param->abort )
    {
        if (ota.task.state == TASK_STATE_S_INIT)
        {
            return SYS_STATUS_ERROR;
        }
        ota.task.state = TASK_STATE_S_DONE;
    }


    switch ( ota.task.state )
    {
        case TASK_STATE_S_INIT:
        {
            ctx->buf = (uint8_t*)OSAL_Malloc(FLASH_SECTOR_SIZE);
            if (ctx->buf == NULL)
            {
                return SYS_STATUS_ERROR;
            }
            
            if (IMAGESTORE_Open() == false)
            {
                OSAL_Free(ctx->buf);
                return SYS_STATUS_ERROR;
            }
            
            IMAGESTORE_Read(param->slot, 0, ctx->buf, FLASH_SECTOR_SIZE);
            
            ota.task.state = TASK_STATE_S_READ_HEADER;
            break;
        }
        case TASK_STATE_S_READ_HEADER:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }

            memcpy(&ctx->img, ctx->buf, sizeof(FIRMWARE_IMAGE_HEADER));
            ota.task.state = TASK_STATE_S_WRITE_HEADER;
            break;
        }
        case TASK_STATE_S_WRITE_HEADER:
        {
            if (ctx->img.status != param->img_status
            &&  param->slot != 0)
            {
                ctx->img.status = param->img_status;
                memcpy(ctx->buf, &ctx->img, sizeof(FIRMWARE_IMAGE_HEADER));
                ctx->buf[FIRMWARE_IMAGE_HEADER_SIGNATURE_BYTE] = ctx->img.status;
                IMAGESTORE_Write(param->slot, 0, ctx->buf, FLASH_SECTOR_SIZE);
                ota.task.state = TASK_STATE_S_VERIFY_HEADER;
            }
            else
            {
                ota.task.state = TASK_STATE_S_ERASE_BOOT_CONTROL;
            }
            break;
        }
        case TASK_STATE_S_VERIFY_HEADER:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }
                        
            IMAGESTORE_Read(param->slot, 0, ctx->buf, FLASH_SECTOR_SIZE);

            ota.task.state = TASK_STATE_S_ERASE_BOOT_CONTROL;
            break;
        }
        case TASK_STATE_S_ERASE_BOOT_CONTROL:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }
            
            ota.task.state = TASK_STATE_S_WRITE_BOOT_CONTROL;
            break;
        }
        case TASK_STATE_S_WRITE_BOOT_CONTROL:
        {   
            INT_Flash_Open();
                        
            ctx->img.status = ota.task.param.pfm_status;
            memcpy(ctx->buf, &ctx->img, sizeof(FIRMWARE_IMAGE_HEADER));
            ctx->buf[FIRMWARE_IMAGE_HEADER_SIGNATURE_BYTE] = ctx->img.status;
            
            INT_Flash_Write(APP_IMG_SLOT_ADDR, ctx->buf, FLASH_SECTOR_SIZE);
            ota.task.state = TASK_STATE_S_DONE;
            break;
        }
        case TASK_STATE_S_DONE:
        {
            if (INT_Flash_Busy())
            {
                break;
            }
            
            INT_Flash_Close();
            IMAGESTORE_Close();
            
            if (ctx->buf != NULL)
            {
                OSAL_Free(ctx->buf);
                ctx->buf = NULL;
            }

            return SYS_STATUS_READY;
        }
        default:
        {
            SYS_ASSERT(false, "Unknown task state");
            return SYS_STATUS_ERROR;
        }
    }

    return SYS_STATUS_BUSY;
}

//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_Task_FactoryReset(void)
  
  Description:
    Mark all downloaded image to DISABLED
  
  Task Parameters:
    None
  
  Return:
    A SYS_STATUS code describing the current status.
*/
//---------------------------------------------------------------------------
typedef struct
{
    uint32_t slot;

} OTA_FACTORY_RESET_TASK_CONTEXT;

typedef enum
{
    TASK_STATE_F_INIT = 0,
    TASK_STATE_F_DISABLE_IMAGE,
    TASK_STATE_F_INVALIDATE_BOOT_CTL,
    TASK_STATE_F_DONE
} OTA_FACTORY_RESET_TASK_STATE;

static SYS_STATUS OTA_Task_FactoryReset(void)
{
    OTA_FACTORY_RESET_TASK_CONTEXT * ctx = (void*)ota.task.context;

    SYS_ASSERT(sizeof(*ctx)<ota.task.context, "Buffer Overflow");
    

    switch ( ota.task.state )
    {
        case TASK_STATE_F_INIT:
        {
            if (APP_IMG_BOOT_CTL->slot == 0)
            {
                return SYS_STATUS_READY;
            }            
            
            if (IMAGESTORE_Open() == false)
            {
                return SYS_STATUS_ERROR;
            }
            
            ctx->slot = APP_IMG_BOOT_CTL->slot + 1;
            if (ctx->slot == IMAGESTORE_NUM_SLOT)
            {
                ctx->slot = 1;
            }
            ota.task.state = TASK_STATE_F_DISABLE_IMAGE;
            break;
        }
        case TASK_STATE_F_DISABLE_IMAGE:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }
            
            IMAGESTORE_Erase(ctx->slot);
            if (ctx->slot == APP_IMG_BOOT_CTL->slot)
            {
                ota.task.state = TASK_STATE_F_INVALIDATE_BOOT_CTL;
                break;
            }
                      
            ctx->slot++;
            if (ctx->slot >= IMAGESTORE_NUM_SLOT)
            {
                ctx->slot = 1;
            }
            break;
        }
        case TASK_STATE_F_INVALIDATE_BOOT_CTL:
        {
            if (IMAGESTORE_Busy())
            {
                break;
            }
            
            INT_Flash_Open();
            INT_Flash_Erase(APP_IMG_SLOT_ADDR, FLASH_SECTOR_SIZE);
            ota.task.state = TASK_STATE_F_DONE;
            break;
        }
        case TASK_STATE_F_DONE:
        {
            if (INT_Flash_Busy())
            {
                break;
            }
            
            INT_Flash_Close();
            IMAGESTORE_Close();
            return SYS_STATUS_READY;
        }
        default:
        {
            SYS_ASSERT(false, "Unknown task state");
            return SYS_STATUS_ERROR;
        }
    }
    return SYS_STATUS_BUSY;
}
//---------------------------------------------------------------------------
static SYS_STATUS OTA_Task_UpdateUser(void)
{
    OTA_COMPLETION_CALLBACK callback = ota.callback;
    SYS_STATUS status = ota.status;
    
    if (ota.downloader != DRV_HANDLE_INVALID)
    {
        DOWNLOADER_Close(ota.downloader);
        ota.downloader = DRV_HANDLE_INVALID;
    }
    
    ota.callback = NULL;
    ota.status = DRV_CLIENT_STATUS_READY;
    if (callback != NULL)
    {
        callback(status);
    }

    return SYS_STATUS_BUSY;
}
//---------------------------------------------------------------------------
SYS_STATUS OTA_Status(void)
{
    return (ota.current_task == OTA_TASK_IDLE)?
        SYS_STATUS_READY:
        SYS_STATUS_BUSY;
}
//---------------------------------------------------------------------------
SYS_STATUS OTA_Start(void *downloader_param, OTA_COMPLETION_CALLBACK callback)
{
    SYS_STATUS status = SYS_STATUS_ERROR;
    OSAL_MUTEX_Lock(&ota.mutex, OSAL_WAIT_FOREVER);

    do
    {
        if (ota.current_task != OTA_TASK_IDLE)
        {
            status = SYS_STATUS_ERROR;
            break;
        }

        ota.downloader = DOWNLOADER_Open(downloader_param);

        if (ota.downloader == DRV_HANDLE_INVALID)
        {
            status = SYS_STATUS_ERROR;
            break;
        }

        ota.current_task = OTA_TASK_ALLOCATE_SLOT;
        ota.status = SYS_STATUS_BUSY;
        ota.callback = callback;
        ota.task.param.img_status = IMG_STATUS_DOWNLOADED;
        ota.task.param.pfm_status = IMG_STATUS_DISABLED;
        ota.task.param.abort = 0;
        status = SYS_STATUS_BUSY;
        break;
    }
    while(0);

    OSAL_MUTEX_Unlock(&ota.mutex);

    return status;
}
//---------------------------------------------------------------------------
SYS_STATUS OTA_Abort(void)
{
    SYS_STATUS status;
    
    
    OSAL_MUTEX_Lock(&ota.mutex, OSAL_WAIT_FOREVER);

    do
    {
        if (ota.current_task == OTA_TASK_ALLOCATE_SLOT
        ||  ota.current_task == OTA_TASK_DOWNLOAD_IMAGE
        ||  ota.current_task == OTA_TASK_VERIFY_IMAGE
        ||  ota.current_task == OTA_TASK_SIGN_IMAGE)
        {
            ota.task.param.abort = 1;
            status = SYS_STATUS_BUSY;
            break;
        }
        // OTA was not kicked off or already completed.
        status = SYS_STATUS_READY;
    }
    while(0);

    OSAL_MUTEX_Unlock(&ota.mutex);
    
    return status;
}
//---------------------------------------------------------------------------
SYS_STATUS OTA_SignImage(OTA_COMPLETION_CALLBACK callback)
{
    SYS_STATUS status = SYS_STATUS_ERROR;
    OSAL_MUTEX_Lock(&ota.mutex, OSAL_WAIT_FOREVER);

    do
    {
        if (ota.current_task != OTA_TASK_IDLE)
        {
            status = SYS_STATUS_ERROR;
            break;
        }

        if (APP_IMG_BOOT_CTL->version != 1)
        {
            status = SYS_STATUS_ERROR;
            break;
        }
        
        ota.current_task = OTA_TASK_SIGN_IMAGE;
        ota.status = SYS_STATUS_BUSY;
        ota.callback = callback;

        //Task Parameters:
        //(IN) slot      - slot for the target image in Image Store
        //(IN) img_status - New state value to set Image Store
        //(IN) pfm_status - New state value to set PFM
        ota.task.param.slot       = APP_IMG_BOOT_CTL->slot; 
        ota.task.param.img_status = IMG_STATUS_VALID;
        ota.task.param.pfm_status = IMG_STATUS_VALID;
        ota.task.param.abort = 0;
        status = ota.status;
        if (APP_IMG_BOOT_CTL->slot == 0)
        {
            ota.current_task = OTA_TASK_UPDATE_USER;
            ota.status = SYS_STATUS_READY;
            status = SYS_STATUS_BUSY;
        } 
    }
    while(0);

    OSAL_MUTEX_Unlock(&ota.mutex);
    return status;
}
//---------------------------------------------------------------------------
SYS_STATUS OTA_Rollback(OTA_COMPLETION_CALLBACK callback)
{
    SYS_STATUS status = SYS_STATUS_ERROR;
    OSAL_MUTEX_Lock(&ota.mutex, OSAL_WAIT_FOREVER);

    do
    {
        if (ota.current_task != OTA_TASK_IDLE)
        {
            status = SYS_STATUS_ERROR;
            break;
        }

        if (APP_IMG_BOOT_CTL->version != 1)
        {
            status = SYS_STATUS_ERROR;
            break;
        }

        if (APP_IMG_BOOT_CTL->order == 0)
        {
            status = SYS_STATUS_READY;
            break;
        }
        
        
        ota.current_task = OTA_TASK_SIGN_IMAGE;
        ota.status = SYS_STATUS_BUSY;
        ota.callback = callback;

        //Task Parameters:
        //(IN) slot      - slot for the target image in Image Store
        //(IN) img_status - New state value to set Image Store
        //(IN) pfm_status - New state value to set PFM
        ota.task.param.slot = APP_IMG_BOOT_CTL->slot; 
        ota.task.param.img_status = IMG_STATUS_DISABLED;
        ota.task.param.pfm_status = IMG_STATUS_DISABLED;
        ota.task.param.abort = 0;

        status = ota.status;
    }
    while(0);

    OSAL_MUTEX_Unlock(&ota.mutex);
    
    return status;
}
//---------------------------------------------------------------------------
SYS_STATUS OTA_FactoryReset(OTA_COMPLETION_CALLBACK callback)
{
    SYS_STATUS status = SYS_STATUS_BUSY;
    
    OSAL_MUTEX_Lock(&ota.mutex, OSAL_WAIT_FOREVER);

    do
    {
        
        if (ota.current_task != OTA_TASK_IDLE)
        {
            status = SYS_STATUS_ERROR;
            break;
        }

        ota.current_task = OTA_TASK_FACTORY_RESET;
        ota.status = SYS_STATUS_READY;
        ota.callback = callback;
    }
    while(0);

    OSAL_MUTEX_Unlock(&ota.mutex);

    return status;
}
//---------------------------------------------------------------------------
void OTA_Tasks(void)
{
    OTA_TASK_ID next = ota.current_task;
    int init_completed = 0;
    
    OSAL_MUTEX_Lock(&ota.mutex, OSAL_WAIT_FOREVER);
    
    switch ( ota.current_task )
    {
        case OTA_TASK_INIT:
        {
            if (IMAGESTORE_Initialize() == true)
            {
                next = OTA_TASK_IDLE;
                init_completed = 1;
            }
            break;
        }
        case OTA_TASK_IDLE:
        {
            break;
        }
        case OTA_TASK_ALLOCATE_SLOT:
        {
            ota.status = OTA_Task_AllocateSlot();
            
            if (ota.status == SYS_STATUS_READY)
            {
                next = OTA_TASK_DOWNLOAD_IMAGE;
            }
            
            if (ota.status  == SYS_STATUS_ERROR)
            {
                next = OTA_TASK_UPDATE_USER;
            }
            
            break;
        }
        case OTA_TASK_DOWNLOAD_IMAGE:
        {
            ota.status = OTA_Task_DownloadImage();
            
            if (ota.status == SYS_STATUS_READY)
            {
                next = OTA_TASK_VERIFY_IMAGE;
            }
            
            if (ota.status == SYS_STATUS_ERROR)
            {
                next = OTA_TASK_UPDATE_USER;
            }
            
            break;
        }
        case OTA_TASK_VERIFY_IMAGE:
        {
            ota.status = OTA_Task_VerifyImage();
            
            if (ota.status == SYS_STATUS_READY)
            {
                next = OTA_TASK_SIGN_IMAGE;
            }
            
            if (ota.status == SYS_STATUS_ERROR)
            {
                next = OTA_TASK_UPDATE_USER;
            }
            break;
        }
        case OTA_TASK_SIGN_IMAGE:
        {
            ota.status = OTA_Task_SignImage();
            
            if (ota.status != SYS_STATUS_BUSY)
            {
                next = OTA_TASK_UPDATE_USER;
                break;
            }
            
            break;
        }
        case OTA_TASK_FACTORY_RESET:
        {
            ota.status = OTA_Task_FactoryReset();
            
            if (ota.status != SYS_STATUS_BUSY)
            {
                next = OTA_TASK_UPDATE_USER;
            }
            break;
        }
        case OTA_TASK_UPDATE_USER:
        {
            ota.current_task = OTA_TASK_IDLE;
            OTA_Task_UpdateUser();
            next = OTA_TASK_IDLE;
            break;
        }
        default:
        {
            SYS_ASSERT(false, "Unknown Task");
            break;
        }
    }
    
    if (next != ota.current_task)
    {
        ota.current_task = next;
        ota.task.state = 0;
        ota.task.dbg = -1;
    }

    OSAL_MUTEX_Unlock(&ota.mutex);
    
    DOWNLOADER_Tasks();

    if (init_completed != 0)
    {
        OTA_SignImage(NULL);
    }
}

//---------------------------------------------------------------------------
/*
*/
//---------------------------------------------------------------------------
void OTA_Initialize(void)
{
    memset(&ota, 0, sizeof(ota));
    ota.downloader = DRV_HANDLE_INVALID;
    ota.task.dbg = -1;
    ota.status = SYS_STATUS_UNINITIALIZED;
    ota.debug_level = OTA_DEBUG_LEVEL;
    
    OSAL_MUTEX_Create(&ota.mutex);
    
    INT_Flash_Initialize();
    DOWNLOADER_Initialize();
}
