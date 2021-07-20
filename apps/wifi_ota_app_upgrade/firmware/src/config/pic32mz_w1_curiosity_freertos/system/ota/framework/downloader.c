/**
 *
 * Copyright (c) 2021 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "../system/ota/framework/http_client/http_client.h"
#include "definitions.h"

//#include "config/pic32mz_w1_curiosity_freertos/net_pres/pres/net_pres.h"
#define DOWNLOADER_BUFFER_SIZE 4096

typedef struct {
    HTTP_CLIENT_HANDLE httpHandle;
    HTTP_CLIENT_ERRORS httpErrorState;
    uint32_t downloadPause;
    size_t   downloadFileSize;
    size_t   downloadedSize;
    uint8_t *buf;
    uint32_t bufIn;
    uint32_t bufOut;
    uint32_t bufLevel;
} DOWNLOADER_HTTP_CLIENT;

static DOWNLOADER_HTTP_CLIENT downloader;
size_t field_content_length;
extern bool HTTP_Client_SetRxReady(HTTP_CLIENT_HANDLE handle, bool ready);
static int DOWNLOADER_BufPut(uint8_t *buf, int size);
static int DOWNLOADER_BufGet(uint8_t *buf, int reqSize);

static void DOWNLOADER_HTTPClient_EventHandler(HTTP_CLIENT_HANDLE httpHandle, uintptr_t eventHandle, HTTP_CLIENT_EVENTS event, void *pEventData, size_t eventDataLength)
{
    if (httpHandle != downloader.httpHandle)
    {
        return;
    }

    switch (event)
    {
        case HTTP_CLIENT_EVENT_GET_SENT:
        {
           
            NET_PRES_SKT_HANDLE_T netPresSock = HTTP_Client_GetNetPresSocket(httpHandle);
            uint32_t bufSize = 512; //TCPIP_TCP_SOCKET_DEFAULT_RX_SIZE;
            #ifdef SYS_OTA_APPDEBUG_ENABLED
            SYS_CONSOLE_PRINT("SYS_OTA : HTTP_CLIENT_EVENT_GET_SENT %d\r\n", 512);
            #endif
            NET_PRES_SocketOptionsGet(netPresSock, TCP_OPTION_RX_BUFF, &bufSize);
            break;
        }

        case HTTP_CLIENT_EVENT_HEADER_RAW_RECEIVED:
        {
            #ifdef SYS_OTA_APPDEBUG_ENABLED
            SYS_CONSOLE_PRINT("SYS_OTA : Header Length is %d\r\n", eventDataLength);
            #endif
            break;
        }

        case HTTP_CLIENT_EVENT_HEADER_RECEIVED:
        {
            HTTP_CLIENT_RESPONSE_MSG *pRspMsg = (HTTP_CLIENT_RESPONSE_MSG*)pEventData;
            if (200 == pRspMsg->httpStatusCode)
            {
                downloader.downloadFileSize = pRspMsg->fieldContentLength;
                field_content_length = pRspMsg->fieldContentLength;
            }
            else
            {
                SYS_CONSOLE_PRINT("SYS_OTA : Request rejected by client :code: %d \r\n", pRspMsg->httpStatusCode);
                HTTP_Client_Cancel(httpHandle);
            }

            break;
        }

        case HTTP_CLIENT_EVENT_PAYLOAD_RECEIVED:
        {
            downloader.downloadedSize += eventDataLength;
            DOWNLOADER_BufPut(pEventData, eventDataLength);           
            break;
        }

        case HTTP_CLIENT_EVENT_PAYLOAD_END:
        {
            break;
        }

        case HTTP_CLIENT_EVENT_ERROR:
        {
            downloader.httpErrorState = *(HTTP_CLIENT_ERRORS*)pEventData;;
            break;
        }

        case HTTP_CLIENT_EVENT_CLOSE:
        {
            if (downloader.httpErrorState == HTTP_CLIENT_ERROR_NONE)
            {
                //SYS_CONSOLE_PRINT("FILE_DOWNLOAD\r\n");
            }
            else
            {
                #ifdef SYS_OTA_APPDEBUG_ENABLED
                SYS_CONSOLE_PRINT("SYS_OTA : HTTP Client Error %d\r\n", downloader.httpErrorState);
                #endif
            }
            downloader.httpHandle = HTTP_CLIENT_HANDLE_INVALID;
            break;
        }

        case HTTP_CLIENT_EVENT_INVALID:
        default:
        {
            break;
        }
    }
}

static int DOWNLOADER_HTTPClient_Abort(void)
{
    if (downloader.httpHandle != HTTP_CLIENT_HANDLE_INVALID)
    {
        SYS_CONSOLE_PRINT("SYS_OTA : DOWNLOADER_HTTPClient_Abort\r\n");

        HTTP_Client_Cancel(downloader.httpHandle);

        do
        {
            downloader.httpHandle = HTTP_Client_Task(downloader.httpHandle);
        }
        while (HTTP_CLIENT_HANDLE_INVALID != downloader.httpHandle);
    }

    return 0;
}
static int DOWNLOADER_BufPut(uint8_t *buf, int size)
{
    int r = size;
    int copyLen = 0;
    
    if (downloader.buf == NULL)
    {
        SYS_CONSOLE_PRINT("SYS_OTA : No Buffer available\r\n");
        return -1;
    }
    #ifdef SYS_OTA_APPDEBUG_ENABLED
    SYS_CONSOLE_PRINT("SYS_OTA : PUT:%d\r\n", size);
    #endif
    if (size > (DOWNLOADER_BUFFER_SIZE - downloader.bufLevel))
    {
        SYS_CONSOLE_PRINT("SYS_OTA : Buffer over-run %d - %d\r\n",
        DOWNLOADER_BUFFER_SIZE, downloader.bufLevel);
        return -1;
    }
        
    downloader.bufLevel += size;
    if (downloader.bufIn < downloader.bufOut)
    {
        if ((DOWNLOADER_BUFFER_SIZE - downloader.bufIn) > size)
        {
            memcpy(&downloader.buf[downloader.bufIn], buf, size);
            downloader.bufIn += size;
            
            if (downloader.bufIn == DOWNLOADER_BUFFER_SIZE)
            {
                downloader.bufIn = 0;
            }
            return r;
        }
        copyLen = DOWNLOADER_BUFFER_SIZE - downloader.bufIn; 
        memcpy(&downloader.buf[downloader.bufIn], buf, copyLen);
        downloader.bufIn = 0;
        size -= copyLen;
        buf += copyLen;
    }
    
    memcpy(&downloader.buf[downloader.bufIn], buf, size);
    downloader.downloadPause = 1;
    HTTP_Client_SetRxReady(downloader.httpHandle, false);
    return r;
}

static int DOWNLOADER_BufGet(uint8_t *buf, int reqSize)
{
    int r = 0;
    //SYS_CONSOLE_PRINT("reqSize : %d \n\r",reqSize);
    if (downloader.buf == NULL)
    {
        return -1;
    }

    if (downloader.bufLevel != 0)
    {
        int copyLen = 0;
        
        
        if (reqSize > downloader.bufLevel)
        {
            reqSize = downloader.bufLevel;
        }
        
        r = reqSize;
        
        if (downloader.bufOut > downloader.bufIn)
        {
            copyLen = reqSize;
            if ((DOWNLOADER_BUFFER_SIZE - downloader.bufOut) < reqSize)
            {
                copyLen = DOWNLOADER_BUFFER_SIZE - downloader.bufOut;
            }
            
            memcpy(buf, &downloader.buf[downloader.bufOut], copyLen);
            downloader.bufOut   += copyLen;
            
            if (downloader.bufOut == DOWNLOADER_BUFFER_SIZE)
            {
                downloader.bufOut = 0;
            }
            buf     += copyLen;
            reqSize -= copyLen;
        }
        
        if (reqSize)
        {
            memcpy(buf, &downloader.buf[downloader.bufOut], reqSize);
            downloader.bufOut += reqSize;
        }
        
        downloader.bufLevel -= r;
    
        if (downloader.bufLevel == 0)
        {
            downloader.bufOut = 0;
            downloader.bufIn  = 0;
            downloader.downloadPause = 0;
            HTTP_Client_SetRxReady(downloader.httpHandle, true);
        }
    }
    
    return r;
}

void DOWNLOADER_Initialize(void)
{
    memset(&downloader, 0, sizeof(downloader));
    downloader.httpHandle = HTTP_CLIENT_HANDLE_INVALID;
}

void DOWNLOADER_Deinitialize(void)
{
    if (downloader.httpHandle != HTTP_CLIENT_HANDLE_INVALID)
    {
        DOWNLOADER_HTTPClient_Abort();
    }
}

DRV_HANDLE DOWNLOADER_Open(void * param)
{
    downloader.httpHandle = HTTP_CLIENT_HANDLE_INVALID;
    downloader.httpErrorState = HTTP_CLIENT_ERROR_NONE;
        
    downloader.buf = (uint8_t *)OSAL_Malloc(DOWNLOADER_BUFFER_SIZE);
    if (downloader.buf == NULL)
    {
        DOWNLOADER_HTTPClient_Abort();
        
        return DRV_HANDLE_INVALID;
    }
    downloader.bufIn         = 0;
    downloader.bufOut        = 0;
    downloader.bufLevel      = 0;
    downloader.downloadPause = 0;
    
    downloader.httpHandle = HTTP_Client_Get(param, DOWNLOADER_HTTPClient_EventHandler, (uintptr_t) NULL);
    if (downloader.httpHandle == HTTP_CLIENT_HANDLE_INVALID)
    {
        return DRV_HANDLE_INVALID;
    }

    return (DRV_HANDLE)&downloader;

}

int DOWNLOADER_Read(DRV_HANDLE handle, unsigned char* buf, int bufSize)
{
    if (handle == DRV_HANDLE_INVALID || handle != (DRV_HANDLE)&downloader)
    {
        return -1;
    }
    if (downloader.httpHandle == HTTP_CLIENT_HANDLE_INVALID)
    {
        return -1;
    }
    return DOWNLOADER_BufGet(buf, bufSize);
}

void DOWNLOADER_Close(DRV_HANDLE handle)
{
    if (handle != DRV_HANDLE_INVALID && handle == (DRV_HANDLE)&downloader)
    {
        DOWNLOADER_HTTPClient_Abort();
        
        if (downloader.buf != NULL)
        {
            OSAL_Free(downloader.buf);
        }
        downloader.buf = NULL;
        downloader.downloadPause = 0;
    }
}

void DOWNLOADER_Tasks(void)
{
    if (downloader.httpHandle != HTTP_CLIENT_HANDLE_INVALID)
    {
        if (downloader.downloadPause == 0)
        {
            downloader.httpHandle = HTTP_Client_Task(downloader.httpHandle);
        }
    }
}
