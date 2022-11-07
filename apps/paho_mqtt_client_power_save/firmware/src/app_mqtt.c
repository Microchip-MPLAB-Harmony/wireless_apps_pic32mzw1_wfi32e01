/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony MQTT application.

  Description:
    This file contains the source code for the MPLAB Harmony MQTT application.
    It implements the logic of the application's state machine and it may call
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
#include "system/mqtt/sys_mqtt.h"
#include "wdrv_pic32mzw_ps.h"
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

DRV_HANDLE g_sSyswifiSrvcDrvHdl = DRV_HANDLE_INVALID ;
SYS_MODULE_OBJ g_sSysMqttHandle = SYS_MODULE_OBJ_INVALID;
SYS_MQTT_Config g_sTmpSysMqttCfg;
volatile unsigned int g_sPllconValue = 0;
volatile bool g_mQttPubStatus = false, g_wIfisleepStatus = false;
void WiFiServCallback(uint32_t event, void * data, void *cookie);
static void APP_Configure_BUCKPSMMode(void);
static void APP_Configure_BUCKPWMMode(void);
static void  APP_Configure_LPMode_Entry(void);
static void APP_Configure_LPMode_Exit(void);
// *****************************************************************************
/*
 * Note: Note: Please do not overwrite the file during code re-generation 
 * via MHC */
// *****************************************************************************

/*RTCC as a wakeup source*/
#define RTCC 

/*
 * Supported PIC/MCU Sleep Modes: 
*/
static POWER_LOW_POWER_MODE g_mcuSleepMode = SYS_WIFI_PIC_MODE;  

/*
 * Supported Wi-Fi sleep Modes
 */
static uint8_t g_wiFiSleepMode = SYS_WIFI_WIFI_POWERSAVE_MODE;

#ifdef RTCC
struct tm sys_time;
struct tm alarm_time;
volatile bool rtcc_alarm = false;

//#define APP_CFG_WITH_MQTT_API
void RTCC_Callback( uintptr_t context)
{

    rtcc_alarm = true;    
}
#endif
// *****************************************************************************
// *****************************************************************************
// Section: Local data
// *****************************************************************************
// *****************************************************************************


int32_t APP_MQTT_PublishMsg(char *message) {
    SYS_MQTT_PublishTopicCfg sMqttTopicCfg;
    int32_t retVal = SYS_MQTT_FAILURE;

    strcpy(sMqttTopicCfg.topicName, SYS_MQTT_DEF_PUB_TOPIC_NAME);
    sMqttTopicCfg.topicLength = strlen(SYS_MQTT_DEF_PUB_TOPIC_NAME);
    sMqttTopicCfg.retain = SYS_MQTT_DEF_PUB_RETAIN;
    sMqttTopicCfg.qos = SYS_MQTT_DEF_PUB_QOS;

    retVal = SYS_MQTT_Publish(g_sSysMqttHandle,
            &sMqttTopicCfg,
            message,
            strlen(message));
    if (retVal != SYS_MQTT_SUCCESS) {
        SYS_CONSOLE_PRINT("\nPublish_PeriodicMsg(): Failed (%d)\r\n", retVal);
    }
    return retVal;
}

static void APP_INT_StatusClear()
{
 
    SYS_INT_SourceStatusClear(INT_SOURCE_TIMER_1);
    SYS_INT_SourceStatusClear(INT_SOURCE_TIMER_3);

    SYS_INT_SourceStatusClear(INT_SOURCE_UART1_FAULT);
    SYS_INT_SourceStatusClear(INT_SOURCE_UART1_TX);
    SYS_INT_SourceStatusClear(INT_SOURCE_UART1_RX);

    SYS_INT_SourceStatusClear(INT_SOURCE_UART2_FAULT);
    SYS_INT_SourceStatusClear(INT_SOURCE_UART2_TX);
    SYS_INT_SourceStatusClear(INT_SOURCE_UART2_RX);

    SYS_INT_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    SYS_INT_SourceStatusClear(INT_SOURCE_I2C1_BUS);

    SYS_INT_SourceStatusClear(INT_SOURCE_RFMAC);
    SYS_INT_SourceStatusClear(INT_SOURCE_RFSMC);
    SYS_INT_SourceStatusClear(INT_SOURCE_RFTM0);

    SYS_INT_SourceStatusClear(INT_SOURCE_CRYPTO1);
    SYS_INT_SourceStatusClear(INT_SOURCE_CRYPTO1_FAULT);

    SYS_INT_SourceStatusClear(INT_SOURCE_FLASH_CONTROL);

#ifndef RTCC    
    SYS_INT_SourceStatusClear(INT_SOURCE_RTCC);
#endif
    
    //SYS_INT_SourceStatusClear(INT_SOURCE_EXTERNAL_0);
    //SYS_INT_SourceStatusClear(INT_SOURCE_CHANGE_NOTICE_A);
}

static void APP_INT_SourceDisable()
{
#ifndef TIMER_WAKEUP           
    SYS_INT_SourceDisable(INT_SOURCE_TIMER_1);
#endif
    SYS_INT_SourceDisable(INT_SOURCE_TIMER_3);

    SYS_INT_SourceDisable(INT_SOURCE_UART1_FAULT);
    SYS_INT_SourceDisable(INT_SOURCE_UART1_TX);
    SYS_INT_SourceDisable(INT_SOURCE_UART1_RX);

    SYS_INT_SourceDisable(INT_SOURCE_UART2_FAULT);
    SYS_INT_SourceDisable(INT_SOURCE_UART2_TX);
    SYS_INT_SourceDisable(INT_SOURCE_UART2_RX);

    SYS_INT_SourceDisable(INT_SOURCE_I2C1_MASTER);
    SYS_INT_SourceDisable(INT_SOURCE_I2C1_BUS);

    SYS_INT_SourceDisable(INT_SOURCE_RFMAC);
    SYS_INT_SourceDisable(INT_SOURCE_RFSMC);
    SYS_INT_SourceDisable(INT_SOURCE_RFTM0);

    SYS_INT_SourceDisable(INT_SOURCE_CRYPTO1);
    SYS_INT_SourceDisable(INT_SOURCE_CRYPTO1_FAULT);

    SYS_INT_SourceDisable(INT_SOURCE_FLASH_CONTROL);
#ifndef RTCC    
    SYS_INT_SourceDisable(INT_SOURCE_RTCC);
#endif    
    //SYS_INT_SourceDisable(INT_SOURCE_EXTERNAL_0);
    //SYS_INT_SourceDisable(INT_SOURCE_CHANGE_NOTICE_A);
    
}

static void APP_INT_SourceRestore()
{

    SYS_INT_SourceRestore(INT_SOURCE_TIMER_1, true);
    SYS_INT_SourceRestore(INT_SOURCE_TIMER_3, true);

    SYS_INT_SourceRestore(INT_SOURCE_UART1_FAULT, true);
    SYS_INT_SourceRestore(INT_SOURCE_UART1_TX, true);
    SYS_INT_SourceRestore(INT_SOURCE_UART1_RX, true);

    SYS_INT_SourceRestore(INT_SOURCE_UART2_FAULT, true);
    SYS_INT_SourceRestore(INT_SOURCE_UART2_TX, true);
    SYS_INT_SourceRestore(INT_SOURCE_UART2_RX, true);

    SYS_INT_SourceRestore(INT_SOURCE_I2C1_MASTER, true);
    SYS_INT_SourceRestore(INT_SOURCE_I2C1_BUS, true);

    SYS_INT_SourceRestore(INT_SOURCE_RFMAC, true);
    SYS_INT_SourceRestore(INT_SOURCE_RFSMC, true);
    SYS_INT_SourceRestore(INT_SOURCE_RFTM0, true);

    
    SYS_INT_SourceRestore(INT_SOURCE_CRYPTO1, true);
    SYS_INT_SourceRestore(INT_SOURCE_CRYPTO1_FAULT, true);

    SYS_INT_SourceRestore(INT_SOURCE_FLASH_CONTROL, true);
#ifndef RTCC    
    SYS_INT_SourceRestore(INT_SOURCE_RTCC, true);
#endif    

    //SYS_INT_SourceRestore(INT_SOURCE_EXTERNAL_0, true);
    //SYS_INT_SourceRestore(INT_SOURCE_CHANGE_NOTICE_A, true);
}

static void APP_SetMCUSleepMode()
{
#ifdef RTCC            
    if (RTCC_AlarmSet(&alarm_time, RTCC_ALARM_MASK_SS/*RTCC_ALARM_MASK_HHMISS*/) == false)
    {
        /* Error setting up alarm */
        while(1);
    }
#endif
    APP_INT_StatusClear();
    APP_INT_SourceDisable();
    RNGCONbits.TRNGEN = 0;
    RNGCONbits.PRNGEN = 0;

    APP_Configure_BUCKPSMMode();
    APP_Configure_LPMode_Entry();
    POWER_LowPowerModeEnter(g_mcuSleepMode);


    APP_Configure_BUCKPWMMode();
    RNGCONbits.TRNGEN = 1;
    RNGCONbits.PRNGEN = 1;

    APP_Configure_LPMode_Exit();
    UART3_Initialize();
    UART1_Initialize();
    APP_INT_SourceRestore();
}

static void APP_DelayMs ( uint32_t delay_ms)
{
    uint32_t startCount, endCount;
    /* Calculate the end count for the given delay */
    endCount=(CORE_TIMER_FREQ/1000)*delay_ms;
    startCount=_CP0_GET_COUNT();
    while((_CP0_GET_COUNT()-startCount)<endCount);
}

static void APP_Configure_BUCKPSMMode(void)
{
    unsigned int O_BUCKEN, O_MLDOEN, O_BUCKMODE;
    unsigned int vreg1 = 0x0;
    unsigned int vreg2 = 0x0;
    unsigned int vreg3 = 0x0;
    unsigned int vreg4 = 0x0;

    PMUOVERCTRLbits.PHWC = 1;
    O_BUCKEN = 1;
    O_MLDOEN = 0;
    O_BUCKMODE = 0;// PMU-Buck PSM Mode
    PMUMODECTRL2 = ((O_BUCKEN << 31) | (O_MLDOEN << 30) | (O_BUCKMODE << 29) |
           (vreg1 << 24) | (vreg2 <<16) | (vreg3 << 8) | vreg4);
    PMUOVERCTRLbits.OVEREN = 0;
    PMUOVERCTRLbits.PHWC = 0;
    PMUCLKCTRLbits.BUCKSRC = 2;
    PMUCLKCTRLbits.BUCKCLKDIV = 0x08;

}

static void APP_Configure_BUCKPWMMode(void)
{
    unsigned int O_BUCKEN, O_MLDOEN, O_BUCKMODE, OVEREN;
    unsigned int vreg1 = 0x0;
    unsigned int vreg2 = 0x0;
    unsigned int vreg3 = 0x0;
    unsigned int vreg4 = 0x0;
    unsigned int *otp_treg3_data = (unsigned int *)0xBFC56FFC;
    unsigned int otp_treg_val;

    #define VREG1_BITS 0x0000001F
    #define VREG2_BITS 0x00001F00
    #define VREG3_BITS 0x001F0000
    #define VREG4_BITS 0x1F000000
    #define TREG_DEFAULT 0x16161616

    PMUOVERCTRLbits.PHWC = 1;
   
    otp_treg_val = *otp_treg3_data;
    if((otp_treg_val == 0xFFFFFFFF) || (otp_treg_val == 0x00000000))
    {
        otp_treg_val = TREG_DEFAULT;
    }
    vreg4 = otp_treg_val & VREG1_BITS;
    vreg3 = (otp_treg_val & VREG2_BITS) >> 8;
    vreg2 = (otp_treg_val & VREG3_BITS) >> 16;
    vreg1 = (otp_treg_val & VREG4_BITS) >> 24;
    
    O_BUCKEN = 1;
    O_MLDOEN = 0;
    O_BUCKMODE = 1;
    OVEREN = 0; // PMU-Buck PWM Mode
    PMUOVERCTRL = ((O_BUCKEN << 31) | (O_MLDOEN << 30) | (O_BUCKMODE << 29) |
            (OVEREN << 23) | (vreg1 << 24) | (vreg2 <<16) | (vreg3 << 8) | vreg4);

    PMUOVERCTRLbits.PHWC = 0;
    if ((PMUCMODEbits.CBUCKMODE) );
       SYS_CONSOLE_PRINT(" Switch to BUCK PWM complete \n");
}

static void  APP_Configure_LPMode_Entry(void)
{
    volatile unsigned int *PLLDBG = (unsigned int*) 0xBF8000E0;

    /* unlock system for clock configuration */
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;

    /* switch SPLL to FRC */
    g_sPllconValue = SPLLCON;
    SPLLCON = 0x40496861;
    CFGCON0bits.SPLLHWMD = 1;
    OSCCON = 0x103;
    while (((OSCCON) & 0x1));
    APP_DelayMs(5);

    /*Disable ethernet clk */
    CFGCON0bits.ETHPLLHWMD = 1;
    EWPLLCONbits.ETHCLKOUTEN = 0;
    while(!((*PLLDBG) & 0x4));
    APP_DelayMs(3);
}

static void APP_Configure_LPMode_Exit(void)
{
    volatile unsigned int *PLLDBG = (unsigned int*) 0xBF8000E0;

    /* unlock system for clock configuration */
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;

    /* switch SPLL back to POSC */
    SPLLCON = g_sPllconValue;
    CFGCON0bits.SPLLHWMD = 1;
    OSCCON = 0x103;
    while (((OSCCON) & 0x1));
    APP_DelayMs(5);

    /* Enable ethernet clk */
    CFGCON0bits.ETHPLLHWMD = 1;
    EWPLLCONbits.ETHCLKOUTEN = 1;
    while(!((*PLLDBG) & 0x4));
    APP_DelayMs(3);
}
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/******************************************************************************
  Function:
    int32_t MqttCallback(SYS_MQTT_EVENT_TYPE eEventType, void *data, uint16_t len, void* cookie)

  Remarks:
    Callback function registered with the SYS_MQTT_Connect() API. For more details 
    check https://microchip-mplab-harmony.github.io/wireless/system/mqtt/docs/interface.html
 */
int32_t MqttCallback(SYS_MQTT_EVENT_TYPE eEventType, void *data, uint16_t len, void* cookie) {
    switch (eEventType) {
        case SYS_MQTT_EVENT_MSG_RCVD:
        {
            /* Message received on Subscribed Topic */
            SYS_MQTT_PublishConfig	*psMsg = (SYS_MQTT_PublishConfig	*)data;
            psMsg->message[psMsg->messageLength] = 0;
            psMsg->topicName[psMsg->topicLength] = 0;
            SYS_CONSOLE_PRINT("\nMqttCallback(): Msg received on Topic: %s ; Msg: %s\r\n", 
                psMsg->topicName, psMsg->message);
        }
        break;

        case SYS_MQTT_EVENT_MSG_DISCONNECTED:
        {
                
            SYS_CONSOLE_PRINT("\n MqttCallback(): SYS_MQTT_EVENT_MSG_DISCONNECTED\r\n");
            if(g_wiFiSleepMode == APP_WIFI_WOFF_MODE)
            {
                
                static const WDRV_PIC32MZW_SYS_INIT wdrvPIC32MZW1InitData = {
                         .pCryptRngCtx  = NULL,
                         .pRegDomName   = "GEN",
                         .powerSaveMode = WDRV_PIC32MZW_POWERSAVE_RUN_MODE,
                         .powerSavePICCorrelation = WDRV_PIC32MZW_POWERSAVE_PIC_ASYNC_MODE
                     };
                     PMUCLKCTRLbits.WLDOOFF = 0;
                     sysObj.syswifi = SYS_WIFI_Initialize(NULL,NULL,NULL);
                     sysObj.drvWifiPIC32MZW1 = WDRV_PIC32MZW_Initialize(WDRV_PIC32MZW_SYS_IDX_0, (SYS_MODULE_INIT*)&wdrvPIC32MZW1InitData);
                     SYS_WIFI_CtrlMsg(sysObj.syswifi, SYS_WIFI_REGCALLBACK, WiFiServCallback, sizeof (uint8_t *));

            }

        }
        break;

        case SYS_MQTT_EVENT_MSG_CONNECTED:
        {
            SYS_CONSOLE_PRINT("\nMqttCallback(): Connected\r\n");
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
            /* MQTT Topic Unsubscribed; Now the Client will not receive any messages for this Topic */
        }
        break;

        case SYS_MQTT_EVENT_MSG_PUBLISHED:
        {
            SYS_CONSOLE_PRINT("SYS_MQTT_EVENT_MSG_PUBLISHED g_wiFiSleepMode=%d\r\n",g_wiFiSleepMode);
            g_mQttPubStatus = true;
#if 00
            if(g_wiFiSleepMode == APP_WIFI_WOFF_MODE)
            {
                SYS_CONSOLE_PRINT("APP_WIFI_WOFF_MODE mode \r\n");
                PMUCLKCTRLbits.WLDOOFF = 1;
                APP_SetMCUSleepMode();
                SYS_WIFI_Deinitialize(sysObj.syswifi);
                WDRV_PIC32MZW_Deinitialize(sysObj.drvWifiPIC32MZW1);
            }
            else
            {
                SYS_CONSOLE_PRINT("normal mode \r\n");
                APP_SetMCUSleepMode();
            }
#endif
        }
        break;

        case SYS_MQTT_EVENT_MSG_CONNACK_TO:
        {
            /* MQTT Client ConnAck TimeOut; User will need to reconnect again */
        }
        break;

        case SYS_MQTT_EVENT_MSG_SUBACK_TO:
        {
            /* MQTT Client SubAck TimeOut; User will need to subscribe again */
        }
        break;

        case SYS_MQTT_EVENT_MSG_PUBACK_TO:
        {
            /* MQTT Client PubAck TimeOut; User will need to publish again */
        }
        break;

        case SYS_MQTT_EVENT_MSG_UNSUBACK_TO:
        {
            /* MQTT Client UnSubAck TimeOut; User will need to Unsubscribe again */
        }
        break;

    }
    SYS_CONSOLE_PRINT("\nMqttCallback(): event '%d'\r\n", eEventType);
    return SYS_MQTT_SUCCESS;
}


void WiFiPSCallback(DRV_HANDLE handle,WDRV_PIC32MZW_POWERSAVE_MODE psMode,bool bSleepEntry,uint32_t u32SleepDurationMs)
{
    if(psMode == WDRV_PIC32MZW_POWERSAVE_WSM_MODE)
    {
        if(bSleepEntry == true)
        {
            g_wIfisleepStatus=true;
        }

    }
    else if(psMode == WDRV_PIC32MZW_POWERSAVE_WDS_MODE)
    {
        if(bSleepEntry == true)
        {
            g_wIfisleepStatus=true;
        }
    }
}

void WiFiServCallback(uint32_t event, void * data, void *cookie) 
{
    switch (event) 
    {
        case SYS_WIFI_CONNECT:
        {

            SYS_CONSOLE_PRINT("\n MCU Sleep mode=%d (Idel=0,sleep=1, dream=2, DS=3, XDS=4) Wi-Fi sleep mode = %d (RUN=0, WSM=1, WDS=2, WOFF=3)\r\n",g_mcuSleepMode,g_wiFiSleepMode);
            //SYS_CONSOLE_PRINT("\n SOSCEN=%x POSCMOD=%x TRNGEN=%x PRNGEN=%x\r\n",CFGCON4bits.SOSCEN, CFGCON2bits.POSCMOD,RNGCONbits.TRNGEN,RNGCONbits.PRNGEN);

            SYS_WIFI_CtrlMsg(sysObj.syswifi,SYS_WIFI_GETDRVHANDLE,&g_sSyswifiSrvcDrvHdl,sizeof (DRV_HANDLE));
#ifdef SYS_WIFI_WAKEUP_DTIM
            WDRV_PIC32MZW_PowerSaveBroadcastTrackingSet(g_sSyswifiSrvcDrvHdl,true);
#else
            WDRV_PIC32MZW_PowerSaveBroadcastTrackingSet(g_sSyswifiSrvcDrvHdl,false);
#endif
            /*Wi-Fi WOFF mode is handled by Application code*/
            if(g_wiFiSleepMode == APP_WIFI_WOFF_MODE)
            {
                WDRV_PIC32MZW_PowerSaveModeSet(g_sSyswifiSrvcDrvHdl,WDRV_PIC32MZW_POWERSAVE_WSM_MODE,SYS_PIC_WIFI_CORRELATION_MODE,WiFiPSCallback);
            }
            else
            {
                WDRV_PIC32MZW_PowerSaveModeSet(g_sSyswifiSrvcDrvHdl,g_wiFiSleepMode,SYS_PIC_WIFI_CORRELATION_MODE,WiFiPSCallback);
            }
            
            break;
            
        }
        default:
        {
            break;
        }
    }
}

/*******************************************************************************
  Function:
    void APP_MQTT_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */
void APP_MQTT_Initialize(void) {

    RCON_RESET_CAUSE resetCause;
    resetCause = RCON_ResetCauseGet();
    POWER_DS_ReleaseGPIO();

    /* Check if RESET was after deep sleep wakeup */
    if (((resetCause & RCON_RESET_CAUSE_DPSLP) == RCON_RESET_CAUSE_DPSLP))
    {
        RCON_ResetCauseClear(RCON_RESET_CAUSE_DPSLP);
    }

    if (POWER_DS_WakeupSourceGet() == POWER_DS_WAKEUP_SOURCE_DSRTC)
    {
        SYS_CONSOLE_PRINT("\r\n\r\nDevice woke up after XDS/DS mode Using RTCC\r\n");
    }
    else if (POWER_DS_WakeupSourceGet() == POWER_DS_WAKEUP_SOURCE_DSINT0)
    {
        SYS_CONSOLE_PRINT("\r\n\r\nDevice woke up after XDS/DS mode Using EXT INT0(SW1 button press)\r\n");
    }
     
    /* INT0 interrupt is used to wake up from Deep Sleep */
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_0);
    
    /* CN interrupt is used to wake up from Idle or Sleep mode */
    GPIO_PinInterruptEnable(CN_INT_PIN);

#ifdef RTCC   
    // Time setting 31-12-2018 23:59:55 Monday
    sys_time.tm_hour = 23;
    sys_time.tm_min = 59;
    sys_time.tm_sec = 55;

    sys_time.tm_year = 18;
    sys_time.tm_mon = 12;
    sys_time.tm_mday = 31;
    sys_time.tm_wday = 1;

    // Alarm setting 01-01-2019 00:00:05 Tuesday
    alarm_time.tm_hour = 00;
    alarm_time.tm_min = 00;
    alarm_time.tm_sec = 55;

    alarm_time.tm_year = 19;
    alarm_time.tm_mon = 01;
    alarm_time.tm_mday = 01;
    alarm_time.tm_wday = 2;

    RTCC_CallbackRegister(RTCC_Callback, (uintptr_t) NULL);
            
    if (RTCC_TimeSet(&sys_time) == false)
    {
        /* Error setting up time */
        while(1);
    }

    if (RTCC_AlarmSet(&alarm_time, RTCC_ALARM_MASK_HHMISS) == false)
    {
        /* Error setting up alarm */
        while(1);
    }
#endif
    /*
    ** For more details check https://microchip-mplab-harmony.github.io/wireless/system/mqtt/docs/interface.html
    */
#ifdef APP_CFG_WITH_MQTT_API

    /* In case the user does not want to use the configuration given in the MHC */

    SYS_MQTT_Config *psMqttCfg;

    memset(&g_sTmpSysMqttCfg, 0, sizeof (g_sTmpSysMqttCfg));
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
    g_sSysMqttHandle = SYS_MQTT_Connect(NULL, /* NULL value means that the MHC configuration should be used for this connection */
                                        MqttCallback, 
                                        NULL);
#endif
    SYS_WIFI_CtrlMsg(sysObj.syswifi, SYS_WIFI_REGCALLBACK, WiFiServCallback, sizeof (uint8_t *));
}

/******************************************************************************
  Function:
    void APP_MQTT_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */
void APP_MQTT_Tasks(void) 
{
    SYS_MQTT_Task(g_sSysMqttHandle);
    if(g_mQttPubStatus && g_wIfisleepStatus)
    {
        g_mQttPubStatus =false;
        g_wIfisleepStatus=false;
        if(g_wiFiSleepMode == APP_WIFI_WOFF_MODE)
        {
            PMUCLKCTRLbits.WLDOOFF = 1;
            APP_SetMCUSleepMode();
            SYS_WIFI_Deinitialize(sysObj.syswifi);
            WDRV_PIC32MZW_Deinitialize(sysObj.drvWifiPIC32MZW1);
        }
        else
        {
            APP_SetMCUSleepMode();
        }
    }

}

/******************************************************************************
  Function:
    int32_t APP_MQTT_GetStatus ( void *)

  Remarks:
    See prototype in app.h.
 */
int32_t APP_MQTT_GetStatus(void *p) 
{
    return SYS_MQTT_GetStatus(g_sSysMqttHandle);
}


/*******************************************************************************
 End of File
 */
