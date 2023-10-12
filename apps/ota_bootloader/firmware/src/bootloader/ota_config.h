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




#ifndef __INC_OTA_CONFIG_H__
#define __INC_OTA_CONFIG_H__
#include <stdint.h>
#include "user.h"
#include "bootloader.h"
#ifdef  __cplusplus
extern "C" {
#endif

#define OTA_VERSION               "1.0.0"


#define FLASH_SECTOR_SIZE         4096
    
    
#ifdef SYS_OTA_BOOTLOAD_FROM_DEDICATED_BOOTFLASH_ENABLED
/*Boot loader will be loaded from the dedicated boot memory and the boot control area resides
 * at beginning of the program memory */    
#define APP_IMG_SLOT_ADDR         0x00000000  //0x00020000  
#define APP_BOOT_CTL_SLOT_ADDR         0x00000000  //0x00020000  
/*Pointer to image boot control area*/     
#define APP_IMG_BOOT_CTL         ((volatile const FIRMWARE_IMAGE_HEADER *)0xB0000000)    
/*Pointer to image boot address*/      
#define APP_IMG_BOOT_ADDR        0x90001000
    
#else 
/*Boot loader  and the boot control area resides at beginning of the program memory */     
    
#define APP_IMG_SLOT_ADDR         0x00020000  
/*Pointer to image boot control area*/     
#define APP_IMG_BOOT_CTL         ((volatile const FIRMWARE_IMAGE_HEADER *)0xb0020000)    
/*Pointer to image boot address*/      
#define APP_IMG_BOOT_ADDR        0xb0021000  
#endif
    
#define FACTORY_RESET_IMG_SIZE    0xDF000
#define OTA_BOOT_CTL_SIZE         FLASH_SECTOR_SIZE
    
#ifndef SYS_OTA_FS_ENABLED
#ifdef SYS_OTA_FILE_JUMP_ENABLE
/*Jump address of the new image.This shall be updated with the proper memory calculation*/
#define APP_IMG_BOOT_ADDR_2      0x900f8200
#endif
#endif



//------------------------------------------------------------------------------    

#ifdef  __cplusplus
}
#endif
#endif // __INC_OTA_CONFIG_H__