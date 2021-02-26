/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_pic32mzw1.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/
/*******************************************************************************
Copyright (C) 2020 released Microchip Technology Inc.  All rights reserved.

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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "system/net/sys_net.h"

// *****************************************************************************
// *****************************************************************************
// Section: Declarations
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
SYS_MODULE_OBJ      g_tcpSrvcHandle = SYS_MODULE_OBJ_INVALID;


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
void TcpClientCallback(uint32_t event, void *data, void* cookie)
{
    switch(event)
    {
        case SYS_NET_EVNT_CONNECTED:
        {
            char networkBuffer[256];
            SYS_CONSOLE_PRINT("TcpClientCallback(): Status UP\r\n");
            memset(networkBuffer, 0, sizeof(networkBuffer));
            sprintf(networkBuffer, "GET / HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "Connection: close\r\n\r\n", g_sSysNetConfig0.host_name);
            
            while(SYS_NET_SendMsg(g_tcpSrvcHandle, (uint8_t*) networkBuffer, strlen(networkBuffer)) == 0);
            break;
        }

        case SYS_NET_EVNT_DISCONNECTED:
        {
            SYS_CONSOLE_PRINT("TcpClientCallback(): Status DOWN\r\n");
            break;
        }

        case SYS_NET_EVNT_RCVD_DATA:
        {
            char networkBuffer[256];
            memset(networkBuffer, 0, sizeof (networkBuffer));
            SYS_NET_RecvMsg(g_tcpSrvcHandle, (uint8_t*) networkBuffer, sizeof (networkBuffer));
            // Google has stopped obeying the 'connection close' command.  So this is a cheap way to see if we got to the 
            // end of the html page.
            if (strstr(networkBuffer, "</html>") != 0) {
                SYS_CONSOLE_PRINT("TcpClientCallback(): End of Html Page\r\n");
            }
            break;
        }

        case SYS_NET_EVNT_SSL_FAILED:
        {
            SYS_CONSOLE_PRINT("TcpClientCallback(): SSL Negotiation Failed\r\n");
            break;
        }

        case SYS_NET_EVNT_DNS_RESOLVE_FAILED:
        {
            SYS_CONSOLE_PRINT("TcpClientCallback(): DNS Resolution Failed\r\n");
            break;
        }

        case SYS_NET_EVNT_SOCK_OPEN_FAILED:
        {
            SYS_CONSOLE_PRINT("TcpClientCallback(): Socket Open Failed\r\n");
            break;
        }
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: Local data
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */
void APP_PIC32MZW1_Initialize ( void )
{
    SYS_CONSOLE_MESSAGE("APP_Initialize\n");
	g_tcpSrvcHandle = SYS_NET_Open(NULL, TcpClientCallback, 0);				
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_PIC32MZW1_Tasks ( void )
{
    /* Check the application's current state. */
    SYS_CMD_READY_TO_READ();
	SYS_NET_Task(g_tcpSrvcHandle);
}


/*******************************************************************************
 End of File
 */
