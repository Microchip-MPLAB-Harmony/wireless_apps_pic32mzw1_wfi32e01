/*******************************************************************************
 MPLAB Harmony Web Socket Server system service template application Source File

  File Name:
    app_wss.c

  Summary:
    Template source file demonstrating the application implementation utilizing the Web Socket Server system service.

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

void wss_user_callback(SYS_WSS_EVENTS event, void *data, int32_t clientIndex, void *cookie) {

    int i = 0;
    switch (event) {
        
        case SYS_WSS_EVENT_CLIENT_CONNECTING:
        {
            SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CONNECTING\r\n",clientIndex);
            break;
        }
        case SYS_WSS_EVENT_CLIENT_CONNECTED:
        {

            SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CONNECTED\r\n",clientIndex);

            break;
        }
        case SYS_WSS_EVENT_CLIENT_BIN_DATA_RX:
        {
            SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_BIN_DATA_RX\r\n",clientIndex);
            for (i = 0; i < ((SYS_WSS_RXDATA*) data)->datalen; i++) {
                SYS_CONSOLE_PRINT("%X ", ((SYS_WSS_RXDATA*) data)->data[i]);
            }
            SYS_CONSOLE_PRINT("\r\n");
            //echo server.
            SYS_WSS_sendMessage(1, SYS_WSS_FRAME_TEXT, ((SYS_WSS_RXDATA*) data)->data, ((SYS_WSS_RXDATA *) data)->datalen, clientIndex);
            break;
        }
        case SYS_WSS_EVENT_CLIENT_TXT_DATA_RX:
        {

            SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_TXT_DATA_RX\r\n",clientIndex);
            for (i = 0; i < ((SYS_WSS_RXDATA *) data)->datalen; i++) {
                SYS_CONSOLE_PRINT("%c", ((SYS_WSS_RXDATA*) data)->data[i]);
            }
            SYS_CONSOLE_PRINT("\r\n");
            //echo server
            SYS_WSS_sendMessage(1, SYS_WSS_FRAME_TEXT, ((SYS_WSS_RXDATA*) data)->data, ((SYS_WSS_RXDATA *) data)->datalen, clientIndex);
            break;
        }
        case SYS_WSS_EVENT_CLIENT_CLOSING:
        {
            SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CLOSING\r\n",clientIndex);
            break;
        }
        case SYS_WSS_EVENT_CLIENT_CLOSED:
        {
            SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CLOSED\r\n",clientIndex);
            break;
        }
        case SYS_WSS_EVENT_ERR_INVALID_FRAME:
        {
            SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_ERR_INVALID_FRAME\r\n",clientIndex);
            break;
        }
        case SYS_WSS_EVENT_CLIENT_PING_RX:
        {
            SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_PING_RX\r\n",clientIndex);
            break;
        }
        case SYS_WSS_EVENT_CLIENT_PONG_RX:
        {
            SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_PONG_RX\r\n",clientIndex);
            break;
        }
        case SYS_WSS_EVENT_CLIENT_CLOSE_FRAME_RX:
        {
            SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CLOSE_FRAME_RX\r\n",clientIndex);
            break;
        }
        case SYS_WSS_EVENT_ERR:
        {
            SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_ERR\r\n",clientIndex);
            break;
        }

        case SYS_WSS_EVENT_CLIENT_TIMEOUT:
        {
            SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_TIMEOUT\r\n",clientIndex);
            break;
        }
    }
}