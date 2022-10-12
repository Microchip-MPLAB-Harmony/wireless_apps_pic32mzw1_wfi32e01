/*******************************************************************************
  MPLAB Harmony Sample Library Module Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    ota.h

  Summary:
    This file contains the interface definition for the OTA library.

  Description:
    This file defines the system and application interface.
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

#ifndef __INC_OTA_H__
#define __INC_OTA_H__

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
*/

#include "system_config.h"
#include "definitions.h"

#ifdef  __cplusplus
extern "C" {
#endif
#define     IMAGE_TYPE                  "0x01"
#define     IMAGE_STATUS_DOWNLOADED     "0xFE"
  
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

typedef void (*OTA_COMPLETION_CALLBACK)(uint32_t event, void * data,void *cookie );

/*provides ota system status*/
typedef enum {
    
    /*Image downloading started*/
    OTA_RESULT_IMAGE_DOWNLOAD_START = 0,
#ifdef SYS_OTA_PATCH_ENABLE            
    /*For patch, Base version of image is not found in OTA DB */        
    OTA_RESULT_PATCH_BASEVERSION_NOTFOUND,    
            
    /*Patch event start*/
    OTA_RESULT_PATCH_EVENT_START, 
#endif            
    /*Image downloaded successfully*/
    OTA_RESULT_IMAGE_DOWNLOADED ,

    /*Image download failed*/        
    OTA_RESULT_IMAGE_DOWNLOAD_FAILED,  
#ifdef SYS_OTA_PATCH_ENABLE     
    /*Image patching completed*/         
    OTA_RESULT_PATCH_EVENT_COMPLETED,
       
    /*Image patch failed*/         
    OTA_RESULT_PATCH_IMAGE_FAILED,
#endif            
    /*Image Digest Verify Start*/ 
    OTA_RESULT_IMAGE_DIGEST_VERIFY_START,
#ifdef SYS_OTA_PATCH_ENABLE           
    /*Patch Image Digest Verify Start*/        
    OTA_RESULT_PATCH_IMAGE_DIGEST_VERIFY_START,        
#endif
    /*Image Digest Verify Success*/        
    OTA_RESULT_IMAGE_DIGEST_VERIFY_SUCCESS,
#ifdef SYS_OTA_PATCH_ENABLE     
    /*Patch Image Digest Verify Success*/         
    OTA_RESULT_PATCH_IMAGE_DIGEST_VERIFY_SUCCESS,
            
    /*patch image digest verify failed*/        
    OTA_RESULT_PATCH_IMAGE_DIGEST_VERIFY_FAILED,        
#endif            
    /*Image verification Failed*/        
    OTA_RESULT_IMAGE_DIGEST_VERIFY_FAILED,
            
    /*Status for image is set*/        
    OTA_RESULT_IMAGE_STATUS_SET,        

    /*image DB full*/
    OTA_RESULT_IMAGE_DATABASE_FULL,        
            
    /*Image database Entry failed*/        
    OTA_RESULT_IMAGE_DB_ENTRY_FAILED,
    
    /*Image Erase failed*/        
    OTA_RESULT_IMAGE_ERASE_FAILED,
           
    /*Image erased successfully*/        
    OTA_RESULT_IMAGE_ERASED,
            
   /*Factory reset done successfully*/ 
    OTA_RESULT_FACTORY_RESET_SUCCESS,
                        
    /*Roll back completed*/
    OTA_RESULT_ROLLBACK_DONE,
            
    /*No update for user*/        
    OTA_RESULT_NONE        
    
}OTA_RESULT;

typedef enum {
    /*Error in DB search*/
    OTA_DB_ERROR = 0,
            
    /*OTA DB is present*/        
    OTA_DB_FOUND,
            
    /*OTA DB does not exist*/        
    OTA_DB_NOT_FOUND,
            
    /*Error in directory handler*/
    OTA_DB_DIRECTORY_ERROR,
            
    /*Max number of images defined by user is not reached*/
    OTA_DB_NOT_FULL,
            
    /*Max number of images defined by user is reached*/
    OTA_DB_FULL
    
}OTA_DB_STATUS;
// Passed a pointer to OTA_Start()
typedef struct
{
    const char *server_name;
    const char *file_name;
    uint32_t    port;
} OTA_DOWNLOADER_HTTP_PARAMS;
#define OTA_URL_SIZE    100
typedef struct OTA_PARAMS {
        char    ota_server_url[OTA_URL_SIZE];
        char    serv_app_digest_string[66];
#ifdef SYS_OTA_SECURE_BOOT_ENABLED		
        char    serv_app_digest_sign_string[98];
#endif		
#ifdef SYS_OTA_PATCH_ENABLE        
        char    serv_app_patch_digest_string[66];
        char    serv_app_base_digest_string[66];
        char    serv_app_target_digest_string[66];
#endif
        char    *image_name;
        char    *status;
        uint32_t version;
        char    *type;
        char    *digest;
#ifdef SYS_OTA_PATCH_ENABLE        
        bool    patch_request;
        uint32_t patch_base_version;
#endif
        uint32_t delete_img_version;
        uint32_t total_data_downloaded; 
        uint32_t server_image_length; 
        
        /*set/reset this variable, as per signature field in manifest file is present/absent*/
        bool signature_verification;
#ifdef SYS_OTA_PATCH_ENABLE        
        uint8_t patch_progress_status;
#endif
} OTA_PARAMS;


// *****************************************************************************
/*
  Function:
    OTA_GetDownloadStatus(OTA_PARAMS *result)

  Summary:
    To get download status.

  Description:
    To get download status.

  Parameters:
    pointer of type ota_params.

  Returns:
    None.
*/
// *****************************************************************************
void OTA_GetDownloadStatus(OTA_PARAMS *result);

// *****************************************************************************
/*
  Function:
    OTA_StoreFactoryImageSignature(void *buf) 

  Summary:
    To store factory image signature.

  Description:
    To store factory image signature.

  Parameters:
    pointer of type void.

  Returns:
    None.
*/
// *****************************************************************************
void OTA_StoreFactoryImageSignature(void *buf); 
// *****************************************************************************
/*
  Function:
    OTA_GetPatchStatus(OTA_PARAMS *result)

  Summary:
    To get patch progress status.

  Description:
    To get patch progress status.

  Parameters:
    pointer of type ota_params.

  Returns:
    None.
*/
// *****************************************************************************
#ifdef SYS_OTA_PATCH_ENABLE
void OTA_GetPatchStatus(OTA_PARAMS *result);
#endif
// *****************************************************************************
/*
  Function:
    void OTA_Initialize(void);

  Summary:
    Initializes the OTA Library.

  Description:
    This function is used to initialize the OTA Library.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.
*/
// *****************************************************************************
void OTA_Initialize(void);


// *****************************************************************************
/* Function:
    SYS_STATUS OTA_Status (void);

  Summary:
    Check OTA Task status.

  Description:
    Check OTA Task status. If OTA is ready to accept new OTA request, this
    function returns SYS_STATUS_READY.
 
  Precondition:
    None.

  Parameters:
    None.

  Returns:
    SYS_STATUS code.
*/
// *****************************************************************************
SYS_STATUS OTA_Status(void);

// *****************************************************************************
/* Function:
    void OTA_Tasks (void);

  Summary:
    Maintains the OTA module state machine. 

  Description:
    This function maintains the OTA module state machine and manages the 
    OTA Module object list items and responds to OTA Module events.
    This function should be called from the SYS_Tasks function.

  Precondition:
    None.

  Parameters:
    index      - Object index for the specified module instance.

  Returns:
    None.

  Example:
    <code>

    while (true)
    {
        OTA_Tasks ();

        // Do other tasks
    }
    </code>

  Remarks:
  Remarks:
    This function is normally not called directly by an application.
*/
// *****************************************************************************
void OTA_Tasks(void);


// *****************************************************************************
/*
  Function:
    SYS_STATUS OTA_Start(OTA_COMPLETION_CALLBACK callback);
  
  Summary:
    Performs OTA. 

  Description:
    Performs OTA. The completion is notified to Application by call-back function.
    The function returns error immediately if OTA process or Factory-Reset is
    in progress.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    SYS_STATUS code.
*/
// *****************************************************************************
SYS_STATUS OTA_Start(OTA_PARAMS *param);


// *****************************************************************************
/*
  Function:
    void OTA_Abort (void);

  Summary:
    Aborts OTA process.
    
  Description:
    Aborts OTA process if running. The callback (passed to OTA_Start will be 
    called) will be called with "DRV_CLIENT_STATUS_ERROR" on the completion 
    of this process.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    SYS_STATUS code.
*/
// *****************************************************************************
SYS_STATUS OTA_Abort(void);


// *****************************************************************************
/*
  Function:
    void OTA_GetImageDbInfo(void)

  Summary:
    To get image DB info.

  Description:
    To get image DB info 
  
  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.
*/
// *****************************************************************************
void OTA_GetImageDbInfo(void);


// *****************************************************************************
/*
  Function:
    SYS_STATUS Rollback(OTA_COMPLETION_CALLBACK callback);

  Summary:
    Perform rollback operation.
  
  Description:
    Perform rollback to the previous successfully loaded firmware. If the
    golden image is running, the function returns ERROR.
    The function returns error immediately if OTA process is in progress.
    The Application must restart system after this function call.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    SYS_STATUS code.
*/
// *****************************************************************************
SYS_STATUS OTA_Rollback(void);

// *****************************************************************************
/*
  Function:
    SYS_STATUS OTA_FactoryReset(OTA_COMPLETION_CALLBACK callback);

  Summary:
    Perform Factory-Reset.

  Description:
    The function returns error immediately if OTA process is in progress.
    The Application must restart system after this function call.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    SYS_STATUS code.
*/
// *****************************************************************************
SYS_STATUS OTA_FactoryReset(void);

// *****************************************************************************
/*
  Function:
    SYS_STATUS OTA_EraseImage(uint32_t version);

  Summary:
    Perform image erase.

  Description:
    Perform image erase.

  Precondition:
    None.

  Parameters:
    Image Version to be erased.

  Returns:
    SYS_STATUS code.
*/
// *****************************************************************************
SYS_STATUS OTA_EraseImage(uint32_t version);

// *****************************************************************************
/*
  Function:
    SYS_STATUS OTA_CallBackReg(void *buffer, uint32_t length);

  Summary:
    Registering callback.

  Description:
    Registering callback.

  Precondition:
    None.

  Parameters:
    buffer - pointer to callback function name.
    length - length of callback function name.

  Returns:
    SYS_STATUS code.
*/
// *****************************************************************************
SYS_STATUS OTA_CallBackReg(void *buffer, uint32_t length);

// *****************************************************************************
/*
  Function:
    bool OTA_IsIdle(void);

  Summary:
    To check if OTA Task is in idle state.

  Description:
    To check if OTA Task is in idle state.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    True - Idle
    False - Not Idle
*/
// *****************************************************************************
bool OTA_IsIdle(void) ;

// *****************************************************************************
/*
  Function:
    SYS_STATUS OTA_Search_ImageVersion(uint32_t ver, char *digest);

  Summary:
    To search a particular image version in OTA DB.

  Description:
    To check if a particular image version is present in OTA DB or not.

  Precondition:
    None.

  Parameters:
    ver - Base version
    digest - digest of base image

  Returns:
    SYS_STATUS code.
*/
// *****************************************************************************
SYS_STATUS OTA_Search_ImageVersion(uint32_t ver, char *digest);
#ifdef  __cplusplus
}
#endif
#endif // __INC_OTA_H__
