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

#include "app.h"
#include "definitions.h"
#include "bsp/bsp.h"

APP_DATA appData;
static SYS_WIFI_CONFIG wificonfig;

void WiFiServCallback(uint32_t event, void * data, void *cookie) {
    IPV4_ADDR *IPAddr;
    switch (event) {
        case SYS_WIFI_CONNECT:
            IPAddr = (IPV4_ADDR *) data;
            if (SYS_WIFI_STA == wificonfig.mode) {
                SYS_CONSOLE_PRINT("Connected to AP. Got IP address = %d.%d.%d.%d \r\n", IPAddr->v[0], IPAddr->v[1], IPAddr->v[2], IPAddr->v[3]);
            //} else if (SYS_WIFI_AP == wificonfig.mode) {
            //    SYS_CONSOLE_PRINT("Client connected to My AP. Giving IP address = %d.%d.%d.%d \r\n", IPAddr->v[0], IPAddr->v[1], IPAddr->v[2], IPAddr->v[3]);
            }
            LED_GREEN_On();
            LED_RED_Off();
            break;
        case SYS_WIFI_DISCONNECT:
            SYS_CONSOLE_PRINT("Device DISCONNECTED \r\n");
            LED_RED_On();
            LED_GREEN_Off();
            break;
        case SYS_WIFI_PROVCONFIG:
            memcpy(&wificonfig,data,sizeof(SYS_WIFI_CONFIG));
            SYS_CONSOLE_PRINT("%s:%d Received Provisioning Data : \r\n Device mode=%s \r\n", __func__, __LINE__, (wificonfig.mode == SYS_WIFI_STA) ? "STA" : "AP");
            if (SYS_WIFI_STA == wificonfig.mode) {
                SYS_CONSOLE_PRINT(" ssid=%s password=%s \r\n",wificonfig.staConfig.ssid,wificonfig.staConfig.psk);
           // } else if (SYS_WIFI_AP == wificonfig.mode) {
           //     SYS_CONSOLE_PRINT(" ssid=%s password=%s \r\n",wificonfig.apConfig.ssid,wificonfig.apConfig.psk);
            }            
            break;
    }
}

void APP_Initialize(void) {
    appData.state = APP_STATE_INIT;
}

void APP_Tasks(void) {

    switch (appData.state) {
        case APP_STATE_INIT:
        {
            SYS_CONSOLE_PRINT("Application: wifi_easy_config \r\n");
            SYS_WIFI_CtrlMsg(sysObj.syswifi, SYS_WIFI_REGCALLBACK, WiFiServCallback, sizeof (uint8_t *));
            LED_RED_On();
            LED_GREEN_Off();
            appData.state = APP_STATE_GETCONFIG;
            break;
        }
        case APP_STATE_GETCONFIG:
        {
            if(SYS_WIFI_SUCCESS == SYS_WIFI_CtrlMsg(sysObj.syswifi, SYS_WIFI_GETCONFIG, &wificonfig, sizeof(SYS_WIFI_CONFIG)))
            {
                SYS_CONSOLE_PRINT("%s:%d Device mode=%s \r\n", __func__, __LINE__, (wificonfig.mode == SYS_WIFI_STA) ? "STA" : "AP");
                appData.state = APP_STATE_SERVICE_TASKS;
            }            
            break;
        }
        case APP_STATE_SERVICE_TASKS:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}
