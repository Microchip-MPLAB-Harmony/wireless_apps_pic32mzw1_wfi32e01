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
Copyright (C) 2020-2021 released Microchip Technology Inc.  All rights reserved.

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

#include "app.h"
#include "app_mqtt.h"

#ifdef SYS_MQTT_DEF_PUB_TOPIC_NAME
#include "system/mqtt/sys_mqtt.h"
#include "system/sys_time_h2_adapter.h"
#include "string.h"
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#ifdef SYS_MQTT_DEF_PUB_TOPIC_NAME
static uint32_t g_lastPubTimeout = 0;
#define MQTT_PERIOIDC_PUB_TIMEOUT   30 //Sec ; if the value is 0, Periodic Publish will disable
#define MQTT_PUB_TIMEOUT_CONST (MQTT_PERIOIDC_PUB_TIMEOUT * SYS_TMR_TickCounterFrequencyGet())
#endif
static SYS_WIFI_CONFIG wificonfig;


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
 */

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
 */

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************
#ifdef SYS_MQTT_DEF_PUB_TOPIC_NAME

void APP_CheckTimeOut(uint32_t timeOutValue, uint32_t lastTimeOut) {
    if (timeOutValue == 0) {
        return;
    }

    if (SYS_MQTT_STATUS_MQTT_CONNECTED != APP_MQTT_GetStatus(NULL)) {
        return;
    }

    if (lastTimeOut == 0) {
        g_lastPubTimeout = SYS_TMR_TickCountGet();
        return;
    }

    if (SYS_TMR_TickCountGet() - lastTimeOut > timeOutValue) {
        char message[32] = {0};
        static uint32_t PubMsgCnt = 0;

        sprintf(message, "message_%d\r\n", PubMsgCnt);
        if (APP_MQTT_PublishMsg(message) == SYS_MQTT_SUCCESS) {
            SYS_CONSOLE_PRINT("\nPublished Msg(%d) to Topic\r\n", PubMsgCnt);
            PubMsgCnt++;
        }

        g_lastPubTimeout = SYS_TMR_TickCountGet();
    }
}
#endif


/* TODO:  Add any necessary local functions.
 */

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

void APP_Initialize(void) {
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT_DONE;



    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks(void) {
    /* Check the application's current state. */
    switch (appData.state) {
            /* Application's initial state. */
        case APP_STATE_INIT_DONE:
        {
            if (SYS_WIFI_CtrlMsg(sysObj.syswifi, SYS_WIFI_GETWIFICONFIG, &wificonfig, sizeof (wificonfig)) == SYS_WIFI_SUCCESS) {
                if ((SYS_WIFI_STA == wificonfig.mode) && (SYS_WIFI_GetStatus(sysObj.syswifi) == SYS_WIFI_STATUS_TCPIP_READY)) {
                    appData.state = APP_STATE_MODE_STA;
                } 
            }
            break;
        }

        case APP_STATE_MODE_STA:
        {
            APP_MQTT_Initialize();
            appData.state = APP_STATE_SERVICE_TASK;
            break;
        }

        case APP_STATE_SERVICE_TASK:
        {
#ifdef SYS_MQTT_DEF_PUB_TOPIC_NAME		
            APP_CheckTimeOut(MQTT_PUB_TIMEOUT_CONST, g_lastPubTimeout);
#endif
            break;
        }
        default:
        {
            break;
        }
    }

    APP_MQTT_Tasks(); 
}


/*******************************************************************************
 End of File
 */
