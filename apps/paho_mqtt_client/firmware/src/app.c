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
#include "app.h"
#include "system/mqtt/sys_mqtt.h"
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
volatile APP_DATA            g_appData;

SYS_MODULE_OBJ      g_sSysMqttHandle = SYS_MODULE_OBJ_INVALID;
SYS_MQTT_Config     g_sTmpSysMqttCfg;
static uint32_t     g_lastPubTimeout = 0;
static uint32_t     PubMsgCnt = 0;

#define MQTT_PERIOIDC_PUB_TIMEOUT   30 //Sec
#define MQTT_PUB_TIMEOUT_CONST (MQTT_PERIOIDC_PUB_TIMEOUT * SYS_TMR_TickCounterFrequencyGet())

//#define APP_CFG_WITH_MQTT_API

// *****************************************************************************
// *****************************************************************************
// Section: Local data
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************
int32_t MqttCallback(SYS_MQTT_EVENT_TYPE eEventType, void *data, uint16_t len, void* cookie)
{
    switch(eEventType)
    {
        case SYS_MQTT_EVENT_MSG_RCVD:
        {   
			SYS_MQTT_PublishConfig	*psMsg = (SYS_MQTT_PublishConfig	*)data;
            psMsg->message[psMsg->messageLength] = 0;
            psMsg->topicName[psMsg->topicLength] = 0;
            SYS_CONSOLE_PRINT("\nMqttCallback(): Msg received on Topic: %s ; Msg: %s\r\n", 
				psMsg->topicName, psMsg->message);
        }
        break;
        
        case SYS_MQTT_EVENT_MSG_DISCONNECTED:
        {
            SYS_CONSOLE_PRINT("\nMqttCallback(): MQTT Disconnected\r\n");    
        }
        break;

        case SYS_MQTT_EVENT_MSG_CONNECTED:
        {
            SYS_CONSOLE_PRINT("\nMqttCallback(): MQTT Connected\r\n");
            g_lastPubTimeout = SYS_TMR_TickCountGet();
        }
        break;

        case SYS_MQTT_EVENT_MSG_SUBSCRIBED:
        {
            SYS_MQTT_SubscribeConfig	*psMqttSubCfg = (SYS_MQTT_SubscribeConfig	*)data;
            SYS_CONSOLE_PRINT("\nMqttCallback(): Subscribed to Topic '%s'\r\n", psMqttSubCfg->topicName);
        }
        break;

        case SYS_MQTT_EVENT_MSG_UNSUBSCRIBED:
        {
            SYS_MQTT_SubscribeConfig	*psMqttSubCfg = (SYS_MQTT_SubscribeConfig	*)data;
            SYS_CONSOLE_PRINT("\nMqttCallback(): UnSubscribed to Topic '%s'\r\n", psMqttSubCfg->topicName);                
        }
        break;

		case SYS_MQTT_EVENT_MSG_PUBLISHED:
		{
            SYS_CONSOLE_PRINT("\nMqttCallback(): Published Msg(%d) to Topic\r\n", PubMsgCnt);                
            g_lastPubTimeout = SYS_TMR_TickCountGet();
		}
		break;
        
        case SYS_MQTT_EVENT_MSG_CONNACK_TO:
		{
            SYS_CONSOLE_PRINT("\nMqttCallback(): Connack Timeout Happened\r\n");                
		}
		break;

        case SYS_MQTT_EVENT_MSG_SUBACK_TO: 
		{
            SYS_CONSOLE_PRINT("\nMqttCallback(): Suback Timeout Happened\r\n");                
		}
		break;

        case SYS_MQTT_EVENT_MSG_PUBACK_TO:
		{
            SYS_CONSOLE_PRINT("\nMqttCallback(): Puback Timeout Happened\r\n");                
            g_lastPubTimeout = SYS_TMR_TickCountGet();
		}
		break;

        case SYS_MQTT_EVENT_MSG_UNSUBACK_TO: 
		{
            SYS_CONSOLE_PRINT("\nMqttCallback(): UnSuback Timeout Happened\r\n");                
		}
		break;

    }
    return SYS_MQTT_SUCCESS;
}

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */
void APP_Initialize ( void )
{
#ifdef APP_CFG_WITH_MQTT_API
            SYS_MQTT_Config   *psMqttCfg;

            memset(&g_sTmpSysMqttCfg, 0, sizeof(g_sTmpSysMqttCfg));
            psMqttCfg = &g_sTmpSysMqttCfg;
            psMqttCfg->sBrokerConfig.autoConnect = false;
            psMqttCfg->sBrokerConfig.tlsEnabled = false;
            strcpy(psMqttCfg->sBrokerConfig.brokerName, "test.mosquitto.org");
            psMqttCfg->sBrokerConfig.serverPort = 1883;
            psMqttCfg->subscribeCount = 1;
            psMqttCfg->sSubscribeConfig[0].qos = 1;
            strcpy(psMqttCfg->sSubscribeConfig[0].topicName, "MCHP/sample/b");
            g_sSysMqttHandle = SYS_MQTT_Connect(&g_sTmpSysMqttCfg, MqttCallback, NULL);    
#else    
            g_sSysMqttHandle = SYS_MQTT_Connect(NULL, MqttCallback, NULL); 
#endif    
}

bool checkTimeOut(uint32_t timeOutValue, uint32_t lastTimeOut)
{
    if(lastTimeOut == 0)
        return 0;
    
    return (SYS_TMR_TickCountGet() - lastTimeOut > timeOutValue);
}

void Publish_PeriodicMsg(void)
{
    if (checkTimeOut(MQTT_PUB_TIMEOUT_CONST, g_lastPubTimeout))
    {
        char        message[32] = {0};
        SYS_MQTT_PublishTopicCfg	sMqttTopicCfg;
        int32_t retVal = SYS_MQTT_FAILURE;

        //reset the timer
        g_lastPubTimeout = 0;
        
        /* All Params other than the message are initialized by the config provided in MHC*/
        strcpy(sMqttTopicCfg.topicName, SYS_MQTT_DEF_PUB_TOPIC_NAME);
        sMqttTopicCfg.topicLength = strlen(SYS_MQTT_DEF_PUB_TOPIC_NAME);
        sMqttTopicCfg.retain = SYS_MQTT_DEF_PUB_RETAIN;
        sMqttTopicCfg.qos = SYS_MQTT_DEF_PUB_QOS;

        sprintf(message, "message_%d\r\n", PubMsgCnt);

        retVal = SYS_MQTT_Publish(g_sSysMqttHandle, 
                            &sMqttTopicCfg,
                            message,
                            sizeof(message));
        if(retVal != SYS_MQTT_SUCCESS)
        {
            SYS_CONSOLE_PRINT("\nPublish_PeriodicMsg(): Failed (%d)\r\n", retVal);                            
        }
        PubMsgCnt++;
    }
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */
void APP_Tasks ( void )
{
    Publish_PeriodicMsg();
    SYS_MQTT_Task(g_sSysMqttHandle);

    /* Check the application's current state. */
    SYS_CMD_READY_TO_READ();
}


/*******************************************************************************
 End of File
 */
