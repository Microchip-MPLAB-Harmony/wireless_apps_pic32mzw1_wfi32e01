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
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/
//typedef unsigned char BYTE;             // 8-bit byte
//typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines

typedef struct {
	uint8_t data[64];
	uint32_t datalen;
	unsigned long long bitlen;
	uint32_t state[8];
} SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const uint8_t data[], size_t len);
void sha256_final(SHA256_CTX *ctx, uint8_t hash[]);


#define CRYPT_SHA256_CTX                        SHA256_CTX
#define CRYPT_SHA256_DIGEST_SIZE 				32
#define CRYPT_SHA256_Initialize(ctx)			sha256_init(ctx)
#define CRYPT_SHA256_DataSizeSet(ctx, len)
#define CRYPT_SHA256_DataAdd(ctx, buf, len)		sha256_update(ctx, buf, len)
#define CRYPT_SHA256_Finalize(ctx, digest)		sha256_final(ctx, digest)

#endif   // SHA256_H
