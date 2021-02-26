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
Copyright (c) 2014-2015 released Microchip Technology Inc.  All rights reserved.

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

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

typedef int (*OTA_COMPLETION_CALLBACK)(SYS_STATUS result);

// Passed a pointer to OTA_Start()
typedef struct
{
    const char *server_name;
    const char *file_name;
    uint32_t    port;
} OTA_DOWNLOADER_HTTP_PARAMS;

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
SYS_STATUS OTA_Start(void *downloader_param, OTA_COMPLETION_CALLBACK callback);


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
    SYS_STATUS OTA_SignImage(OTA_COMPLETION_CALLBACK callback);

  Summary:
    Validate the current firmware.

  Description:
    Application firmware must call this function once the image passes the safe
  
  Precondition:
    None.

  Parameters:
    None.

  Returns:
    SYS_STATUS code.
*/
// *****************************************************************************
SYS_STATUS OTA_SignImage(OTA_COMPLETION_CALLBACK callback);


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
SYS_STATUS OTA_Rollback(OTA_COMPLETION_CALLBACK callback);

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
SYS_STATUS OTA_FactoryReset(OTA_COMPLETION_CALLBACK callback);


#ifdef  __cplusplus
}
#endif
#endif // __INC_OTA_H__