/*******************************************************************************
  MPLAB Harmony Sample Library Module Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    sample_module.h

  Summary:
    Sample of a MPLAB Harmony library module.

  Description:
    This is a sample of a MPLAB Harmony library interface module.  This file 
    defines the system and application interface.
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

#ifndef INC_DOWNLOADER_H
#define	INC_DOWNLOADER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
*/
//#include "ota.h"
#include "system_config.h"
#include "driver/driver_common.h"

#ifdef	__cplusplus
extern "C" {
#endif

//#define APP_USR_CONTEXT 1
// *****************************************************************************
/*
  Function:
    void DOWNLOADER_Initialize (void);

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
void DOWNLOADER_Initialize(void);


// *****************************************************************************
/* Function:
    void DOWNLOADER_Tasks (void);

  Summary:
    Maintains the DOWNLOADER_Tasks module state machine. 

  Description:
    This function maintains the DOWNLOADER_Tasks module state machine and manages
    the DOWNLOADER_Tasks Module object list items and responds to 
    DOWNLOADER_Tasks Module events.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.
*/
// *****************************************************************************
void DOWNLOADER_Tasks(void);


// *****************************************************************************
/*
  Function:
    void DOWNLOADER_Deinitialize (void);

  Summary:
    Deinitializes the DOWNLOADER Library.

  Description:
    This function is used to initialize the DOWNLOADER Library.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.
*/
// *****************************************************************************
void DOWNLOADER_Deinitialize(void);


// *****************************************************************************
/*
  Function:
    DRV_HANDLE DOWNLOADER_Open(const DRV_IO_INTENT ioIntent);

  Summary:
    Open OTA Image Downloader library.

  Description:
    The driver starts OTA downloading. 
    For example, if downloader is HTTP client transport, HTTP DOWNLOADER does
        1. Connect OTA server
        2. Send "HTTP-GET" request

  Precondition:
    None.

  Parameters:
    ioIntent - DRV_IO_INTENT_READ.

  Returns:
    Driver handle.
*/
// *****************************************************************************
DRV_HANDLE DOWNLOADER_Open(void * param);


// *****************************************************************************
/*
  Function:
    int DOWNLOADER_Read(DRV_HANDLE handle,
                        unsigned char* buffer,
                        const int maxsize);

  Summary:
    Obtain N bytes of data from OTA image stream.

  Description:
    

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    RX'ed byte length.
*/
// *****************************************************************************  
int DOWNLOADER_Read(DRV_HANDLE handle, unsigned char* buffer, int maxsize);

// *****************************************************************************
/*
  Function:
    void DOWNLOADER_Close(DRV_HANDLE handle);

  Summary:
    Close DOWNLOADER.

  Description:
    DOWNLOADER stops the OTA image downloading. The function may called in 
    the middle of data streaming if OTA framework identifys a data error while
    copying the data.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.
*/
// *****************************************************************************
void DOWNLOADER_Close(DRV_HANDLE handle);



#ifdef  __cplusplus
}
#endif
#endif	/* INC_DOWNLOADER_H */

