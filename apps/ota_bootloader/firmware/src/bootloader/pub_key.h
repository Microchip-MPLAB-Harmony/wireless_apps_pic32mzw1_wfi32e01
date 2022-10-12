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
   pub_key.h

  Summary:
    The header file defines an array of public key, that will be used for Signature verification.

  Description:
    The header file defines an array of public key, that will be used for Signature verification.
*******************************************************************************/
#include "../bootloader/bootloader.h"
#ifndef _PUBKEY_H_
#define _PUBKEY_H_

#ifdef SYS_OTA_SECURE_BOOT_ENABLED 
    static const unsigned char pubKey[] =
{
0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 
0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 
0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0x6E, 0xEE, 0xF6, 
0x00, 0x3E, 0x1B, 0xB1, 0x8B, 0x05, 0xC3, 0x94, 0x51, 0x71, 
0x19, 0x85, 0x54, 0x45, 0x35, 0x52, 0xCE, 0x0F, 0xB8, 0xDE, 
0xF7, 0x4E, 0xAB, 0x2B, 0x4B, 0x29, 0x52, 0xAF, 0xE3, 0x5A, 
0x0E, 0x6D, 0x3E, 0x17, 0xE9, 0xCC, 0x8E, 0xFE, 0x76, 0x52, 
0xCB, 0x9F, 0xA9, 0x78, 0xF8, 0x01, 0xFE, 0x38, 0x06, 0x4A, 
0xD0, 0x42, 0x2C, 0xBC, 0x04, 0xC8, 0x8C, 0xF3, 0x9E, 0x21, 
0xCE, 
};

    static const int sizeof_pubKey = sizeof(pubKey);
#endif
#endif /*_PUBKEY_H_*/
