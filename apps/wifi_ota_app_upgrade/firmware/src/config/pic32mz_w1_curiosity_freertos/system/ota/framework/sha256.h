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
/*********************************************************************
* Filename:   sha256.h
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding SHA1 implementation.
*********************************************************************/

#ifndef SHA256_H
#define SHA256_H

/*************************** HEADER FILES ***************************/
#include <stddef.h>
#include <stdint.h>
/****************************** MACROS ******************************/
#define OTA_SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/
//typedef unsigned char BYTE;             // 8-bit byte
//typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines

typedef struct {
	uint8_t data[64];
	uint32_t datalen;
	unsigned long long bitlen;
	uint32_t state[8];
} OTA_SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void ota_sha256_init(OTA_SHA256_CTX *ctx);
void ota_sha256_update(OTA_SHA256_CTX *ctx, const uint8_t data[], size_t len);
void ota_sha256_final(OTA_SHA256_CTX *ctx, uint8_t hash[]);


#define OTA_CRYPT_SHA256_CTX                        OTA_SHA256_CTX
#define OTA_CRYPT_SHA256_DIGEST_SIZE 				32
#define OTA_CRYPT_SHA256_Initialize(ctx)			ota_sha256_init(ctx)
#define OTA_CRYPT_SHA256_DataSizeSet(ctx, len)
#define OTA_CRYPT_SHA256_DataAdd(ctx, buf, len)		ota_sha256_update(ctx, buf, len)
#define OTA_CRYPT_SHA256_Finalize(ctx, digest)		ota_sha256_final(ctx, digest)

#endif   // SHA256_H
