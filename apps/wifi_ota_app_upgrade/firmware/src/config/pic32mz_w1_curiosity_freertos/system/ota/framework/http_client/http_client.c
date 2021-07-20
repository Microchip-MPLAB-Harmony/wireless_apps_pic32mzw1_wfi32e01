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

#include <stddef.h>
#include <stdbool.h>


#include "http_client.h"
#include "definitions.h"

#define HTTP_CLIENT_NUM_CONNECTIONS     1
#define HTTP_CLIENT_MAX_HOSTNAME_LEN    256
#define HTTP_CLIENT_MAX_PATH_LEN        256
#define HTTP_CLIENT_BUFFER_SIZE         1460
#define HTTP_CLIENT_SOCKET_CONN_TIMEOUT 360000
#define HTTP_TCP_RX_WINDOW_SIZE         11680

typedef enum {
    URI_SCHEME_UNKNOWN,
    URI_SCHEME_HTTP,
    URI_SCHEME_HTTPS,
} URI_SCHEME;

typedef struct {
    URI_SCHEME scheme;
    char authorityHost[HTTP_CLIENT_MAX_HOSTNAME_LEN];
    uint16_t authorityPort;
    char path[HTTP_CLIENT_MAX_PATH_LEN];
} URI;

typedef enum {
    HTTP_CLIENT_STATE_UNKNOWN,
    HTTP_CLIENT_STATE_NAME_LOOKUP,
    HTTP_CLIENT_STATE_CONNECT,
    HTTP_CLIENT_STATE_CONNECTING,
    HTTP_CLIENT_STATE_CONNECTED,
    HTTP_CLIENT_STATE_REQUEST_SENT,
    HTTP_CLIENT_STATE_CLOSE
} HTTP_CLIENT_STATE;

typedef struct {
    bool isValid;
    HTTP_CLIENT_STATE state;
    HTTP_CLIENT_ERRORS errorCode;
    URI uri;
    IP_MULTI_ADDRESS ipAddress;
    NET_PRES_SKT_HANDLE_T socket;
    NET_PRES_SIGNAL_HANDLE sigHandler;
    uint8_t recvBuffer[HTTP_CLIENT_BUFFER_SIZE];
    uint8_t *pResponse;
    size_t responseLength;
    bool isResponseComplete;
    HTTP_CLIENT_EVENT_HANDLER pClientEventHandler;
    uintptr_t eventHandle;
    HTTP_CLIENT_RESPONSE_MSG rspMessage;
    size_t contentLengthReceived;
    uint32_t lastDNSReqTimeMs;
    uint32_t socketTimer;
    bool clientRxReady;
} HTTP_CLIENT_DCPT;


static uint32_t timerMS;

void _timerCallback(uint32_t status, uintptr_t context) {
    timerMS++;
}

void OtaHttpTimerInit(void) {
    timerMS = 0;
    TMR2_CallbackRegister(&_timerCallback, 0);
    TMR2_Start();
}

uint32_t HttpGetSysTimeMs(void) {
    return timerMS;
}

static HTTP_CLIENT_DCPT httpClientState[HTTP_CLIENT_NUM_CONNECTIONS];
static OSAL_SEM_HANDLE_TYPE httpClientStateSemaphore;

static void _socketSignalHandler(NET_PRES_SKT_HANDLE_T socket, NET_PRES_SIGNAL_HANDLE hNet, uint16_t sigType, const void* param) {
    HTTP_CLIENT_DCPT * const pDcpt = (HTTP_CLIENT_DCPT * const) param;

    if (NULL == param) {
        return;
    }

    if (0 != (sigType & TCPIP_TCP_SIGNAL_RX_RST)) {
        pDcpt->errorCode = HTTP_CLIENT_ERROR_RECV_FAILED;
        pDcpt->state = HTTP_CLIENT_STATE_CLOSE;
    }
}

void *http_realloc(void *ptr, size_t len) {
    void *new_ptr = NULL;

    new_ptr = OSAL_Malloc(len);
    memset(new_ptr, 0, len);
    if (new_ptr) {
        if (ptr != NULL)
            memcpy(new_ptr, ptr, len);
    }
    OSAL_Free(ptr);
    return (new_ptr);
}

void HTTP_Client_Init(void) {
    int i;

    for (i = 0; i < HTTP_CLIENT_NUM_CONNECTIONS; i++) {
        httpClientState[i].isValid = false;
    }

    OSAL_SEM_Create(&httpClientStateSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1);
    OtaHttpTimerInit();
}

bool HTTP_Client_ParseURI(const char *pURIText, URI *pURI) {
    int i;
    size_t urlLength;

    if ((NULL == pURIText) || (NULL == pURI)) {
        return false;
    }

    urlLength = strlen(pURIText);
    if (SYS_OTA_ENFORCE_TLS == false) {
        if (0 == strncmp(pURIText, "https:", 6)) {
            pURI->scheme = URI_SCHEME_HTTPS;
            pURI->authorityPort = 443;
            pURIText += 6;
            urlLength -= 6;
        } else if (0 == strncmp(pURIText, "http:", 5)) {
            pURI->scheme = URI_SCHEME_HTTP;
            pURI->authorityPort = 80;
            pURIText += 5;
            urlLength -= 5;
        } else {
            return false;
        }
    } else {
        if (0 == strncmp(pURIText, "https:", 6)) {
            pURI->scheme = URI_SCHEME_HTTPS;
            pURI->authorityPort = 443;
            pURIText += 6;
            urlLength -= 6;
        } else {
            return false;
        }
    }

    if ((urlLength < 2) || (0 != strncmp(pURIText, "//", 2))) {
        return false;
    }

    pURIText += 2;
    urlLength -= 2;

    i = 0;
    while (('\0' != *pURIText) && (urlLength > 0) && ('/' != *pURIText) && (':' != *pURIText)) {
        pURI->authorityHost[i++] = *pURIText++;
        urlLength--;
    }

    pURI->authorityHost[i] = '\0';

    if (':' == *pURIText) {
        char *pEndPtr;

        pURIText++;
        urlLength--;

        pURI->authorityPort = strtoul(pURIText, &pEndPtr, 10);

        urlLength -= (pEndPtr - pURIText);
        pURIText = pEndPtr;
    }

    if ((urlLength < 1) && ('/' != *pURIText)) {
        return false;
    }

    i = 0;
    while (('\0' != *pURIText) && (urlLength > 0) && ('?' != *pURIText) && ('#' != *pURIText)) {
        pURI->path[i++] = *pURIText++;
        urlLength--;
    }

    pURI->path[i] = '\0';

    return true;
}

static bool HTTP_Client_ParseHeaderFields(char *pHeaders, size_t headerLength, HTTP_CLIENT_RESPONSE_MSG *pRsp) {
    char *pField;

    if ((NULL == pHeaders) || (0 == headerLength) || (NULL == pRsp)) {
        return false;
    }

    pField = pHeaders;

    while (headerLength) {
        char *p;
        char *pFieldValue;
        char *pFieldName;
        int fieldNameLength;
        int fieldValueLength;

        // header-field = field-name ":" OWS field-value OWS

        p = pField;

        pFieldName = p;

        while ((headerLength > 0) && (':' != *p)) {
            p++;
            headerLength--;
        }

        if ((0 == headerLength) || (':' != *p++)) {
            return false;
        }

        headerLength--;

        fieldNameLength = (p - pFieldName - 1);

        // OWS

        while ((headerLength > 0) && ((' ' == *p) || (0x09 == *p))) {
            p++;
            headerLength--;
        }

        if (0 == headerLength) {
            return false;
        }

        // field-value

        pFieldValue = p;

        while ((headerLength > 0) && ('\r' != *p)) {
            p++;
            headerLength--;
        }

        fieldValueLength = p - pFieldValue;

        // CRLF

        if (headerLength < 2) {
            return false;
        }

        if (0 != strncmp(p, "\r\n", 2)) {
            return false;
        }

        p += 2;
        headerLength -= 2;

        pField = p;

        // Remove OWS from end of field-value

        p = &pFieldValue[fieldValueLength - 1];

        while ((fieldValueLength > 0) && ((' ' == *p) || (0x09 == *p))) {
            fieldValueLength--;
            p--;
        }

        if ((14 == fieldNameLength) && (0 == strncmp(pFieldName, "Content-Length", 14))) {
            pRsp->fieldContentLength = strtoul(pFieldValue, NULL, 10);
        }
    }

    return true;
}

static bool HTTP_Client_ParseResponseMsg(char *pMessage, size_t messageLength, HTTP_CLIENT_RESPONSE_MSG *pRsp) {
    char *__attribute__((unused)) pReasonPhrase;
    int i;

    if ((NULL == pMessage) || (messageLength < 15) || (NULL == pRsp)) {
        return false;
    }

    memset(pRsp, 0, sizeof (HTTP_CLIENT_RESPONSE_MSG));

    // status-line = HTTP-version SP status-code SP reason-phrase CRLF

    // HTTP-version = HTTP-name "/" DIGIT "." DIGIT

    if (0 != strncmp(pMessage, "HTTP", 4)) {
        return false;
    }

    pMessage += 4;

    if ('/' != *pMessage++) {
        return false;
    }

    if ((*pMessage < '0') || (*pMessage > '9')) {
        return false;
    }

    pRsp->httpVerMaj = *pMessage++ -'0';

    if ('.' != *pMessage++) {
        return false;
    }

    if ((*pMessage < '0') || (*pMessage > '9')) {
        return false;
    }

    pRsp->httpVerMin = *pMessage++ -'0';

    messageLength -= 8;

    // SP

    if (' ' != *pMessage++) {
        return false;
    }

    messageLength--;

    // status-code = 3DIGIT

    pRsp->httpStatusCode = 0;

    for (i = 0; i < 3; i++) {
        if ((*pMessage < '0') || (*pMessage > '9')) {
            return false;
        }

        pRsp->httpStatusCode = (pRsp->httpStatusCode * 10) + (*pMessage++ -'0');
    }

    messageLength -= 3;

    // SP

    if (' ' != *pMessage++) {
        return false;
    }

    messageLength--;

    // reason-phrase = *( HTAB / SP / VCHAR / obs-text )

    pReasonPhrase = pMessage;

    while ((messageLength > 0) && ('\r' != *pMessage)) {
        pMessage++;
        messageLength--;
    }

    // CRLF

    if ((messageLength < 2) || (0 != strncmp(pMessage, "\r\n", 2))) {
        return false;
    }

    pMessage += 2;
    messageLength -= 2;

    return HTTP_Client_ParseHeaderFields(pMessage, messageLength, pRsp);
}

HTTP_CLIENT_HANDLE HTTP_Client_Open(HTTP_CLIENT_EVENT_HANDLER pClientEventHandler, uintptr_t eventHandle) {
    int i;
    HTTP_CLIENT_DCPT *pDcpt = NULL;

    if (OSAL_RESULT_TRUE != OSAL_SEM_Pend(&httpClientStateSemaphore, OSAL_WAIT_FOREVER)) {
        SYS_CONSOLE_PRINT("OSAL_RESULT_TRUE != OSAL_SEM_Pend(&httpClientStateSemaphore, OSAL_WAIT_FOREVER)\n\r");
        return HTTP_CLIENT_HANDLE_INVALID;
    }

    for (i = 0; i < HTTP_CLIENT_NUM_CONNECTIONS; i++) {
        if (false == httpClientState[i].isValid) {
            pDcpt = &httpClientState[i];
            break;
        }
    }

    if (NULL == pDcpt) {
        SYS_CONSOLE_PRINT("NULL == pDcpt");
        OSAL_SEM_Post(&httpClientStateSemaphore);
        return HTTP_CLIENT_HANDLE_INVALID;
    }

    memset(pDcpt, 0, sizeof (HTTP_CLIENT_DCPT));

    pDcpt->isValid = true;
    pDcpt->pClientEventHandler = pClientEventHandler;
    pDcpt->eventHandle = eventHandle;

    OSAL_SEM_Post(&httpClientStateSemaphore);

    return (HTTP_CLIENT_HANDLE) pDcpt;
}

void HTTP_Client_Close(HTTP_CLIENT_HANDLE handle) {
    HTTP_CLIENT_DCPT *pDcpt = (HTTP_CLIENT_DCPT*) handle;

    if (NULL == pDcpt) {
        return;
    }

    memset(pDcpt, 0, sizeof (HTTP_CLIENT_DCPT));

    pDcpt->isValid = false;
}

void HTTP_Client_SocketReceveProcess(HTTP_CLIENT_HANDLE handle, uint8_t *pRecvBuffer, size_t recvBufferLength) {
    HTTP_CLIENT_DCPT *pDcpt = (HTTP_CLIENT_DCPT*) handle;
    uint8_t *pPayload = NULL;
    size_t payloadSize = 0;
    int i;

    if (NULL == pDcpt) {
        return;
    }

    if (false == pDcpt->isValid) {
        return;
    }

    /* If the received headers are so far incomplete, append the new data
     to the headers, but only up to the \r\n\r\n header/payload divide */

    if ((false == pDcpt->isResponseComplete) && (NULL != pDcpt->pResponse) && (recvBufferLength >= 4)) {
        char searchBuffer[7];

        /* Make a temporary buffer of the last three bytes of the headers so far
         and the first four bytes of the new data, use this to check for \r\n\r\n
         which spans the two messages. */

        memcpy(searchBuffer, &pDcpt->pResponse[pDcpt->responseLength - 3], 3);
        memcpy(&searchBuffer[3], pRecvBuffer, 4);

        for (i = 0; i < 4; i++) {
            if (('\r' == searchBuffer[i]) && (0 == strncmp((char*) &searchBuffer[i], "\r\n\r\n", 4))) {
                i += 2;

                pDcpt->responseLength -= 3 - i;
                pDcpt->pResponse[pDcpt->responseLength] = '\0';
                pDcpt->isResponseComplete = true;

                pDcpt->contentLengthReceived = 0;
                pRecvBuffer = &pRecvBuffer[2 + i];
                recvBufferLength = recvBufferLength - (2 + i);
                break;
            }
        }
    }

    if (false == pDcpt->isResponseComplete) {
        /* If we still don't have all the headers then search the new message
         for the \r\n\r\n pattern. */

        for (i = 0; i < (recvBufferLength - 3); i++) {
            if (('\r' == pRecvBuffer[i]) && (0 == strncmp((char*) &pRecvBuffer[i], "\r\n\r\n", 4))) {
                /* We've found the end of the headers in this new message. */

                i += 2;

                if (NULL == pDcpt->pResponse) {
                    /* No previous headers so create a buffer to hold these
                     headers while they are processed. */

                    pDcpt->pResponse = OSAL_Malloc(i);

                    if (NULL == pDcpt->pResponse) {
                        pDcpt->errorCode = HTTP_CLIENT_ERROR_MEMORY;
                        pDcpt->state = HTTP_CLIENT_STATE_CLOSE;
                        break;
                    }

                    pDcpt->responseLength = 0;
                } else {
                    pDcpt->pResponse = http_realloc(pDcpt->pResponse, pDcpt->responseLength + i);
                }

                /* Copy the headers to the holding buffer. */

                memcpy(&pDcpt->pResponse[pDcpt->responseLength], pRecvBuffer, i);
                pDcpt->responseLength = pDcpt->responseLength + i;
                pDcpt->isResponseComplete = true;

                pPayload = &pRecvBuffer[2 + i];
                payloadSize = recvBufferLength - (2 + i);
                break;
            }
        }

        if (false == pDcpt->isResponseComplete) {
            /* The headers are still incomplete and the new message doesn't
             contain \r\n\r\n so store it to a holding buffer. */

            if (NULL == pDcpt->pResponse) {
                pDcpt->pResponse = OSAL_Malloc(recvBufferLength);
                pDcpt->responseLength = 0;
            } else {
                pDcpt->pResponse = http_realloc(pDcpt->pResponse, pDcpt->responseLength + recvBufferLength);
            }

            memcpy(&pDcpt->pResponse[pDcpt->responseLength], pRecvBuffer, recvBufferLength);
            pDcpt->responseLength = pDcpt->responseLength + recvBufferLength;
        } else {
            /* Headers are now complete. */

            if (true == HTTP_Client_ParseResponseMsg((char*) pDcpt->pResponse, pDcpt->responseLength, &pDcpt->rspMessage)) {
                if (NULL != pDcpt->pClientEventHandler) {
                    pDcpt->pClientEventHandler((HTTP_CLIENT_HANDLE) pDcpt, pDcpt->eventHandle, HTTP_CLIENT_EVENT_HEADER_RAW_RECEIVED, pDcpt->pResponse, pDcpt->responseLength);
                    pDcpt->pClientEventHandler((HTTP_CLIENT_HANDLE) pDcpt, pDcpt->eventHandle, HTTP_CLIENT_EVENT_HEADER_RECEIVED, &pDcpt->rspMessage, sizeof (HTTP_CLIENT_RESPONSE_MSG));
                }

                pDcpt->contentLengthReceived = 0;

                if (0 == pDcpt->rspMessage.fieldContentLength) {
                    pDcpt->state = HTTP_CLIENT_STATE_CLOSE;
                }
            } else {
                pDcpt->errorCode = HTTP_CLIENT_ERROR_RESPONSE_HEADERS;
                pDcpt->state = HTTP_CLIENT_STATE_CLOSE;
            }
        }
    } else {
        /* Header has already been received, message is all payload. */

        pPayload = pRecvBuffer;
        payloadSize = recvBufferLength;
    }

    if (0 == payloadSize) {
        pPayload = NULL;
    } else {
        /* We have new payload data. */

        pDcpt->contentLengthReceived += payloadSize;

        if (NULL != pDcpt->pClientEventHandler) {
            pDcpt->pClientEventHandler((HTTP_CLIENT_HANDLE) pDcpt, pDcpt->eventHandle, HTTP_CLIENT_EVENT_PAYLOAD_RECEIVED, pPayload, payloadSize);
        }

        if (pDcpt->contentLengthReceived == pDcpt->rspMessage.fieldContentLength) {
            pDcpt->state = HTTP_CLIENT_STATE_CLOSE;

            if (NULL != pDcpt->pClientEventHandler) {
                pDcpt->pClientEventHandler((HTTP_CLIENT_HANDLE) pDcpt, pDcpt->eventHandle, HTTP_CLIENT_EVENT_PAYLOAD_END, NULL, 0);
            }
        }
    }
}

HTTP_CLIENT_HANDLE HTTP_Client_Get(const char *pGetURL, HTTP_CLIENT_EVENT_HANDLER pClientEventHandler, uintptr_t eventHandle) {
    HTTP_CLIENT_HANDLE handle;
    HTTP_CLIENT_DCPT *pDcpt = NULL;

    if ((NULL == pGetURL) || (NULL == pClientEventHandler)) {
        return HTTP_CLIENT_HANDLE_INVALID;
    }

    handle = HTTP_Client_Open(pClientEventHandler, eventHandle);

    if (HTTP_CLIENT_HANDLE_INVALID == handle) {
        SYS_CONSOLE_PRINT("HTTP_CLIENT_HANDLE_INVALID == handle\n\r");
        return HTTP_CLIENT_HANDLE_INVALID;
    }

    pDcpt = (HTTP_CLIENT_DCPT*) handle;

    if (OSAL_RESULT_TRUE != OSAL_SEM_Pend(&httpClientStateSemaphore, OSAL_WAIT_FOREVER)) {
        return HTTP_CLIENT_HANDLE_INVALID;
    }

    pDcpt->pResponse = NULL;
    pDcpt->isResponseComplete = false;
    pDcpt->socket = NET_PRES_INVALID_SOCKET;
    pDcpt->sigHandler = NULL;
    pDcpt->clientRxReady = true;

    if (false == HTTP_Client_ParseURI(pGetURL, &pDcpt->uri)) {
        OSAL_SEM_Post(&httpClientStateSemaphore);
        return HTTP_CLIENT_HANDLE_INVALID;
    }

    pDcpt->ipAddress.v4Add.Val = 0;

    if ((pDcpt->uri.authorityHost[0] >= '0') && (pDcpt->uri.authorityHost[0] <= '9')) {
        TCPIP_Helper_StringToIPAddress(pDcpt->uri.authorityHost, &pDcpt->ipAddress.v4Add);
    }

    if (0 == pDcpt->ipAddress.v4Add.Val) {
        TCPIP_DNS_RESULT dnsResult;

        dnsResult = TCPIP_DNS_Resolve(pDcpt->uri.authorityHost, TCPIP_DNS_TYPE_A);

        if (dnsResult < 0) {
            pDcpt->errorCode = HTTP_CLIENT_ERROR_DNS_FAILED;
            pDcpt->state = HTTP_CLIENT_STATE_CLOSE;
        } else if (TCPIP_DNS_RES_NAME_IS_IPADDRESS == dnsResult) {
            TCPIP_Helper_StringToIPAddress(pDcpt->uri.authorityHost, &pDcpt->ipAddress.v4Add);

            pDcpt->state = HTTP_CLIENT_STATE_CONNECT;
        } else {
            pDcpt->lastDNSReqTimeMs = HttpGetSysTimeMs();
            pDcpt->state = HTTP_CLIENT_STATE_NAME_LOOKUP;
        }
    } else {
        pDcpt->state = HTTP_CLIENT_STATE_CONNECT;
    }

    OSAL_SEM_Post(&httpClientStateSemaphore);

    return handle;
}

void HTTP_Client_Cancel(HTTP_CLIENT_HANDLE handle) {
    HTTP_CLIENT_DCPT *pDcpt = (HTTP_CLIENT_DCPT*) handle;

    if (NULL == pDcpt) {
        return;
    }

    if (false == pDcpt->isValid) {
        return;
    }

    pDcpt->errorCode = HTTP_CLIENT_ERROR_USER_CANCELLED;
    pDcpt->state = HTTP_CLIENT_STATE_CLOSE;
}

bool HTTP_Client_GetPeerAddress(HTTP_CLIENT_HANDLE handle, IP_MULTI_ADDRESS *peerAddr) {
    HTTP_CLIENT_DCPT *pDcpt = (HTTP_CLIENT_DCPT*) handle;

    if ((NULL == pDcpt) || (NULL == peerAddr)) {
        return false;
    }

    if ((pDcpt->state >= HTTP_CLIENT_STATE_CONNECT) && (pDcpt->state < HTTP_CLIENT_STATE_CLOSE)) {
        memcpy(peerAddr, &pDcpt->ipAddress, sizeof (IP_MULTI_ADDRESS));
        return true;
    }

    return false;
}

NET_PRES_SKT_HANDLE_T HTTP_Client_GetNetPresSocket(HTTP_CLIENT_HANDLE handle) {
    HTTP_CLIENT_DCPT *pDcpt = (HTTP_CLIENT_DCPT*) handle;

    if (NULL == pDcpt) {
        return NET_PRES_INVALID_SOCKET;
    }

    return pDcpt->socket;
}

bool HTTP_Client_SetRxReady(HTTP_CLIENT_HANDLE handle, bool ready) {
    HTTP_CLIENT_DCPT *pDcpt = (HTTP_CLIENT_DCPT*) handle;

    if (NULL == pDcpt) {
        return false;
    }

    pDcpt->clientRxReady = ready;
    return true;
}

HTTP_CLIENT_HANDLE HTTP_Client_Task(HTTP_CLIENT_HANDLE handle) {
    HTTP_CLIENT_DCPT *pDcpt = (HTTP_CLIENT_DCPT*) handle;

    if (NULL == pDcpt) {
        return HTTP_CLIENT_HANDLE_INVALID;
    }

    if (false == pDcpt->isValid) {
        return HTTP_CLIENT_HANDLE_INVALID;
    }

    if (OSAL_RESULT_TRUE != OSAL_SEM_Pend(&httpClientStateSemaphore, OSAL_WAIT_FOREVER)) {
        return handle;
    }

    switch (pDcpt->state) {
        case HTTP_CLIENT_STATE_NAME_LOOKUP:
        {
            TCPIP_DNS_RESULT dnsResult;

            dnsResult = TCPIP_DNS_IsResolved(pDcpt->uri.authorityHost, &pDcpt->ipAddress, TCPIP_DNS_TYPE_A);

            if (dnsResult < 0) {
                pDcpt->errorCode = HTTP_CLIENT_ERROR_DNS_FAILED;
                pDcpt->state = HTTP_CLIENT_STATE_CLOSE;
            } else if (TCPIP_DNS_RES_PENDING == dnsResult) {
                if ((HttpGetSysTimeMs() - pDcpt->lastDNSReqTimeMs) > 1000) {
                    TCPIP_DNS_Resolve(pDcpt->uri.authorityHost, TCPIP_DNS_TYPE_A);
                    pDcpt->lastDNSReqTimeMs = HttpGetSysTimeMs();
                }
            } else {
                pDcpt->state = HTTP_CLIENT_STATE_CONNECT;
            }

            break;
        }

        case HTTP_CLIENT_STATE_CONNECT:
        {
            NET_PRES_SKT_ERROR_T sockError;

            if (URI_SCHEME_HTTP == pDcpt->uri.scheme) {
                pDcpt->socket = NET_PRES_SocketOpen(0, NET_PRES_SKT_UNENCRYPTED_STREAM_CLIENT, IP_ADDRESS_TYPE_IPV4, pDcpt->uri.authorityPort, (NET_PRES_ADDRESS*) & pDcpt->ipAddress, &sockError);
            } else if (URI_SCHEME_HTTPS == pDcpt->uri.scheme) {
                pDcpt->socket = NET_PRES_SocketOpen(0, NET_PRES_SKT_ENCRYPTED_STREAM_CLIENT, IP_ADDRESS_TYPE_IPV4, pDcpt->uri.authorityPort, (NET_PRES_ADDRESS*) & pDcpt->ipAddress, &sockError);
            }

            if (NET_PRES_INVALID_SOCKET != pDcpt->socket) {
                pDcpt->state = HTTP_CLIENT_STATE_CONNECTING;

                pDcpt->sigHandler = NET_PRES_SocketSignalHandlerRegister(pDcpt->socket, TCPIP_TCP_SIGNAL_RX_RST, _socketSignalHandler, pDcpt);
                pDcpt->socketTimer = HttpGetSysTimeMs();

                NET_PRES_SocketOptionsSet(pDcpt->socket, TCP_OPTION_RX_BUFF, (void*) HTTP_TCP_RX_WINDOW_SIZE);
            } else {
                pDcpt->errorCode = HTTP_CLIENT_ERROR_CONNECT_FAILED;
                pDcpt->state = HTTP_CLIENT_STATE_CLOSE;
            }

            break;
        }

        case HTTP_CLIENT_STATE_CONNECTING:
        {
            if ((HttpGetSysTimeMs() - pDcpt->socketTimer) > HTTP_CLIENT_SOCKET_CONN_TIMEOUT) {
                pDcpt->errorCode = HTTP_CLIENT_ERROR_CONNECT_FAILED;
                pDcpt->state = HTTP_CLIENT_STATE_CLOSE;
                SYS_CONSOLE_PRINT("Socket connect timeout\n\r");
                break;
            }

            if (URI_SCHEME_HTTP == pDcpt->uri.scheme) {
                if (true == NET_PRES_SocketIsConnected(pDcpt->socket)) {
                    pDcpt->state = HTTP_CLIENT_STATE_CONNECTED;
                }
            } else if (URI_SCHEME_HTTPS == pDcpt->uri.scheme) {
                if (true == NET_PRES_SocketIsSecure(pDcpt->socket)) {
                    if (true == NET_PRES_SocketIsConnected(pDcpt->socket)) {
                        pDcpt->state = HTTP_CLIENT_STATE_CONNECTED;
                    }
                } else if (false == NET_PRES_SocketIsNegotiatingEncryption(pDcpt->socket)) {
                    pDcpt->errorCode = HTTP_CLIENT_ERROR_CONNECT_FAILED;
                    pDcpt->state = HTTP_CLIENT_STATE_CLOSE;
                }
            }

            break;
        }

        case HTTP_CLIENT_STATE_CONNECTED:
        {
            char *pSendBuffer;

            pSendBuffer = OSAL_Malloc(HTTP_CLIENT_BUFFER_SIZE);

            if (NULL != pSendBuffer) {
                int sendLength;

                sendLength = snprintf(pSendBuffer, HTTP_CLIENT_BUFFER_SIZE, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", pDcpt->uri.path, pDcpt->uri.authorityHost);

                if (NET_PRES_SocketWriteIsReady(pDcpt->socket, sendLength, 0)) {
                    if (0 == NET_PRES_SocketWrite(pDcpt->socket, pSendBuffer, sendLength)) {
                        pDcpt->errorCode = HTTP_CLIENT_ERROR_SEND_FAILED;
                        pDcpt->state = HTTP_CLIENT_STATE_CLOSE;
                    } else {
                        pDcpt->state = HTTP_CLIENT_STATE_REQUEST_SENT;
                        if (NULL != pDcpt->pClientEventHandler) {
                            pDcpt->pClientEventHandler((HTTP_CLIENT_HANDLE) pDcpt, pDcpt->eventHandle, HTTP_CLIENT_EVENT_GET_SENT, pSendBuffer, sendLength);
                        }

                        NET_PRES_SocketWasReset(pDcpt->socket);
                    }
                }

                OSAL_Free(pSendBuffer);
            }

            break;
        }

        case HTTP_CLIENT_STATE_REQUEST_SENT:
        {
            uint16_t readDataLength;

            while (NET_PRES_SocketReadIsReady(pDcpt->socket) > 0) {
                if (pDcpt->clientRxReady == false) {
                    break;
                }

                readDataLength = NET_PRES_SocketRead(pDcpt->socket, pDcpt->recvBuffer, HTTP_CLIENT_BUFFER_SIZE);

                if (readDataLength > 0) {
                    HTTP_Client_SocketReceveProcess(handle, pDcpt->recvBuffer, readDataLength);
                }
            }

            break;
        }

        case HTTP_CLIENT_STATE_CLOSE:
        {
            if (NULL != pDcpt->pClientEventHandler) {
                if (HTTP_CLIENT_ERROR_NONE != pDcpt->errorCode) {
                    pDcpt->pClientEventHandler((HTTP_CLIENT_HANDLE) pDcpt, pDcpt->eventHandle, HTTP_CLIENT_EVENT_ERROR, &pDcpt->errorCode, sizeof (HTTP_CLIENT_ERRORS));
                }

                pDcpt->pClientEventHandler((HTTP_CLIENT_HANDLE) pDcpt, pDcpt->eventHandle, HTTP_CLIENT_EVENT_CLOSE, NULL, 0);
            }

            if (NET_PRES_INVALID_SOCKET != pDcpt->socket) {
                NET_PRES_SocketSignalHandlerDeregister(pDcpt->socket, pDcpt->sigHandler);
                NET_PRES_SocketClose(pDcpt->socket);
                pDcpt->socket = NET_PRES_INVALID_SOCKET;
            }

            pDcpt->isValid = false;

            if (NULL != pDcpt->pResponse) {
                OSAL_Free(pDcpt->pResponse);
                pDcpt->pResponse = NULL;
            }

            handle = HTTP_CLIENT_HANDLE_INVALID;
            break;
        }

        case HTTP_CLIENT_STATE_UNKNOWN:
        default:
        {
            break;
        }
    }

    OSAL_SEM_Post(&httpClientStateSemaphore);

    return handle;
}
