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
0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0x24, 0x40, 0x95, 
0x76, 0xDC, 0xE7, 0x73, 0x20, 0xCF, 0xC1, 0x99, 0x09, 0x7E, 
0xCD, 0xF3, 0xF7, 0xAF, 0xE8, 0xA2, 0x6E, 0x10, 0x72, 0x43, 
0xCB, 0x52, 0x7E, 0x96, 0x63, 0xCA, 0xA3, 0x30, 0xFB, 0xA1, 
0xB8, 0x45, 0x06, 0x00, 0xAA, 0xD8, 0xF8, 0xF5, 0x84, 0x86, 
0x32, 0x24, 0xDE, 0xAF, 0x9E, 0xFE, 0x28, 0x6E, 0x2D, 0x0F, 
0x61, 0x11, 0x5E, 0xDF, 0x35, 0x55, 0xBF, 0xF4, 0xAE, 0x7B, 
0xF4, 
};

    static const int sizeof_pubKey = sizeof(pubKey);
#endif
#endif /*_PUBKEY_H_*/
