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
  OTA Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_ota.c

  Summary:
    This file contains the source code for the OTA application.

  Description:
    This file contains the source code for the ota application. It includes necessary callback function
    and respective states. It includes logic to register callback function. Developer can build own logic
 *  on the top of it. 
 *******************************************************************************/


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app_ota.h"
#include "definitions.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************


void sys_ota_cb(uint32_t event, void * data, void *cookie) {
    switch (event) {
        
        case SYS_OTA_UPDATE_CHECK_START:
        {
            /*OTA update check start . Developer can build their own custom logic */
            SYS_CONSOLE_PRINT("SYS_OTA_UPDATE_CHECK_START\r\n");
            break;
        }
        case SYS_OTA_UPDATE_CHECK_FAILED:
        {
            /*OTA update check failed . Developer can build their own custom logic */
            SYS_CONSOLE_PRINT("SYS_OTA_UPDATE_CHECK_FAILED\r\n");
            break;
        }
        case SYS_OTA_UPDATE_AVAILABLE:
        {
            /*OTA update available . Developer can build their own custom logic */
            SYS_CONSOLE_PRINT("SYS_OTA_UPDATE_AVAILABLE\r\n");
            break;
        }
        case SYS_OTA_UPDATE_NOTAVAILABLE:
        {
            /*OTA update not available. Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_UPDATE_NOTAVAILABLE\r\n");
            break;
        }
        case SYS_OTA_TRIGGER_OTA_FAILED:
        {
            /*OTA trigger failed . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_TRIGGER_OTA_FAILED\r\n");
            break;
        }
        case SYS_OTA_FACTORY_RESET_SUCCESS:
        {
            /*OTA Factory reset success . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_FACTORY_RESET_SUCCESS\r\n");
            break;
        }
        case SYS_OTA_FACTORY_RESET_FAILED:
        {
            /*OTA Factory reset failed . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_FACTORY_RESET_FAILED\r\n");
            break;
        }
        case SYS_OTA_ROLLBACK_SUCCESS:
        {
            /*OTA rollback success . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_ROLLBACK_SUCCESS\r\n");
            break;
        }
        case SYS_OTA_ROLLBACK_FAILED:
        {
            /*OTA rollback failed . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_ROLLBACK_FAILED\r\n");
            break;
        }
        case SYS_OTA_PATCH_BASEVERSION_NOTFOUND:
        {
            /*Base image version for patching is not found in OTA DB . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_PATCH_BASEVERSION_NOTFOUND\r\n");
            break;
        }
        case SYS_OTA_PATCH_EVENT_START:
        {
            /*Patch event initiated, now onwards all callback will be in patch event context. Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_PATCH_EVENT_START\r\n");
            break;
        }
        case SYS_OTA_PATCH_EVENT_COMPLETED:
        {
            /*Patch event completed. Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_PATCH_EVENT_COMPLETED\r\n");
            break;
        }
        case SYS_OTA_DOWNLOAD_START:
        {
            /*OTA image download start . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_DOWNLOAD_START\r\n");
            break;
        }
        case SYS_OTA_DOWNLOAD_SUCCESS:
        {
            /*OTA image download success . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_DOWNLOAD_SUCCESS\r\n");
            break;
        }
        case SYS_OTA_DOWNLOAD_FAILED:
        {
            /*OTA image download failed . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_DOWNLOAD_FAILED\r\n");
            break;
        }
        case SYS_OTA_IMAGE_DIGEST_VERIFY_START:
        {
            /*OTA image digest verify start . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_IMAGE_DIGEST_VERIFY_START\r\n");
            break;
        }
        case SYS_OTA_IMAGE_DIGEST_VERIFY_SUCCESS:
        {
            /*OTA image digest verify success . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_IMAGE_DIGEST_VERIFY_SUCCESS\r\n");
            break;
        }
        case SYS_OTA_IMAGE_DIGEST_VERIFY_FAILED:
        {
            /*OTA image digest verify failed . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_IMAGE_VERIFICATION_FAILED\r\n");
            break;
        }
        case SYS_OTA_PATCH_IMAGE_DIGEST_VERIFY_START:
        {
            /*Patch image digest verify start . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_PATCH_IMAGE_DIGEST_VERIFY_START\r\n");
            break;
        }
        case SYS_OTA_PATCH_IMAGE_DIGEST_VERIFY_SUCCESS:
        {
            /*Patch OTA image digest verify success . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_PATCH_IMAGE_DIGEST_VERIFY_SUCCESS\r\n");
            break;
        }
        case SYS_OTA_PATCH_IMAGE_DIGEST_VERIFY_FAILED:
        {
            /*Patch OTA image digest verify failed . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_PATCH_IMAGE_DIGEST_VERIFY_FAILED\r\n");
            break;
        }
        case SYS_OTA_DB_ENTRY_SUCCESS:
        {
            /*OTA database entry successful . Developer can build their own custom logic*/
            /*For Manual reset, user must trigger system reset, after system reach this state only*/
            /*Do not use API call from here. Use variables to get the status*/
            SYS_CONSOLE_PRINT("SYS_OTA_DB_ENTRY_SUCCESS\r\n");
            break;
        }
        case SYS_OTA_IMAGE_ERASE_FAILED:
        {
            /*OTA image erase failed . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_IMAGE_ERASE_FAILED\r\n");
            break;
        }
        case SYS_OTA_IMAGE_ERASED:
        {
            /*OTA image erase success . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_IMAGE_ERASED\r\n");
            break;
        }
        case SYS_OTA_IMAGE_DATABASE_FULL:
        {
            /*OTA image database full . Developer can build their own custom logic*/
            SYS_CONSOLE_PRINT("SYS_OTA_IMAGE_DATABASE_FULL\r\n");
            break;
        }
        default:
        {
            /*unknown state*/
            break;
        }
    }
}
// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
 */

/*******************************************************************************
  Function:
    bool ota_app_reg_cb(void)

  Remarks:
    Registering OTA service callback function.
  Return:
    True: if success
    False: if fail
 */
bool ota_app_reg_cb(void) {
    if (SYS_OTA_SUCCESS == SYS_OTA_CtrlMsg(SYS_OTA_REGCALLBACK, sys_ota_cb, sizeof (uint8_t *))) {
        return true;
    } else {
        return false;
    }
}


/*******************************************************************************
 End of File
 */
