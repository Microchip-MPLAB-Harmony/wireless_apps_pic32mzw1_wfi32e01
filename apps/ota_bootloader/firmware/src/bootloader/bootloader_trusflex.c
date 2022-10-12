// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2020-2021 released Microchip Technology Inc.  All rights reserved.

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
#include "int_flash.h"
#include "sha256.h"
#include "system/fs/sys_fs.h"
#include "../bootloader/csv/csv.h"
#include "ota_database_parser.h"

#include "atca_basic.h"
#include "atcacert/atcacert_der.h"                
//---------------------------------------------------------------------------

static bool factory_reset = true;
static bool new_image = false;
static int selected_row;
static char digest_str[66];
static char digest_gl[4];
static uint8_t digest_g[32];
static OTA_DB_BUFFER *imageDB;
static bool database_found;

typedef uint32_t BOOTLOADER_STATUS;

#ifdef  SYS_OTA_SECURE_BOOT_ENABLED
#define FACTORY_IMAGE_VERIFICATION_ENABLED
#endif

#define SYS_CONSOLE_DEBUG1      printf
#define BOOTLOADER_STATUS_SUCCESS                  0
#define BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED 1
#define BOOTLOADER_STATUS_ERROR                   -1
#define BOOTLOADER_STATUS_ERROR_NO_IMAGE_FOUND    -2


#ifdef SYS_OTA_APPDEBUG_ENABLED
#define OTA_DEBUG  
#endif 

#ifdef OTA_DEBUG
#define FIRMWARE_IMAGE_HEADER_SIGNATURE_BYTE (4095)
#endif

#define __woraround_unused_variable(x) ((void)x)

typedef enum {
    BOOTLOADER_TASK_INIT = 0,
    BOOTLOADER_TASK_CHECK_IMAGE,
    BOOTLOADER_TASK_SELECT_IMAGE,
    BOOTLOADER_TASK_PROGRAM_IMAGE,
    BOOTLOADER_TASK_VERIFY_IMAGE,
    BOOTLOADER_TASK_SET_IMAGE_STATUS,
    BOOTLOADER_TASK_INVALIDATE_IMAGE,
    BOOTLOADER_TASK_NO_IMAGE_FOUND,
    BOOTLOADER_TASK_JUMP_TO_APP,
    BOOTLOADER_TASK_FACTORY_RESET
} BOOTLOADER_TASK_ID;

typedef struct {
    FIRMWARE_IMAGE_HEADER img;
} BOOTLOADER_TASK_PARAM;

typedef struct {
    uint8_t buf[FLASH_SECTOR_SIZE];

    struct {
        uint8_t context[1024];
        int state;
        BOOTLOADER_TASK_PARAM param;
    } task;
    BOOTLOADER_TASK_ID current_task;

    struct {
        bool disk_mount;
        bool reading_file;
        bool file_opened;
    } file_sys;
} BOOTLOADER_DATA;

static BOOTLOADER_DATA __attribute__((coherent, aligned(128))) bootloader;

#define APP_MOUNT_NAME          "/mnt/myDrive1"
#define APP_DEVICE_NAME         "/dev/mtda1"
#define APP_FS_TYPE             FAT
#define APP_OTA_DATABASE_NAME   "image_database.csv"
#define APP_OTA_DATABASE_PATH   "/mnt/myDrive1/ota/image_database.csv"
#define APP_FILE_NAME           "ota/factory_reset.bin"
#define APP_FACTORY_IMAGE_DIGEST_FILE_NAME  "ota/factory_reset_digest.txt"
#define APP_FACTORY_IMAGE_SIGNATURE_FILE_NAME  "ota/factory_image_sign.txt"
#define BUFFER_SIZE                (4096U)
uint8_t CACHE_ALIGN work[SYS_FS_FAT_MAX_SS];

typedef enum {
    /* The app mounts the disk */
    APP_MOUNT_DISK = 0,

    /* The disk mount success */
    APP_MOUNT_SUCCESS,

    /* The app formats the disk. */
    APP_FORMAT_DISK,
            
    /* Create directory. */        
    APP_CREATE_DIR,
    
    /* Open directory. */         
    APP_OPEN_DIR,        

    /* The app opens the file */
    APP_OPEN_FILE,

    APP_ERROR

} APP_FILE_STATES;

typedef struct {
    /* SYS_FS File handle */
    SYS_FS_HANDLE fileHandle;

    /* Application's current state */
    APP_FILE_STATES state;

} APP_DATA_FILE;

static APP_DATA_FILE CACHE_ALIGN appFile;
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

void Bootloader_TraceHeader(void *addr) {
    volatile FIRMWARE_IMAGE_HEADER *hdr = (volatile FIRMWARE_IMAGE_HEADER *)addr;
    printf("status    :%02X\n", hdr->status);
    printf("version   :%02X\n", hdr->version);
    //printf("order     :%02X\n", hdr->order);
    printf("type      :%02X\n", hdr->type);
    //printf("sz        :%08X\n", hdr->sz);
    //printf("slot      :%08X\n", hdr->slot);
    printf("boot_addr :%08X\n", hdr->boot_addr);

    printf("\n");
}

#ifdef SYS_OTA_SECURE_BOOT_ENABLED
static char signature_str[98];
extern ATCAIfaceCfg atecc608_0_init_data;
ATCAIfaceCfg* device_cfg = &atecc608_0_init_data;
uint8_t sernum[9];
char displayStr[30];
size_t displen = sizeof (displayStr);
ATCA_STATUS sig_verify_status;
bool Bootloader_Hash_Signature_Verify(const char* base64Signature)
{
#ifdef OTA_DEBUG
    printf("\n\rhash_firmware_verify\n\r");
#endif    
    uint8_t hash[34];
    bool ret = false;
    int i;
    
    sig_verify_status = atcab_init(device_cfg);
    
#ifdef OTA_DEBUG
    printf("atcab_init: %d\r\n", sig_verify_status);
#endif
    if (ATCA_SUCCESS == sig_verify_status) {
        sig_verify_status = atcab_read_serial_number(sernum);
        atcab_bin2hex(sernum, 9, displayStr, &displen);
        
#ifdef OTA_DEBUG
        printf("Serial Number of the Device: %s\r\n\n", displayStr);
#endif
    }
    else{
        atcab_release();
        return false;
    }

#ifdef OTA_DEBUG
    uint8_t public_key[ATCA_ECCP256_PUBKEY_SIZE];
    sig_verify_status = atcab_read_pubkey(15, public_key);

    printf("Public key from Slot 15:    \r\n");
    
    for (i = 0; i < ATCA_ECCP256_PUBKEY_SIZE; i++) {
        printf("%02X, ", public_key[i]);
    }
    printf("\r\n\r\n");
#endif  
    uint8_t signatureDer[80] = {0};
    size_t sigDerLen = 80;
    atcab_base64decode(base64Signature, strlen(base64Signature), signatureDer, &sigDerLen);
    
    for (i = 0; i < 32; i++) {
        hash[i] = digest_g[i];
        
#ifdef OTA_DEBUG 
        printf("hash[%d] : %x\n\r",i,hash[i]);
#endif
    }
    bool is_verified = false;
    sig_verify_status = atcab_verify_stored(hash, signatureDer, 15, &is_verified);
    ret = is_verified;
    
#ifdef OTA_DEBUG
    printf("\r\nStatus = 0x%X     isverified = %s\r\n", sig_verify_status, is_verified ? "True" : "False");
#endif
    atcab_release();
    return ret;
}

#endif
// *****************************************************************************
// *****************************************************************************
// Section: To Initialize OTA bootloader related parameters
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  void Bootloader_Initialize(void)
 
  Description:
    To Initialize OTA bootloader related parameters
  
  Task Parameters:
    None
 
  Return:
    None
 */
//---------------------------------------------------------------------------

void Bootloader_Initialize(void) {
    memset(&bootloader, 0, sizeof (bootloader));
    bootloader.current_task = BOOTLOADER_TASK_INIT;
    printf("\n");
    printf("***********************************************************\n");
    printf("BOOTLOADER Build %s %s\n", __DATE__, __TIME__);
    printf("OTA VERSION %s\n", OTA_VERSION);
    printf("***********************************************************\n");

    INT_Flash_Initialize();
    appFile.state = APP_MOUNT_DISK;
}

// *****************************************************************************
// *****************************************************************************
// Section: To open a file
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  void open_file(void)
 
  Description:
    To open a file 
  
  Task Parameters:
    None
 
  Return:
    None
 */
//---------------------------------------------------------------------------

void open_file(void) {
    SYS_FS_FORMAT_PARAM opt;
    /*State would have been set by mount_disk() function*/
    switch (appFile.state) {
        case APP_FORMAT_DISK:
        {
            #ifdef OTA_DEBUG
            printf("Formating File");
            #endif
            opt.fmt = SYS_FS_FORMAT_FAT;
            opt.au_size = 0;

            if (SYS_FS_DriveFormat(APP_MOUNT_NAME, &opt, (void *) work, SYS_FS_FAT_MAX_SS) != SYS_FS_RES_SUCCESS) {
                /* Format of the disk failed. */
                appFile.state = APP_ERROR;
            } else {
                /* Format succeeded. Open a file. */
                appFile.state = APP_CREATE_DIR;
            }
            break;
        }
        case APP_CREATE_DIR:
        {
            SYS_FS_RESULT res;
            res = SYS_FS_DirectoryMake("ota");
            if(res == SYS_FS_RES_FAILURE){
                // Directory make failed
            }
            else{
                appFile.state = APP_OPEN_FILE;
            }
            break;
        }
        case APP_OPEN_DIR:
        {
            SYS_FS_HANDLE dirHandle;
            dirHandle = SYS_FS_DirOpen("/mnt/myDrive1/ota");
            if(dirHandle != SYS_FS_HANDLE_INVALID){
                // Directory open is successful
                appFile.state = APP_OPEN_FILE;
            }
            else{
                appFile.state = APP_ERROR;
            }
            break;
        }
        case APP_OPEN_FILE:
        {
#ifdef OTA_DEBUG
            printf("opening File");
#endif
            if (bootloader.file_sys.reading_file == true)
                appFile.fileHandle = SYS_FS_FileOpen(APP_MOUNT_NAME"/"APP_FILE_NAME, (SYS_FS_FILE_OPEN_READ_PLUS));
            else
                appFile.fileHandle = SYS_FS_FileOpen(APP_MOUNT_NAME"/"APP_FILE_NAME, (SYS_FS_FILE_OPEN_WRITE_PLUS));

            if (appFile.fileHandle == SYS_FS_HANDLE_INVALID) {
                /* File open unsuccessful */
                appFile.state = APP_ERROR;
            } else {
                /* File open was successful. Write to the file. */
#ifdef OTA_DEBUG
                printf("File Opened");
#endif
                bootloader.file_sys.file_opened = true;
            }
            break;
        }
        case APP_ERROR:
        {
            printf("file open error\n");
            break;
        }
        default:
        {
            printf("file open error\n");
            break;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Mounting Disk in external flash
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  static void mount_disk()

  Description:
   Mounting Disk in external flash

  Task Parameters:
    None
  Return:
    None
 */
//---------------------------------------------------------------------------

static void mount_disk() {
    
    switch (appFile.state) {
        case APP_MOUNT_DISK:
        {
            /* Mount the disk */
            if (SYS_FS_Mount(APP_DEVICE_NAME, APP_MOUNT_NAME, APP_FS_TYPE, 0, NULL) != 0) {
                /* The disk could not be mounted. Try mounting again until
                 * the operation succeeds. */
                appFile.state = APP_MOUNT_DISK;
            } else {
                /* Mount was successful. */

                appFile.state = APP_MOUNT_SUCCESS;
            }
            break;
        }
        case APP_MOUNT_SUCCESS:
        {
            bootloader.file_sys.disk_mount = true;
            appFile.state = APP_OPEN_FILE;
            break;
        }
        case APP_ERROR:
        {
            break;
        }
        default:
        {

            break;
        }
    }
}
#ifdef FACTORY_IMAGE_BACKUP_DISABLE
// *****************************************************************************
// *****************************************************************************
// Section: Formating Disk in external flash
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  static void format_disk()

  Description:
   Formating Disk in external flash

  Task Parameters:
    None
  Return:
    None
 */
//---------------------------------------------------------------------------

static void format_disk() {
    SYS_FS_FORMAT_PARAM opt;
    SYS_FS_DriveFormat(APP_MOUNT_NAME, &opt, (void *) work, SYS_FS_FAT_MAX_SS);
}
#endif
// *****************************************************************************
// *****************************************************************************
// Section: Return BOOTLOADER_STATUS_SUCCESS if the images on INT/EXT are the same.Otherwise returns BOOTLOADER_STATUS_ERROR
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_CheckImage(void)

  Description:
    Return BOOTLOADER_STATUS_SUCCESS if the images on INT/EXT are the same.
    Otherwise returns BOOTLOADER_STATUS_ERROR

  Task Parameters:
    None

  Return:
    A BOOTLOADER_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

typedef struct {
    uint8_t *buf;
} BOOTLOADER_CHECK_IMAGE_TASK_CONTEXT;

typedef enum {
    TASK_STATE_C_INIT = 0,
    TASK_STATE_C_PARSE_DATABASE,
    TASK_STATE_C_INVALIDATE_BOOT_CTL,
    TASK_STATE_C_INVALIDATE_IMAGESTORE,
    TASK_STATE_C_DONE
} BOOTLOADER_CHECK_IMAGE_TASK_STATE;

static BOOTLOADER_STATUS Bootloader_Task_CheckImage(void) {
    BOOTLOADER_CHECK_IMAGE_TASK_CONTEXT *ctx = (void *) bootloader.task.context;

    switch (bootloader.task.state) {
        case TASK_STATE_C_INIT:
        {
            uint32_t *p = (uint32_t*) APP_IMG_BOOT_CTL;
            if (p[0] == 0xffffffff && p[1] == 0xffffffff
                    && p[2] == 0xffffffff && p[3] == 0xffffffff) {
                //APP_IMG_BOOT_CTL is uninitialized.
                factory_reset = true;
                printf("Uninitialized sector\n");
                return BOOTLOADER_STATUS_ERROR;
            }
            
            factory_reset = false;
            imageDB = OTA_GetDBBuffer();
            int ret_db_status = OTAGetDb(imageDB, APP_OTA_DATABASE_PATH);

            /*check for error code*/
            if (ret_db_status > 1)
                return SYS_STATUS_ERROR;
            else if (ret_db_status == OTA_DB_NOT_FOUND) {
                database_found = false;
#ifdef OTA_DEBUG
                printf("no database found");
#endif
                /*need to load factory reset image*/
                factory_reset = true;
                return BOOTLOADER_STATUS_ERROR;
            } else {
                database_found = true;
                
            }
            ctx->buf = bootloader.buf;
            bootloader.task.state = TASK_STATE_C_PARSE_DATABASE;
            break;
        }
        case TASK_STATE_C_PARSE_DATABASE:
        {
            FIRMWARE_IMAGE_HEADER *img;
            img = (FIRMWARE_IMAGE_HEADER *) ctx->buf;
            if (database_found == true) {
#ifdef OTA_DEBUG
                SYS_CONSOLE_DEBUG1("\n\nDatabase found\n\n");
#endif
                selected_row = GetImageRow(APP_IMG_BOOT_CTL->version, imageDB);
                if (selected_row == -1) {
#ifdef OTA_DEBUG
                    SYS_CONSOLE_DEBUG1("\n\nImage version is not found\n\n");
#endif
                    return SYS_STATUS_ERROR;
                }
                if (GetFieldValue(imageDB, OTA_IMAGE_STATUS, selected_row, &img->status) != 0) {
#ifdef OTA_DEBUG
                    SYS_CONSOLE_DEBUG1("Image status field not read properly\n");
#endif
                    return SYS_STATUS_ERROR;
                }
                if (GetFieldValue_32Bit(imageDB, OTA_IMAGE_VERSION, selected_row, &img->version) != 0) {
#ifdef OTA_DEBUG
                    SYS_CONSOLE_DEBUG1("Image version field not read properly\n");
#endif
                    return SYS_STATUS_ERROR;
                }

                /*If highest version is already present in internal flash, no need to copy it again*/
                if (APP_IMG_BOOT_CTL->status == IMG_STATUS_VALID) {
                    if (img->version == APP_IMG_BOOT_CTL->version
                            && img->status == IMG_STATUS_VALID) {
                        if (img->status == IMG_STATUS_VALID) {
                            csv_destroy_buffer(imageDB);
                            return BOOTLOADER_STATUS_SUCCESS;
                        }
                    }
                }
                bootloader.task.state = TASK_STATE_C_DONE;
            }
            break;
        }
        case TASK_STATE_C_DONE:
        {

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

// *****************************************************************************
// *****************************************************************************
// Section: convert string digest into hex format
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  SYS_STATUS formulate_digest(void)

  Description:
    convert string digest into hex format

  Task Parameters:
    None
  Return:
    None
 */
//---------------------------------------------------------------------------

static void formulate_digest(void) {
    int i;
    int index = 0;
	
	#ifdef FACTORY_IMAGE_VERIFICATION_ENABLED
    if(factory_reset == true){
        for (i = 0; i < 32; i++) {
            digest_g[i] = digest_str[i];
        }
    }else{
        for (i = 0; i < 32; i++) {
            strncpy(digest_gl, &digest_str[index], 2);
            index = index + 2;
            digest_g[i] = (uint8_t) strtol(digest_gl, NULL, 16);
        }
    }
	#else
	 for (i = 0; i < 32; i++) {
            strncpy(digest_gl, &digest_str[index], 2);
            index = index + 2;
            digest_g[i] = (uint8_t) strtol(digest_gl, NULL, 16);
        }
	#endif
#ifdef OTA_DEBUG
        SYS_CONSOLE_DEBUG1("formulated digest[%d]: %x\n", i, digest_g[i]);
#endif
    
}

// *****************************************************************************
// *****************************************************************************
// Section: Select the higest ranked image from the Image Store
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_SelectImage(void)

  Description:
    Select the higest ranked image from the Image Store

  Task Parameters:
    None

  Return:
    BOOTLOADER_STATUS_ERROR_NO_IMAGE_FOUND if no image found,
    BOOTLOADER_STATUS_SUCCESS otherwise.
 */
//---------------------------------------------------------------------------

typedef struct {
    uint8_t *buf;
    uint32_t slot;
    uint32_t selected;
    uint32_t version;
} BOOTLOADER_SELECT_IMAGE_TASK_CONTEXT;

typedef enum {
    TASK_STATE_S_INIT = 0,
    TASK_STATE_S_PARSE_DB,
    TASK_STATE_S_CHECK_IMAGE,
    TASK_STATE_S_SELECT_IMAGE,
    TASK_STATE_S_DONE
} BOOTLOADER_SELECT_IMAGE_TASK_STATE;
BOOTLOADER_TASK_PARAM *param = &bootloader.task.param;

static BOOTLOADER_STATUS Bootloader_Task_SelectImage(void) {
    BOOTLOADER_SELECT_IMAGE_TASK_CONTEXT *ctx = (void*) bootloader.task.context;
    if(factory_reset == true)
    {
        /*open factory image file*/
        if (bootloader.file_sys.file_opened == false) {
            
#ifdef FACTORY_IMAGE_VERIFICATION_ENABLED
#ifdef SYS_OTA_SECURE_BOOT_ENABLED                    
                    
                    char image_signature_file[100];
                    strcpy(image_signature_file, APP_MOUNT_NAME"/"APP_FACTORY_IMAGE_SIGNATURE_FILE_NAME);
                    appFile.fileHandle = SYS_FS_FileOpen(image_signature_file, (SYS_FS_FILE_OPEN_READ_PLUS));
                    SYS_FS_FileRead(appFile.fileHandle, (void *) signature_str, 100);
                    SYS_FS_FileClose(appFile.fileHandle);

#ifdef OTA_DEBUG
                    printf("Signature file name : %s\n\r",image_signature_file);
                    printf("Signature : %s\n\r",signature_str);
                    SYS_CONSOLE_DEBUG1("image digest signature : %s\n", signature_str);
#endif
                    
#endif
#endif            
            
			#ifdef FACTORY_IMAGE_VERIFICATION_ENABLED
            /*Read digest here*/
            appFile.fileHandle = SYS_FS_FileOpen(APP_MOUNT_NAME"/"APP_FACTORY_IMAGE_DIGEST_FILE_NAME, (SYS_FS_FILE_OPEN_READ));
            SYS_FS_FileRead(appFile.fileHandle, (void *) digest_str, 32);
            SYS_FS_FileClose(appFile.fileHandle);
#ifdef OTA_DEBUG             
            int i;
            for (i = 0; i < 32; i++)
                printf("Bootloader_Task_SelectImage : digest read : %x\n",digest_str[i]);
#endif
			#endif	
            /*Set the flag to  open file in read mode*/
            bootloader.file_sys.reading_file = true;
            open_file();
            return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
        } else {
            bootloader.file_sys.reading_file = false;
        }
        
        #ifdef OTA_DEBUG
            printf("file is opened successfully");
        #endif
        SYS_FS_FileDirectoryRemove(APP_OTA_DATABASE_PATH);      
    }
    switch (bootloader.task.state) {
        case TASK_STATE_S_INIT:
        {
#ifdef OTA_DEBUG
            SYS_CONSOLE_DEBUG1("TASK_STATE_S_INIT\n");
#endif
            bootloader.task.state = TASK_STATE_S_PARSE_DB;
            ctx->buf = bootloader.buf;
            break;
        }
        case TASK_STATE_S_PARSE_DB:
        {
            if (factory_reset == false) {
                /*Get the row of the best image in the DB*/
                uint8_t total_images = GetTotalImgs(imageDB);
#ifdef OTA_DEBUG
                SYS_CONSOLE_DEBUG1("total_images : %d\n", total_images);
#endif
                selected_row = -1;
                /*set version variables to zero initially, and go through the image database to 
                 get the latest version of image */
                uint32_t ver = 0;
                param->img.version = 0;

                uint8_t i;
                /*gothrough the image DB to get the latest and best image version */
                for (i = 0; i < total_images; i++) {
                    if (GetFieldValue_32Bit(imageDB, OTA_IMAGE_VERSION, i, &ver) != 0) {
#ifdef OTA_DEBUG
                        SYS_CONSOLE_DEBUG1("Image version field not read properly\n");
#endif
                        /*if database file is present , but empty  */
                        if(total_images == 1)
                            factory_reset = true;
                        return SYS_STATUS_ERROR;
                    }
                    if (param->img.version < ver) {
                        if (GetFieldValue(imageDB, OTA_IMAGE_STATUS, i, &param->img.status) != 0) {
#ifdef OTA_DEBUG
                            SYS_CONSOLE_DEBUG1("Image status field not read properly\n");
#endif
                            return SYS_STATUS_ERROR;
                        }
#ifdef OTA_DEBUG
                        SYS_CONSOLE_DEBUG1("Image status : %d\n\r",param->img.status);
#endif
                        /*Check if the status of image found is valid , if yes select the row*/
                        if (param->img.status == IMG_STATUS_DOWNLOADED) {
                            selected_row = i;
                            param->img.version = ver;
                        }
                        if (param->img.status == IMG_STATUS_VALID) {
#ifdef OTA_DEBUG
                            printf("Valid image\n");
#endif
                            selected_row = i;
                            param->img.version = ver;
                        }
                    }
                }
                if (selected_row == -1) {
#ifdef OTA_DEBUG
                    SYS_CONSOLE_DEBUG1("\n\nProper image is not found in external flash :: move to factory reset image\n\n");
#endif
                    
                    /*Proper image is not found in external flash :: move to factory reset image, o go on*/
                    bootloader.task.state = TASK_STATE_S_PARSE_DB;
                    factory_reset = true;
                    break;
                } else {

                    /*Get the name of the selected image*/
                    char image_name[100];
                    char image_path[100];
                    strcpy(image_path, APP_MOUNT_NAME"/ota/");
                    if (GetFieldString(imageDB, OTA_IMAGE_NAME, selected_row, image_name) != 0) {
#ifdef OTA_DEBUG
                        printf("Image name field not read properly\n");
#endif
                        return SYS_STATUS_ERROR;
                    }
                    strcat(image_path, image_name);
                    if (GetFieldString(imageDB, OTA_IMAGE_DIGEST, selected_row, digest_str) != 0) {
#ifdef OTA_DEBUG
                        SYS_CONSOLE_DEBUG1("Image Digest field not read properly\n");
#endif
                        return SYS_STATUS_ERROR;
                    }
#ifdef SYS_OTA_SECURE_BOOT_ENABLED                    
                    char image_signature_file[100];
                    strcpy(image_signature_file, image_path);
                    strcpy((strrchr(image_signature_file, '.') ), "_sign.txt");
                    appFile.fileHandle = SYS_FS_FileOpen(image_signature_file, (SYS_FS_FILE_OPEN_READ_PLUS));
                    SYS_FS_FileRead(appFile.fileHandle, (void *) signature_str, 100);
                    SYS_FS_FileClose(appFile.fileHandle);
                    
#ifdef OTA_DEBUG
                    printf("Signature file name : %s\n\r",image_signature_file);
                    printf("Signature : %s\n\r",signature_str);
#endif
                    

#ifdef OTA_DEBUG
                    SYS_CONSOLE_DEBUG1("image digest signature : %s\n", signature_str);
#endif
                    
#endif
#ifdef OTA_DEBUG
                    SYS_CONSOLE_DEBUG1("image digest : %s\n", digest_str);
                    /*************/

                    SYS_CONSOLE_DEBUG1("image name : %s", image_name);
                    SYS_CONSOLE_DEBUG1(" image_path: %s", image_path);
#endif
                    /*open image file*/
                    appFile.fileHandle = SYS_FS_FileOpen(image_path, (SYS_FS_FILE_OPEN_READ_PLUS));
                    if (appFile.fileHandle == SYS_FS_HANDLE_INVALID) {
#ifdef OTA_DEBUG
                        /* File open unsuccessful */
                        SYS_CONSOLE_DEBUG1("File Open Failed");
#endif
                        /*select factory reset image*/
                        bootloader.task.state = TASK_STATE_S_PARSE_DB;
                        factory_reset = true;
                        break;
                    } else {
                        /* File open was successful. */
#ifdef OTA_DEBUG
                        SYS_CONSOLE_DEBUG1("Image File Opened");
#endif
                        new_image = true;
                    }
                    bootloader.task.state = TASK_STATE_S_CHECK_IMAGE;
                    break;
                }
            }

#ifdef OTA_DEBUG
            FIRMWARE_IMAGE_HEADER *img;
            img = (FIRMWARE_IMAGE_HEADER *) ctx->buf;
            printf("img->status : %d,img->version : %d\n", img->status, img->version);
#endif
            SYS_FS_FileSeek(appFile.fileHandle, 0, SYS_FS_SEEK_SET);

            if (SYS_FS_FileEOF(appFile.fileHandle) != false) {
#ifdef OTA_DEBUG
                printf("Factory image is Empty\n");
#endif
                return SYS_STATUS_ERROR;
            }
            SYS_FS_FileRead(appFile.fileHandle, (void *) ctx->buf, FLASH_SECTOR_SIZE);
#ifdef OTA_DEBUG            
            Bootloader_TraceHeader((void*) ctx->buf);
#endif
            bootloader.task.state = TASK_STATE_S_CHECK_IMAGE;
            break;
        }
        case TASK_STATE_S_CHECK_IMAGE:
        {
#ifdef OTA_DEBUG 
            printf("TASK_STATE_S_CHECK_IMAGE\n");
#endif
            if (factory_reset == true) {
                memcpy(&param->img, ctx->buf, sizeof (FIRMWARE_IMAGE_HEADER));
#ifdef OTA_DEBUG 
                Bootloader_TraceHeader((void*) ctx->buf);
#endif
                if (param->img.status == IMG_STATUS_VALID) {
                    printf("Valid image\n");
                    bootloader.task.state = TASK_STATE_S_DONE;
                    break;
                }
                return BOOTLOADER_STATUS_ERROR;
            } else {
                #ifdef OTA_DEBUG
                SYS_CONSOLE_DEBUG1("Selected row : %d\n",selected_row);
                if (GetFieldString(imageDB, OTA_IMAGE_DIGEST, selected_row, digest_str) != 0) {
                    SYS_CONSOLE_DEBUG1("Image status field not read properly\n");
                    while(1);
                    return SYS_STATUS_ERROR;
                }
                
                SYS_CONSOLE_DEBUG1("image digest : %s\n digest_str : %s\n", param->img.digest, digest_str);
                #endif
                bootloader.task.state = TASK_STATE_S_DONE;
            }
            break;
        }

        case TASK_STATE_S_DONE:
        {

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

// *****************************************************************************
// *****************************************************************************
// Section: Copy selected image to PFM.
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_ProgramImage(void)

  Description:
    Copy selected image to PFM.
    
  Task Parameters:
    None

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

typedef enum {
    TASK_STATE_P_INIT = 0,
    TASK_STATE_P_ERASE_SLOT,
    TASK_STATE_P_READ_IMAGE,
    TASK_STATE_P_PROGRAM_IMAGE,
    TASK_STATE_P_FORMULATE_BOOT_CTL,
    TASK_STATE_P_PROGRAM_BOOT_CTL,
    TASK_STATE_P_DONE
} BOOTLOADER_PROGRAM_IMAGE_TASK_STATE;
char boot_ctl[FLASH_SECTOR_SIZE];

static BOOTLOADER_STATUS Bootloader_Task_ProgramImage(void) {
    BOOTLOADER_PROGRAM_IMAGE_TASK_CONTEXT *ctx = (void*) bootloader.task.context;
    BOOTLOADER_TASK_PARAM *param = &bootloader.task.param;

    switch (bootloader.task.state) {
        case TASK_STATE_P_INIT:
        {
#ifdef OTA_DEBUG
            printf("TASK_STATE_P_INIT\n");
#endif
            ctx->buf = bootloader.buf;
            ctx->len = FLASH_SECTOR_SIZE;
            ctx->offset = FLASH_SECTOR_SIZE;
#ifdef FACTORY_IMAGE_VERIFICATION_ENABLED
            if (new_image == true) {
                /*Get the image file size*/
                ctx->copy_len = SYS_FS_FileSize(appFile.fileHandle);
#ifdef OTA_DEBUG
                printf("ctx->copy_len : %d\n", ctx->copy_len);
#endif
                param->img.sz = ctx->copy_len;
            } else{
                ctx->copy_len = SYS_FS_FileSize(appFile.fileHandle);//(FACTORY_RESET_IMG_SIZE + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE;
            	param->img.sz = ctx->copy_len;
			}
			
#else
             if (new_image == true) {
                /*Get the image file size*/
                ctx->copy_len = SYS_FS_FileSize(appFile.fileHandle);
#ifdef OTA_DEBUG
                printf("ctx->copy_len : %d\n", ctx->copy_len);
#endif
                param->img.sz = ctx->copy_len;
            } else
                ctx->copy_len = (FACTORY_RESET_IMG_SIZE + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE;
#endif
            /*header area will be used during image verification , copy it in buffer "boot_ctl" */
            SYS_FS_FileSeek(appFile.fileHandle, 0, SYS_FS_SEEK_SET);
            SYS_FS_FileRead(appFile.fileHandle, ctx->buf, ctx->len);
            memcpy(boot_ctl, ctx->buf, ctx->len);

#ifdef OTA_DEBUG
            int i;
            for (i = 0; i < 32; i++) {
                SYS_CONSOLE_DEBUG1("TASK_STATE_P_READ_IMAGE\n");
                SYS_CONSOLE_DEBUG1("boot_ctl[%d] : %d\n", i, boot_ctl[i]);
            }
#endif

            bootloader.task.state = TASK_STATE_P_ERASE_SLOT;
            break;
        }
        case TASK_STATE_P_ERASE_SLOT:
        {
#ifdef OTA_DEBUG
            SYS_CONSOLE_DEBUG1("TASK_STATE_P_ERASE_SLOT\n");
#endif
            if (INT_Flash_Erase(APP_IMG_SLOT_ADDR, FACTORY_RESET_IMG_SIZE) == false) {
                ctx->buf = NULL;
                return BOOTLOADER_STATUS_ERROR;
            }
            bootloader.task.state = TASK_STATE_P_READ_IMAGE;
            break;
        }
        case TASK_STATE_P_READ_IMAGE:
        {
            if (INT_Flash_Busy()) {
                break;
            }
#ifdef OTA_DEBUG
            SYS_CONSOLE_DEBUG1("TASK_STATE_P_READ_IMAGE\n");
#endif

            SYS_FS_FileSeek(appFile.fileHandle, ctx->offset, SYS_FS_SEEK_SET);
            SYS_FS_FileRead(appFile.fileHandle, ctx->buf, ctx->len);
            bootloader.task.state = TASK_STATE_P_PROGRAM_IMAGE;
            break;
        }
        case TASK_STATE_P_PROGRAM_IMAGE:
        {
            if (INT_Flash_Busy()) {
                break;
            }
#ifdef OTA_DEBUG
            SYS_CONSOLE_DEBUG1("TASK_STATE_P_PROGRAM_IMAGE\n");
#endif
            INT_Flash_Write(APP_IMG_SLOT_ADDR + ctx->offset, ctx->buf, FLASH_SECTOR_SIZE);
            ctx->offset += ctx->len;
            if (ctx->offset >= ctx->copy_len) {
                bootloader.task.state = TASK_STATE_P_FORMULATE_BOOT_CTL;
            } else {
                bootloader.task.state = TASK_STATE_P_READ_IMAGE;
            }
            break;
        }
        case TASK_STATE_P_FORMULATE_BOOT_CTL:
        {
            if (INT_Flash_Busy()) {
                break;
            }
#ifdef OTA_DEBUG
            SYS_CONSOLE_DEBUG1("TASK_STATE_P_READ_HEADER\n");
#endif
            if (new_image == true) {
                memset(ctx->buf, 0, sizeof (FIRMWARE_IMAGE_HEADER));
                param->img.type = IMG_TYPE_PRODUCTION;
                param->img.boot_addr = APP_IMG_BOOT_ADDR;
                memcpy(ctx->buf, &param->img, sizeof (FIRMWARE_IMAGE_HEADER));
                bootloader.task.state = TASK_STATE_P_PROGRAM_BOOT_CTL;
                break;
            }

            /*If factory reset image, extract the header information from factory reset image file*/
            SYS_FS_FileSeek(appFile.fileHandle, 0, SYS_FS_SEEK_SET);
            SYS_FS_FileRead(appFile.fileHandle, ctx->buf, FLASH_SECTOR_SIZE);
#ifdef OTA_DEBUG
            Bootloader_TraceHeader((void*) ctx->buf);
#endif
            bootloader.task.state = TASK_STATE_P_PROGRAM_BOOT_CTL;
            break;
        }
        case TASK_STATE_P_PROGRAM_BOOT_CTL:
        {
#ifdef OTA_DEBUG
            printf("TASK_STATE_P_PROGRAM_BOOT_CTL\n");
#endif
            if (INT_Flash_Busy()) {
                break;
            }
#ifdef OTA_DEBUG            
            Bootloader_TraceHeader((void*) ctx->buf);
#endif
            if (INT_Flash_Write(APP_IMG_SLOT_ADDR, ctx->buf, FLASH_SECTOR_SIZE) == false) {
                ctx->buf = NULL;
                return BOOTLOADER_STATUS_ERROR;
            }
            bootloader.task.state = TASK_STATE_P_DONE;
            break;
        }
        case TASK_STATE_P_DONE:
        {
            if (INT_Flash_Busy()) {
                break;
            }
#ifdef OTA_DEBUG
            printf("TASK_STATE_P_DONE\n");
#endif
            /*Close factory reset file*/
            if(factory_reset == true)
                SYS_FS_FileClose(appFile.fileHandle);
            
            ctx->buf = NULL;
            return BOOTLOADER_STATUS_SUCCESS;
        }
        default:
        {
            return BOOTLOADER_STATUS_ERROR;
        }
    }
    return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
}

// *****************************************************************************
// *****************************************************************************
// Section: Verify the copied image in INT Flash.This example code uses SHA256 for image validation.
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  static BOOTLOADER_STATUS Bootloader_Task_VerifyImageDigest(void)

  Description:
    Verify the copied image in INT Flash..
    This example code uses SHA256 for image validation.

  Task Parameters:
    None

  Return:
    A BOOTLOADER_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

typedef struct {
    CRYPT_SHA256_CTX sha256; // This has to be 128 bytes aligned !!
    uint8_t *buf;
    uint32_t offset;
    uint32_t len;
    uint32_t img_sz;
} BOOTLOADER_VERIFY_IMAGE_TASK_CONTEXT;

typedef enum {
    TASK_STATE_V_INIT = 0,
    TASK_STATE_V_READ_IMG,
    TASK_STATE_V_DONE,
} BOOTLOADER_V_IMAGE_TASK_STATE;

static BOOTLOADER_STATUS Bootloader_Task_VerifyImageDigest(void) {
    BOOTLOADER_VERIFY_IMAGE_TASK_CONTEXT *ctx = (void*) &bootloader.task.context;
    BOOTLOADER_TASK_PARAM *param = &bootloader.task.param;

    switch (bootloader.task.state) {
        case TASK_STATE_V_INIT:
        {
            /*If factory reset image no verification required*/
			#ifndef FACTORY_IMAGE_VERIFICATION_ENABLED
            if (factory_reset == true)
                return BOOTLOADER_STATUS_SUCCESS;
			#endif
            ctx->offset = 0;
            ctx->len = FLASH_SECTOR_SIZE;
            ctx->buf = bootloader.buf;
            ctx->img_sz = param->img.sz;
#ifdef OTA_DEBUG
            SYS_CONSOLE_DEBUG1("TASK_STATE_V_INIT : ctx->img_sz : %d\n", ctx->img_sz);
#endif
            CRYPT_SHA256_Initialize(&ctx->sha256);
            CRYPT_SHA256_DataSizeSet(&ctx->sha256, param->img.sz);
            INT_Flash_Read(APP_IMG_SLOT_ADDR, ctx->buf, ctx->len);
#ifdef OTA_DEBUG
            int i;
            for (i = 0; i < 32; i++) {
                SYS_CONSOLE_DEBUG1("TASK_STATE_V_INIT\n");
                SYS_CONSOLE_DEBUG1("ctx->buf[%d] : %d\n", i, ctx->buf[i]);
            }
#endif
            bootloader.task.state = TASK_STATE_V_READ_IMG;
            formulate_digest();
#ifdef SYS_OTA_SECURE_BOOT_ENABLED
			#ifdef FACTORY_IMAGE_VERIFICATION_ENABLED
            //if(factory_reset == false){
			#endif
#ifdef OTA_DEBUG
            printf("sizeof signature : %d\n\r",strlen(signature_str));
            printf("signature : %s\n\r",signature_str);
#endif
            bool ret = Bootloader_Hash_Signature_Verify(signature_str);
            
            if(ret == false)
            {
                printf("Broken Image : Signature verification failed\n\r");
                SYS_FS_FileClose(appFile.fileHandle);
                return BOOTLOADER_STATUS_ERROR;
            }
            else
            {
                printf("Signature verification passed\n\r");
                
            }
			#ifdef FACTORY_IMAGE_VERIFICATION_ENABLED
            //}
			#endif
#endif
            break;
        }
        case TASK_STATE_V_READ_IMG:
        {
            
            if (INT_Flash_Busy()) {
                break;
            }
            if (ctx->offset == 0) {
                memset(ctx->buf, 0, sizeof (FIRMWARE_IMAGE_HEADER));
#ifdef OTA_DEBUG
                FIRMWARE_IMAGE_HEADER * img = (FIRMWARE_IMAGE_HEADER *) ctx->buf;
                img->status = 0xFD;
                img->version = 0x01; //param->img.version;
                img->sz = 0xFF; //param->img.sz;
                img->type = 0x03;
                img->boot_addr = APP_IMG_BOOT_ADDR;
                ctx->buf[FIRMWARE_IMAGE_HEADER_SIGNATURE_BYTE] = 0xFF;
#endif
                memcpy(ctx->buf, boot_ctl, FLASH_SECTOR_SIZE);
            }
            CRYPT_SHA256_DataAdd(&ctx->sha256, ctx->buf, ctx->len);
            ctx->offset += ctx->len;
            if (ctx->offset < ctx->img_sz) {
                if ((ctx->offset + ctx->len) > ctx->img_sz) {
                    ctx->len = ctx->img_sz - ctx->offset;
                }
                INT_Flash_Read(APP_IMG_SLOT_ADDR + ctx->offset, ctx->buf, ctx->len);
            } else {
                bootloader.task.state = TASK_STATE_V_DONE;
            }
            break;
        }
        case TASK_STATE_V_DONE:
        {
            uint8_t digest[CRYPT_SHA256_DIGEST_SIZE];
            if (INT_Flash_Busy()) {
                break;
            }
            CRYPT_SHA256_Finalize(&ctx->sha256, digest);
            int i;
            for (i = 0; i < 32; i++) {
                param->img.digest[i] = digest_g[i];
            }
#ifdef OTA_DEBUG
            for (i = 0; i < 32; i++)
                printf("digest : %d \n param->img.digest : %d\n", digest[i], param->img.digest[i]);
#endif
            if (memcmp(digest, param->img.digest, CRYPT_SHA256_DIGEST_SIZE) != 0) {
                printf("Broken Image\n");
                return BOOTLOADER_STATUS_ERROR;
            }
            return BOOTLOADER_STATUS_SUCCESS;
        }
        default:
        {
            return BOOTLOADER_STATUS_ERROR;
        }
    }

    return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
}
// *****************************************************************************
// *****************************************************************************
// Section: Mark INT flash image to UNBOOTED.
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_SetImageStatus(void)

  Description:
    Mark INT flash image to UNBOOTED.
    
  Task Parameters:
    None

  Return:
    A BOOTLOADER_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

typedef struct {
    uint8_t *buf;
} BOOTLOADER_SIGN_IMAGE_TASK_CONTEXT;

typedef enum {
    TASK_STATE_SI_INIT = 0,
    TASK_STATE_SI_SET_STATUS,
    TASK_STATE_SI_DONE
} BOOTLOADER_SIGN_IMAGE_TASK_STATE;

static BOOTLOADER_STATUS Bootloader_Task_SetImageStatus(void) {
    BOOTLOADER_SIGN_IMAGE_TASK_CONTEXT *ctx = (void*) bootloader.task.context;
    BOOTLOADER_TASK_PARAM *param = &bootloader.task.param;

    switch (bootloader.task.state) {
        case TASK_STATE_SI_INIT:
        {
            ctx->buf = bootloader.buf;
            INT_Flash_Read(APP_IMG_SLOT_ADDR, ctx->buf, FLASH_SECTOR_SIZE);
            bootloader.task.state = TASK_STATE_SI_SET_STATUS;
            break;
        }
        case TASK_STATE_SI_SET_STATUS:
        {
            if (INT_Flash_Busy()) {
                break;
            }
            param->img.status &= IMG_STATUS_UNBOOTED;
            memcpy(ctx->buf, &param->img, sizeof (FIRMWARE_IMAGE_HEADER));
#ifdef OTA_DEBUG
            ctx->buf[FIRMWARE_IMAGE_HEADER_SIGNATURE_BYTE] = param->img.status;
#endif
            INT_Flash_Write(APP_IMG_SLOT_ADDR, ctx->buf, FLASH_SECTOR_SIZE);
            bootloader.task.state = TASK_STATE_SI_DONE;
            break;
        }
        case TASK_STATE_SI_DONE:
        {
            if (INT_Flash_Busy()) {
                break;
            }
            return BOOTLOADER_STATUS_SUCCESS;
        }
        default:
        {
            return BOOTLOADER_STATUS_ERROR;
        }
    }

    return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
}

// *****************************************************************************
// *****************************************************************************
// Section: Invalidate image in Image Store to DISABLED.
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_InvalidateImage(void)

  Description:
    Invalidate image in Image Store to DISABLED.
 
  Task Parameters:
    None

  Return:
    A BOOTLOADER_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

typedef enum {
    TASK_STATE_I_INIT = 0,
    TASK_STATE_I_INVALIDATE,
    TASK_STATE_I_DONE
} BOOTLOADER_INVALIDATE_IMAGE_TASK_STATE;

static BOOTLOADER_STATUS Bootloader_Task_InvalidateImage(void) {
    BOOTLOADER_TASK_PARAM *param = &bootloader.task.param;
    switch (bootloader.task.state) {
        case TASK_STATE_I_INIT:
        {
            bootloader.task.state = TASK_STATE_I_INVALIDATE;
        }
        case TASK_STATE_I_INVALIDATE:
        {
            param->img.status = IMG_STATUS_DISABLED;
            char status[4];
            sprintf(status, "%x", param->img.status);
            SetFieldValue(imageDB, OTA_IMAGE_STATUS, selected_row, status);
            OTADbDeleteEntry(imageDB,selected_row);
            OTASaveDb(imageDB, APP_OTA_DATABASE_PATH);
            /*if (param->img.order == 0) {
                printf("The golden image is broken!!\n");
                bootloader.task.state = TASK_STATE_I_DONE;
                break;
            }*/
            bootloader.task.state = TASK_STATE_I_DONE;
            break;
        }
        case TASK_STATE_I_DONE:
        {
            return BOOTLOADER_STATUS_SUCCESS;
        }
    }
    return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
}

// *****************************************************************************
// *****************************************************************************
// Section:To jump to application code
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_JumpApplication(void)

  Description:
  To jump to application code

  Task Parameters:
    None
  Return:
    A BOOTLOADER_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

static BOOTLOADER_STATUS Bootloader_Task_JumpApplication(void) {
    void(*fptr)(void);
    EVIC_Deinitialize();
    fptr = (void(*)(void))(APP_IMG_BOOT_CTL->boot_addr);
    fptr();
    return BOOTLOADER_STATUS_SUCCESS;
}


// *****************************************************************************
// *****************************************************************************
// Section: Factory reset task to maintain state machine for factory reset image
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  BOOTLOADER_STATUS Bootloader_Task_FactoryReset(void)

  Description:
  Factory reset task to maintain state machine for factory reset image

  Task Parameters:
    None

  Return:
    A BOOTLOADER_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

typedef struct {
	#ifdef FACTORY_IMAGE_VERIFICATION_ENABLED
    CRYPT_SHA256_CTX sha256;
	#endif
    uint8_t *buf;
    uint32_t offset;
    uint32_t len;
    uint32_t copy_len;
    uint32_t dbg;
} BOOTLOADER_FACTORY_RESET_TASK_CONTEXT;

typedef enum {
    TASK_STATE_FR_INIT = 0,
    TASK_STATE_FR_READ_IMAGE,
    TASK_STATE_FR_PROGRAM_IMAGE,
    TASK_STATE_FR_ERASE_BOOT_CTL,
    TASK_STATE_FR_DONE,
} BOOTLOADER_FR_STATE;

static BOOTLOADER_STATUS Bootloader_Task_FactoryReset(void) {
    BOOTLOADER_FACTORY_RESET_TASK_CONTEXT *ctx = (void*) &bootloader.task.context;
	#ifdef FACTORY_IMAGE_VERIFICATION_ENABLED
    static uint8_t digest[CRYPT_SHA256_DIGEST_SIZE];
	#endif
    /*Open factory reset file*/
    if (bootloader.file_sys.file_opened == false) {
        open_file();
        return BOOTLOADER_STATUS_MORE_PROCESSING_REQUIRED;
    }
    switch (bootloader.task.state) {
        case TASK_STATE_FR_INIT:
        {
#ifdef OTA_DEBUG
            printf("In factory reset init\n");
#endif
            ctx->buf = bootloader.buf;
            ctx->copy_len = FACTORY_RESET_IMG_SIZE;
#ifdef OTA_DEBUG
            printf("ctx->copy_len : %d\n", ctx->copy_len);
#endif
            ctx->offset = 0;
            ctx->dbg = 0;
            bootloader.task.state = TASK_STATE_FR_READ_IMAGE;
			#ifdef FACTORY_IMAGE_VERIFICATION_ENABLED
            CRYPT_SHA256_Initialize(&ctx->sha256);
            CRYPT_SHA256_DataSizeSet(&ctx->sha256, FACTORY_RESET_IMG_SIZE);
			#endif
            break;
        }
        case TASK_STATE_FR_READ_IMAGE:
        {
            INT_Flash_Read(APP_IMG_SLOT_ADDR + ctx->offset, ctx->buf, FLASH_SECTOR_SIZE);
            bootloader.task.state = TASK_STATE_FR_PROGRAM_IMAGE;
            break;
        }
        case TASK_STATE_FR_PROGRAM_IMAGE:
        {
            if (INT_Flash_Busy()) {
                break;
            }
            if (ctx->offset == 0) {
                FIRMWARE_IMAGE_HEADER *img;
                img = (FIRMWARE_IMAGE_HEADER *) ctx->buf;
                img->type = IMG_TYPE_FACTORY_RESET;
                ctx->buf[FIRMWARE_IMAGE_HEADER_STATUS_BYTE] = 0xF8; //img->status;
            }
			#ifdef FACTORY_IMAGE_VERIFICATION_ENABLED
            CRYPT_SHA256_DataAdd(&ctx->sha256, ctx->buf, FLASH_SECTOR_SIZE);
			#endif
            SYS_FS_FileWrite(appFile.fileHandle, ctx->buf, FLASH_SECTOR_SIZE);
            bootloader.task.state = TASK_STATE_FR_READ_IMAGE;
            ctx->offset += FLASH_SECTOR_SIZE;
            if (ctx->offset >= ctx->copy_len) {
                bootloader.task.state = TASK_STATE_FR_ERASE_BOOT_CTL;
            }
            break;
        }
        case TASK_STATE_FR_ERASE_BOOT_CTL:
        {
#ifdef OTA_DEBUG
            printf("TASK_STATE_FR_ERASE_BOOT_CTL\n");
#endif
            #ifdef FACTORY_IMAGE_VERIFICATION_ENABLED
            CRYPT_SHA256_Finalize(&ctx->sha256, digest);
			#endif	
            /*Set the values for inernal flash header area and write it into flash*/
            FIRMWARE_IMAGE_HEADER *img;
            img = (FIRMWARE_IMAGE_HEADER *) ctx->buf;
            img->type = IMG_TYPE_FACTORY_RESET;
            img->status = 0xF8;
            INT_Flash_Write(APP_IMG_SLOT_ADDR, ctx->buf, FLASH_SECTOR_SIZE);

            bootloader.task.state = TASK_STATE_FR_DONE;
            break;
        }
        case TASK_STATE_FR_DONE:
        {
            if (INT_Flash_Busy()) {
                break;
            }

            /*Close factory reset image file*/
            if (SYS_FS_FileClose(appFile.fileHandle) != 0) {
#ifdef OTA_DEBUG
                printf("close error\n\r");
#endif
            } else {
#ifdef OTA_DEBUG
                printf("File closed\n");
#endif
            }
			#ifdef FACTORY_IMAGE_VERIFICATION_ENABLED
            char *digest_l;
            char digest_ll[32];
            int i;
#ifdef OTA_DEBUG            
            for (i = 0; i < 32; i++)
                printf("digest : %c \n", digest[i]);
#endif
            for (i = 0; i < 32; i++)
                digest_ll[i] = digest[i];
            digest_l = digest_ll;
#ifdef OTA_DEBUG
            for (i = 0; i < 32; i++)
                printf("digest_l : %x \n", digest_ll[i]);
#endif            
            appFile.fileHandle = SYS_FS_FileOpen(APP_MOUNT_NAME"/"APP_FACTORY_IMAGE_DIGEST_FILE_NAME, (SYS_FS_FILE_OPEN_WRITE_PLUS));
            SYS_FS_FileWrite(appFile.fileHandle, digest_l, CRYPT_SHA256_DIGEST_SIZE);
            SYS_FS_FileClose(appFile.fileHandle);
            
            char digest_buf[32];
            appFile.fileHandle = SYS_FS_FileOpen(APP_MOUNT_NAME"/"APP_FACTORY_IMAGE_DIGEST_FILE_NAME, (SYS_FS_FILE_OPEN_READ));
            SYS_FS_FileRead(appFile.fileHandle, (void *) digest_buf, 32);
            SYS_FS_FileClose(appFile.fileHandle);
           
#ifdef OTA_DEBUG            
            for (i = 0; i < 32; i++)
                printf("digest read : %x\n",digest_buf[i]);
            
            /*Following statement should be enabled to make above prints visible on console */
            /*If we dont enable following statement, system will reset automatically and above print will not be visible on console*/
            //while(1);
#endif      
			#endif
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
// *****************************************************************************
// *****************************************************************************
// Section: To maintain OTA bootloader task state machine
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  void Bootloader_Tasks(void)
 
  Description:
    To maintain OTA bootloader task state machine
  
  Task Parameters:
    None
 
  Return:
    None
 */
//---------------------------------------------------------------------------

void Bootloader_Tasks(void) {
    BOOTLOADER_TASK_ID next = bootloader.current_task;
    BOOTLOADER_STATUS status;

    switch (bootloader.current_task) {
        case BOOTLOADER_TASK_INIT:
        {
            
#ifdef OTA_DEBUG
            printf("Init state\r");
#endif
            /*Mounting file system disk*/
            mount_disk();
            /*Wait till mount disk is successful*/
            if (bootloader.file_sys.disk_mount == true) {
                INT_Flash_Open();
                next = BOOTLOADER_TASK_CHECK_IMAGE;
                Bootloader_TraceHeader((void*) APP_IMG_BOOT_CTL);
                {
#ifdef OTA_DEBUG
                    printf("Inside valid state\r");
#endif
                    switch (APP_IMG_BOOT_CTL->type) {
                        case IMG_TYPE_PRODUCTION:
                        {
                            next = BOOTLOADER_TASK_CHECK_IMAGE;
                            break;
                        }
                        case IMG_TYPE_DEBUG:
                        {
                            next = BOOTLOADER_TASK_JUMP_TO_APP;
                            break;
                        }
                        case IMG_TYPE_FACTORY_RESET:
                        {
                            /*Disabling factory reset backup should only be used for application debug, during development*/
                            /***User should define FACTORY_IMAGE_BACKUP_DISABLE explicitly to disable factory reset backup***/
                            /***USE ONLY FOR DEBUG***/
                            #ifdef FACTORY_IMAGE_BACKUP_DISABLE
                            format_disk();
                            next = BOOTLOADER_TASK_JUMP_TO_APP;
                            break;
                            #endif
							
                            appFile.state = APP_FORMAT_DISK;
                            next = BOOTLOADER_TASK_FACTORY_RESET;
                            if (APP_IMG_BOOT_CTL->status == IMG_STATUS_VALID) {
                                next = BOOTLOADER_TASK_JUMP_TO_APP;
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            break;
        }
        case BOOTLOADER_TASK_CHECK_IMAGE:
        {
#ifdef OTA_DEBUG 
            printf("new bootloader\n");
            printf("BOOTLOADER_TASK_CHECK_IMAGE\n");
#endif
            status = Bootloader_Task_CheckImage();
            if (status == BOOTLOADER_STATUS_SUCCESS) {
                next = BOOTLOADER_TASK_JUMP_TO_APP;
            }
            if (status == BOOTLOADER_STATUS_ERROR) {
                next = BOOTLOADER_TASK_SELECT_IMAGE;
            }
            break;
        }
        case BOOTLOADER_TASK_SELECT_IMAGE:
        {
            status = Bootloader_Task_SelectImage();
#ifdef OTA_DEBUG
            printf("Select image status: %d\n", status);
#endif
            if (status == BOOTLOADER_STATUS_SUCCESS) {
                next = BOOTLOADER_TASK_PROGRAM_IMAGE;
            }
            if (status == BOOTLOADER_STATUS_ERROR_NO_IMAGE_FOUND) {
                next = BOOTLOADER_TASK_NO_IMAGE_FOUND;
            }
            if (status == BOOTLOADER_STATUS_ERROR) {
                bootloader.task.state = BOOTLOADER_TASK_INIT;
            }
            break;
        }
        case BOOTLOADER_TASK_PROGRAM_IMAGE:
        {
            status = Bootloader_Task_ProgramImage();
            if (status == BOOTLOADER_STATUS_SUCCESS) {
#ifdef OTA_DEBUG                
                printf("BOOTLOADER_TASK_PROGRAM_IMAGE success\n");
#endif
                next = BOOTLOADER_TASK_VERIFY_IMAGE;
            }
            if (status == BOOTLOADER_STATUS_ERROR) {
#ifdef OTA_DEBUG
                printf("BOOTLOADER_TASK_PROGRAM_IMAGE Fail\n");
#endif
                next = BOOTLOADER_TASK_SELECT_IMAGE;
            }
            break;
        }
        case BOOTLOADER_TASK_VERIFY_IMAGE:
        {
#ifdef OTA_DEBUG
            printf("BOOTLOADER_TASK_VERIFY_IMAGE\n");
#endif
            status = Bootloader_Task_VerifyImageDigest();
            if (status == BOOTLOADER_STATUS_SUCCESS) {
#ifdef OTA_DEBUG
                printf("BOOTLOADER_TASK_VERIFY_IMAGE success\n");
#endif
                next = BOOTLOADER_TASK_SET_IMAGE_STATUS;
            }
            if (status == BOOTLOADER_STATUS_ERROR) {
#ifdef OTA_DEBUG
                printf("BOOTLOADER_TASK_VERIFY_IMAGE fail\n");
#endif
                next = BOOTLOADER_TASK_INVALIDATE_IMAGE;

            }
            break;
        }
        case BOOTLOADER_TASK_SET_IMAGE_STATUS:
        {
#ifdef OTA_DEBUG
            printf("BOOTLOADER_TASK_SET_IMAGE_STATUS\n");
#endif
            status = Bootloader_Task_SetImageStatus();
            if (status == BOOTLOADER_STATUS_SUCCESS) {
                next = BOOTLOADER_TASK_JUMP_TO_APP;
#ifdef OTA_DEBUG
                printf("BOOTLOADER_TASK_SET_IMAGE_STATUS success\n");
#endif
            }
            if (status == BOOTLOADER_STATUS_ERROR) {
                next = BOOTLOADER_TASK_INVALIDATE_IMAGE;

#ifdef OTA_DEBUG
                printf("BOOTLOADER_TASK_SIGN_BOOT_CTL fail\n");
#endif
            }
            break;
        }
        case BOOTLOADER_TASK_INVALIDATE_IMAGE:
        {
            status = Bootloader_Task_InvalidateImage();
            if (status == BOOTLOADER_STATUS_SUCCESS) {
                next = BOOTLOADER_TASK_SELECT_IMAGE;
            }
            
            break;
        }
        case BOOTLOADER_TASK_JUMP_TO_APP:
        {
            Bootloader_Task_JumpApplication();
            break;
        }
        case BOOTLOADER_TASK_NO_IMAGE_FOUND:
        {
            SYS_ASSERT(false, "Unexpected state");
            break;
        }
        case BOOTLOADER_TASK_FACTORY_RESET:
        {
            Bootloader_Task_FactoryReset();
            break;
        }
        default:
        {
            break;
        }
    }

    if (next != bootloader.current_task) {
        {
            static const char * s[] __attribute__((unused)) = {
                "BOOTLOADER_TASK_INIT",
                "BOOTLOADER_TASK_CHECK_IMAGE",
                "BOOTLOADER_TASK_SELECT_IMAGE",
                "BOOTLOADER_TASK_PROGRAM_IMAGE",
                "BOOTLOADER_TASK_VERIFY_IMAGE",
                "BOOTLOADER_TASK_SIGN_BOOT_CTL",
                "BOOTLOADER_TASK_INVALIDATE_IMAGE",
                "BOOTLOADER_TASK_NO_IMAGE_FOUND",
                "BOOTLOADER_TASK_JUMP_TO_APP",
                "BOOTLOADER_TASK_FACTORY_RESET"
            };

            printf("%s\n", s[next]);

            if (bootloader.current_task != BOOTLOADER_TASK_INIT
                    && next == BOOTLOADER_TASK_JUMP_TO_APP) {
                Bootloader_TraceHeader((void*) APP_IMG_BOOT_CTL);
                printf("***********************************************************\n");
            }
        }
        bootloader.current_task = next;
        bootloader.task.state = 0;
    }
}
