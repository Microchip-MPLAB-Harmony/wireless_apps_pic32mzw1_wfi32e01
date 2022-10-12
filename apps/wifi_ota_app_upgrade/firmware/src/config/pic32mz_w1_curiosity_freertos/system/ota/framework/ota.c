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
#include "system/ota/framework/csv/csv.h"

#ifdef SYS_OTA_PATCH_ENABLE
#include "ota_patch.h"
#endif

#define OTA_DEBUG   1
#define OTA_MAIN_CODE   2

#ifdef SYS_OTA_APPDEBUG_ENABLED
#define SERVICE_TYPE    OTA_DEBUG  
#else
#define SERVICE_TYPE    OTA_MAIN_CODE  
#endif



#define BOOT_ADDRESS    0xB0020000 + 0x00001000
#define APP_IMG_BOOT_CTL_BLANK      { 0xFF, 0xFF, 0xFF, 0x03, 0xFFFFFFFF,  0x00000001   ,  BOOT_ADDRESS  }
#define OTA_DOWNLOADER_TIMEOUT 1000
#define __woraround_unused_variable(x) ((void)x)
#define APP_MOUNT_NAME          "/mnt/myDrive1"
#define APP_DIR_NAME            "/mnt/myDrive1/ota"
#define OTA_DB_NAME             "image_database.csv"
#define APP_DEVICE_NAME         "/dev/mtda1"
#define APP_FS_TYPE             FAT
#define FACTORY_IMAGE           "factory_reset.bin"


#ifdef SYS_OTA_FREE_SECTOR_CHECK_ENABLE
#define OTA_CHECK_FREE_SECTOR
#endif

typedef struct {
    FIRMWARE_IMAGE_HEADER header;
    uint8_t rsvd[4096 - sizeof (FIRMWARE_IMAGE_HEADER) - 1];
    uint8_t signature;
} OTA_BOOT_CONTROL;


const OTA_BOOT_CONTROL BOOT_CONTROL_INSTANCE\
 __attribute__((section(".ota_boot_control"))) = {\
    APP_IMG_BOOT_CTL_BLANK,
    {}, IMG_STATUS_VALID
};

typedef enum {
    OTA_TASK_INIT = 0,
    OTA_TASK_IDLE,
    OTA_TASK_ALLOCATE_SLOT,
    OTA_TASK_CHECK_DB,
#ifdef SYS_OTA_PATCH_ENABLE            
    OTA_TASK_SEARCH_PATCH_BASEVERSION,
#endif
    OTA_TASK_DOWNLOAD_IMAGE,
    OTA_TASK_VERIFY_IMAGE_DIGEST,
#ifdef SYS_OTA_PATCH_ENABLE
    OTA_TASK_VERIFY_PATCH_IMAGE_DIGEST,
    OTA_TASK_PATCH_BUILD_IMAGE,
#endif
    OTA_TASK_DATABASE_ENTRY,
    OTA_TASK_SET_IMAGE_STATUS,
    OTA_TASK_FACTORY_RESET,
    OTA_TASK_ERASE_IMAGE,
    OTA_TASK_UPDATE_USER
} OTA_TASK_ID;

typedef struct {
    uint32_t slot;
    uint32_t version;
    uint8_t abort;
    uint8_t img_status;
    uint8_t pfm_status;
} OTA_TASK_PARAM;

typedef struct {

    struct {
        uint8_t context[256 + 64 + 1024];
        int state;
        OTA_TASK_PARAM param;
    } task;

    OTA_TASK_ID current_task;
    SYS_STATUS status;
    OTA_RESULT ota_result;
    OTA_COMPLETION_CALLBACK callback;
    DRV_HANDLE downloader;
    OSAL_MUTEX_HANDLE_TYPE mutex;
    bool new_downloaded_img;
    bool ota_rollback_initiated;
    bool ota_idle;
    bool db_full;
} OTA_DATA;

static OTA_DATA __attribute__((coherent, aligned(128))) ota;

static OTA_PARAMS ota_params;
static OTA_DB_BUFFER *imageDB;
extern size_t field_content_length;
static char image_name[100];
#ifdef SYS_OTA_SECURE_BOOT_ENABLED
static char image_signature_file_name[100];
#endif
static bool disk_mount = false;
#ifdef SYS_OTA_PATCH_ENABLE
static bool patch_verification = false;
static uint8_t serv_app_patch_digest_g[32];
static char serv_app_patch_digest_gl[4];
#endif
static uint8_t serv_app_digest_g[32];
static char serv_app_digest_gl[4];
static int8_t selected_row;
static uint32_t offset;
static SYS_FS_HANDLE dirHandle;
static SYS_FS_FSTAT stat;
static uint32_t erase_ver;
static bool ota_isTls_request;
CACHE_COHERENCY ota_original_cache_policy;
#if (OTA_NVM_INT_CALLBACK_ENABLE == false)
extern bool mutex_nvm_g_status;
#endif

typedef enum {
    /* The app mounts the disk */
    APP_MOUNT_DISK = 0,

    /* The disk mount success */
    APP_MOUNT_SUCCESS,

    /* The app formats the disk. */
    APP_FORMAT_DISK,

    /* An app error has occurred */
    APP_ERROR

} APP_FILE_STATES;


uint8_t CACHE_ALIGN work[SYS_FS_FAT_MAX_SS];

typedef struct {
    /* SYS_FS File handle */
    SYS_FS_HANDLE fileHandle1;

    /*  current state */
    APP_FILE_STATES state;

    long fileSize;

} APP_DATA_FILE;
APP_DATA_FILE CACHE_ALIGN appFile;

#ifdef SYS_OTA_PATCH_ENABLE
OTA_PATCH_PARAMS_t patch_param;
#endif
// *****************************************************************************
// *****************************************************************************
// Section: To check if image download request is via TLS connection 
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  void OTA_IsTls_Request(const char *)

  Description:
    To check if image download request is via TLS connection 

  Task Parameters:
    Server URL
  Return:
    true - If TLS request
    false - If not a TLS connection
 */
//---------------------------------------------------------------------------
static bool OTA_IsTls_Request(const char *URIText){
    if (SYS_OTA_ENFORCE_TLS == false) {
        if (0 == strncmp(URIText, "https:", 6)) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : TLS request\r\n");
#endif
            return true;
        } else if (0 == strncmp(URIText, "http:", 5)) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : non-TLS request\r\n");
#endif
            return false;
        } else {
            return false;
        }
    } else {
        if (0 == strncmp(URIText, "https:", 6)) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : TLS request\r\n");
#endif
            return true;
        } else {
            return false;
        }
    }
}
// *****************************************************************************
// *****************************************************************************
// Section:  To change cache policy 
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  void OTA_SetCachePolicy(bool)

  Description:
   To set cache policy 

  Task Parameters:
   restore_policy : true : request is for restoring  policy   
 *                  false : request is not for restoring  policy. Policy will be changed to "CACHE_WRITETHROUGH_WRITEALLOCATE"
 *                          after taking backup of current policy 
  Return:
    None
 */
//---------------------------------------------------------------------------
void OTA_SetCachePolicy(bool restore_policy){
    
    OSAL_CRITSECT_DATA_TYPE critSect;
    /* Enter the critical section.*/
    critSect = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
    
    /*take a back up of current cache policy, this will be restored after download is completed*/
    /*Back up is to be taken only when moving from original to new. Restoring to original does not
     require back up*/
    if(restore_policy == false)
        ota_original_cache_policy = CACHE_CacheCoherencyGet();
    /*Flushing cache, after memory is synchronized */
    CACHE_CacheFlush();
    
    if(restore_policy == true){
        /*Set original cache policy after download*/
        CACHE_CacheCoherencySet(ota_original_cache_policy);
    }
    else{
        /*Set new cache policy*/
        CACHE_CacheCoherencySet(CACHE_WRITETHROUGH_WRITEALLOCATE);
    }
    
    /* Exit the critical section*/
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH, critSect);
}
// *****************************************************************************
// *****************************************************************************
// Section:  To get download status 
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  void OTA_GetDownloadStatus(OTA_PARAMS *result)

  Description:
   To get download status 

  Task Parameters:
    pointer of type ota_params 
  Return:
    None
 */
//---------------------------------------------------------------------------
void OTA_GetDownloadStatus(OTA_PARAMS *result) {
    result->server_image_length = ota_params.server_image_length;
    result->total_data_downloaded = ota_params.total_data_downloaded;
}

// *****************************************************************************
// *****************************************************************************
// Section:  To store factory image signature 
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  void OTA_StoreFactoryImageSignature(void *buf)

  Description:
   To store factory image signature

  Task Parameters:
    pointer to signature string 
  Return:
    None
 */
//---------------------------------------------------------------------------
#ifdef SYS_OTA_SECURE_BOOT_ENABLED
void OTA_StoreFactoryImageSignature(void *buf) {
    OTA_DB_ENTRY image_data;
    image_data.digest_sign = buf;
    SYS_CONSOLE_PRINT("\n\rfactory image signature : %s\n\r", image_data.digest_sign);
    //image_data.digest_sign = ota_params.serv_app_digest_sign_string;
    //strcpy(image_data.digest_sign, buf);
    strcpy(image_signature_file_name, APP_DIR_NAME);
    strcat(image_signature_file_name, "/factory_image_sign.txt");
    
    //if(ota_params.signature_verification == true){
        appFile.fileHandle1 = SYS_FS_FileOpen(image_signature_file_name, (SYS_FS_FILE_OPEN_WRITE_PLUS));
        SYS_FS_FileWrite(appFile.fileHandle1, image_data.digest_sign, strlen(image_data.digest_sign));
        SYS_FS_FileClose(appFile.fileHandle1);
    //}
}
#endif
// *****************************************************************************
// *****************************************************************************
// Section:  To get download status 
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  void OTA_GetPatchStatus(OTA_PARAMS *result)

  Description:
   To get patch progress status 

  Task Parameters:
    pointer of type ota_params 
  Return:
    None
 */
//---------------------------------------------------------------------------
#ifdef SYS_OTA_PATCH_ENABLE
void OTA_GetPatchStatus(OTA_PARAMS *result) {
    result->patch_progress_status = OTA_PatchProgressStatus();
}
#endif
void OTA_GetImageDbInfo(void)
{
    if(ota.current_task == OTA_TASK_DOWNLOAD_IMAGE){
        SYS_CONSOLE_PRINT("Download In Progress, Please try later\n\r");
        return;
    }
    OTA_DB_BUFFER *otaimageDB;
    
    otaimageDB = OTA_GetDBBuffer();
    int ret_db_status = OTAGetDb(otaimageDB, APP_DIR_NAME"/image_database.csv");
    
    if(ret_db_status >= 1){
        SYS_CONSOLE_PRINT("NO info found\n\r");
        return;
    }
        
    uint8_t total_images = GetTotalImgs(otaimageDB);
    if(total_images == 0){
        SYS_CONSOLE_PRINT("NO info found\n\r");
        return;
    }
    uint8_t i;
    uint32_t ver;
    char digest_str[66];
    char image_name[100];
    uint8_t status;
    SYS_CONSOLE_PRINT("\tImage name\t\tStatus\t\tVersion\t\tDigest\n\r");
    for (i = 0; i < total_images; i++) {
        GetFieldValue_32Bit(otaimageDB, OTA_IMAGE_VERSION, i, &ver);
        GetFieldString(otaimageDB, OTA_IMAGE_NAME, i, image_name);
        GetFieldString(otaimageDB, OTA_IMAGE_DIGEST, i, digest_str);
        GetFieldValue(otaimageDB, OTA_IMAGE_STATUS, i, &status);
        SYS_CONSOLE_PRINT("%s\t\t%x\t\t%d\t\t%s\n\r",image_name,status,ver,digest_str);
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
                /* Mount was successful. Format the disk. */
                appFile.state = APP_MOUNT_SUCCESS;
            }
            break;
        }
        case APP_MOUNT_SUCCESS:
        {
            disk_mount = true;
            break;
        }
        default:
        {

            break;
        }
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: Creating image file in FS
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  static bool create_image_file()

  Description:
   Creating image file in FS

  Task Parameters:
    None
  Return:
    True- if successful image creation
    False- if image creation failed
 */
//---------------------------------------------------------------------------

static bool create_image_file() {

    strcpy(image_name, APP_DIR_NAME);
    strcat(image_name, (strrchr(ota_params.ota_server_url, '/')));
    
#ifdef SYS_OTA_PATCH_ENABLE    
    if(ota_params.patch_request == true)
        strcpy((strrchr(image_name, '.') + 1), "patch");
    else
        strcpy((strrchr(image_name, '.') + 1), "bin");
#else
    strcpy((strrchr(image_name, '.') + 1), "bin");
#endif    

#if (SERVICE_TYPE == OTA_DEBUG)
    SYS_CONSOLE_PRINT("SYS OTA : file path:%s\r\n", image_name);
#endif
    appFile.fileHandle1 = SYS_FS_FileOpen(image_name, (SYS_FS_FILE_OPEN_WRITE_PLUS));
    if (appFile.fileHandle1 == SYS_FS_HANDLE_INVALID) {
        /* File open unsuccessful */
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : File open unsuccessful\r\n");
#endif
        return false;

    } else {
        /* File open was successful. Write to the file. */
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : File Opened\r\n");
#endif
        return true;
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: Building images path (source file, patch file & target file) , required for patch-ota 
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  static void OTA_patch_build_image_path()

  Description:
   Building images path (source file, patch file & target file) , required for patch-ota

  Task Parameters:
    None
  Return:
    None
 */
//---------------------------------------------------------------------------
#ifdef SYS_OTA_PATCH_ENABLE
static void OTA_patch_build_image_path() {

#if (SERVICE_TYPE == OTA_DEBUG)
    SYS_CONSOLE_PRINT("OTA : image name:%s\n\r",image_name); 
#endif
    
    strcpy(patch_param.patch_file, image_name);

#if (SERVICE_TYPE == OTA_DEBUG)
    SYS_CONSOLE_PRINT("OTA : patch_param.patch_file:%s\n\r",patch_param.patch_file);
#endif
    
    strcpy(patch_param.target_file, APP_DIR_NAME);
    strcat(patch_param.target_file, (strrchr(image_name, '/')));
    strcpy((strrchr(patch_param.target_file, '.') + 1), "bin");
#if (SERVICE_TYPE == OTA_DEBUG)
    SYS_CONSOLE_PRINT("OTA : patch_param.target_file:%s\n\r",patch_param.target_file);
#endif
    strcpy((strrchr(image_name, '.') + 1), "bin");
}
#endif
// *****************************************************************************
// *****************************************************************************
// Section: convert string digest into hex format
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  static void formulate_digest()

  Description:
   convert string digest into hex format

  Task Parameters:
    None
  Return:
    None
 */
//---------------------------------------------------------------------------

static void formulate_digest() {
    int i;
    int index = 0;
#if (SERVICE_TYPE == OTA_DEBUG)
    SYS_CONSOLE_PRINT("SYS OTA : serv_app_digest: %s\r\n", ota_params.serv_app_digest_string);
#endif
    for (i = 0; i < 32; i++) {

        strncpy(serv_app_digest_gl, &ota_params.serv_app_digest_string[index], 2);
        index = index + 2;
        serv_app_digest_g[i] = (uint8_t) strtol(serv_app_digest_gl, NULL, 16);
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : formulated digest[%d]: %x\r\n", i, serv_app_digest_g[i]);
#endif
    }
    
#ifdef SYS_OTA_PATCH_ENABLE
    if(ota_params.patch_request == true){
        index = 0;
        
        for (i = 0; i < 32; i++) {

                strncpy(serv_app_patch_digest_gl, &ota_params.serv_app_patch_digest_string[index], 2);
                index = index + 2;
                serv_app_patch_digest_g[i] = (uint8_t) strtol(serv_app_patch_digest_gl, NULL, 16);
        #if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("SYS OTA : formulated patch digest[%d]: %x\r\n", i, serv_app_patch_digest_g[i]);
        #endif
        }
    }
#endif
}

// *****************************************************************************
// *****************************************************************************
// Section: Initialize required parameters for setting image status
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  void OTA_ImgStatus_Params(void)

  Description:
   Initialize required parameters for setting image status

  Task Parameters:
    None
  Return:
    None
 */
//---------------------------------------------------------------------------

void OTA_ImgStatus_Params(void) {
    /*Bootloader would have set APP_IMG_BOOT_CTL->type and APP_IMG_BOOT_CTL->version*/
    ota.task.param.version = APP_IMG_BOOT_CTL->version;
#if (SERVICE_TYPE == OTA_DEBUG)
    SYS_CONSOLE_PRINT("SYS OTA : newly uploaded image version : %d\r\n", APP_IMG_BOOT_CTL->version);
#endif
    ota.task.param.img_status = IMG_STATUS_VALID;
    ota.task.param.pfm_status = IMG_STATUS_VALID;
    ota.task.param.abort = 0;
    if (APP_IMG_BOOT_CTL->type == IMG_TYPE_FACTORY_RESET) {
        ota.current_task = OTA_TASK_UPDATE_USER;
        ota.status = SYS_STATUS_READY;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: To check if image database in the external flash exist
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  static OTA_DB_STATUS OTA_IsDbExist(void)

  Description:
   To check if image database in the external flash exist

  Task Parameters:
    None
  Return:
    A OTA_DB_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

static OTA_DB_STATUS OTA_IsDbExist(void) {
#if (SERVICE_TYPE == OTA_DEBUG)
    SYS_CONSOLE_PRINT("SYS OTA : Opening Directory\r\n");
#endif
    dirHandle = SYS_FS_DirOpen(APP_DIR_NAME);

    if (dirHandle == SYS_FS_HANDLE_INVALID) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : Directory open Failed\r\n");
#endif
        SYS_FS_DirClose(dirHandle);
        return OTA_DB_ERROR;
    }

#if (SERVICE_TYPE == OTA_DEBUG)
    SYS_CONSOLE_PRINT("SYS OTA : Searching directory\r\n");
#endif
    if (SYS_FS_DirSearch(dirHandle, OTA_DB_NAME, SYS_FS_ATTR_ARC, &stat) == SYS_FS_RES_FAILURE) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : specified file not found\r\n");
#endif 
        SYS_FS_DirClose(dirHandle);
        return OTA_DB_NOT_FOUND;
    } else {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : specified file found\r\n");
#endif
        SYS_FS_DirClose(dirHandle);
        return OTA_DB_FOUND;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: To check if image database in the external flash is reached max
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  static OTA_DB_STATUS OTA_Task_DbEntryCheck(void)

  Description:
    To check if image database in the external flash is reached max 

  Task Parameters:
    None
  Return:
    A OTA_DB_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

static OTA_DB_STATUS OTA_Task_DbEntryCheck(void) {

    OTA_DB_STATUS db_ret = OTA_IsDbExist();
    if ((db_ret == OTA_DB_ERROR)) {

#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : Directory Error\r\n");
#endif
        return OTA_DB_DIRECTORY_ERROR;
    }
    if (((db_ret) == OTA_DB_NOT_FOUND)) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : DB not found\r\n");
#endif
        return OTA_DB_NOT_FOUND;
    }

    imageDB = OTA_GetDBBuffer();
    /*Check if max number of images defined by user reached*/
    uint8_t ret = OTAGetDb(imageDB, APP_DIR_NAME"/image_database.csv");
    if (ret > 0) {
        csv_destroy_buffer((CSV_BUFFER *) imageDB);
        return OTA_DB_ERROR;
    }

    if (GetTotalImgs(imageDB) < SYS_OTA_NUM_IMGS) {
        csv_destroy_buffer((CSV_BUFFER *) imageDB);
        return OTA_DB_NOT_FULL;
    } else {
        csv_destroy_buffer((CSV_BUFFER *) imageDB);
        return OTA_DB_FULL;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: To clean up files (without entry in DB) in the directory 
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  static void OTA_CleanUp(void) 

  Description:
    To clean up files (without entry in DB) in the directory 

  Task Parameters:
    None
  Return:
    None
 */
//---------------------------------------------------------------------------

static void OTA_CleanUp(void) {
    bool no_db = false;
    uint8_t total_images = 0;
    int i = 0;
    OTA_DB_STATUS db_ret = OTA_IsDbExist();

    if ((db_ret == OTA_DB_ERROR)) {
        /*Error in directory function*/
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : Directory Error\r\n");
#endif
        return;
    }

    if (((db_ret) == OTA_DB_NOT_FOUND)) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : DB not found\r\n");
#endif
        no_db = true;
    } else {
        imageDB = OTA_GetDBBuffer();
        /*Get DB to local buffer*/
        uint8_t ret = OTAGetDb(imageDB, APP_DIR_NAME"/image_database.csv");
        if (ret > 0) {
            /*error during opening of file*/
            csv_destroy_buffer((CSV_BUFFER *) imageDB);
            return;
        }
        total_images = GetTotalImgs(imageDB);
    }

    dirHandle = SYS_FS_DirOpen(APP_DIR_NAME);
    if (dirHandle != SYS_FS_HANDLE_INVALID) {
        /*Do nothing*, directory open success*/
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : Directory open is successful\r\n");
#endif
    }
    else {
        /*Error during directory open*/
        csv_destroy_buffer((CSV_BUFFER *) imageDB);
        SYS_FS_DirClose(dirHandle);
    }

    while (1) {
        if (SYS_FS_DirSearch(dirHandle, "*.bin", SYS_FS_ATTR_ARC, &stat) == SYS_FS_RES_FAILURE) {
            if (SYS_FS_Error() == SYS_FS_ERROR_NO_FILE) {
                /*end of directory*/
#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("SYS OTA : end of directory\r\n");
#endif
            } else {
                SYS_CONSOLE_PRINT("SYS OTA : Error in directory search : clean up\r\n");
            }
            break;
        } else {
            /* Directory read succeeded.*/
            /*continue reading the directory.*/
#if (SERVICE_TYPE == OTA_DEBUG)
            
            SYS_CONSOLE_PRINT("File read\r\n");
#endif
            SYS_FS_RESULT res;
            /*if no DB is present, remove the file(may bepartial image)*/
            if(strncmp(FACTORY_IMAGE,stat.fname,strlen(FACTORY_IMAGE)) != 0){
                if (no_db == true) {
                    if(strncmp(FACTORY_IMAGE,stat.fname,strlen(FACTORY_IMAGE)) != 0){   
                        char file_to_remove[100];
                        
                        strcpy(file_to_remove,APP_DIR_NAME);
                        strcat(file_to_remove,"/");
                        strcat(file_to_remove,stat.fname);
                        res = SYS_FS_FileDirectoryRemove(file_to_remove);
                        if (res == SYS_FS_RES_FAILURE) {
                            SYS_CONSOLE_PRINT("SYS OTA : File remove operation failed : clean up\r\n");
                        } else {
                                /*Do nothing File is removed successfully*/
                            SYS_CONSOLE_PRINT("SYS OTA : Removed file : %s\r\n", stat.fname);
                        }
                    }
                } else {
                    /*initialize flag to false in every iteration*/
                    bool image_found = false;
                    /*search the image in the DB*/
                    for (i = 0; i < total_images; i++) {
                        char image_name[100];
                        /*Check for image name in the DB*/
                        if (GetFieldString(imageDB, OTA_IMAGE_NAME, i, image_name) == 0) {
                            if (!strncmp(image_name, stat.fname, strlen(stat.fname))) {
                                /*if image is present in the DB set the flag and break from loop*/
                                image_found = true;
                                break;
                            }
                        }
                    }
                    /*if image not found remove file from directory*/
                    if (image_found == false) {
                        char file_to_remove[100];
                        
                        strcpy(file_to_remove,APP_DIR_NAME);
                        strcat(file_to_remove,"/");
                        strcat(file_to_remove,stat.fname);
                        res = SYS_FS_FileDirectoryRemove(file_to_remove);
						#if (SERVICE_TYPE == OTA_DEBUG)
                        SYS_CONSOLE_PRINT("File to be removed : %s\n\r",file_to_remove);
                        #endif
						if (res == SYS_FS_RES_FAILURE) {
                            SYS_CONSOLE_PRINT("SYS OTA : File remove operation failed : clean up\r\n");
                            
                        } else {
                            /*Do nothing File is removed successfully*/
                            SYS_CONSOLE_PRINT("SYS OTA : Removed file : %s\r\n", stat.fname);
                        }
#ifdef SYS_OTA_SECURE_BOOT_ENABLED        
                        
                        if(ota_params.signature_verification == true){
                            char image_signature_file[100];
                            strcpy(image_signature_file, file_to_remove);
                            strcpy((strrchr(image_signature_file, '.') ), "_sign.txt");
                            res = SYS_FS_FileDirectoryRemove(image_signature_file);
                            #if (SERVICE_TYPE == OTA_DEBUG)
                            SYS_CONSOLE_PRINT("File to be removed : %s\n\r",image_signature_file);
                            #endif
                            if (res == SYS_FS_RES_FAILURE) {

                                SYS_CONSOLE_PRINT("SYS OTA : File remove operation failed : clean up\r\n");

                            } else {
                                /*Do nothing File is removed successfully*/
                                SYS_CONSOLE_PRINT("SYS OTA : Removed file : %s\r\n", image_signature_file);

                            }
                        }
#endif
                    }
                }
            }
        }
    }
    if (no_db == false)
        csv_destroy_buffer((CSV_BUFFER *) imageDB);
    SYS_FS_DirClose(dirHandle);
}
// *****************************************************************************
// *****************************************************************************
// Section: To update user about OTA status
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  static void OTA_Task_UpdateUser(void)
  
  Description:
    To update user about OTA status
  
  Task Parameters:
    None
 
  Return:
    None
 */
//---------------------------------------------------------------------------

static void OTA_Task_UpdateUser(void) {
    OTA_COMPLETION_CALLBACK callback = ota.callback;

    /*if callback is for image download start , downloader should not be closed*/
#ifdef SYS_OTA_PATCH_ENABLE    
    if ((ota.ota_result != OTA_RESULT_IMAGE_DOWNLOAD_START) && (ota.ota_result != OTA_RESULT_PATCH_EVENT_START)) {
        if (ota.downloader != DRV_HANDLE_INVALID) {
            DOWNLOADER_Close(ota.downloader);
            ota.downloader = DRV_HANDLE_INVALID;
        }
    }
#else
    if ((ota.ota_result != OTA_RESULT_IMAGE_DOWNLOAD_START)) {
        if (ota.downloader != DRV_HANDLE_INVALID) {
            DOWNLOADER_Close(ota.downloader);
            ota.downloader = DRV_HANDLE_INVALID;
        }
    }
#endif
    ota.status = SYS_STATUS_READY;
    if (callback != NULL) {
        callback(ota.ota_result, NULL, NULL);
    }
}
// *****************************************************************************
// *****************************************************************************
// Section: Download image. The image is copied to FS in Image Store.
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_Task_DownloadImage(void)

  Description:
    Download image. The image is copied to FS
    in Image Store.

  Task Parameters:

  Return:
    A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

typedef struct {
    uint8_t *buf;
    uint32_t buf_len;
    uint32_t copied_len;
    uint32_t total_len;
    uint32_t timeout;
} OTA_DOWNLOAD_TASK_CONTEXT;

typedef enum {
    TASK_STATE_D_INIT = 0,
    TASK_STATE_D_GET_CONTENT_LENGTH,
    TASK_STATE_D_DOWNLOAD,
    TASK_STATE_D_WRITE,
    TASK_STATE_D_DONE,
    TASK_STATE_D_ERROR
} OTA_DOWNLOAD_TASK_STATE;

static SYS_STATUS OTA_Task_DownloadImage(void) {
    OTA_DOWNLOAD_TASK_CONTEXT * ctx = (void*) ota.task.context;
    OTA_TASK_PARAM *param = &ota.task.param;
    SYS_STATUS status = SYS_STATUS_BUSY;
    int rx_len;
    if (param->abort) {
        /*For future implementation*/

    } else {
        if (ota.task.state != TASK_STATE_D_INIT
                && ota.task.state != TASK_STATE_D_DONE) {
            if ((ctx->timeout - SYS_TMR_TickCountGet()) & 0x80000000) {
                status = SYS_STATUS_ERROR;
                ota.task.state = TASK_STATE_D_DONE;
            }
        }
    }

    switch (ota.task.state) {
        case TASK_STATE_D_INIT:
        {
            ota.ota_result = OTA_RESULT_IMAGE_DOWNLOAD_START;
            OTA_Task_UpdateUser();
#if (SERVICE_TYPE == OTA_DEBUG)
            SYS_CONSOLE_PRINT("SYS OTA : TASK_STATE_D_INIT\r\n");
#endif
            if (create_image_file() == false) {
                ota.task.state = TASK_STATE_D_DONE;
                status = SYS_STATUS_ERROR;
            }
            ctx->total_len = 0;
            ctx->copied_len = 0;
            ctx->buf_len = 0;
            ctx->buf = (uint8_t*) OSAL_Malloc(FLASH_SECTOR_SIZE);
            ctx->timeout = SYS_TMR_TickCountGet() + (OTA_DOWNLOADER_TIMEOUT * SYS_TMR_TickCounterFrequencyGet());

            ota.task.state = TASK_STATE_D_GET_CONTENT_LENGTH;

            if (ctx->buf == NULL) {
                ota.task.state = TASK_STATE_D_DONE;
                status = SYS_STATUS_ERROR;
            }
            if(ota_isTls_request == true){
                OTA_SetCachePolicy(false);
            }
            else{
#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("not changing cache policy : Not a TLS request\n\r");
#endif 
            }
            
            break;
        }
        case TASK_STATE_D_GET_CONTENT_LENGTH:
        {

            int rx_len;
            rx_len = DOWNLOADER_Read(ota.downloader, &ctx->buf[ctx->buf_len], (FLASH_SECTOR_SIZE - ctx->buf_len));

            if (rx_len <= 0) {
                if (rx_len < 0) {
                    ota.task.state = TASK_STATE_D_DONE;
                    status = SYS_STATUS_ERROR;
                }
                break;
            }

            ctx->buf_len += rx_len;
            if (ctx->buf_len != FLASH_SECTOR_SIZE) {
                break;
            }
            ctx->total_len = field_content_length;
            ota_params.server_image_length = field_content_length;
#if (SERVICE_TYPE == OTA_DEBUG)
            SYS_CONSOLE_PRINT("SYS OTA : field_content_length : %d\r\n", field_content_length);
#endif
            SYS_FS_FileSeek(appFile.fileHandle1, 0, SYS_FS_SEEK_SET);
            if (SYS_FS_FileWrite(appFile.fileHandle1, ctx->buf, FLASH_SECTOR_SIZE) == -1) {
                /* Failed to write to the file. */
                ota.task.state = TASK_STATE_D_DONE;
                status = SYS_STATUS_ERROR;
                break;
            } else {
                /* File write was successful. */
            }
            ota.task.state = TASK_STATE_D_DOWNLOAD;
            ctx->copied_len += ctx->buf_len;
            ctx->buf_len = 0;
            break;
        }
        case TASK_STATE_D_DOWNLOAD:
        {
            /*To track already copied length */
            int req_len = ctx->total_len - ctx->copied_len;

            /*To make sure that requested length does not exceed 4096 */
            if (req_len > FLASH_SECTOR_SIZE) {
                req_len = FLASH_SECTOR_SIZE;
            }

            /*Copy data to local buffer
             Also get the number of data rcvd*/
            rx_len = DOWNLOADER_Read(ota.downloader, &ctx->buf[ctx->buf_len], (req_len - ctx->buf_len));

#if (SERVICE_TYPE == OTA_DEBUG)
            SYS_CONSOLE_PRINT("SYS OTA : TASK_STATE_D_DOWNLOAD :: rx_len : %d\r\n", rx_len);
#endif
            if (rx_len <= 0) {
                /*if rcvd lenghth is less than 0 there is a problem*/
                if (rx_len < 0) {
                    ota.task.state = TASK_STATE_D_DONE;
                    status = SYS_STATUS_ERROR;
                }
                break;
            }

            /*Add up rcv number of data*/
            ctx->buf_len += rx_len;
            
            /*If requested length is rcvd , need t program in ext flash*/
            if (ctx->buf_len == req_len) {
                ota.task.state = TASK_STATE_D_WRITE;
            }
            break;
        }
        case TASK_STATE_D_WRITE:
        {
            offset = 0 + ctx->copied_len;
            SYS_FS_FileSeek(appFile.fileHandle1, offset, SYS_FS_SEEK_SET);
            if (SYS_FS_FileWrite(appFile.fileHandle1, ctx->buf, ctx->buf_len) == -1) {
                /* Failed to write to the file. */

#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("SYS OTA : write_fail\r\n");
#endif
                ota.task.state = TASK_STATE_D_DONE;
                status = SYS_STATUS_ERROR;
                break;
            } else {
                /* File write was successful. */

            }

            /*Add up total copied length*/
            ctx->copied_len += ctx->buf_len;
            
            /*total data downloaded*/
            ota_params.total_data_downloaded = ctx->copied_len;
            
            #if (SERVICE_TYPE == OTA_DEBUG)
            SYS_CONSOLE_PRINT("data downloaded : %d bytes \r\n",ota_params.total_data_downloaded);
            #endif

            /*buf length should be made zero to track new rcv data legth*/
            ctx->buf_len = 0;
            if (ctx->copied_len < ctx->total_len) {
                ota.task.state = TASK_STATE_D_DOWNLOAD;
            } else {
                ota.task.state = TASK_STATE_D_DONE;
            }
            break;
        }
        case TASK_STATE_D_DONE:
        {
#if (SERVICE_TYPE == OTA_DEBUG)
            SYS_CONSOLE_PRINT("SYS OTA : TASK_STATE_D_DONE\r\n");
#endif
            break;
        }
        default:
        {
            return SYS_STATUS_ERROR;
        }
    }

    if (ota.task.state == TASK_STATE_D_DONE) {
        if (ctx->buf != NULL) {
            OSAL_Free(ctx->buf);
            ctx->buf = NULL;
        }
        if (ota.downloader != DRV_HANDLE_INVALID) {
            DOWNLOADER_Close(ota.downloader);
            ota.downloader = DRV_HANDLE_INVALID;
        }
        return status == SYS_STATUS_ERROR ?
                SYS_STATUS_ERROR : SYS_STATUS_READY;

    }
    return SYS_STATUS_BUSY;
}

// *****************************************************************************
// *****************************************************************************
// Section: Verify the downloaded image in Image Store.(This example code uses SHA256 for image validation)
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_Task_VerifyImage(void)
  
  Description:
    Verify the downloaded image in Image Store.
    (This example code uses SHA256 for image validation)
  Task Parameters:
    None
  Return:
     A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

typedef struct {
    CRYPT_SHA256_CTX *sha256;
    uint8_t *buf;
    uint32_t len;
    uint32_t offset;
    uint32_t img_sz;
} OTA_VERIFY_TASK_CONTEXT;

typedef enum {
    TASK_STATE_V_INIT = 0,
    TASK_STATE_V_READ_HEADER,
    TASK_STATE_V_READ,
    TASK_STATE_V_DONE
} OTA_VERIFY_TASK_STATE;

static SYS_STATUS OTA_Task_VerifyImage(void) {
    OTA_VERIFY_TASK_CONTEXT * ctx = (void*) ota.task.context;
    OTA_TASK_PARAM *param = &ota.task.param;
    SYS_STATUS status;
    SYS_ASSERT(sizeof (*ctx) < ota.task.context, "Buffer Overflow");
    if (param->abort) {
        /*For future implementation*/
    }

    switch (ota.task.state) {
        case TASK_STATE_V_INIT:
        {
            ctx->offset = 0;
            ctx->len = FLASH_SECTOR_SIZE;
            ctx->buf = (uint8_t*) OSAL_Malloc(
                    FLASH_SECTOR_SIZE + sizeof (CRYPT_SHA256_CTX) + 128);
            formulate_digest();
            if (ctx->buf == NULL) {

                return SYS_STATUS_ERROR;
            }
            /*Below step need to check*/
            ctx->sha256 = (CRYPT_SHA256_CTX *) (((unsigned long) ctx->buf + FLASH_SECTOR_SIZE + 128) & 0xFFFFFF80);

            memset(ctx->sha256, 0, sizeof (CRYPT_SHA256_CTX));
            CRYPT_SHA256_Initialize(ctx->sha256);

            ctx->img_sz = field_content_length;
            
#ifdef SYS_OTA_PATCH_ENABLE              
            if(ota_params.patch_request == true)
                ctx->img_sz = SYS_FS_FileSize(appFile.fileHandle1);
#endif            
            
            offset = 0;
            if (SYS_FS_FileSeek(appFile.fileHandle1, 0, SYS_FS_SEEK_SET) == -1) {
                /* File seek caused an error */
                SYS_FS_FileClose(appFile.fileHandle1);
                return SYS_STATUS_ERROR;
            }
            if (SYS_FS_FileRead(appFile.fileHandle1, ctx->buf, ctx->len) == -1) {
                /* There was an error while reading the file. Close the file
                 * and error out. */
                SYS_FS_FileClose(appFile.fileHandle1);
                return SYS_STATUS_ERROR;
            }
            ota.task.state = TASK_STATE_V_READ;
#ifdef SYS_OTA_PATCH_ENABLE              
            if(patch_verification == true)
                ota.ota_result = OTA_RESULT_PATCH_IMAGE_DIGEST_VERIFY_START ;
            else
                ota.ota_result = OTA_RESULT_IMAGE_DIGEST_VERIFY_START;
#else
            ota.ota_result = OTA_RESULT_IMAGE_DIGEST_VERIFY_START;
#endif
            OTA_Task_UpdateUser();
            break;
        }
        case TASK_STATE_V_READ:
        {

            CRYPT_SHA256_DataAdd(ctx->sha256, ctx->buf, ctx->len);
            ctx->offset += ctx->len;

            if (ctx->offset == ctx->img_sz) {

                ota.task.state = TASK_STATE_V_DONE;
                break;
            }

            if ((ctx->offset + ctx->len) > ctx->img_sz) {
                ctx->len = ctx->img_sz - ctx->offset;
            }

            offset = ctx->offset;
            SYS_FS_FileSeek(appFile.fileHandle1, offset, SYS_FS_SEEK_SET);

            if (SYS_FS_FileRead(appFile.fileHandle1, ctx->buf, ctx->len) == -1) {
                /* There was an error while reading the file. Close the file
                 * and error out. */
                SYS_FS_FileClose(appFile.fileHandle1);
                return SYS_STATUS_ERROR;
            }
            break;
        }
        case TASK_STATE_V_DONE:
        {
            uint8_t digest[CRYPT_SHA256_DIGEST_SIZE];
            FIRMWARE_IMAGE_HEADER *img;
            if (param->abort == 0) {

                CRYPT_SHA256_Finalize(ctx->sha256, digest);
                img = (FIRMWARE_IMAGE_HEADER*) ctx->buf;
                int i;
#ifdef SYS_OTA_PATCH_ENABLE                 
                if(patch_verification == false)
                {
                    for (i = 0; i < 32; i++) {
                        img->digest[i] = serv_app_digest_g[i];
                    }
                }
                else
                {
                    for (i = 0; i < 32; i++) {
                        img->digest[i] = serv_app_patch_digest_g[i];
                    }
                }
#else
                for (i = 0; i < 32; i++) {
                        img->digest[i] = serv_app_digest_g[i];
                    }
#endif
#if (SERVICE_TYPE == OTA_DEBUG)
                for (i = 0; i < 32; i++) {
                    SYS_CONSOLE_PRINT("SYS OTA : digest[%d] = %d, img->digest[%d] = %d\r\n", i, digest[i], i, img->digest[i]);
                }
#endif
                if (memcmp(digest, img->digest, CRYPT_SHA256_DIGEST_SIZE) != 0) {
                    SYS_CONSOLE_MESSAGE("SYS OTA : digest mismatch\r\n");
                    status = SYS_STATUS_ERROR;
                } else {
                    status = SYS_STATUS_READY;
                }
            } else {
                SYS_CONSOLE_MESSAGE("abort\r\n");
                status = SYS_STATUS_ERROR;
            }

            if (ctx->buf != NULL) {
                OSAL_Free(ctx->buf);
            }
            ctx->buf = NULL;
            return status == SYS_STATUS_READY ?
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

// *****************************************************************************
// *****************************************************************************
// Section: Mark downloaded image to DOWNLOADED and mark PFM to VALID.
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_Task_SetImageStatus(void)
  
  Description:
    Mark downloaded image to DOWNLOADED and mark PFM to VALID.
    
  Task Parameters:  
    None
  Return:
    A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

typedef struct {
    FIRMWARE_IMAGE_HEADER img;
    uint8_t *buf;
} OTA_SIGN_IMAGE_TASK_CONTEXT;

typedef enum {
    TASK_STATE_S_INIT = 0,
    TASK_STATE_S_READ_HEADER,
    TASK_STATE_S_WRITE_HEADER,
    TASK_STATE_S_UPDATE_DB,
    TASK_STATE_S_VERIFY_HEADER,
    TASK_STATE_S_ERASE_BOOT_CONTROL,
    TASK_STATE_S_WRITE_BOOT_CONTROL,
    TASK_STATE_S_DONE,
} OTA_SIGN_IMAGE_TASK_STATE;

OTA_SIGN_IMAGE_TASK_CONTEXT *ctx = (void*) ota.task.context;

static SYS_STATUS OTA_Task_SetImageStatus(void) {

    OTA_TASK_PARAM *param = &ota.task.param;
    if (param->abort) {
        /*For future implementation*/
    }
    switch (ota.task.state) {
        case TASK_STATE_S_INIT:
        {
            ctx->buf = (uint8_t*) OSAL_Malloc(FLASH_SECTOR_SIZE);
            if (ctx->buf == NULL) {
                return SYS_STATUS_ERROR;
            }
            imageDB = OTA_GetDBBuffer();
            if (OTAGetDb(imageDB, APP_DIR_NAME"/image_database.csv") > 1)
                return SYS_STATUS_ERROR;
            if (ota.new_downloaded_img == false || ota.ota_rollback_initiated == true) {
                selected_row = GetImageRow(ota.task.param.version, imageDB);
                if (selected_row == -1) {
#if (SERVICE_TYPE == OTA_DEBUG)
                    SYS_CONSOLE_PRINT("\r\nSYS OTA : Image version is not found\r\n");
#endif
                    return SYS_STATUS_ERROR;
                }
            } else {
                selected_row = GetTotalImgs(imageDB) - 1;
            }
            ota.task.state = TASK_STATE_S_UPDATE_DB;
            break;
        }
        case TASK_STATE_S_UPDATE_DB:
        {
            if (GetFieldValue(imageDB, OTA_IMAGE_STATUS, selected_row, &ctx->img.status) != 0) {
#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("SYS OTA : Image status field not read properly\r\n");
#endif
                return SYS_STATUS_ERROR;
            }
            if (GetFieldValue_32Bit(imageDB, OTA_IMAGE_VERSION, selected_row, &ctx->img.version) != 0) {
#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("SYS OTA : Image version field not read properly\r\n");
#endif
                return SYS_STATUS_ERROR;
            }
#if (SERVICE_TYPE == OTA_DEBUG)
            SYS_CONSOLE_PRINT("SYS OTA : ctx->img.status : %x\r\n", ctx->img.status);
            SYS_CONSOLE_PRINT("SYS OTA : ctx->img.version : %x\r\n", ctx->img.version);
#endif
            if (ctx->img.status != param->img_status) {
                char status[10];
                sprintf(status, "%x", param->img_status);
                SetFieldValue(imageDB, OTA_IMAGE_STATUS, selected_row, status);
                OTASaveDb(imageDB, APP_DIR_NAME"/image_database.csv");
                ota.task.state = TASK_STATE_S_WRITE_BOOT_CONTROL;
            } else {
                ota.task.state = TASK_STATE_S_WRITE_BOOT_CONTROL;
            }
            break;
        }

        case TASK_STATE_S_WRITE_BOOT_CONTROL:
        {

            INT_Flash_Open();
            ctx->img.status = ota.task.param.pfm_status;
            ctx->img.order = 0xFF;
            ctx->img.type = IMG_TYPE_PRODUCTION;
            ctx->img.boot_addr = APP_IMG_BOOT_ADDR;
            INT_Flash_Erase(APP_IMG_BOOT_CTL_WR, FLASH_SECTOR_SIZE);
            memcpy(ctx->buf, &ctx->img, sizeof (FIRMWARE_IMAGE_HEADER));
            INT_Flash_Write(APP_IMG_BOOT_CTL_WR, ctx->buf, FLASH_SECTOR_SIZE);
            ota.task.state = TASK_STATE_S_DONE;
            break;
        }
        case TASK_STATE_S_DONE:
        {
            if (INT_Flash_Busy()) {
                break;
            }
            INT_Flash_Close();

            if (ctx->buf != NULL) {
                OSAL_Free(ctx->buf);
                ctx->buf = NULL;
            }

            return SYS_STATUS_READY;
        }
        default:
        {
            return SYS_STATUS_ERROR;
        }
    }

    return SYS_STATUS_BUSY;
}

// *****************************************************************************
// *****************************************************************************
// Section: Make a new entry to OTA database
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_Task_DataEntry(void)
  
  Description:
    Make a new entry to OTA database 
  
  Task Parameters:
    None
  
  Return:
    A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

static SYS_STATUS OTA_Task_DataEntry() {
    OTA_DB_ENTRY image_data;
    image_data.image_name = image_name;
    image_data.type = IMAGE_TYPE;
    image_data.version = ota_params.version;
    image_data.status = IMAGE_STATUS_DOWNLOADED;
    image_data.digest = ota_params.serv_app_digest_string;
	
#ifdef SYS_OTA_SECURE_BOOT_ENABLED	
    if(ota_params.signature_verification == true){
        image_data.digest_sign = ota_params.serv_app_digest_sign_string;

        strcpy(image_signature_file_name, APP_DIR_NAME);
        strcat(image_signature_file_name, (strrchr(ota_params.ota_server_url, '/')));
        strcpy((strrchr(image_signature_file_name, '.') ), "_sign.txt");
    }
#endif
    if (ota.db_full == true)
        image_data.db_full = true;
    else
        image_data.db_full = false;
    if (OTADbNewEntry(APP_DIR_NAME"/image_database.csv", &image_data) == -1) {
        return SYS_STATUS_ERROR;
    }
    
#ifdef SYS_OTA_SECURE_BOOT_ENABLED   
    if(ota_params.signature_verification == true){
        appFile.fileHandle1 = SYS_FS_FileOpen(image_signature_file_name, (SYS_FS_FILE_OPEN_WRITE_PLUS));
        SYS_FS_FileWrite(appFile.fileHandle1, image_data.digest_sign, strlen(image_data.digest_sign));
        SYS_FS_FileClose(appFile.fileHandle1);
    }
#endif	
    return SYS_STATUS_READY;
}

// *****************************************************************************
// *****************************************************************************
// Section: Mark all downloaded image to DISABLED
// *****************************************************************************
// *****************************************************************************
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

typedef struct {
    uint32_t slot;

} OTA_FACTORY_RESET_TASK_CONTEXT;

typedef enum {
    TASK_STATE_F_INIT = 0,
    TASK_STATE_F_DISABLE_IMAGE,
    TASK_STATE_F_INVALIDATE_BOOT_CTL,
    TASK_STATE_F_DONE
} OTA_FACTORY_RESET_TASK_STATE;

static SYS_STATUS OTA_Task_FactoryReset(void) {


    SYS_ASSERT(sizeof (*ctx) < ota.task.context, "Buffer Overflow");

    switch (ota.task.state) {
        case TASK_STATE_F_INIT:
        {

            ota.task.state = TASK_STATE_F_DISABLE_IMAGE;
            break;
        }
        case TASK_STATE_F_DISABLE_IMAGE:
        {
#if (SERVICE_TYPE == OTA_DEBUG)
            SYS_CONSOLE_PRINT("Removing \r\n");
#endif
            SYS_FS_RESULT res;
            res = SYS_FS_FileDirectoryRemove(APP_DIR_NAME"/image_database.csv");
            if (res == SYS_FS_RES_FAILURE) {
                // Directory remove operation failed
#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("SYS OTA : Directory remove operation failed\r\n");
                //while(1);
#endif
            }
            ota.task.state = TASK_STATE_F_INVALIDATE_BOOT_CTL;
            break;
        }
        case TASK_STATE_F_INVALIDATE_BOOT_CTL:
        {

            INT_Flash_Open();
            INT_Flash_Erase(APP_IMG_BOOT_CTL_WR, FLASH_SECTOR_SIZE);
            ota.task.state = TASK_STATE_F_DONE;
            break;
        }
        case TASK_STATE_F_DONE:
        {
            if (INT_Flash_Busy()) {
                break;
            }
            INT_Flash_Close();
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

// *****************************************************************************
// *****************************************************************************
// Section: To register user call back function
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  _OTA_RegCB(void)
  
  Description:
    To register user call back function
  
  Task Parameters:
    None
 
  Return:
     A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

static inline SYS_STATUS _OTA_RegCB(OTA_COMPLETION_CALLBACK callback) {
    SYS_STATUS ret = SYS_STATUS_ERROR;
    if (!ota.callback) {
        /* Copy the client function pointer */
        ota.callback = callback;
        ret = SYS_STATUS_READY;
    }
    return ret;
}

// *****************************************************************************
// *****************************************************************************
// Section: To get free sector information in external disk
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  OTA_IsDisk_Full(void)
  
  Description:
    To get free sector information in external disk 
  
  Task Parameters:
    None
 
  Return:
    true - if free sector count is less than 10
    false -  if free sector count is greater than 10
 */
//---------------------------------------------------------------------------
#ifdef OTA_CHECK_FREE_SECTOR
static bool OTA_IsDisk_Full(void){
    
  uint32_t totalSectors, freeSectors;
  SYS_FS_RESULT res;  
  res = SYS_FS_DriveSectorGet(APP_DIR_NAME, &totalSectors, &freeSectors);
  if(res == SYS_FS_RES_FAILURE)
  {
        //Sector information get operation failed.
        #if (SERVICE_TYPE == OTA_DEBUG)
            SYS_CONSOLE_PRINT("SYS OTA : Sector information get operation failed\r\n");
        #endif
  }
 
  #if (SERVICE_TYPE == OTA_DEBUG)
         SYS_CONSOLE_PRINT("SYS OTA : totalSectors : %d , freeSectors : %d\n\r",totalSectors,freeSectors);
  #endif
  if(freeSectors <= 10){
      return true;
  }
  
  return false;
}
#endif
// *****************************************************************************
// *****************************************************************************
// Section: Registering OTA callback function
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  
  Description:
 Registering OTA callback function
  
  Task Parameters:
    buffer - callback function name 
    length - function pointer length
  Return:
     A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

SYS_STATUS OTA_CallBackReg(void *buffer, uint32_t length) {
    uint8_t status = SYS_STATUS_ERROR;

    OTA_COMPLETION_CALLBACK g_otaFunPtr = buffer;
    if ((g_otaFunPtr) && (length == sizeof (g_otaFunPtr))) {
        /* Register the client callback function */
        status = _OTA_RegCB(g_otaFunPtr);
    }
    return status;
}

// *****************************************************************************
// *****************************************************************************
// Section: Starting OTA process
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_Start(OTA_PARAMS *param) 
  Description:
    Starting OTA process
  
  Task Parameters:
    Parameters related to OTA, like ota url,image version etc.. 
 
  Return:
     A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

SYS_STATUS OTA_Start(OTA_PARAMS *param) {

    if (ota.current_task != OTA_TASK_IDLE) {
        return SYS_STATUS_ERROR;
    }
    
#ifdef OTA_CHECK_FREE_SECTOR
    if(OTA_IsDisk_Full() == true){
        SYS_CONSOLE_PRINT("SYS OTA : No Free Sector, Download not possible\n\r");
        return SYS_STATUS_ERROR;
    }
#endif
    ota_isTls_request = OTA_IsTls_Request(param->ota_server_url);
    ota.downloader = DOWNLOADER_Open(param->ota_server_url);
    if (ota.downloader == DRV_HANDLE_INVALID) {

        return SYS_STATUS_ERROR;
    }

    strncpy(ota_params.serv_app_digest_string, param->serv_app_digest_string, 64);
#ifdef SYS_OTA_SECURE_BOOT_ENABLED	
    ota_params.signature_verification = param->signature_verification;
    if(ota_params.signature_verification == true)
        strncpy(ota_params.serv_app_digest_sign_string, param->serv_app_digest_sign_string, 96);
#endif    
	memcpy(ota_params.ota_server_url, param->ota_server_url, strlen(param->ota_server_url) + 1);
    ota_params.version = param->version;
    
#ifdef SYS_OTA_PATCH_ENABLE      
    if(param->patch_request == true)
    {
        ota_params.patch_request = true;
        ota_params.patch_base_version = param->patch_base_version;
        strncpy(ota_params.serv_app_patch_digest_string, param->serv_app_patch_digest_string, 64);
        strncpy(ota_params.serv_app_base_digest_string, param->serv_app_base_digest_string, 64);
    }

    if(param->patch_request == true)
        ota.current_task = OTA_TASK_SEARCH_PATCH_BASEVERSION;
    else
        ota.current_task = OTA_TASK_DOWNLOAD_IMAGE;
#else
    ota.current_task = OTA_TASK_DOWNLOAD_IMAGE;
#endif
    
    ota.status = SYS_STATUS_BUSY;
    ota.task.param.img_status = IMG_STATUS_DOWNLOADED;
    ota.task.param.pfm_status = IMG_STATUS_DISABLED;
    ota.task.param.abort = 0;
    return SYS_STATUS_READY;
}

//---------------------------------------------------------------------------

// *****************************************************************************
// *****************************************************************************
// Section: API for upper layer to initiate Roll back
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_Rollback(void)
 
  Description:
    API for upper layer to initiate Roll back
  
  Task Parameters:
    None 
 
  Return:
     A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

SYS_STATUS OTA_Rollback(void) {
    if (ota.current_task != OTA_TASK_IDLE) {
        return SYS_STATUS_ERROR;
    }

    if (APP_IMG_BOOT_CTL->type == IMG_TYPE_FACTORY_RESET) {
        return SYS_STATUS_READY;
    }
    ota.current_task = OTA_TASK_SET_IMAGE_STATUS;
    ota.status = SYS_STATUS_BUSY;
    ota.task.param.version = APP_IMG_BOOT_CTL->version;
    ota.task.param.img_status = IMG_STATUS_DISABLED;
    ota.task.param.pfm_status = IMG_STATUS_DISABLED;
    ota.task.param.abort = 0;
    ota.ota_rollback_initiated = true;
    return SYS_STATUS_READY;
}

// *****************************************************************************
// *****************************************************************************
// Section: API for upper layer to initiate factory reset
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_FactoryReset(void) 
 
  Description:
    API for upper layer to initiate factory reset 
  
  Task Parameters:
    None 
  Return:
     A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

SYS_STATUS OTA_FactoryReset(void) {
    if (ota.current_task != OTA_TASK_IDLE) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS OTA : Factory reset OTA task not idle : %d\r\n", ota.current_task);
#endif
        return SYS_STATUS_ERROR;
    }
    ota.current_task = OTA_TASK_FACTORY_RESET;
    ota.status = SYS_STATUS_READY;
    return SYS_STATUS_READY;
}

// *****************************************************************************
// *****************************************************************************
// Section:  API for upper layer to Erase a particular version of image
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_EraseImage(uint32_t version)
 
  Description:
    API for upper layer to Erase a particular version of image 
  
  Task Parameters:
    Image version to be erased 
 
  Return:
     A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

SYS_STATUS OTA_EraseImage(uint32_t version) {
    if (ota.current_task != OTA_TASK_IDLE) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("OTA not in idle : %d\r\n", ota.current_task);
#endif
        return SYS_STATUS_ERROR;
    }
    erase_ver = version;
    ota.current_task = OTA_TASK_ERASE_IMAGE;
    ota.status = SYS_STATUS_BUSY;
    return SYS_STATUS_READY;
}

// *****************************************************************************
// *****************************************************************************
// Section: Erase a particular version of image 
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_EraseImage(uint32_t version)
 
  Description:
    Erase a particular version of image 
  
  Task Parameters:
    Image version to be erased 
 
  Return:
     A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------

SYS_STATUS OTA_Task_EraseImage(uint32_t version) {
    selected_row = -1;
    imageDB = OTA_GetDBBuffer();
    if (OTAGetDb(imageDB, APP_DIR_NAME"/image_database.csv") > 1) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS_OTA : DB load failed\r\n");
#endif
        return SYS_STATUS_ERROR;
    }
    selected_row = GetImageRow(version, imageDB);
    if (selected_row == -1) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("\n\nSYS_OTA : Image version is not found\n\n");
#endif
        return SYS_STATUS_ERROR;
    }

    char image_name[100];
    if (GetFieldString(imageDB, OTA_IMAGE_NAME, selected_row, image_name) != 0) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS_OTA : Image status field not read properly\r\n");
#endif
        return SYS_STATUS_ERROR;
    }

#if (SERVICE_TYPE == OTA_DEBUG)
    SYS_CONSOLE_PRINT("SYS_OTA : image name : %s\r\n", image_name);
#endif

    if (OTADbDeleteEntry(imageDB, selected_row) == -1) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("\n\nSYS_OTA : Database Entry could not be removed\n\n");
#endif
        return SYS_STATUS_ERROR;
    }

    OTASaveDb(imageDB, APP_DIR_NAME"/image_database.csv");
    SYS_FS_RESULT res;
    char file_to_remove[100];
                        
    strcpy(file_to_remove,APP_DIR_NAME);
    strcat(file_to_remove,"/");
    strcat(file_to_remove,image_name);
    res = SYS_FS_FileDirectoryRemove(file_to_remove);
    
    if (res == SYS_FS_RES_FAILURE) {
        // image remove operation failed
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("\r\nSYS_OTA : Image file removing is failed\r\n");
#endif
        return SYS_STATUS_ERROR;
    }
#if (SERVICE_TYPE == OTA_DEBUG)
    SYS_CONSOLE_PRINT("\r\nSYS_OTA : Erased Image version \r\n");
#endif

 #if (SERVICE_TYPE == OTA_DEBUG)   
    csv_destroy_buffer((CSV_BUFFER *) imageDB);
    imageDB = OTA_GetDBBuffer();
    if (OTAGetDb(imageDB, APP_DIR_NAME"/image_database.csv") > 1) {
        SYS_CONSOLE_PRINT("SYS_OTA : DB load failed\r\n");
    }
    size_t my_string_size = 30;
    char *my_string = malloc(my_string_size + 1);
    int k;
    for (k = 0; k < csv_get_height((CSV_BUFFER *) imageDB); k++) {
        csv_get_field(my_string, my_string_size, (CSV_BUFFER *) imageDB, k, 0);
        SYS_CONSOLE_PRINT("SYS OTA : Got string = \"%s\"\r\n", my_string);
        csv_get_field(my_string, my_string_size, (CSV_BUFFER *) imageDB, k, 1);
        SYS_CONSOLE_PRINT("SYS OTA : Got string = \"%s\"\r\n", my_string);
        csv_get_field(my_string, my_string_size, (CSV_BUFFER *) imageDB, k, 2);
        SYS_CONSOLE_PRINT("SYS OTA : Got string = \"%c\"\r\n", my_string[0]);
        if (GetFieldString(imageDB, OTA_IMAGE_DIGEST, k, image_name) != 0) {
            SYS_CONSOLE_PRINT("SYS OTA : Image status field not read properly\r\n");
        } else {
            SYS_CONSOLE_PRINT("SYS OTA : digest : %s\r\n", image_name);
        }

    }
#endif
    return SYS_STATUS_READY;
}

// *****************************************************************************
// *****************************************************************************
// Section: Search for Patch Base image version in OTA DB, based on user input(Manifest information) 
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  SYS_STATUS OTA_Search_ImageVersion(uint32_t version)
 
  Description:
    Search for Patch Base image version in OTA DB, based on user input(Manifest information)  
  
  Task Parameters:
    Base version of image 
 
  Return:
     A SYS_STATUS code describing the current status.
 */
//---------------------------------------------------------------------------
#ifdef SYS_OTA_PATCH_ENABLE 
SYS_STATUS OTA_Search_ImageVersion(uint32_t version,char *base_ver_digest) {
    char source_file[500];
    char image_digest[500];
    char server_base_ver_digest[500];
    int8_t selected_row = -1;
    imageDB = OTA_GetDBBuffer();
    if (OTAGetDb(imageDB, APP_DIR_NAME"/image_database.csv") > 1) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("SYS_OTA : DB load failed\r\n");
#endif
        return SYS_STATUS_ERROR;
    }
    selected_row = GetImageRow(version, imageDB);
    
    if (selected_row == -1) {
#if (SERVICE_TYPE == OTA_DEBUG)
        SYS_CONSOLE_PRINT("\n\nSYS_OTA : Image version is not found\n\n");
#endif
        return SYS_STATUS_ERROR;
    }
    GetFieldString(imageDB,OTA_IMAGE_NAME,selected_row,source_file);
#if (SERVICE_TYPE == OTA_DEBUG)
    SYS_CONSOLE_PRINT("\n\nSYS_OTA : selected_row : %d\r\n",selected_row);
#endif
    strcpy(server_base_ver_digest,base_ver_digest);
    GetFieldString(imageDB,OTA_IMAGE_DIGEST,selected_row,image_digest);
    SYS_CONSOLE_PRINT("\n\rimage_digest : %s\n\r",image_digest);
    SYS_CONSOLE_PRINT("\n\rserver_base_ver_digest : %s\n\r",server_base_ver_digest);
    if(strncmp(image_digest,server_base_ver_digest,64) != 0)
        return SYS_STATUS_ERROR;
    strcpy(patch_param.source_file,APP_DIR_NAME);
    strcat(patch_param.source_file,"/");
    strcat(patch_param.source_file,source_file);
#if (SERVICE_TYPE == OTA_DEBUG)
    SYS_CONSOLE_PRINT("\n\nSYS_OTA : patch base image : %s\r\n",patch_param.source_file);
#endif
    

    return SYS_STATUS_READY;
}
#endif
// *****************************************************************************
// *****************************************************************************
// Section: To check if OTA state is idel
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  bool OTA_IsIdle(void)
 
  Description:
    To check if OTA state is idel 
  
  Task Parameters:
    None
 
  Return:
    True- if state is idle
    False- if state is not idle
 */
//---------------------------------------------------------------------------

bool OTA_IsIdle(void) {

    return ota.ota_idle;
}

// *****************************************************************************
// *****************************************************************************
// Section: To maintain OTA task state machine
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  void OTA_Tasks(void)
 
  Description:
    To maintain OTA task state machine
  
  Task Parameters:
    None
 
  Return:
    None
 */
//---------------------------------------------------------------------------

void OTA_Tasks(void) {

    switch (ota.current_task) {
        case OTA_TASK_INIT:
        {
#if (OTA_NVM_INT_CALLBACK_ENABLE == false)
            if(mutex_nvm_g_status == false){
                SYS_CONSOLE_PRINT("SYS OTA : NVM Mutex CREATION FAILED\n\r");
            }
#endif
            ota.ota_idle = false;
#if (SERVICE_TYPE == OTA_DEBUG)
            SYS_CONSOLE_PRINT("SYS OTA : in OTA init\r\n");
#endif
            mount_disk();
            if (disk_mount == true) {
                /*removing unnecessary files if any*/
                OTA_CleanUp();

#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("SYS OTA : Checking OTA DB\r\n");
#endif
                OTA_DB_STATUS db_ret = OTA_Task_DbEntryCheck();
                if (db_ret == OTA_DB_NOT_FULL) {
#if (SERVICE_TYPE == OTA_DEBUG)
                    SYS_CONSOLE_PRINT("SYS OTA : OTA DB not FULL\r\n");
#endif
                    ota.db_full = false;
                }
                if (db_ret == OTA_DB_FULL) {

                    SYS_CONSOLE_PRINT("SYS OTA : OTA DB FULL\r\n");

                    ota.db_full = true;
                    ota.ota_result = OTA_RESULT_IMAGE_DATABASE_FULL;
                }
                ota.current_task = OTA_TASK_SET_IMAGE_STATUS;
                ota.task.state = OTA_TASK_INIT;
                OTA_ImgStatus_Params();
                disk_mount = false;
            }
            break;
        }
        case OTA_TASK_SET_IMAGE_STATUS:
        {
            ota.ota_idle = false;
            ota.status = OTA_Task_SetImageStatus();
            if (ota.status != SYS_STATUS_BUSY) {
                ota.ota_result = OTA_RESULT_NONE;
                if (ota.new_downloaded_img == true)
                    ota.ota_result = OTA_RESULT_IMAGE_STATUS_SET;
                if (ota.ota_rollback_initiated == true)
                    ota.ota_result = OTA_RESULT_ROLLBACK_DONE;
                ota.current_task = OTA_TASK_UPDATE_USER;
                ota.task.state = OTA_TASK_INIT;
#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("SYS OTA : OTA_TASK_SET_IMAGE_STATUS done\r\n");
#endif
            }

            break;
        }
        case OTA_TASK_UPDATE_USER:
        {
            ota.ota_idle = false;
            ota.current_task = OTA_TASK_IDLE;
            if (ota.ota_result == OTA_RESULT_IMAGE_DOWNLOADED) {
#ifdef SYS_OTA_PATCH_ENABLE                 
                if(ota_params.patch_request == true){
                    ota.current_task = OTA_TASK_VERIFY_PATCH_IMAGE_DIGEST;
                    ota.task.state = OTA_TASK_INIT;
                    
                }else{
                    ota.current_task = OTA_TASK_VERIFY_IMAGE_DIGEST;
                    ota.task.state = OTA_TASK_INIT;
                }
#else
                ota.current_task = OTA_TASK_VERIFY_IMAGE_DIGEST;
                ota.task.state = OTA_TASK_INIT;
#endif
            }
            
#ifdef SYS_OTA_PATCH_ENABLE               
            if (ota.ota_result == OTA_RESULT_PATCH_IMAGE_DIGEST_VERIFY_SUCCESS) {
                ota.current_task = OTA_TASK_PATCH_BUILD_IMAGE;
                ota.task.state = OTA_TASK_INIT;
            }
#endif
            if (ota.ota_result == OTA_RESULT_IMAGE_DIGEST_VERIFY_SUCCESS) {
                ota.current_task = OTA_TASK_DATABASE_ENTRY;
                ota.task.state = OTA_TASK_INIT;
            }
#ifdef SYS_OTA_PATCH_ENABLE               
            if (ota.ota_result == OTA_RESULT_PATCH_EVENT_START){
                ota.current_task = OTA_TASK_DOWNLOAD_IMAGE;
                ota.task.state = OTA_TASK_INIT;
            }
            
            if (ota.ota_result == OTA_RESULT_PATCH_EVENT_COMPLETED){
                ota.current_task = OTA_TASK_VERIFY_IMAGE_DIGEST;
                ota.task.state = OTA_TASK_INIT;
            }
#endif
            OTA_Task_UpdateUser();
            break;
        }
        case OTA_TASK_IDLE:
        {
            ota.ota_idle = true;
            break;
        }
#if (SERVICE_TYPE == OTA_DEBUG)
        case OTA_TASK_CHECK_DB:
        {
            ota.ota_idle = false;
            ota.status = OTA_Task_DbEntryCheck();
            if (ota.status == SYS_STATUS_READY) {

                ota.current_task = OTA_TASK_DOWNLOAD_IMAGE;
                ota.task.state = OTA_TASK_INIT;
                while (1);
            }
            if (ota.status == SYS_STATUS_ERROR) {

                SYS_CONSOLE_PRINT("SYS OTA : OTA DB FULL\r\n");
                ota.current_task = OTA_TASK_UPDATE_USER;
                ota.task.state = OTA_TASK_INIT;
                while (1);
            }
            break;
        }
#endif
        
#ifdef SYS_OTA_PATCH_ENABLE        
        case OTA_TASK_SEARCH_PATCH_BASEVERSION:
        {
            ota.ota_idle = false;
#if (SERVICE_TYPE == OTA_DEBUG)
            SYS_CONSOLE_MESSAGE("SYS OTA : OTA_TASK_SEARCH_PATCH_BASEVERSION\r\n");
#endif
            ota.status = OTA_Search_ImageVersion(ota_params.patch_base_version,ota_params.serv_app_base_digest_string);      
            if (ota.status == SYS_STATUS_ERROR) {
                ota.ota_result = OTA_RESULT_PATCH_BASEVERSION_NOTFOUND;
                ota.current_task = OTA_TASK_UPDATE_USER;
            } else {
                ota.ota_result = OTA_RESULT_PATCH_EVENT_START;
                ota.current_task = OTA_TASK_UPDATE_USER;
            }
            break;
        }
#endif        
        case OTA_TASK_DOWNLOAD_IMAGE:
        {
            ota.ota_idle = false;
            ota.status = OTA_Task_DownloadImage();
            if (ota.status == SYS_STATUS_READY) {
                if(ota_isTls_request == true){
                    OTA_SetCachePolicy(true);
                }
#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("SYS OTA : Downloaded image\r\n");
#endif
                ota.ota_result = OTA_RESULT_IMAGE_DOWNLOADED;
                ota.current_task = OTA_TASK_UPDATE_USER;
                ota.task.state = OTA_TASK_INIT;
                
            }

            if (ota.status == SYS_STATUS_ERROR) {
                if(ota_isTls_request == true){
                    OTA_SetCachePolicy(true);
                }
                SYS_CONSOLE_PRINT("SYS OTA : Download error\r\n");
                ota.ota_result = OTA_RESULT_IMAGE_DOWNLOAD_FAILED;
                ota.current_task = OTA_TASK_UPDATE_USER;
                ota.task.state = OTA_TASK_INIT;
            }

            break;
        }
        
#ifdef SYS_OTA_PATCH_ENABLE        
        case OTA_TASK_PATCH_BUILD_IMAGE:
        {           
            SYS_FS_FileClose(appFile.fileHandle1);
            OTA_patch_build_image_path();
            ota.status = OTA_ProcessPatch(&patch_param);
            
            SYS_FS_RESULT res;
            /*Remove patch file*/
            res = SYS_FS_FileDirectoryRemove(patch_param.patch_file);
            if (res == SYS_FS_RES_FAILURE) {
                SYS_CONSOLE_PRINT("SYS OTA : File remove operation failed\r\n");
            } else {
                    /*Do nothing File is removed successfully*/
            }
#if (SERVICE_TYPE == OTA_DEBUG)
            SYS_CONSOLE_PRINT("SYS OTA : OTA_TASK_PATCH_BUILD_IMAGE\r\n");
#endif
            if (ota.status == SYS_STATUS_READY) {
                appFile.fileHandle1 = SYS_FS_FileOpen(patch_param.target_file, (SYS_FS_FILE_OPEN_READ));
    #if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("appFile.fileHandle1 : %d , %s\n\r",appFile.fileHandle1,patch_param.target_file);
    #endif
                ota.ota_result = OTA_RESULT_PATCH_EVENT_COMPLETED;
                ota.current_task = OTA_TASK_UPDATE_USER;
                ota.task.state = OTA_TASK_INIT;
            }
            else if (ota.status == SYS_STATUS_ERROR) {
                ota.ota_result = OTA_RESULT_PATCH_IMAGE_FAILED;
                ota.current_task = OTA_TASK_UPDATE_USER;
                ota.task.state = OTA_TASK_INIT;
            }
            break;
        }
#endif
        case OTA_TASK_VERIFY_IMAGE_DIGEST:
        {
            ota.ota_idle = false;
#ifdef SYS_OTA_PATCH_ENABLE            
            patch_verification = false;
#endif
            ota.status = OTA_Task_VerifyImage();
            if (ota.status == SYS_STATUS_READY) {

#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("SYS OTA : Verified image\r\n");
#endif
                ota.ota_result = OTA_RESULT_IMAGE_DIGEST_VERIFY_SUCCESS;
                ota.current_task = OTA_TASK_UPDATE_USER;
                ota.task.state = OTA_TASK_INIT;
                ota.new_downloaded_img = true;
                SYS_FS_FileClose(appFile.fileHandle1);
            }

            if (ota.status == SYS_STATUS_ERROR) {
#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("SYS OTA : Image verification error\r\n");
#endif
                ota.ota_result = OTA_RESULT_IMAGE_DIGEST_VERIFY_FAILED;
                ota.current_task = OTA_TASK_UPDATE_USER;
                SYS_FS_FileClose(appFile.fileHandle1);
                SYS_FS_RESULT res;
                res = SYS_FS_FileDirectoryRemove(image_name);
                if (res == SYS_FS_RES_FAILURE) {
                    SYS_CONSOLE_PRINT("SYS OTA : File remove operation failed\r\n");
                } else {
                    /*Do nothing File is removed successfully*/
                }
            }
            break;
        }
        
#ifdef SYS_OTA_PATCH_ENABLE        
        case OTA_TASK_VERIFY_PATCH_IMAGE_DIGEST:
        {
            ota.ota_idle = false;
            patch_verification = true;
            ota.status = OTA_Task_VerifyImage();
            
            if (ota.status == SYS_STATUS_READY) {
#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("SYS OTA : Verified patch image\r\n");
#endif
                ota.ota_result = OTA_RESULT_PATCH_IMAGE_DIGEST_VERIFY_SUCCESS;
                ota.current_task = OTA_TASK_UPDATE_USER;
                ota.task.state = OTA_TASK_INIT;
                
            }
            if (ota.status == SYS_STATUS_ERROR) {
#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("SYS OTA : patch Image verification error\r\n");
#endif
                ota.ota_result = OTA_RESULT_PATCH_IMAGE_DIGEST_VERIFY_FAILED;
                ota.current_task = OTA_TASK_UPDATE_USER;
                SYS_FS_FileClose(appFile.fileHandle1);
                SYS_FS_RESULT res;
                res = SYS_FS_FileDirectoryRemove(image_name);
                if (res == SYS_FS_RES_FAILURE) {
                    SYS_CONSOLE_PRINT("SYS OTA : File remove operation failed\r\n");
                } else {
                    /*Do nothing File is removed successfully*/
                }
            }
            break;
        }
#endif
        case OTA_TASK_DATABASE_ENTRY:
        {
            ota.ota_idle = false;
            ota.status = OTA_Task_DataEntry();
            if (ota.status == SYS_STATUS_READY) {
#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_PRINT("SYS OTA : Data Entered\r\n");
#endif
                ota.current_task = OTA_TASK_SET_IMAGE_STATUS;
                ota.task.state = OTA_TASK_INIT;
                
            }

            if (ota.status == SYS_STATUS_ERROR) {
#if (SERVICE_TYPE == OTA_DEBUG)
                SYS_CONSOLE_MESSAGE("SYS OTA : Database entry error\r\n");
#endif
                
                ota.ota_result = OTA_RESULT_IMAGE_DB_ENTRY_FAILED;
                ota.current_task = OTA_TASK_UPDATE_USER;
            }
            break;
        }
        case OTA_TASK_FACTORY_RESET:
        {
            ota.ota_idle = false;
#if (SERVICE_TYPE == OTA_DEBUG)
            SYS_CONSOLE_MESSAGE("SYS OTA : OTA_TASK_FACTORY_RESET\r\n");
#endif
            ota.status = OTA_Task_FactoryReset();

            if (ota.status != SYS_STATUS_BUSY) {
                ota.ota_result = OTA_RESULT_FACTORY_RESET_SUCCESS;
                ota.current_task = OTA_TASK_UPDATE_USER;
                //SYS_FS_FileClose(appFile.fileHandle);
            }
            break;
        }
        case OTA_TASK_ERASE_IMAGE:
        {
            ota.ota_idle = false;
#if (SERVICE_TYPE == OTA_DEBUG)
            SYS_CONSOLE_MESSAGE("SYS OTA : OTA_TASK_ERASE_IMAGE\r\n");
#endif
            ota.status = OTA_Task_EraseImage(erase_ver);

            if (ota.status == SYS_STATUS_ERROR) {
                ota.ota_result = OTA_RESULT_IMAGE_ERASE_FAILED;
                ota.current_task = OTA_TASK_UPDATE_USER;
            } else {
                ota.ota_result = OTA_RESULT_IMAGE_ERASED;
                ota.current_task = OTA_TASK_UPDATE_USER;
            }
            break;
        }

        default:
        {
            break;
        }
    }
    DOWNLOADER_Tasks();
}

// *****************************************************************************
// *****************************************************************************
// Section: To Initialize OTA related parameters
// *****************************************************************************
// *****************************************************************************
//---------------------------------------------------------------------------
/*
  void OTA_Initialize(void
 
  Description:
    To Initialize OTA related parameters
  
  Task Parameters:
    None
 
  Return:
    None
 */
//---------------------------------------------------------------------------

void OTA_Initialize(void) {
    memset(&ota, 0, sizeof (ota));
    /*Initializing downloader status*/
    ota.downloader = DRV_HANDLE_INVALID;
    ota.status = SYS_STATUS_UNINITIALIZED;
    ota.current_task = OTA_TASK_INIT;
    ota.callback = NULL;
    ota.task.param.abort = 0;
    /* Registering NVM callback function */
    INT_Flash_Initialize();
    /*Initialization of Download protocol parameters*/
    DOWNLOADER_Initialize();
}
