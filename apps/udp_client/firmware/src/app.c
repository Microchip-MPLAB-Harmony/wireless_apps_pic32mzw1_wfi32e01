/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

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
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
static SYS_MODULE_OBJ      g_udpServHandle = SYS_MODULE_OBJ_INVALID;

#define RECV_BUFFER_LEN		1400
uint8_t recv_buffer[RECV_BUFFER_LEN + 1];

void UdpClientCallback(uint32_t event, void *data, void* cookie)
{
    switch(event)
    {
        case SYS_NET_EVNT_CONNECTED:
        {
            const char *str = "hello";
            SYS_CONSOLE_PRINT("UdpClientCallback(): Status UP\r\n");
            while(SYS_NET_SendMsg(g_udpServHandle, (uint8_t *)str, 5) <= 0);
            break;
        }

        case SYS_NET_EVNT_DISCONNECTED:
        {
            SYS_CONSOLE_PRINT("UdpClientCallback(): Status DOWN\r\n");
            break;
        }

        case SYS_NET_EVNT_RCVD_DATA:
        {
			int32_t cumm_len = 0;
            int32_t len = RECV_BUFFER_LEN;
            while(len == RECV_BUFFER_LEN)
            {
                len = SYS_NET_RecvMsg(g_udpServHandle, recv_buffer, RECV_BUFFER_LEN);
                if(len>0)
                {
                    if(cumm_len == 0)
                    {
                            uint8_t buffer[33];
                            int32_t tmp_len = (len > 32)? 32 : len;

                            memcpy(buffer, recv_buffer, tmp_len);
                            buffer[tmp_len] = '\0';
                            SYS_CONSOLE_PRINT("UdpClientCallback(): Data Rcvd = %s\r\n", buffer);
                    }
                    cumm_len += len;
                }
            }
            SYS_CONSOLE_PRINT("UdpClientCallback(): Total Data Rcvd = %d Bytes\r\n", cumm_len);
            
            break;
        }
    }
}

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */
void APP_Initialize ( void )
{
    SYS_CONSOLE_MESSAGE("APP_Initialize\n");
   
    g_udpServHandle = SYS_NET_Open(NULL, UdpClientCallback, 0); 
    if(g_udpServHandle != SYS_MODULE_OBJ_INVALID)
        SYS_CONSOLE_PRINT("UDP Service Initialized Successfully\r\n");
    
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    SYS_CMD_READY_TO_READ();
    SYS_NET_Task(g_udpServHandle);
}


/*******************************************************************************
 End of File
 */
