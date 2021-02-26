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
SYS_MODULE_OBJ g_tcpServHandle = SYS_MODULE_OBJ_INVALID;
SYS_NET_Config g_tcpServCfg;

#define RECV_BUFFER_LEN		1400
uint8_t recv_buffer[RECV_BUFFER_LEN + 1];

uint16_t position = 0;
extern volatile uint8_t measurement_done_touch;
uint8_t valButton_1, valButton_2, valSlider;

void TcpServCallback(uint32_t event, void *data, void* cookie)
{
    char message[25];
    
    switch(event)
    {
        case SYS_NET_EVNT_CONNECTED:
        {
            SYS_CONSOLE_PRINT("TcpServCallback(): Status UP\r\n");
            sprintf(message, "{\"data\":\"Hello\"}");
            while(SYS_NET_SendMsg(g_tcpServHandle, (uint8_t*)message , strlen(message)) <= 0);            
            break;
        }

        case SYS_NET_EVNT_DISCONNECTED:
        {
            SYS_CONSOLE_PRINT("TcpServCallback(): Status DOWN\r\n");
            break;
        }

        case SYS_NET_EVNT_RCVD_DATA:
        {
			int32_t cumm_len = 0;
            int32_t len = RECV_BUFFER_LEN;
            while(len == RECV_BUFFER_LEN)
            {
                len = SYS_NET_RecvMsg(g_tcpServHandle, recv_buffer, RECV_BUFFER_LEN);
                if(len>0)
                {
                    if(cumm_len == 0)
                    {
                            uint8_t buffer[33];
                            int32_t tmp_len = (len > 32)? 32 : len;

                            memcpy(buffer, recv_buffer, tmp_len);
                            buffer[tmp_len] = '\0';
                            SYS_CONSOLE_PRINT("TcpServCallback(): Data Rcvd = %s\r\n", buffer);
                    }
                    cumm_len += len;
                    SYS_NET_SendMsg(g_tcpServHandle, recv_buffer, len);
                }
            }
            SYS_CONSOLE_PRINT("TcpServCallback(): Total Data Rcvd = %d Bytes\r\n", cumm_len);
            
            break;
        }
    }
}


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
void APP_Initialize ( void )
{
    SYS_CONSOLE_MESSAGE("APP_Initialize\n");
   
    g_tcpServCfg.mode = SYS_NET_INDEX0_MODE;
    g_tcpServCfg.port = SYS_NET_INDEX0_PORT;
    g_tcpServCfg.enable_tls = SYS_NET_INDEX0_ENABLE_TLS;
    g_tcpServCfg.ip_prot = SYS_NET_INDEX0_IPPROT;
    g_tcpServCfg.enable_reconnect = SYS_NET_INDEX0_RECONNECT;
    g_tcpServHandle = SYS_NET_Open(&g_tcpServCfg, TcpServCallback, 0); 
    if(g_tcpServHandle != SYS_MODULE_OBJ_INVALID)
    SYS_CONSOLE_PRINT("TCP Service Initialized Successfully\r\n");
    
    /* Initialize LED pins */
    GPIO_PinOutputEnable(GPIO_PIN_RK6); //button 1
    GPIO_PinClear(GPIO_PIN_RK6);
    GPIO_PinSet(GPIO_PIN_RK6);

    GPIO_PinOutputEnable(GPIO_PIN_RA5); //button 2
    GPIO_PinClear(GPIO_PIN_RA5);
    GPIO_PinSet(GPIO_PIN_RA5);
    
    GPIO_PinOutputEnable(GPIO_PIN_RA11); //slider 1
    GPIO_PinClear(GPIO_PIN_RA11);
    GPIO_PinSet(GPIO_PIN_RA11);
    
    GPIO_PinOutputEnable(GPIO_PIN_RK5); //slider 2
    GPIO_PinClear(GPIO_PIN_RK5);
    GPIO_PinSet(GPIO_PIN_RK5);
    
    GPIO_PinOutputEnable(GPIO_PIN_RK4); //slider 3
    GPIO_PinClear(GPIO_PIN_RK4);
    GPIO_PinSet(GPIO_PIN_RK4);

    GPIO_PinOutputEnable(GPIO_PIN_RB7); //slider 4
    GPIO_PinClear(GPIO_PIN_RB7);
    GPIO_PinSet(GPIO_PIN_RB7);
    
    GPIO_PinOutputEnable(GPIO_PIN_RA4); //slider 5
    GPIO_PinClear(GPIO_PIN_RA4);
    GPIO_PinSet(GPIO_PIN_RA4);

    GPIO_PinOutputEnable(GPIO_PIN_RB12); //slider 6
    GPIO_PinClear(GPIO_PIN_RB12);
    GPIO_PinSet(GPIO_PIN_RB12);
    
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    uint8_t temp_Button_1, temp_Button_2;
    char message[20];
    
    SYS_CMD_READY_TO_READ();
    SYS_NET_Task(g_tcpServHandle);
    
    while (!measurement_done_touch) 
    {
        touch_process();
    }
    measurement_done_touch = 0;

    temp_Button_1 = (get_sensor_state(0) & 0x80);
    if (temp_Button_1 != valButton_1)
    {
        valButton_1 = temp_Button_1;
        memset(message, 0, 20);
        sprintf(message, "{\"Button1\":%d}", valButton_1);
        if (SYS_NET_STATUS_CONNECTED == SYS_NET_GetStatus(g_tcpServHandle))
        {
            while(SYS_NET_SendMsg(g_tcpServHandle, (uint8_t*)message, strlen(message)) <= 0);
        }
    }
    if(valButton_1)
    {
        GPIO_PinClear(GPIO_PIN_RK6);
    }
    else
    {
        GPIO_PinSet(GPIO_PIN_RK6);
    }

    temp_Button_2 = (get_sensor_state(1) & 0x80);
    if (temp_Button_2 != valButton_2)
    {
        valButton_2 = temp_Button_2;
        memset(message, 0, 20);
        sprintf(message, "{\"Button2\":%d}", valButton_2);
        if (SYS_NET_STATUS_CONNECTED == SYS_NET_GetStatus(g_tcpServHandle))
        {
            while(SYS_NET_SendMsg(g_tcpServHandle, (uint8_t*)message, strlen(message)) <= 0);
        }
    }
    if(valButton_2)
    {
        GPIO_PinClear(GPIO_PIN_RA5);
    }
    else
    {
        GPIO_PinSet(GPIO_PIN_RA5);
    }

    if(get_scroller_state(0) == 0u)
    {
        GPIO_PinSet(GPIO_PIN_RA11);
        GPIO_PinSet(GPIO_PIN_RK5);
        GPIO_PinSet(GPIO_PIN_RK4);
        GPIO_PinSet(GPIO_PIN_RB7);
        GPIO_PinSet(GPIO_PIN_RA4);
        GPIO_PinSet(GPIO_PIN_RB12);  
    }
    else
    {
        position = get_scroller_position(0);

        if (valSlider != position)
        {
            valSlider = position;
            memset(message, 0, 20);
            sprintf(message, "{\"Slider\":%d}", valSlider);
            if (SYS_NET_STATUS_CONNECTED == SYS_NET_GetStatus(g_tcpServHandle))
            {
                while(SYS_NET_SendMsg(g_tcpServHandle, (uint8_t*)message, strlen(message)) <= 0);
            }
        }

        if (position < 42)
        {
            GPIO_PinClear(GPIO_PIN_RA11);
            GPIO_PinSet(GPIO_PIN_RK5);
            GPIO_PinSet(GPIO_PIN_RK4);
            GPIO_PinSet(GPIO_PIN_RB7);
            GPIO_PinSet(GPIO_PIN_RA4);
            GPIO_PinSet(GPIO_PIN_RB12);
        }
        else if (position < 85)
        {
            GPIO_PinClear(GPIO_PIN_RA11);
            GPIO_PinClear(GPIO_PIN_RK5);
            GPIO_PinSet(GPIO_PIN_RK4);
            GPIO_PinSet(GPIO_PIN_RB7);
            GPIO_PinSet(GPIO_PIN_RA4);
            GPIO_PinSet(GPIO_PIN_RB12);
        }
        else if (position < 128)
        {
            GPIO_PinClear(GPIO_PIN_RA11);
            GPIO_PinClear(GPIO_PIN_RK5);
            GPIO_PinClear(GPIO_PIN_RK4);
            GPIO_PinSet(GPIO_PIN_RB7);
            GPIO_PinSet(GPIO_PIN_RA4);
            GPIO_PinSet(GPIO_PIN_RB12);
        }
        else if (position < 171)
        {
            GPIO_PinClear(GPIO_PIN_RA11);
            GPIO_PinClear(GPIO_PIN_RK5);
            GPIO_PinClear(GPIO_PIN_RK4);
            GPIO_PinClear(GPIO_PIN_RB7);
            GPIO_PinSet(GPIO_PIN_RA4);
            GPIO_PinSet(GPIO_PIN_RB12);
        }
        else if (position < 214)
        {
            GPIO_PinClear(GPIO_PIN_RA11);
            GPIO_PinClear(GPIO_PIN_RK5);
            GPIO_PinClear(GPIO_PIN_RK4);
            GPIO_PinClear(GPIO_PIN_RB7);
            GPIO_PinClear(GPIO_PIN_RA4);
            GPIO_PinSet(GPIO_PIN_RB12);
        }
        else if (position < 256)
        {
            GPIO_PinClear(GPIO_PIN_RA11);
            GPIO_PinClear(GPIO_PIN_RK5);
            GPIO_PinClear(GPIO_PIN_RK4);
            GPIO_PinClear(GPIO_PIN_RB7);
            GPIO_PinClear(GPIO_PIN_RA4);
            GPIO_PinClear(GPIO_PIN_RB12);           
        }
    }        
}


/*******************************************************************************
 End of File
 */
