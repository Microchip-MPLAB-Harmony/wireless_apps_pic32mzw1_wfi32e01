/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    ota_patch.h
    
  Summary:
    Interface for patch library.

  Description:
    This file contains the interfaces to access OTA patch library.
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
#include "osal/osal.h"



// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
#define     OTA_PATCH_BUFFER_SIZE       1024
// *****************************************************************************
/* OTA patch structure.

  Summary:
    structure of patch functionality related parameters.

  Description:
    structure of patch functionality related parameters.

  Remarks:
   None.
*/
typedef struct OTA_PATCH_ELEMENTS {
    /*source/base image file name in OTA DB*/
    char source_file[500];
    
    /*patch image file name extracted from JSON manifest*/
    char patch_file[100];
    
    /*target image file name extracted from JSON manifest*/
    char target_file[100];
} OTA_PATCH_PARAMS_t;


// *****************************************************************************
/*
  Function:
    OTA_PatchProcess(OTA_PATCH_PARAMS_t *patch_param) 

  Summary:
    To process patch functionality.

  Description:
    To process patch functionality.

  Parameters:
    pointer of type OTA_PATCH_PARAMS_t.

  Returns:
    None.
*/
// *****************************************************************************
SYS_STATUS OTA_ProcessPatch(OTA_PATCH_PARAMS_t *patch_param);

// *****************************************************************************
/*
  Function:
    OTA_PatchProgressStatus(void) 

  Summary:
    To get patch progress status.

  Description:
    To get patch progress status.

  Parameters:
    None.

  Returns:
    None.
*/
// *****************************************************************************
uint8_t OTA_PatchProgressStatus(void);





