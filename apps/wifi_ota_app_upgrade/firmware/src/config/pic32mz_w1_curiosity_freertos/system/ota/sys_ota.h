//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (C) 2021 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
//DOM-IGNORE-END

/*******************************************************************************
  OTA System Service Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    sys_ota.h

  Summary:
     OTA system service interface.

  Description:
    This file defines the interface to the OTA system service.  This
    system service provides a simple APIs to enable PIC32MZW1 OTA 
    Functionality.
 *******************************************************************************/



#ifndef _SYS_OTA_H
#define _SYS_OTA_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "system_config.h"
#include "driver/driver_common.h"
#include "system/system_module.h"
#include "system/ota/framework/ota_config.h"
#include "system/ota/framework/ota.h"
#include "system/ota/framework/downloader.h"
#include "system/ota/framework/ota_image.h"
#include "system/ota/framework/int_flash.h"
#include "system/ota/framework/sha256.h"
#include "system/ota/framework/http_client/http_client.h"
#include "system/ota/framework/ota_database_parser.h"
// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
    // DOM-IGNORE-END

    // *****************************************************************************
    // *****************************************************************************
    // Section: Type Definitions
    // *****************************************************************************
    // *****************************************************************************
#ifdef SYS_OTA_CLICMD_ENABLED        
    /*json url max length*/
    #define JSON_URL_MAX_LEN    1000
#endif
    // *****************************************************************************

    /* Application states

      Summary:
        Application states enumeration

      Description:
        This enumeration defines the valid application states.  These states
        determine the behavior of the application at various times.
     */

    typedef enum {
        /* Service's state machine's initial state. */
        SYS_OTA_REGWIFISRVCALLBCK = 0,

        /* Wait for network connection */
        SYS_OTA_WAITFOR_NETWORK_CONNECTION,

        /*Wait for OTA core to be idle*/
        SYS_OTA_WAITFOR_OTAIDLE,

        /*Registering OTA callback*/
        SYS_OTA_REGOTACALLBCK,

        /*To check if OTA auto update is enabled by user*/
        SYS_OTA_AUTO_CONFIGURATION_CHECK,

        /*Check update availability with OTA server*/
        SYS_OTA_SERVER_UPDATE_CHECK,

        /*Waiting for timer , if OTA is enabled for periodic update check by user*/
        SYS_OTA_WAIT_FOR_OTA_TIMER_TRIGGER,

        /*Check update availability with OTA server, if OTA auto update is enabled by user*/
        SYS_OTA_AUTO_UPDATE_CHECK,

        /*Trigger OTA*/
        SYS_OTA_TRIGGER_OTA,
                
        /*Wait for OTA complete*/
        SYS_OTA_WAIT_FOR_OTA_COMPLETE,

        /*To erase image from External File system*/
        SYS_OTA_ERASE_IMAGE,

        /*To trigger factory reset*/
        SYS_OTA_FACTORY_RESET,

        /*To trigger rollback*/
        SYS_OTA_ROLLBACK,

        /*to update user about current service state*/
        SYS_OTA_UPDATE_USER,

        /*For resetting system if auto reset is enabled by user*/
        SYS_OTA_AUTORESET,

        /*For resetting system on user trigger*/
        SYS_OTA_SYSTEM_RESET,

        /*Service Idle state*/
        SYS_OTA_STATE_IDLE,

        /*Init state for update check*/
        SYS_OTA_UPDATE_CHECK_INIT,

        /*Connecting to server while checking update availability*/
        SYS_OTA_UPDATE_CHECK_CNCT_TO_SRVR,

        /*Reading JSON after download*/
        SYS_OTA_UPDATE_CHECK_READ_JSON,

        /*Checking JSON content for proper value and mandatory fields */
        SYS_OTA_UPDATE_CHECK_JSON_CONTENT,

        /*Complete of OTA update check*/
        SYS_OTA_UPDATE_CHECK_DONE,
        
        /*update check trigger to check update, manually using APIs */
        SYS_OTA_SERVER_UPDATE_CHECK_TRIGGER,
        /* TODO: Define states used by the application state machine. */

    } SYS_OTA_STATES;

    // *****************************************************************************

    /* System ota service control message types

      Summary:
        Identifies the control message for which the client has called 
        the SYS_OTA_CtrlMsg().

      Description:
        Identifies the control message for which the client has called 
        the SYS_OTA_CtrlMsg().

      Remarks:
       The different control messages which can be invoked by the client. 
     */

    typedef enum {
        /* Control message type for registering a ota system service 
           client callback */
        SYS_OTA_REGCALLBACK = 0,

        /* Control message type for checking OTA update availability */
        SYS_OTA_UPDATECHCK,

        /* Control message type for triggering OTA update */
        SYS_OTA_INITIATE_OTA,

        /* Control message type for triggering system reset */
        SYS_OTA_TRIGGER_SYSTEM_RESET,

        /* Control message type for triggering factory reset */
        SYS_OTA_TRIGGER_FACTORY_RESET,

        /* Control message type for triggering Roll back */
        SYS_OTA_TRIGGER_ROLLBACK
    } SYS_OTA_CTRLMSG;

    // *****************************************************************************

    /* System ota service status types

      Summary:
        provide status of ota system.

      Description:
        provide status of ota system.

      Remarks:
   
     */
    typedef enum {
        /* To provide status of OTA system */
        
        /*system in idle state*/
        SYS_OTA_IDLE = 0,
        
        /*waiting for network connection*/        
        SYS_OTA_WAITING_FOR_NETWORK_CONNECTION,    
                
        /*waiting for OTA core to become IDLE*/        
        SYS_OTA_WAITING_FOR_OTACORE_IDLE,        
                
        /*waiting for user defined period*/        
        SYS_OTA_WAITING_FOR_USER_DEFINED_PERIOD,        
                
        /*update check with server start*/
        SYS_OTA_UPDATE_CHECK_START,
          
        /*update check with server failed*/        
        SYS_OTA_UPDATE_CHECK_FAILED,
                
        /*update available in server*/
        SYS_OTA_UPDATE_AVAILABLE,

        /*update not available*/
        SYS_OTA_UPDATE_NOTAVAILABLE,

        /*OTA trigger failed*/
        SYS_OTA_TRIGGER_OTA_FAILED,

        /*Factory reset success*/
        SYS_OTA_FACTORY_RESET_SUCCESS,

        /*factory reset failed*/
        SYS_OTA_FACTORY_RESET_FAILED,

        /*rollback success*/
        SYS_OTA_ROLLBACK_SUCCESS,

        /*rollback failed*/
        SYS_OTA_ROLLBACK_FAILED,

        /*download start*/
        SYS_OTA_DOWNLOAD_START,

        /*download success*/
        SYS_OTA_DOWNLOAD_SUCCESS,

        /*download failed*/
        SYS_OTA_DOWNLOAD_FAILED,

        /*image digest verify start*/
        SYS_OTA_IMAGE_DIGEST_VERIFY_START,

        /*image digest verify success*/
        SYS_OTA_IMAGE_DIGEST_VERIFY_SUCCESS,

        /*image digest verify failed*/
        SYS_OTA_IMAGE_DIGEST_VERIFY_FAILED,
        
        /*Database entry successful*/        
        SYS_OTA_DB_ENTRY_SUCCESS, 
        
        /*erasing image failed*/
        SYS_OTA_IMAGE_ERASE_FAILED,

        /*erasing image success*/
        SYS_OTA_IMAGE_ERASED,

        /*image database full*/
        SYS_OTA_IMAGE_DATABASE_FULL,

        /*not a defined(as mentioned above) status*/
        SYS_OTA_NONE
    } SYS_OTA_STATUS;


    // *****************************************************************************

    /* system service Data

      Summary:
        system service data

      Description:
        This structure holds the system service's data.

      Remarks:
    
     */

    typedef struct {
        /* service current state */
        SYS_OTA_STATES state;
		
        /*to indicate ,update check with server is failed*/
        bool update_check_failed;

        /*to mage states required for parsing JSON content  */
        SYS_OTA_STATES update_check_state;

        /*to keep track of user configure auto update check interval*/
        uint32_t time_interval;

        /*keep track if device connected to network*/
        bool dev_cnctd_to_nw;

        /*to keep track if OTA process is in progress*/
        bool otaFwInProgress;

        /*to keep track if OTA update check with server is in progress*/
        bool otaUpdateCheckInProgress;

        /*to keep track if erase image is triggered by user and in progress*/
        bool otaEraseInProgress;

        /*to keep track if image download success*/
        bool download_success;

        /*to check if user requested for erase functionality*/
        bool erase_request;

        /*check if json contents are proper and required fields are present */
        bool json_content_parse_result;

        /*to track timer callback*/
        volatile bool ota_timer_trigger;

        /*buffer used for JSON content parsing*/
        char json_buf[SYS_OTA_JSON_FILE_MAXSIZE];

        /*control interface result*/
        SYS_OTA_STATUS ota_srvc_status;

    } SYS_OTA_DATA;


    // *****************************************************************************

    /* System ota service Result.

      Summary:
        Result of a ota system service client interface operation.

      Description:
        Result of a ota system service client interface operation.
     * 
      Remarks:
       None.
     */

    typedef enum {
        /* Operation completed with success */
        SYS_OTA_SUCCESS = 0,

        /* Operation Failed.*/
        SYS_OTA_FAILURE,

    } SYS_OTA_RESULT;
    
    // *****************************************************************************

    /* System OTA Instance Configuration

      Summary:
        Used for passing on the configuration related to the OTA service. 

      Remarks:
        None.
     */
    typedef struct {
        /*Auto reset is enabled or not*/
        uint8_t autoreset;

        /*OTA periodic check is enabled or not*/
        bool ota_periodic_check;

        /*OTA auto update is enabled or not*/
        bool ota_auto_update;

        /*periodic check time interval*/
        uint32_t periodic_check_interval;

        /*Application version*/
        uint8_t app_version;

        /*ota server url*/
        char *json_url;
        
#ifdef SYS_OTA_CLICMD_ENABLED        
        /*json url while taking input from CLI command*/
        char json_url_cli[JSON_URL_MAX_LEN];
#endif

    } SYS_OTA_Config;


    
    // *****************************************************************************
    /* Function:
        typedef void (*SYS_OTA_CALLBACK )(uint32_t event, void * data,void *cookie )

       Summary:
        Pointer to a OTA system service callback function.

       Description:
        This data type defines a pointer to a OTA service callback function.
        Callback functions can be registered by client at initialization or using 
        control message type.

       Precondition:
        The OTA service must have been initialized using the SYS_OTA_Initialize
        function if client registering callback using control message.

       Parameters:
        event     - A event value, event can be any  of SYS_OTA_CTRLMSG types.
        data      - OTA service Data.
        cookie    - Client register cookie.

       Returns:
        None.

      Example:
        <code>
        void sys_ota_cb(uint32_t event, void * data, void *cookie) 
        {
            switch (event) 
            {
                    case SYS_OTA_UPDATE_AVAILABLE:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_UPDATE_AVAILABLE\n");
                        break;
                    }
                    case SYS_OTA_UPDATE_NOTAVAILABLE:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_UPDATE_NOTAVAILABLE\n");
                        break;
                    }
                    case SYS_OTA_TRIGGER_OTA_FAILED:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_TRIGGER_OTA_FAILED\n");
                        break;
                    }
                    case SYS_OTA_FACTORY_RESET_SUCCESS:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_FACTORY_RESET_SUCCESS\n");
                        break;
                    }
                    case SYS_OTA_FACTORY_RESET_FAILED:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_FACTORY_RESET_FAILED\n");
                        break;
                    }
                    case SYS_OTA_ROLLBACK_SUCCESS:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_ROLLBACK_SUCCESS\n");
                        break;
                    }
                    case SYS_OTA_ROLLBACK_FAILED:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_ROLLBACK_FAILED\n");
                        break;
                    }
                    case SYS_OTA_DOWNLOAD_START:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_DOWNLOAD_START\n");
                        break;
                    }
                    case SYS_OTA_DOWNLOAD_SUCCESS:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_DOWNLOAD_SUCCESS\n");
                        break;
                    }
                    case SYS_OTA_DOWNLOAD_FAILED:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_DOWNLOAD_FAILED\n");
                        break;
                    }
                    case SYS_OTA_IMAGE_DIGEST_VERIFY_START:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_IMAGE_DIGEST_VERIFY_START\n");
                        break;
                    }
                    case SYS_OTA_IMAGE_DIGEST_VERIFY_SUCCESS:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_IMAGE_DIGEST_VERIFY_SUCCESS\n");
                        break;
                    }
                    case SYS_OTA_IMAGE_VERIFICATION_FAILED:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_IMAGE_VERIFICATION_FAILED\n");
                        break;
                    }
                    case SYS_OTA_IMAGE_ERASE_FAILED:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_IMAGE_ERASE_FAILED\n");
                        break;
                    }
                    case SYS_OTA_IMAGE_ERASED:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_IMAGE_ERASED\n");
                        break;
                    }
                    case SYS_OTA_IMAGE_DATABASE_FULL:
                    {

                        SYS_CONSOLE_PRINT("SYS_OTA_IMAGE_DATABASE_FULL\n");
                        break;
                    }
                    default:
                    {

                        break;
                    }
            }
        }
        void APP_Initialize(void) {
        appData.state = APP_STATE_INIT;
       }
    
        void APP_Tasks(void) {

        switch (appData.state) {
            case APP_STATE_INIT:
            {
                SYS_OTA_CtrlMsg(SYS_OTA_REGCALLBACK, sys_ota_cb, sizeof (uint8_t *));
                appData.state=APP_STATE_SERVICE_TASKS;
                break;
            }

            case APP_STATE_SERVICE_TASKS:
            {

                break;
            }
            default:
            {
                break;
            }
        }
        }

        </code>

      Remarks:
        None.
     */
    typedef void (*SYS_OTA_CALLBACK)(uint32_t event, void * data, void *cookie);

    
    // *****************************************************************************
    
    /* Function:
       SYS_OTA_RESULT SYS_OTA_CtrlMsg(uint32_t event, void *buffer, uint32_t length);

      Summary:
        Returns success/ failure for the update check/ota trigger/system reset operations asked by client. 

      Description:
        This function is used to make control message request
        (update check,initiate OTA,system reset,factory reset,register callback) to OTA system service.

      Precondition:
           The SYS_OTA_Initialize function should have been called before calling
           this function.

      Parameters:
       
           event    - A event value, event can be any of SYS_OTA_CTRLMSG types
           buffer   - Control message data input.
           length   - size of buffer data


      Returns:
           return SYS_OTA_RESULT.

      Example:
            <code>

            bool ota_app_reg_cb(void) {
                if (SYS_OTA_SUCCESS == SYS_OTA_CtrlMsg(SYS_OTA_REGCALLBACK, sys_ota_cb, sizeof (uint8_t *))) {
                    return true;
                } else {
                    return false;
                }
            }

            </code>

      Remarks:
        None
     */
    SYS_OTA_RESULT SYS_OTA_CtrlMsg(uint32_t event, void *buffer, uint32_t length);

    // *****************************************************************************

    /* Function:
       SYS_OTA_RESULT SYS_OTA_CtrlMsg(uint32_t event, void *buffer, uint32_t length);

      Summary:
        Returns success/ failure for the update check/ota trigger/system reset operations asked by client. 

      Description:
        This function is used to set OTA server and image related parameters
        

      Precondition:
           The SYS_OTA_Initialize function should have been called before calling
           this function.

      Parameters:
       
           url      - pointer to server 
           version  - version number of image present in OTA server
           digest   - SHA256 key of OTA image


      Returns:
            None

      Example:
            <code>

            if(ota_app_reg_cb() == true)
                {
                    uint8_t version = 2;
                    char *digest = "ef90bf8bf7fd96205c3240c31e1a378430e7a8f053a300e5c91ebe64fefea197";
                    SYS_OTA_SET_PARAMETERS("http://192.168.43.173:8000/wifi_ota_demo.bin", version , digest);
                    appData.state = APP_STATE_SERVICE_TASKS;
                }

            </code>

      Remarks:
        None
     */

    void SYS_OTA_SET_PARAMETERS(char *url, uint8_t version, char *digest);

    // *****************************************************************************

    /* Function:
        SYS_OTA_Initialize(void)

       Summary:
            Initializes the System OTA module.

       Description:
            This function is used for initializing the data structures of the OTA service.
            OTA service supports only one instance of client.

       Parameters:
            None

       Returns:
            None

       Example:
            <code>

            </code>


      Remarks:
            This routine can only be called once during system initialization.
            If the OTA system service is enabled using MHC, then auto generated 
            code will take care of system OTA initialization.
     */
    void SYS_OTA_Initialize(void);


    
    // *****************************************************************************
    
    /* Function:
       void SYS_OTA_Tasks( void )

      Summary:
        Maintains the OTA System tasks and functionalities. 

      Description:
        This function is used to run the various tasks and functionalities 
        of OTA system service.

      Precondition:
           The SYS_OTA_Initialize function should have been called before calling
           this function.

      Parameters:
            None

      Returns:
            None

      Example:
            <code>
            while(1)
            {
            
                SYS_OTA_Task();
            
            }

            </code>

      Remarks:
        If the OTA system service is enabled using MHC, then auto generated code 
        will take care of system task execution.
     */
    void SYS_OTA_Tasks(void);

    //DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _SYS_OTA_H */

/*******************************************************************************
 End of File
 */

