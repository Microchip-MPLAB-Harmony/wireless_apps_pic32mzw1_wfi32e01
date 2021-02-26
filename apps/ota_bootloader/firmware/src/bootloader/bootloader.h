/*******************************************************************************
  MPLAB Harmony Sample Library Module Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    bootloader.h

  Summary:
    Bootloader API definitions.

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

#ifndef __INC_BOOTLOADER_H__
#define __INC_BOOTLOADER_H__


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
*/

#ifdef	__cplusplus
extern "C" {
#endif

// *****************************************************************************
/*
  Function:
    void Bootloader_Initialize (void);

  Summary:
    Initializes the Bootloader Library.

  Description:
    This function is used to initialize the Bootloader Library.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    This routine must be called before other Bootloader Library functions.

*/
// *****************************************************************************
void  Bootloader_Initialize(void);

// *****************************************************************************
/* Function:
    void Bootloader_Tasks (SYS_MODULE_INDEX index);

  Summary:
    Maintains the Bootloader module state machine. It manages the Bootloader 
    module object list items and responds to Bootloader module primitive events.

  Description:
    This function maintains the Bootloader module state machine and manages the 
    Bootloader Module object list items and responds to Bootloader Module events.
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
        Bootloader_Tasks ();

        // Do other tasks
    }
    </code>

  Remarks:
    This function is normally not called directly by an application.
*/
// *****************************************************************************
void Bootloader_Tasks(void);

#ifdef  __cplusplus
}
#endif
#endif //__INC_BOOTLOADER_H__