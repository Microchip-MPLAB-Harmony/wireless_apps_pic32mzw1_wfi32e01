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

#ifndef _HTTP_CLIENT_H
#define _HTTP_CLIENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "tcpip/tcpip.h"
#include "net_pres/pres/net_pres.h"
#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif

#define HTTP_CLIENT_HANDLE_INVALID  0

typedef uintptr_t HTTP_CLIENT_HANDLE;

typedef enum
{
    HTTP_CLIENT_EVENT_INVALID = 0,
    HTTP_CLIENT_EVENT_GET_SENT,
    HTTP_CLIENT_EVENT_HEADER_RAW_RECEIVED,
    HTTP_CLIENT_EVENT_HEADER_RECEIVED,
    HTTP_CLIENT_EVENT_PAYLOAD_RECEIVED,
    HTTP_CLIENT_EVENT_PAYLOAD_END,
    HTTP_CLIENT_EVENT_ERROR,
    HTTP_CLIENT_EVENT_CLOSE
} HTTP_CLIENT_EVENTS;

typedef enum
{
    HTTP_CLIENT_ERROR_NONE = 0,
    HTTP_CLIENT_ERROR_USER_CANCELLED,
    HTTP_CLIENT_ERROR_DNS_FAILED,
    HTTP_CLIENT_ERROR_CONNECT_FAILED,
    HTTP_CLIENT_ERROR_MEMORY,
    HTTP_CLIENT_ERROR_RESPONSE_HEADERS,
    HTTP_CLIENT_ERROR_RECV_FAILED,
    HTTP_CLIENT_ERROR_SEND_FAILED
} HTTP_CLIENT_ERRORS;

typedef struct
{
    uint8_t     httpVerMaj;
    uint8_t     httpVerMin;
    uint16_t    httpStatusCode;
    size_t      fieldContentLength;
} HTTP_CLIENT_RESPONSE_MSG;

typedef void (*HTTP_CLIENT_EVENT_HANDLER)(HTTP_CLIENT_HANDLE handle, uintptr_t eventHandle, HTTP_CLIENT_EVENTS event, void *pEventData, size_t eventDataLength);

void HTTP_Client_Init(void);
HTTP_CLIENT_HANDLE HTTP_Client_Get(const char *pGetURL, HTTP_CLIENT_EVENT_HANDLER pClientEventHandler, uintptr_t eventHandle);
HTTP_CLIENT_HANDLE HTTP_Client_Open(HTTP_CLIENT_EVENT_HANDLER pClientEventHandler, uintptr_t eventHandle);
void HTTP_Client_Close(HTTP_CLIENT_HANDLE handle);
void HTTP_Client_SocketReceveProcess(HTTP_CLIENT_HANDLE handle, uint8_t *pRecvBuffer, size_t recvBufferLength);
void HTTP_Client_Cancel(HTTP_CLIENT_HANDLE handle);
bool HTTP_Client_GetPeerAddress(HTTP_CLIENT_HANDLE handle, IP_MULTI_ADDRESS *peerAddr);
NET_PRES_SKT_HANDLE_T HTTP_Client_GetNetPresSocket(HTTP_CLIENT_HANDLE handle);
HTTP_CLIENT_HANDLE HTTP_Client_Task(HTTP_CLIENT_HANDLE handle);

#ifdef __cplusplus
}
#endif

#endif /* _HTTP_CLIENT_H */
