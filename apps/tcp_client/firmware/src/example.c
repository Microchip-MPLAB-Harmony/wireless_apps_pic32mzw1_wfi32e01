/*******************************************************************************
  WINC Example Application

  File Name:
    tcp_client.c

  Summary:
    WINC TCP client example.

  Description:
    This example demonstrates the use of TCP client socket with the WINC.

    The configuration defines for this demo are:
        WLAN_SSID           -- BSS to search for
        WLAN_AUTH           -- Security for the BSS
        WLAN_PSK            -- Passphrase for WPA security
        TCP_SERVER_IP_ADDR  -- IP address for the TCP server
        TCP_SERVER_PORT_NUM -- Port number for the TCP server
        TCP_BUFFER_SIZE     -- Size of the socket buffer holding the receive data
        TCP_SEND_MESSAGE    -- Customizable TCP packet content
*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#include "app.h"
#include "wdrv_winc_client_api.h"
#include "example_conf.h"

extern APP_DATA appData;

typedef enum
{
    /* Example's state machine's initial state. */
    EXAMP_STATE_INIT=0,
    EXAMP_STATE_CONNECTING,
    EXAMP_STATE_CONNECTED,
    EXAMP_STATE_SOCKET_OPEN,
    EXAMP_STATE_DISCONNECTED,
    EXAMP_STATE_ERROR,
} EXAMP_STATES;

static EXAMP_STATES state;
static SOCKET clientSocket;
static uint8_t recvBuffer[TCP_BUFFER_SIZE];

static void APP_ExampleSocketEventCallback(SOCKET socket, uint8_t messageType, void *pMessage)
{
    if (socket != clientSocket)
    {
        return;
    }

    switch(messageType)
    {
        case SOCKET_MSG_CONNECT:
        {
            tstrSocketConnectMsg *pConnectMessage = (tstrSocketConnectMsg*)pMessage;

            if ((NULL != pConnectMessage) && (pConnectMessage->s8Error >= 0))
            {
                SYS_CONSOLE_Print(appData.consoleHandle, "Socket %d connect success\r\n", socket);
                recv(clientSocket, recvBuffer, TCP_BUFFER_SIZE, 0);
                send(clientSocket, TCP_SEND_MESSAGE, sizeof(TCP_SEND_MESSAGE), 0);
            }
            else
            {
                SYS_CONSOLE_Print(appData.consoleHandle, "Socket %d connect error %d\r\n", socket, pConnectMessage->s8Error);

                if (-1 != clientSocket)
                {
                    shutdown(clientSocket);
                    clientSocket = -1;
                }

                state = EXAMP_STATE_ERROR;
            }
            break;
        }

        case SOCKET_MSG_RECV:
        {
            tstrSocketRecvMsg *pRecvMessage = (tstrSocketRecvMsg*)pMessage;

            if ((NULL != pRecvMessage) && (pRecvMessage->s16BufferSize > 0))
            {
                SYS_CONSOLE_Print(appData.consoleHandle, "Socket receive, size = %d bytes ", pRecvMessage->s16BufferSize);

                if (pRecvMessage->s16BufferSize > 20)
                {
                    pRecvMessage->pu8Buffer[20] = '\0';
                }
                else
                {
                    pRecvMessage->pu8Buffer[pRecvMessage->s16BufferSize] = '\0';
                }

                SYS_CONSOLE_Print(appData.consoleHandle, (char*)pRecvMessage->pu8Buffer);

                if (pRecvMessage->s16BufferSize > 20)
                {
                    SYS_CONSOLE_Print(appData.consoleHandle, "...");
                }

                SYS_CONSOLE_Print(appData.consoleHandle, "]\r\n");

                recv(socket, recvBuffer, TCP_BUFFER_SIZE, 0);
            }
            else
            {
                SYS_CONSOLE_Print(appData.consoleHandle, "Receive on socket %d failed\r\n", socket);

                if (-1 != clientSocket)
                {
                    shutdown(clientSocket);
                    clientSocket = -1;
                }
            }
            break;
        }

        case SOCKET_MSG_SEND:
        {
            SYS_CONSOLE_Print(appData.consoleHandle, "Socket %d send completed\r\n", socket);
            break;
        }

        default:
        {
            break;
        }
    }
}

static void APP_ExampleDHCPAddressEventCallback(DRV_HANDLE handle, uint32_t ipAddress)
{
    char s[20];

    SYS_CONSOLE_Print(appData.consoleHandle, "IP address is %s\r\n", inet_ntop(AF_INET, &ipAddress, s, sizeof(s)));
}

static void APP_ExampleConnectNotifyCallback(DRV_HANDLE handle, WDRV_WINC_ASSOC_HANDLE assocHandle, WDRV_WINC_CONN_STATE currentState, WDRV_WINC_CONN_ERROR errorCode)
{
    if (WDRV_WINC_CONN_STATE_CONNECTED == currentState)
    {
        SYS_CONSOLE_Print(appData.consoleHandle, "Connected\r\n");

        state = EXAMP_STATE_CONNECTED;
    }
    else if (WDRV_WINC_CONN_STATE_DISCONNECTED == currentState)
    {
        if (EXAMP_STATE_CONNECTING == state)
        {
            SYS_CONSOLE_Print(appData.consoleHandle, "Failed to connect\r\n");

            state = EXAMP_STATE_INIT;
        }
        else
        {
            SYS_CONSOLE_Print(appData.consoleHandle, "Disconnected\r\n");

            if (-1 != clientSocket)
            {
                shutdown(clientSocket);
                clientSocket = -1;
            }

            state = EXAMP_STATE_DISCONNECTED;
        }
    }
}

void APP_ExampleInitialize(DRV_HANDLE handle)
{
    SYS_CONSOLE_Print(appData.consoleHandle, "\r\n");
    SYS_CONSOLE_Print(appData.consoleHandle, "===========================\r\n");
    SYS_CONSOLE_Print(appData.consoleHandle, "WINC TCP Client Example\r\n");
    SYS_CONSOLE_Print(appData.consoleHandle, "===========================\r\n");

    state = EXAMP_STATE_INIT;

    clientSocket = -1;
}

void APP_ExampleTasks(DRV_HANDLE handle)
{
    switch (state)
    {
        case EXAMP_STATE_INIT:
        {
            WDRV_WINC_AUTH_CONTEXT authCtx;
            WDRV_WINC_BSS_CONTEXT  bssCtx;

            /* Enable use of DHCP for network configuration, DHCP is the default
             but this also registers the callback for notifications. */

            WDRV_WINC_IPUseDHCPSet(handle, &APP_ExampleDHCPAddressEventCallback);

            /* Preset the error state incase any following operations fail. */

            state = EXAMP_STATE_ERROR;

            /* Initialize the BSS context to use default values. */

            if (WDRV_WINC_STATUS_OK != WDRV_WINC_BSSCtxSetDefaults(&bssCtx))
            {
                break;
            }

            /* Update BSS context with target SSID for connection. */

            if (WDRV_WINC_STATUS_OK != WDRV_WINC_BSSCtxSetSSID(&bssCtx, (uint8_t*)WLAN_SSID, strlen(WLAN_SSID)))
            {
                break;
            }

#if defined(WLAN_AUTH_OPEN)
            /* Initialize the authentication context for open mode. */

            if (WDRV_WINC_STATUS_OK != WDRV_WINC_AuthCtxSetOpen(&authCtx))
            {
                break;
            }
#elif defined(WLAN_AUTH_WPA_PSK)
            /* Initialize the authentication context for WPA. */

            if (WDRV_WINC_STATUS_OK != WDRV_WINC_AuthCtxSetWPA(&authCtx, (uint8_t*)WLAN_PSK, strlen(WLAN_PSK)))
            {
                break;
            }
#endif

            /* Connect to the target BSS with the chosen authentication. */

            if (WDRV_WINC_STATUS_OK == WDRV_WINC_BSSConnect(handle, &bssCtx, &authCtx, &APP_ExampleConnectNotifyCallback))
            {
                state = EXAMP_STATE_CONNECTING;
            }

            /* Register callback for socket events. */

            WDRV_WINC_SocketRegisterEventCallback(handle, &APP_ExampleSocketEventCallback);
            break;
        }

        case EXAMP_STATE_CONNECTING:
        {
            /* Wait for the BSS connect to trigger the callback and update
             the connection state. */
            break;
        }

        case EXAMP_STATE_CONNECTED:
        {
            /* Wait for the IP link to become active. */

            if (false == WDRV_WINC_IPLinkActive(handle))
            {
                break;
            }

            state = EXAMP_STATE_ERROR;

            /* Create the client socket. */

            clientSocket = socket(AF_INET, SOCK_STREAM, 0);

            if (clientSocket >= 0)
            {
                struct sockaddr_in addr;

                /* Connect the socket to the server. */

                addr.sin_family = AF_INET;
                addr.sin_port = _htons(TCP_SERVER_PORT_NUM);
                addr.sin_addr.s_addr = inet_addr(TCP_SERVER_IP_ADDR);

                if (connect(clientSocket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) >= 0)
                {
                    state = EXAMP_STATE_SOCKET_OPEN;
                }
                else if (-1 != clientSocket)
                {
                    shutdown(clientSocket);
                    clientSocket = -1;
                }
            }

            break;
        }

        case EXAMP_STATE_SOCKET_OPEN:
        {
            break;
        }

        case EXAMP_STATE_DISCONNECTED:
        {
            /* If we become disconnected, trigger a reconnection. */

            if (WDRV_WINC_STATUS_OK == WDRV_WINC_BSSReconnect(handle, &APP_ExampleConnectNotifyCallback))
            {
                state = EXAMP_STATE_CONNECTING;
            }
            break;
        }

        case EXAMP_STATE_ERROR:
        {
            break;
        }

        default:
        {
            break;
        }
    }
}
