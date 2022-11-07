/*******************************************************************************
 MPLAB Harmony Web Socket Server system service Source File

  File Name:
    sys_wss.c

  Summary:
    Source code for the Web Socket Server system service implementation.

  Description:
    This file contains the source code for the Web Socket Server system service
    implementation.
 *******************************************************************************/

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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "definitions.h"
#include <wolfssl/wolfcrypt/coding.h>
#include <wolfssl/wolfcrypt/sha.h>
#include <wolfssl/wolfcrypt/hash.h>
#include <strings.h>

typedef struct {
    SYS_MODULE_OBJ wssNetHandle;
    SYS_NET_Config wssNetCfg;
    SYS_WSS_STATE wssState;
    SYS_WSS_CALLBACK userCallback;
    void * userCookie;
    SYS_WSS_HANDSHAKE_CTXT wssHandshake;
    uint8_t recv_buffer[SYS_WSS_MAX_RX_BUFFER + 1];
    char sHandshake[256];
    uint32_t kaTimerCount;
} SYS_WSS_OBJ;

SYS_WSS_OBJ g_wssSrvcObj[SYS_WSS_MAX_NUM_CLIENTS];
static SYS_TIME_HANDLE kaTimerHandle;
static volatile bool isBoot=true;
static volatile bool kaTimerExpired=false;

#if (1 == SYS_WSS_ENABLE_DEBUG)
#define WSS_DEBUG_PRINT(...)            \
    do                                  \
    {                                   \
        SYS_CONSOLE_PRINT(__VA_ARGS__); \
    } while (0);
#else
#define WSS_DEBUG_PRINT(...)
#endif

//No support for Sec-WebSocket-Extensions
#define WSS_HANDSHAKE_TEMPLATE "HTTP/1.1 101 Switching Protocols\r\n" \
                               "Sec-WebSocket-Accept: %s\r\n"         \
                               "Upgrade: websocket\r\n"               \
                               "Connection: Upgrade\r\n"              \
                               "Sec-WebSocket-Version: 13\r\n"        \
                               "\r\n"

#define WSS_ERR_TEMPLATE    "HTTP/1.1 %u %s \r\n" \
                            "Content-Type: text/html \r\n" \
                            "Content-Length: %u \r\n\r\n" \
                            "<!doctype html>\r\n" \
                            "<html>\r\n" \
                            "<head><title>Error %03d</title></head>\r\n" \
                            "<body>\r\n" \
                            "<h2>Error %03d</h2>\r\n" \
                            "<p>%s</p>\r\n" \
                            "</body>\r\n" \
                            "</html>\r\n"
static SYS_WSS_RESULT wssFormatServerHandshake(int32_t clientIndex) {
    sprintf(g_wssSrvcObj[clientIndex].sHandshake, WSS_HANDSHAKE_TEMPLATE, g_wssSrvcObj[clientIndex].wssHandshake.serverKey);

    WSS_DEBUG_PRINT("\r\nWSS: server handshake response\r\n");
    WSS_DEBUG_PRINT("%s", g_wssSrvcObj[clientIndex].sHandshake);

    //Successful processing
    return SYS_WSS_SUCCESS;
}

static SYS_WSS_RESULT wssSendErrorResponse( SYS_WSS_RESULT res,int32_t clientIndex)
{
    uint32_t length;
    uint16_t statusCode;
    char err_txMsg[500],message[100],statusMsg[50];
    SYS_WSS_RESULT result = SYS_WSS_SUCCESS;
    if (SYS_WSS_FAILURE!=res){
        statusCode=400;
        strcpy(statusMsg,"Bad Request");
        strcpy(message,"Invalid handshake request");
    }
    else{
        statusCode=500;
        strcpy(statusMsg,"Internal Server Error");
        strcpy(message,"Encountered an internal error.Server unable to process the request");
    }  


    length = strlen(WSS_ERR_TEMPLATE) + strlen(message);
    sprintf(err_txMsg,WSS_ERR_TEMPLATE,statusCode,statusMsg,length, statusCode, statusCode, message);
    WSS_DEBUG_PRINT("\r\n  Error Message %s \r\n", err_txMsg);
    SYS_NET_SendMsg(g_wssSrvcObj[clientIndex].wssNetHandle, (uint8_t *) err_txMsg, length);
    return result;
}


static void wssUserCallback(SYS_WSS_EVENTS event, void* data, int32_t clientIndex) {

    g_wssSrvcObj[clientIndex].userCallback(event, data, clientIndex, g_wssSrvcObj[clientIndex].userCookie);
}

static size_t wssFormatFrameHeader(bool fin, SYS_WSS_FRAME type, size_t payloadLen, uint8_t *buffer) {

    size_t frameLen;
    SYS_WSS_FRAME_HEADER *header;
    /*pack data into frame*/
    header = (SYS_WSS_FRAME_HEADER *) buffer;
    header->resvd = 0;
    header->fin = fin;
    header->opcode = type;
    /*Set mask 0 for frames send from server*/
    header->mask = FALSE;

    frameLen = sizeof (SYS_WSS_FRAME_HEADER);
    WSS_DEBUG_PRINT("\r\nwssFormatFrameHeader :payloadlen %u \r\n", payloadLen);
    
    //Check the length of the payload
    if (payloadLen <= 125) {
        header->payloadLen = payloadLen;
    } else if (payloadLen <= 65535) {
        header->payloadLen = 126;
        frameLen += sizeof (uint16_t);
        header->extPayloadLen[1] = payloadLen & 0xff;
        header->extPayloadLen[0] = payloadLen >>8 ;
        
    } else {
        header->payloadLen = 127;
        frameLen += sizeof (uint64_t);

    }
    //Successfully updated the frame
    return frameLen;
}


static SYS_WSS_RESULT wssCloseConnection(SYS_WSS_STATUS_CODE stausCode, uint8_t *data, size_t dataLen, int32_t clientIndex) {
    SYS_WSS_RESULT result = SYS_WSS_SUCCESS;
    uint8_t buffer[SYS_WSS_MAX_RX_BUFFER], *bufferOffset;
    size_t frameLen;
    frameLen = wssFormatFrameHeader(1, SYS_WSS_FRAME_CLOSE, (dataLen+SYS_WSS_STATUS_CODE_LEN), (uint8_t *) buffer);
    *(buffer + frameLen) = (uint8_t) ((stausCode & 0xFF00) >> 8);
    *(buffer + frameLen + 1) = (uint8_t) (stausCode & 0x00FF);
    WSS_DEBUG_PRINT(" \r\n close frame = %d %d\r\n", *(buffer + frameLen), *(buffer + frameLen + 1));
   if((NULL!=data) && (0!=dataLen)){
        bufferOffset = (buffer + frameLen + sizeof (uint16_t));
        memcpy(bufferOffset, data, dataLen);
    }
    WSS_DEBUG_PRINT("  close frame =   %d data = %s \r\n", buffer, data);
    SYS_NET_SendMsg(g_wssSrvcObj[clientIndex].wssNetHandle, buffer, (frameLen + SYS_WSS_STATUS_CODE_LEN + dataLen));
    return result;
}

static SYS_WSS_RESULT wssSendResponse(bool fin, SYS_WSS_FRAME type, uint8_t *data, size_t dataLen, int32_t clientIndex) {
    uint8_t buffer[SYS_WSS_MAX_RX_BUFFER],result = SYS_WSS_SUCCESS;;
    size_t frameLen = 0;
    frameLen = wssFormatFrameHeader(fin, type, dataLen, (uint8_t *) buffer);
    memcpy((buffer + frameLen), data, dataLen);
    WSS_DEBUG_PRINT("\r\nWSS: TxResponse %s , with length %d..\r\n", data, dataLen);

    result=SYS_NET_SendMsg(g_wssSrvcObj[clientIndex].wssNetHandle, buffer, (dataLen + frameLen));
    if (0 > result){
        result = SYS_WSS_FAILURE;  
        WSS_DEBUG_PRINT("\r\nWSS:Sending response to the client failed\r\n");
    }
    return result;
}

static SYS_WSS_RESULT wssSendPongMessage( uint8_t *data, size_t dataLen, int32_t clientIndex) {
    int8_t result = 0;
    uint8_t buffer[SYS_WSS_MAX_RX_BUFFER];
    size_t frameLen = 0;
    frameLen = wssFormatFrameHeader(1, SYS_WSS_FRAME_PONG, dataLen, (uint8_t *) buffer);
    memcpy((buffer + frameLen), data, dataLen);
    WSS_DEBUG_PRINT("\r\nWSS: Pong Response %s , with length %d..\r\n", data, dataLen);
    
    result = SYS_NET_SendMsg(g_wssSrvcObj[clientIndex].wssNetHandle, buffer, (dataLen + frameLen));
    if (0 > result){
        result = SYS_WSS_FAILURE;  
        WSS_DEBUG_PRINT("\r\nWSS: Sending Pong message failed \r\n" );
    }
    
    return result;
}


static SYS_WSS_RESULT wssSendPingMessage( uint8_t *data, size_t dataLen, int32_t clientIndex) {
    uint32_t result = SYS_WSS_SUCCESS;
    uint8_t buffer[SYS_WSS_MAX_RX_BUFFER];
    size_t frameLen = 0;
    frameLen = wssFormatFrameHeader(1, SYS_WSS_FRAME_PING, dataLen, (uint8_t *) buffer);
    memcpy((buffer + frameLen), data, dataLen);
    WSS_DEBUG_PRINT("\r\nWSS: Response %s , with length %d..\r\n", data, dataLen);
    result = SYS_NET_SendMsg(g_wssSrvcObj[clientIndex].wssNetHandle, buffer, (dataLen + frameLen));
    if (0 > result){
        result = SYS_WSS_FAILURE;   
    }
    return result;
}

static SYS_WSS_RESULT wssGenerateServerKey(int32_t clientIndex) {
    SYS_WSS_RESULT result = SYS_WSS_SUCCESS;
    uint32_t key_len;

    byte buffer[SYS_WSS_SERVER_KEY_SIZE] = "\0", shaSum[SYS_WSS_SHA1_DIGEST_SIZE] = "\0";
    uint32_t outlen = SYS_WSS_SERVER_KEY_SIZE;
    int ret = 1;
    //Debug message
    WSS_DEBUG_PRINT("WSS: Generating server's key...\r\n");

    /*Concatenate the Sec-WebSocket-Key with the GUID(258EAFA5-E914-47DA-95CA-C5AB0DC85B11) string
    and generate SHA-1 of the resulting string*/
    strcpy((char *) buffer, g_wssSrvcObj[clientIndex].wssHandshake.clientKey);
    strcat((char *) buffer, SYS_WSS_GUID);
    key_len = strlen((char *) buffer);
    ret = wc_ShaHash(buffer, key_len, shaSum);
    if (ret) {
        result = SYS_WSS_FAILURE;
    } else {
        memset(buffer, 0, SYS_WSS_SERVER_KEY_SIZE);
        key_len = (uint32_t) strlen((char *) shaSum);
        //Encode the result using Base64
        ret = Base64_Encode((uint8_t *) shaSum, SYS_WSS_SHA1_DIGEST_SIZE, (uint8_t *) & buffer, &outlen);
        WSS_DEBUG_PRINT("WSS:  Server key from wolfssl: %s\r\n", (char *) buffer);
        WSS_DEBUG_PRINT("WSS:  Server key len wolfssl: %d\r\n", outlen);
        if (ret) {
            result = SYS_WSS_FAILURE;
        } else {
            memcpy((char *) g_wssSrvcObj[clientIndex].wssHandshake.serverKey, (char *) &buffer, outlen - 1);
            WSS_DEBUG_PRINT("WSS:  Server key: %.*s\r\n", outlen, g_wssSrvcObj[clientIndex].wssHandshake.serverKey);
            WSS_DEBUG_PRINT("WSS:  Server key len: %d\r\n", outlen);
        }
    }
    //Successful processing
    return result;
}

static SYS_WSS_RESULT validateClientHandshake(int32_t clientIndex) {
    SYS_WSS_RESULT result = SYS_WSS_SUCCESS;
    uint8_t buffer[SYS_WSS_CLIENT_KEY_SIZE];
    int ret = 1;
    uint32_t outlen = SYS_WSS_CLIENT_KEY_SIZE;
    WSS_DEBUG_PRINT(" Validate the client handshake \r\n");

    /* Check the header fields of the client handshake */
    if (g_wssSrvcObj[clientIndex].wssHandshake.http_version < SYS_WSS_HTTP_VERSION_1_1) {
        WSS_DEBUG_PRINT(" Invalid http version \r\n");
        result = SYS_WSS_ERROR_INVALID_REQUEST;
    }
    if (!g_wssSrvcObj[clientIndex].wssHandshake.upgradeWebSocket) {
        WSS_DEBUG_PRINT(" Invalid websocket field \r\n");
        result = SYS_WSS_ERROR_INVALID_REQUEST;
    }
    if (!g_wssSrvcObj[clientIndex].wssHandshake.connectionUpgrade) {
        WSS_DEBUG_PRINT(" Invalid connection upgrade field \r\n");
        result = SYS_WSS_ERROR_INVALID_REQUEST;
    }
    if (g_wssSrvcObj[clientIndex].wssHandshake.iskey != true) {
        WSS_DEBUG_PRINT(" Invalid clientkey field \r\n");
        result = SYS_WSS_ERROR_INVALID_REQUEST;
    }
    if (SYS_WSS_SUCCESS == result) {
        WSS_DEBUG_PRINT("WSS: Verifying client's key...Key : %s  Key Len :%d. \r\n", g_wssSrvcObj[clientIndex].wssHandshake.clientKey, (strlen(g_wssSrvcObj[clientIndex].wssHandshake.clientKey)));
        ret = Base64_Decode((uint8_t *) g_wssSrvcObj[clientIndex].wssHandshake.clientKey, (uint32_t )(strlen(g_wssSrvcObj[clientIndex].wssHandshake.clientKey)), (uint8_t *) & buffer, &outlen);
        if (ret) {

            WSS_DEBUG_PRINT("Base64_Decode failed\r\n");
            result = SYS_WSS_ERROR_INVALID_KEY;
            /*RFC 6455 :Section 4.1.  Client Requirements*/
        } else if (outlen != SYS_WSS_CLIENT_KEY_SIZE_DECODED) {
            WSS_DEBUG_PRINT("Incorrect client key length \r\n");
            result = SYS_WSS_ERROR_INVALID_KEY;
        } else {
            WSS_DEBUG_PRINT("Client key decoded: %s \r\n", (char *) buffer);
        }
    }

    return result;
}

static SYS_WSS_RESULT parseHandshake(void *buffer, uint16_t length, int32_t clientIndex) {
    char *token;
    SYS_WSS_RESULT result = SYS_WSS_SUCCESS;
    WSS_DEBUG_PRINT("In parseHandshake :%s\r\n", buffer);

    //Check if the request line starts with GET method
    token = strtok(buffer, " \r\n");
    WSS_DEBUG_PRINT("In parseHandshake TOKEN1: %s\r\n", token);

    if ((token == NULL) || (strcasecmp(token, "GET"))) {
        result = SYS_WSS_ERROR_INVALID_REQUEST;
        //break;
    }

    //Check for HTTP version
    token = strtok(NULL, " ");
    token = strtok(NULL, " \r\n");
    WSS_DEBUG_PRINT("\r\nIn parseHandshake TOKEN2: %s", token);

    if (token == NULL) {
        // version number is WS_HTTP_VERSION_0_9
        g_wssSrvcObj[clientIndex].wssHandshake.http_version = SYS_WSS_HTTP_VERSION_0_9;
    }//HTTP version 1.0?
    else if (!strcasecmp(token, "HTTP/1.0")) {
        //Save version number
        g_wssSrvcObj[clientIndex].wssHandshake.http_version = SYS_WSS_HTTP_VERSION_1_0;

    }//HTTP version 1.1?
    else if (!strcasecmp(token, "HTTP/1.1")) {
        //Save version number
        g_wssSrvcObj[clientIndex].wssHandshake.http_version = SYS_WSS_HTTP_VERSION_1_1;
    }//HTTP version not supported?
    else {
        //Report an error
        result = SYS_WSS_ERROR_INVALID_REQUEST;
    }

    do {
        //Check for URI
        token = strtok(NULL, " \r\n");
        WSS_DEBUG_PRINT("\r\nIn parseHandshake TOKEN3: %s", token);
        if (!strcasecmp(token, "HOST:")) {
            token = strtok(NULL, " \r\n");
            WSS_DEBUG_PRINT("\r\nIn parseHandshake TOKEN4: %s", token);
        } else if (!strcasecmp(token, "UPGRADE:")) {

            token = strtok(NULL, " \r\n");
            WSS_DEBUG_PRINT("\r\nIn parseHandshake TOKEN5: %s", token);
            if (!strcasecmp(token, "websocket")) {
                g_wssSrvcObj[clientIndex].wssHandshake.upgradeWebSocket = true;
            }


        } else if (!strcasecmp(token, "ORIGIN:")) {
            token = strtok(NULL, " \r\n");
            g_wssSrvcObj[clientIndex].wssHandshake.origin = true;
            WSS_DEBUG_PRINT("\r\nIn parseHandshake TOKEN6: %s", token);
        } else if (!strcasecmp(token, "Sec-WebSocket-Key:")) {
            token = strtok(NULL, " \r\n");
            strcpy(g_wssSrvcObj[clientIndex].wssHandshake.clientKey, token);
            g_wssSrvcObj[clientIndex].wssHandshake.iskey = true;
            WSS_DEBUG_PRINT("\r\nIn parseHandshake TOKEN7: %s", g_wssSrvcObj[clientIndex].wssHandshake.clientKey);
        } else if (!strcasecmp(token, "Sec-WebSocket-Version:")) {
            token = strtok(NULL, " \r\n");
            //  memcpy((uint8_t *)g_wssSrvcObj[clientIndex].wssHandshake.ws_version,(char *) token,1);
            WSS_DEBUG_PRINT("\r\nIn parseHandshake TOKEN8: %s", g_wssSrvcObj[clientIndex].wssHandshake.ws_version);
        } else if (!strcasecmp(token, "Connection:")) {
            token = strtok(NULL, " \r\n");
            if (!strcasecmp(token, "Upgrade")) {
                g_wssSrvcObj[clientIndex].wssHandshake.connectionUpgrade = true;
                WSS_DEBUG_PRINT("\r\nIn parseHandshake TOKEN9: %s", token);
            }
        } else {

            WSS_DEBUG_PRINT("\r\nIn parseHandshake :Handshake parsing completed:");
            break;
        }
    } while (token != NULL);

    return result;
}

void processData(void *buffer, uint16_t length, int32_t clientIndex) {
    //TODO: Implement Queuing
    SYS_WSS_RESULT result = SYS_WSS_SUCCESS;
    SYS_WSS_STATUS_CODE statusCode=SYS_WSS_STATUS_CODE_NORMAL_CLOSURE;
    SYS_WSS_RXDATA rxdata;
    uint64_t dataLength=0;
    switch (g_wssSrvcObj[clientIndex].wssState) {
        case SYS_WSS_STATE_CONNECTING:
        {
            result = parseHandshake(buffer, length, clientIndex);
            if (SYS_WSS_SUCCESS == result) {
                WSS_DEBUG_PRINT("In processdata (%d):Successfully parsed the request\r\n", clientIndex);
                result = validateClientHandshake(clientIndex);
                if (SYS_WSS_SUCCESS == result) {
                    WSS_DEBUG_PRINT("\tSuccessfully validated the client handshake request\r\n");
                    result = wssGenerateServerKey(clientIndex);
                    if (SYS_WSS_SUCCESS == result) {
                        WSS_DEBUG_PRINT("\tSuccessfully generated the server handshake\r\n");
                        result = wssFormatServerHandshake(clientIndex);
                        SYS_NET_SendMsg(g_wssSrvcObj[clientIndex].wssNetHandle, (uint8_t *) g_wssSrvcObj[clientIndex].sHandshake, strlen(g_wssSrvcObj[clientIndex].sHandshake));

                        wssUserCallback(SYS_WSS_EVENT_CLIENT_CONNECTED, NULL, clientIndex);
                        g_wssSrvcObj[clientIndex].wssState = SYS_WSS_STATE_CONNECTED;
                    }
                }
            }
            if (SYS_WSS_SUCCESS != result) {
                wssUserCallback(SYS_WSS_EVENT_CLIENT_CLOSING, NULL, clientIndex);
                (void)wssSendErrorResponse(result,clientIndex);
                g_wssSrvcObj[clientIndex].wssState = SYS_WSS_STATE_CLOSING;
            }
        }
            break;
        case SYS_WSS_STATE_CONNECTED:
        {
            WSS_DEBUG_PRINT("In processData SYS_WSS_STATE_CONNECTED (%d)\r\n)", clientIndex);
            SYS_WSS_FRAME_HEADER *dataframe;
            uint8_t headerbytes, masking_key[4], masking_key1[4], *tmp=NULL, *tmp_buf=NULL;
            uint16_t j, k;

            static SYS_WSS_FRAME g_expectedFrame;
            
            //reset KA Timer.
            g_wssSrvcObj[clientIndex].kaTimerCount=0;
            dataframe = (SYS_WSS_FRAME_HEADER *) buffer;
            /*Check the fields of the frame*/
            if (dataframe->fin == 1) { /*Final frame of the message*/
                g_expectedFrame = 0;
                WSS_DEBUG_PRINT("In processData SYS_WSS_STATE_CONNECTED(%d) :final or single frame received\r\n", clientIndex);
            } else {
                if (dataframe->opcode == SYS_WSS_FRAME_BINARY || dataframe->opcode == SYS_WSS_FRAME_TEXT) {
                    g_expectedFrame = dataframe->opcode;
                } else { /*Control message must not be fragmented*/
                    wssUserCallback(SYS_WSS_EVENT_ERR_INVALID_FRAME, NULL, clientIndex);
                    g_wssSrvcObj[clientIndex].wssState = SYS_WSS_STATE_CLOSING;
                    wssCloseConnection(SYS_WSS_STATUS_CODE_PROTOCOL_ERROR, NULL, 0, clientIndex);
                    break;
                }
            }

            /*Check the payloadlength*/
            if (dataframe->payloadLen <= 125) {
                //  headerbytes=2+maskingbytes;
                headerbytes = 2;
                dataLength = dataframe->payloadLen;
            } else if (dataframe->payloadLen == 126) {
                headerbytes = 2 + sizeof (uint16_t);
                tmp = (uint8_t *) buffer + 2;
                dataLength = tmp[1] | tmp[0]<<8 ;
                WSS_DEBUG_PRINT("\tPayload len (%d) : %d\r\n", clientIndex, (uint16_t) dataLength);
            } else if (dataframe->payloadLen == 127) {
                headerbytes = 2 + sizeof (uint64_t); //length
                // no_maskingBytes
                tmp = (uint8_t *) buffer + 2;
                dataLength |=tmp[3]<<24;
                dataLength |=tmp[2]<<16;
                dataLength |=tmp[1]<<8;
                dataLength |=tmp[0];
                WSS_DEBUG_PRINT("\tPayload len (%d) : %d\r\n", clientIndex,  dataLength);
                
            } else {
                headerbytes = 0;
            }

            WSS_DEBUG_PRINT("\theaderbytes (%d) : %d\r\n", clientIndex, (uint8_t) headerbytes);

            //Check whether the masking key is present
            if (dataframe->mask) {
                WSS_DEBUG_PRINT("\r\nClient message received with masking\r\n");
                tmp = &masking_key[0];
                tmp_buf = ((uint8_t *) buffer + headerbytes);
                j = 4;
                while (j) {
                    *tmp = *tmp_buf;
                    tmp++;
                    tmp_buf++;
                    j--;
                }
                memcpy(&masking_key1, (uint8_t *) buffer + headerbytes, sizeof (uint32_t));

                headerbytes += 4;
                tmp_buf = (((uint8_t *) buffer) + headerbytes);
                //Unmask the data
                for (j = 0; j < dataLength; j++) {
                    k = j % 4;
                    ((uint8_t *) tmp_buf)[j] ^= masking_key[k];
                }
            /*Package the data and the datalen to the structure  before sending to the application*/
            rxdata.data=tmp_buf;
            rxdata.datalen =(int64_t)dataLength;

            //Uncomment to implement a echoServer
            //wssSendResponse(dataframe->fin, dataframe->opcode, tmp_buf, (size_t) (strlen((const char *) tmp_buf)), clientIndex);

            } else {
                /*Client message shall always  be masked*/
                WSS_DEBUG_PRINT("\r\nClient message received without masking, invalid frame");
                wssUserCallback(SYS_WSS_EVENT_ERR_INVALID_FRAME, NULL, clientIndex);
                g_wssSrvcObj[clientIndex].wssState = SYS_WSS_STATE_CLOSING;
                wssCloseConnection(SYS_WSS_STATUS_CODE_PROTOCOL_ERROR, NULL, 0, clientIndex);
                break;

            }
            if (dataframe->opcode == SYS_WSS_FRAME_CONTINUATION ||(dataframe->opcode == SYS_WSS_FRAME_TEXT)||(dataframe->opcode == SYS_WSS_FRAME_BINARY) ) {
                if ((g_expectedFrame == SYS_WSS_FRAME_BINARY) || (dataframe->opcode == SYS_WSS_FRAME_BINARY)) {
                    wssUserCallback(SYS_WSS_EVENT_CLIENT_BIN_DATA_RX, &rxdata, clientIndex);

                } else if ((g_expectedFrame == SYS_WSS_FRAME_TEXT) || (dataframe->opcode == SYS_WSS_FRAME_TEXT)) {
                    wssUserCallback(SYS_WSS_EVENT_CLIENT_TXT_DATA_RX, &rxdata, clientIndex);
                } else {
                }
            } else if (dataframe->opcode == SYS_WSS_FRAME_CLOSE) {
                WSS_DEBUG_PRINT("\r\nClose frame received\r\n");
                //Check the length of the payload data
                if (dataframe->payloadLen == 0) {
                    //close frame shall have the status code 
                    statusCode = SYS_WSS_STATUS_CODE_PROTOCOL_ERROR;
                }
                /*No more frames expected*/
                g_expectedFrame = 0;
                /*Close the connection*/
                wssUserCallback(SYS_WSS_EVENT_CLIENT_CLOSING, NULL, clientIndex);
                g_wssSrvcObj[clientIndex].wssState = SYS_WSS_STATE_CLOSING;
                wssCloseConnection(statusCode, NULL, 0, clientIndex);

            } else if (dataframe->opcode == SYS_WSS_FRAME_PING) {
                //Create and send a pong frame
                WSS_DEBUG_PRINT("\r\n Ping frame received");
                wssUserCallback(SYS_WSS_EVENT_CLIENT_PING_RX, NULL, clientIndex);
                {
                wssSendPongMessage(tmp_buf, (size_t) dataLength, clientIndex);
                }

            } else if (dataframe->opcode == SYS_WSS_FRAME_PONG) {
                //reset the timer if any to retain the connection
                WSS_DEBUG_PRINT("\r\n Pong frame received");
                wssUserCallback(SYS_WSS_EVENT_CLIENT_PONG_RX, NULL, clientIndex);
            } else {
            }
        break;
        }
           
        case SYS_WSS_STATE_CLOSING:
            
            break;
        case SYS_WSS_STATE_CLOSED:
            break;
    }
}

static void wssNetCallback(uint32_t event, void *data, void *cookie) {
    int32_t len;
    int32_t clientIndex = (int32_t) cookie;
    switch (event) {
        case SYS_NET_EVNT_CONNECTED:
        {
            wssUserCallback(SYS_WSS_EVENT_CLIENT_CONNECTING, NULL, clientIndex);
            g_wssSrvcObj[clientIndex].wssState = SYS_WSS_STATE_CONNECTING;
            break;
        }
        case SYS_NET_EVNT_DISCONNECTED:
        {
            wssUserCallback(SYS_WSS_EVENT_CLIENT_CLOSED, NULL, clientIndex);
            g_wssSrvcObj[clientIndex].wssState = SYS_WSS_STATE_CLOSED;
            break;
        }
        case SYS_NET_EVNT_RCVD_DATA:
        {
            len = SYS_NET_RecvMsg(g_wssSrvcObj[clientIndex].wssNetHandle, g_wssSrvcObj[clientIndex].recv_buffer, SYS_WSS_MAX_RX_BUFFER);
            WSS_DEBUG_PRINT("\r\n Data received from lower layer of length : %u ",len);
            if (len > 0) {

                    processData(g_wssSrvcObj[clientIndex].recv_buffer, len ,clientIndex);
            }
            break;
        }
            break;
#if (SYS_WSS_ENABLE_TLS == true)      
        case SYS_NET_EVNT_SSL_FAILED:
        {
            WSS_DEBUG_PRINT("\r\n SSL Negotiation Failed");
            wssUserCallback(SYS_WSS_EVENT_SSL_FAILED, NULL, clientIndex);
            break;
        }
#endif   //(SYS_WSS_ENABLE_TLS == true) 
        case SYS_NET_EVNT_SOCK_OPEN_FAILED:
        {
            SYS_CONSOLE_PRINT("\r\n Socket Open Failed");
            wssUserCallback(SYS_WSS_EVENT_SOCK_OPEN_FAILED, NULL, clientIndex);
            break;
        }
        case SYS_NET_EVNT_LL_INTF_DOWN:
        {
            SYS_CONSOLE_PRINT("\r\n Lower Layer Down");
            wssUserCallback(SYS_WSS_EVENT_LL_INTF_DOWN, NULL, clientIndex);
            break;
        }
        case SYS_NET_EVNT_LL_INTF_UP:
        {
            SYS_CONSOLE_PRINT("\r\n Lower Layer Up\r\n");
            wssUserCallback(SYS_WSS_EVENT_LL_INTF_UP, NULL, clientIndex);
            break;
        }     
    }
}

void wss_timer_callback ( uintptr_t context){
    kaTimerExpired=true;
}

void wssProcessKATimer(void){
    int i=0;
    for (i = 0; i < SYS_WSS_MAX_NUM_CLIENTS; i++) {
        if (SYS_WSS_STATE_CONNECTED==g_wssSrvcObj[i].wssState){
            if (g_wssSrvcObj[i].kaTimerCount < SYS_WSS_CLIENT_TIMEOUT){
                g_wssSrvcObj[i].kaTimerCount+=SYS_WSS_KA_TIMER_PERIOD;
            }
            else{
                //closing due to timeout
                wssUserCallback(SYS_WSS_EVENT_CLIENT_TIMEOUT,0,i);
                wssCloseConnection(SYS_WSS_STATUS_CODE_NORMAL_CLOSURE,NULL,0,i);
            }
        }
    }
}

SYS_MODULE_OBJ SYS_WSS_Initialize(SYS_WSS_CONFIG *config, SYS_WSS_CALLBACK callback, void *cookie) {
    int i = 0;
    uint16_t port = SYS_WSS_PORT;
    bool enableTls = SYS_WSS_ENABLE_TLS;

    /*Override values coming from MHC, with application provided data*/
    if (NULL != config) {
        port = config->port;
        enableTls = config->isTLSEnabled;
    }

    for (i = 0; i < SYS_WSS_MAX_NUM_CLIENTS; i++) {
        g_wssSrvcObj[i].wssState = SYS_WSS_STATE_CLOSED;
        g_wssSrvcObj[i].wssNetCfg.mode = SYS_NET_MODE_SERVER;
        g_wssSrvcObj[i].wssNetCfg.port = port;
        g_wssSrvcObj[i].wssNetCfg.enable_tls = enableTls;
        g_wssSrvcObj[i].wssNetCfg.ip_prot = SYS_NET_IP_PROT_TCP;
        g_wssSrvcObj[i].wssNetCfg.enable_reconnect = SYS_NET_INDEX0_RECONNECT;
        memset(g_wssSrvcObj[i].sHandshake, 0, sizeof (g_wssSrvcObj[i].sHandshake));
        g_wssSrvcObj[i].wssNetHandle=(uintptr_t)NULL;
        g_wssSrvcObj[i].kaTimerCount=0;
    }
    
    kaTimerHandle = SYS_TIME_CallbackRegisterMS(wss_timer_callback,(uintptr_t) NULL,SYS_WSS_KA_TIMER_PERIOD, SYS_TIME_PERIODIC);
    if (kaTimerHandle == SYS_TIME_HANDLE_INVALID)
    {
        SYS_CONSOLE_PRINT("ERROR: SYS_WSS Timer creation failed. KA will not be honored. \r\n");
    }
    if (1==SYS_WSS_START_AT_BOOT || false==isBoot){
        for (i = 0; i < SYS_WSS_MAX_NUM_CLIENTS; i++) {
            g_wssSrvcObj[i].wssNetHandle = SYS_NET_Open(&g_wssSrvcObj[i].wssNetCfg, wssNetCallback, (void *)i);
            if (g_wssSrvcObj[i].wssNetHandle != SYS_MODULE_OBJ_INVALID) {
                WSS_DEBUG_PRINT("Web Socket Server initialized for client (%d)\r\n", i);
            } else {
                WSS_DEBUG_PRINT("Web Socket Server initialization failed for client (%d)\r\n", i);
            }
        }
    }
    isBoot=false;
    return (SYS_MODULE_OBJ)&g_wssSrvcObj;
}

SYS_WSS_RESULT SYS_WSS_PingClient( uint8_t *data, size_t dataLen, int32_t clientIndex) {
    
    SYS_WSS_RESULT result = SYS_WSS_SUCCESS;
    int i=0;
    WSS_DEBUG_PRINT("\r\n ClientIndex %d",clientIndex);
    if ( clientIndex < 0 ){
	    WSS_DEBUG_PRINT("\r\n Send ping message to all clients");
        for (i = 0; i < SYS_WSS_MAX_NUM_CLIENTS; i++) {
            if(SYS_WSS_STATE_CONNECTED == g_wssSrvcObj[i].wssState){
                result = wssSendPingMessage( data, dataLen, i);
            }
        }
    }
    else{
        result = wssSendPingMessage( data, dataLen, clientIndex);
    }
    return result;
}
SYS_WSS_RESULT SYS_WSS_sendMessage(bool fin, SYS_WSS_FRAME type, uint8_t *data, size_t dataLen, int32_t clientIndex){

    SYS_WSS_RESULT result = SYS_WSS_SUCCESS;
    int i=0;
    WSS_DEBUG_PRINT("\r\n ClientIndex %d",clientIndex);
    if ( clientIndex < 0 ){
	    WSS_DEBUG_PRINT("\r\n Send message to all clients");
        for (i = 0; i < SYS_WSS_MAX_NUM_CLIENTS; i++) {
            if(SYS_WSS_STATE_CONNECTED == g_wssSrvcObj[i].wssState){
                result= wssSendResponse(fin, type, data, dataLen, i);
            }
        }
    }    
    else{
            result = wssSendResponse(fin, type, data, dataLen, clientIndex);
    }   
   return result;
}

SYS_WSS_RESULT SYS_WSS_CloseConnection(SYS_WSS_STATUS_CODE code, uint8_t *data, size_t dataLen, int32_t clientIndex) {
    SYS_WSS_RESULT result =SYS_WSS_SUCCESS;
    int i=0;
    WSS_DEBUG_PRINT("\r\n ClientIndex %d",clientIndex);
    if ( clientIndex < 0 )    {
	    WSS_DEBUG_PRINT("\r\n Close the connection to all clients");
        for (i = 0; i < SYS_WSS_MAX_NUM_CLIENTS; i++) {
            if(SYS_WSS_STATE_CONNECTED == g_wssSrvcObj[i].wssState){
                g_wssSrvcObj[i].wssState = SYS_WSS_STATE_CLOSING;
                result = wssCloseConnection(code, data,  dataLen,i);
            }
        }
    }
    else{
        WSS_DEBUG_PRINT("\r\n Close the connection to the client %d",clientIndex);
        g_wssSrvcObj[clientIndex].wssState = SYS_WSS_STATE_CLOSING;
        result = wssCloseConnection(code, data,  dataLen,clientIndex);
    }    
    return result;
}


void SYS_WSS_Deinitialize(SYS_MODULE_OBJ *sysWSSObj) {
    int i = 0;
/*Deinit the wss service object for all clients*/
    for (i = 0; i < SYS_WSS_MAX_NUM_CLIENTS; i++) {
    g_wssSrvcObj[i].wssState = SYS_WSS_STATE_CLOSED;
    g_wssSrvcObj[i].wssNetCfg.mode = 0;
    g_wssSrvcObj[i].wssNetCfg.port = 0;
    g_wssSrvcObj[i].wssNetCfg.enable_tls = 0;
    g_wssSrvcObj[i].wssNetCfg.ip_prot = 0;
    g_wssSrvcObj[i].wssNetCfg.enable_reconnect = 0;
    memset(g_wssSrvcObj[i].sHandshake, 0, sizeof (g_wssSrvcObj[i].sHandshake));
    g_wssSrvcObj[i].kaTimerCount=0;
    SYS_NET_Close(g_wssSrvcObj[i].wssNetHandle);
    g_wssSrvcObj[i].wssNetHandle=0;

    }
    WSS_DEBUG_PRINT("Web Socket Server de initialization completed");
}



SYS_WSS_RESULT SYS_WSS_register_callback(SYS_WSS_CALLBACK userCallback, void* cookie) {
    int i = 0;
    for (i = 0; i < SYS_WSS_MAX_NUM_CLIENTS; i++) {
        g_wssSrvcObj[i].userCallback = userCallback;
        g_wssSrvcObj[i].userCookie = cookie;
    }
    return SYS_WSS_SUCCESS;
}

uint8_t SYS_WSS_Task(SYS_MODULE_OBJ object) {
    int i = 0;
    SYS_CMD_READY_TO_READ();
    for (i = 0; i < SYS_WSS_MAX_NUM_CLIENTS; i++) {
        if((uintptr_t)NULL!=g_wssSrvcObj[i].wssNetHandle){
            SYS_NET_Task(g_wssSrvcObj[i].wssNetHandle);
        }
    }
    if (true==kaTimerExpired){
        wssProcessKATimer();
        kaTimerExpired=false;
    }
    return 0;
}
