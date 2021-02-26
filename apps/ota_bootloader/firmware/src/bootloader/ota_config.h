/*******************************************************************************
 Module for Microchip OTA Library

  Company:
    Microchip Technology Inc.

  File Name:
   ota_config.h

  Summary:
    The header file defines Bootloader/OTA firmware image layout.

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


#ifndef __INC_OTA_CONFIG_H__
#define __INC_OTA_CONFIG_H__
#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define OTA_VERSION               "1.0.0"

#define INT_FLASH_SIZE            0x100000
#define INT_FLASH_LOGICAL_ADDR    0xb0000000
#define FLASH_SECTOR_SIZE         4096

#define APP_IMG_SLOT_ADDR         0x00010000  // 0x10000(64KB) for loadable proj, otherwise 0.
#define APP_IMG_BOOT_CTL_OFFSET   0x00000000  // Fixed: BOOT_CTL offset in APP_SLOT
#define APP_IMG_ENTRY_OFFSET      0x00001000  // Fixed: Entry point offset in APP_SLOT 
   
#define IMAGESTORE_ADDR           0x00000000  // IMAGESTORE address in external flash.
#define IMAGESTORE_NUM_SLOT       3
#define IMAGESTORE_SLOT_SIZE      0xd0000

#define OTA_BOOT_CTL_SIZE         FLASH_SECTOR_SIZE

//------------------------------------------------------------------------------    
#define APP_IMG_ADDR             (APP_IMG_SLOT_ADDR + APP_IMG_ENTRY_OFFSET)
#define APP_IMG_BOOT_ADDR        (APP_IMG_SLOT_ADDR + APP_IMG_ENTRY_OFFSET + INT_FLASH_LOGICAL_ADDR)
#define APP_IMG_BOOT_CTL_ADDR    (APP_IMG_SLOT_ADDR + APP_IMG_BOOT_CTL_OFFSET)
#define APP_IMG_BOOT_CTL         ((volatile const FIRMWARE_IMAGE_HEADER *)(APP_IMG_BOOT_CTL_ADDR + INT_FLASH_LOGICAL_ADDR))
//------------------------------------------------------------------------------    

#ifdef  __cplusplus
}
#endif
#endif // __INC_OTA_CONFIG_H__