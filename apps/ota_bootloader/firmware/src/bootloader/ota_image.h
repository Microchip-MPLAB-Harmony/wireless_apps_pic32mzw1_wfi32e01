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
   ota_image.h

  Summary:
    The header file defines Bootloader/OTA firmware image layout.

  Description:
    This file defines the system and application interface.
*******************************************************************************/



#ifndef __INC_OTA_IMAGE_H__
#define __INC_OTA_IMAGE_H__
#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------

#define FIRMWARE_IMAGE_HEADER_VERSION 1

typedef struct {
    uint8_t     status;
    uint8_t     slot;
    uint8_t     order;
    uint8_t     type;
    
    uint32_t    sz;
    uint32_t    version;
    uint32_t    boot_addr;

    uint8_t     digest[32];

    uint8_t     rsvd[208];
} FIRMWARE_IMAGE_HEADER;

#define FIRMWARE_IMAGE_HEADER_STATUS_BYTE     (0)

//---------------------------------------------------------------------------
typedef enum {
    IMG_STATUS_INVALID         = 0xFF,
    IMG_STATUS_DOWNLOADED      = 0xFE,
    IMG_STATUS_UNBOOTED        = 0xFC,
    IMG_STATUS_VALID           = 0xF8,
    IMG_STATUS_DISABLED        = 0xF0
} IMG_STATUS;

//---------------------------------------------------------------------------
typedef enum {
    IMG_TYPE_FLASH_PROGRAMMER  = 0x00,
    IMG_TYPE_PRODUCTION        = 0x01,
    IMG_TYPE_DEBUG             = 0x02,
    IMG_TYPE_FACTORY_RESET     = 0x03,
} IMG_TYPE;

#ifdef  __cplusplus
}
#endif
#endif // __INC_OTA_IMAGE_H__
