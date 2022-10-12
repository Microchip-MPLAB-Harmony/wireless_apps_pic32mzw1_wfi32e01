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

#define APP_IFTTT_EVENT_BUTTON_PRESS    0
#define APP_IFTTT_EVENT_PERIODIC_TIMER  1

//IFTTT related configuration
#define APP_IFTTT_EVENT_TYPE            APP_IFTTT_EVENT_BUTTON_PRESS //Event Type - Button Press/ Periodic Trigger
#define APP_IFTTT_EVENT_PERIOIDC_TIMER_TIMEOUT   100 //in Sec; Valid only when Event Type is Periodic Timer
#define APP_IFTTT_EVENT                 "YOUR_EVENT" //Event
#define APP_IFTTT_KEY                   "YOUR_KEY" //Key
#define APP_IFTTT_NUM_OF_VALUES         1 //Number of values/ data that needs to be sent
#define APP_IFTTT_VALUE1                "Value1"
#define APP_IFTTT_VALUE2                "Value2"
#define APP_IFTTT_VALUE3                "Value3"
#define APP_IFTTT_USER_CALLBACK         NULL //APP_IFTTT_Callback - called before sending the trigger
#define APP_IFTTT_USER_CALLBACK_CTX     NULL

SYS_MODULE_OBJ g_tcpSrvcHandle = SYS_MODULE_OBJ_INVALID;
APP_DATA appData;
bool APP_IFTTT_Callback(void*);
static bool g_swPressed = false;
static uint32_t g_lastTriggerTimeout = 0;
static bool g_sendingTrigger = false;
#define APP_IFTTT_TRIGGER_TIMEOUT_CONST (APP_IFTTT_EVENT_PERIOIDC_TIMER_TIMEOUT * SYS_TMR_TickCounterFrequencyGet())

typedef bool(*userCallback)(void *);

typedef struct {
    // Event Type - Button Press or Periodic
    int eventType;

    //Ifttt server
    char server[32];

    //ifttt applet key
    char key[128];

    //ifttt applet event
    char event[32];

    //ifttt number of values/ data
    int numOfValues;

    //ifttt value1
    char value1[12];

    //ifttt value2
    char value2[12];

    //ifttt value3
    char value3[12];

    //ifttt trigger
    char trigger[256];

    //user callback
    userCallback userCb;

    //user callback context
    void *userCbCtx;
} iftttConfig;

iftttConfig g_sIftttCfg;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

void APP_PinEventHandler(GPIO_PIN pin, uintptr_t context)
{
    if (SWITCH1_STATE_PRESSED == SWITCH1_Get() && g_sendingTrigger == false) {
        g_swPressed = true;
    }
}

bool
APP_IFTTT_Callback(void *data)
{
    static int i = 0;

    /*
     ** User needs to populate this function as per his/ her requirements
     ** We are initializing the value1 with a running counter here for 
     ** demo purpose only
     */
    i++;

    memset(g_sIftttCfg.value1, 0, sizeof (g_sIftttCfg.value1));

    sprintf(g_sIftttCfg.value1, "%d", i);

    /* 
     ** Function should return true if it wants to send the trigger to 
     ** ifttt server else false based on the processing of the above data 
     */
    return true;
}

void
APP_IFTTT_SendTrigger(void)
{
    char networkBuffer[512];
    char finalTrigger[300];
    int numOfValues = APP_IFTTT_NUM_OF_VALUES;

    memset(finalTrigger, 0, sizeof (finalTrigger));
    memset(networkBuffer, 0, sizeof (networkBuffer));

    switch (numOfValues) {
    case 1:
    {
        sprintf(finalTrigger, "%s?value1=%s",
                g_sIftttCfg.trigger, g_sIftttCfg.value1);
    }
        break;
    case 2:
    {
        sprintf(finalTrigger, "%s?value1=%s&value2=%s",
                g_sIftttCfg.trigger, g_sIftttCfg.value1, g_sIftttCfg.value2);
    }
        break;
    case 3:
    {
        sprintf(finalTrigger, "%s?value1=%s&value2=%s&value3=%s",
                g_sIftttCfg.trigger, g_sIftttCfg.value1, g_sIftttCfg.value2,
                g_sIftttCfg.value3);
    }
        break;
    }

    sprintf(networkBuffer, "POST %s "
            "HTTP/1.1\r\n"
            "Host: %s" "\r\n"
            "Connection: close\r\n\r\n", finalTrigger, g_sIftttCfg.server);

    SYS_CONSOLE_PRINT("%s", networkBuffer);

    while (SYS_NET_SendMsg(g_tcpSrvcHandle, (uint8_t*) networkBuffer,
            strlen(networkBuffer)) == 0);

    g_sendingTrigger = false;

    LED_RED_Off();
}

void
APP_TcpClientCallback(uint32_t event, void *data, void* cookie)
{
    switch (event) {
    case SYS_NET_EVNT_CONNECTED:
    {
        SYS_CONSOLE_PRINT("\nAPP_TcpClientCallback(): Status UP\r\n");
        APP_IFTTT_SendTrigger();
        break;
    }

    case SYS_NET_EVNT_DISCONNECTED:
    {
        SYS_CONSOLE_PRINT("\nAPP_TcpClientCallback(): Status DOWN\r\n");
        break;
    }

    case SYS_NET_EVNT_RCVD_DATA:
    {
        char networkBuffer[512];
        memset(networkBuffer, 0, sizeof (networkBuffer));
        SYS_NET_RecvMsg(g_tcpSrvcHandle, (uint8_t*) networkBuffer,
                sizeof (networkBuffer));
        SYS_CONSOLE_PRINT("\nReceived: %s", networkBuffer);
        break;
    }

    case SYS_NET_EVNT_SSL_FAILED:
    {
        SYS_CONSOLE_PRINT("\nAPP_TcpClientCallback(): SSL Negotiation Failed\r\n");
        break;
    }

    case SYS_NET_EVNT_DNS_RESOLVE_FAILED:
    {
        SYS_CONSOLE_PRINT("\nAPP_TcpClientCallback(): DNS Resolution Failed\r\n");
        break;
    }

    case SYS_NET_EVNT_SOCK_OPEN_FAILED:
    {
        SYS_CONSOLE_PRINT("\nAPP_TcpClientCallback(): Socket Open Failed\r\n");
        break;
    }

    case SYS_NET_EVNT_LL_INTF_DOWN:
    {
        /* 
         ** User needs to take a decision if they want to close the socket or
         ** wait for the Lower layer to come up
         */
        SYS_CONSOLE_PRINT("\nAPP_TcpClientCallback(): Lower Layer Down\r\n");
        break;
    }

    case SYS_NET_EVNT_LL_INTF_UP:
    {
        /* 
         ** The lower layer was down and now it has come up again. 
         ** The socket was in connected state all this while
         */
        SYS_CONSOLE_PRINT("\nAPP_TcpClientCallback(): Lower Layer Up\r\n");
        break;
    }
    }
}

void APP_CheckTimeOut(uint32_t timeOutValue, uint32_t lastTimeOut)
{
    if (timeOutValue == 0) {
        return;
    }

    if (lastTimeOut == 0) {
        g_lastTriggerTimeout = SYS_TMR_TickCountGet();
        return;
    }

    if (SYS_TMR_TickCountGet() - lastTimeOut > timeOutValue) {
        if (g_sendingTrigger == false) {
            g_swPressed = true;
        }

        g_lastTriggerTimeout = SYS_TMR_TickCountGet();
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
    void APP_IFTTT_Initialize ( void )

  Remarks:
 */
void
APP_IFTTT_Initialize(void)
{
    memset(&g_sIftttCfg, 0, sizeof (g_sIftttCfg));

    g_sIftttCfg.eventType = APP_IFTTT_EVENT_TYPE;

    memcpy(g_sIftttCfg.server, g_sSysNetConfig0.host_name,
            sizeof (g_sSysNetConfig0.host_name));

    memcpy(g_sIftttCfg.key, APP_IFTTT_KEY, sizeof (APP_IFTTT_KEY));

    memcpy(g_sIftttCfg.event, APP_IFTTT_EVENT, sizeof (APP_IFTTT_EVENT));

    g_sIftttCfg.numOfValues = APP_IFTTT_NUM_OF_VALUES;

    g_sIftttCfg.userCb = APP_IFTTT_USER_CALLBACK;

    g_sIftttCfg.userCbCtx = APP_IFTTT_USER_CALLBACK_CTX;

    switch (g_sIftttCfg.numOfValues) {
    case 3:
    {
        memcpy(g_sIftttCfg.value3, APP_IFTTT_VALUE3, sizeof (APP_IFTTT_VALUE3));
    }

    case 2:
    {
        memcpy(g_sIftttCfg.value2, APP_IFTTT_VALUE2, sizeof (APP_IFTTT_VALUE2));
    }

    case 1:
    {
        memcpy(g_sIftttCfg.value1, APP_IFTTT_VALUE1, sizeof (APP_IFTTT_VALUE1));
    }
        break;
    }

    sprintf(g_sIftttCfg.trigger, "https://%s/trigger/%s/with/key/%s",
            g_sIftttCfg.server, g_sIftttCfg.event, g_sIftttCfg.key);

    if (g_sIftttCfg.eventType == APP_IFTTT_EVENT_BUTTON_PRESS) {
        GPIO_PinInterruptCallbackRegister(GPIO_PIN_RA10, APP_PinEventHandler,
                (uintptr_t) NULL);

        GPIO_PinInterruptEnable(GPIO_PIN_RA10);
    }
}

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */
void
APP_Initialize(void)
{
    SYS_CONSOLE_MESSAGE("APP_Initialize\n");
    APP_IFTTT_Initialize();
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void
APP_Tasks(void)
{
    /* Check the application's current state. */
    switch (appData.state) {
        /* Application's initial state. */
    case APP_STATE_INIT:
    {
        appData.state = APP_STATE_SERVICE_TASKS;
        break;
    }

    case APP_STATE_SERVICE_TASKS:
    {
        if (g_swPressed == true) {
            SYS_CONSOLE_PRINT("PIN event\r\n");

            g_swPressed = false;

            /* In case the user has defined a callback */
            if (g_sIftttCfg.userCb) {
                if (g_sIftttCfg.userCb(g_sIftttCfg.userCbCtx) == true) {
                    g_sendingTrigger = true;

                    g_tcpSrvcHandle = SYS_NET_Open(NULL,
                            APP_TcpClientCallback, 0);

                    LED_RED_On();
                }
            } else {
                g_sendingTrigger = true;

                g_tcpSrvcHandle = SYS_NET_Open(NULL,
                        APP_TcpClientCallback, 0);

                LED_RED_On();
            }
        }
        break;
    }


        /* The default state should never be executed. */
    default:
    {
        /* TODO: Handle error in application's state machine. */
        break;
    }
    }

    /* Check the application's current state. */
    SYS_CMD_READY_TO_READ();

    SYS_NET_Task(g_tcpSrvcHandle);

    if (g_sIftttCfg.eventType == APP_IFTTT_EVENT_PERIODIC_TIMER) {
        APP_CheckTimeOut(APP_IFTTT_TRIGGER_TIMEOUT_CONST, g_lastTriggerTimeout);
    }
}

/*******************************************************************************
 End of File
 */
