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
static uint32_t     g_lastPubTimeout = 0;
#define MQTT_PERIOIDC_PUB_TIMEOUT   (60*5) //Sec ; if the value is 0, Periodic Publish will disable
#define MQTT_PUB_TIMEOUT_CONST (MQTT_PERIOIDC_PUB_TIMEOUT * SYS_TMR_TickCounterFrequencyGet())
#endif


/* MCP9808 registers */
#define MCP9808_I2C_ADDRESS         0x18 
#define MCP9808_REG_CONFIG          0x01
#define MCP9808_REG_TAMBIENT		0x05
#define MCP9808_REG_MANUF_ID		0x06
#define MCP9808_REG_DEVICE_ID		0x07
#define MCP9808_REG_RESOLUTION		0x08

/* MCP9808 other settings */
#define MCP9808_CONFIG_DEFAULT		0x00

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
#define ADC_VREF                (3.3f)
#define ADC_MAX_COUNT           (4095)
#define ADC_AVG_COUNT           (10)
int16_t temperature;
uint16_t deviceID;
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

void APP_CheckTimeOut(uint32_t timeOutValue, uint32_t lastTimeOut)
{
    if(timeOutValue == 0)
	{
        return ;
	}
    
	if(SYS_MQTT_STATUS_MQTT_CONNECTED != APP_MQTT_GetStatus(NULL))
	{
		return;
	}
	
	if(lastTimeOut == 0)
	{
		g_lastPubTimeout = SYS_TMR_TickCountGet();
		return;
	}
	
    if (SYS_TMR_TickCountGet() - lastTimeOut > timeOutValue)
	{
		char message[32] = {0};
		
		sprintf(message, "Temperature = %d <C>\r\n", temperature);
		if (APP_MQTT_PublishMsg(message)== SYS_MQTT_SUCCESS)
		{
                    SYS_CONSOLE_PRINT("\nPublished Msg( %s ) to Topic\r\n", message); 
		}
		
		g_lastPubTimeout = SYS_TMR_TickCountGet(); 
	}
}
#endif
/* I2C transfer callback */
static void i2cTransferCallback(DRV_I2C_TRANSFER_EVENT event, 
        DRV_I2C_TRANSFER_HANDLE transferHandle, 
        uintptr_t context){
    switch(event)
    {
        case DRV_I2C_TRANSFER_EVENT_COMPLETE:
            appData.i2c.transferStatus = I2C_TRANSFER_STATUS_SUCCESS;            
            break;
        case DRV_I2C_TRANSFER_EVENT_ERROR:
            appData.i2c.transferStatus = I2C_TRANSFER_STATUS_ERROR;
            break;
        default:
            break;
    }
}
/* I2C read */
static bool i2cReadReg(uint8_t addr, uint16_t reg, uint8_t size){
    bool ret = false;
    //appData.i2c.transferHandle == DRV_I2C_TRANSFER_HANDLE_INVALID;
    appData.i2c.txBuffer[0] = (uint8_t)reg;
    
    DRV_I2C_WriteReadTransferAdd(appData.i2c.i2cHandle, 
            addr, 
            (void*)appData.i2c.txBuffer, 1, 
            (void*)&appData.i2c.rxBuffer, size, 
            &appData.i2c.transferHandle);
    if(appData.i2c.transferHandle == DRV_I2C_TRANSFER_HANDLE_INVALID)
    {
        SYS_CONSOLE_PRINT("I2C read reg %x error \r\n", reg);
        ret = false;
    }
    else
        ret = true;
    return ret;
}

/* I2C read complete */
static void i2cReadRegComp(uint8_t addr, uint8_t reg){
    appData.i2c.rxBuffer = (appData.i2c.rxBuffer << 8) | (appData.i2c.rxBuffer >> 8);
    //SYS_CONSOLE_PRINT("I2C read complete - periph addr %x val %x\r\n", addr, appData.i2c.rxBuffer);
    switch(addr)
    {   
        /* MCP9808 */
        case MCP9808_I2C_ADDRESS:
            if (reg == MCP9808_REG_TAMBIENT){
                uint8_t upperByte = (uint8_t)(appData.i2c.rxBuffer >> 8);
                uint8_t lowerByte = ((uint8_t)(appData.i2c.rxBuffer & 0x00FF));
                upperByte = upperByte & 0x1F;
                if ((upperByte & 0x10) == 0x10){         // Ta < 0 degC
                    upperByte = upperByte & 0x0F;       // Clear sign bit
                    temperature = 256 - ((upperByte * 16) + lowerByte/16);
                }
                else{
                    temperature = ((upperByte * 16) + lowerByte/16);
                }
                //SYS_CONSOLE_PRINT("MCP9808 Temperature %d (C)\r\n", temperature);                
            }
            else if (reg == MCP9808_REG_DEVICE_ID){
                deviceID = appData.i2c.rxBuffer;
                SYS_CONSOLE_PRINT("MCP9808 Device ID %x\r\n", deviceID);                
            }
            break;

        default:
            break;
    }
}

/* I2C write */
static bool i2cWriteReg(uint8_t addr, uint16_t reg, uint16_t val){
    bool ret = false;
    //appData.i2c.transferHandle == DRV_I2C_TRANSFER_HANDLE_INVALID;
    appData.i2c.txBuffer[0] = (uint8_t)reg;
    appData.i2c.txBuffer[1] = (uint8_t)(val >> 8);
    appData.i2c.txBuffer[2] = (uint8_t)(val & 0x00FF);
    
    DRV_I2C_WriteTransferAdd(appData.i2c.i2cHandle, 
            addr, 
            (void*)appData.i2c.txBuffer, 3, 
            &appData.i2c.transferHandle);
    if(appData.i2c.transferHandle == DRV_I2C_TRANSFER_HANDLE_INVALID)
    {
        SYS_CONSOLE_PRINT("I2C write reg %x error \r\n", reg);
        ret = false;
    }
    else
        ret = true;
    
    return ret;
}

/* I2C write complete */
static void i2cWriteRegComp(uint8_t addr, uint8_t reg){
    SYS_CONSOLE_PRINT("I2C write complete - periph addr %x\r\n", addr);
}
void APP_TempI2CRead(uintptr_t context)
{
        appData.i2c.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
        if(i2cReadReg(MCP9808_I2C_ADDRESS, MCP9808_REG_TAMBIENT, 2))
            appData.state = APP_CTRL_WAIT_READ_TEMP;            /* Wait till ADC conversion result is available */
}

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

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;


    TMR3_Start();
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
	 APP_MQTT_Initialize();
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */



void APP_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appData.state )
    {
        case APP_STATE_INIT:
        {
            /* Open I2C driver client */
            appData.i2c.i2cHandle = DRV_I2C_Open( DRV_I2C_INDEX_0, DRV_IO_INTENT_READWRITE );
            if (appData.i2c.i2cHandle == DRV_HANDLE_INVALID)
            {
                SYS_CONSOLE_PRINT("Failed to open I2C driver for sensors reading\r\n");
            }
            else{
                DRV_I2C_TransferEventHandlerSet(appData.i2c.i2cHandle, i2cTransferCallback, 0);
                SYS_TIME_CallbackRegisterMS(APP_TempI2CRead, (uintptr_t)NULL, 5000, SYS_TIME_PERIODIC);
                appData.state = APP_CTRL_TURN_ON_MCP9808;
            }
        }
        /* MCP9808 turn on */
        case APP_CTRL_TURN_ON_MCP9808:
        {
            appData.i2c.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
            if(i2cWriteReg(MCP9808_I2C_ADDRESS, MCP9808_REG_CONFIG, MCP9808_CONFIG_DEFAULT))
                appData.state = APP_CTRL_WAIT_TURN_ON_MCP9808;
            else
                appData.state = APP_CTRL_WAIT_TURN_ON_MCP9808;
            break;
        }
        /* MCP9808 wait for turn on */
        case APP_CTRL_WAIT_TURN_ON_MCP9808:
        {
            if(appData.i2c.transferStatus == I2C_TRANSFER_STATUS_SUCCESS){
                i2cWriteRegComp(MCP9808_I2C_ADDRESS, MCP9808_REG_CONFIG);
                appData.state = APP_CTRL_READ_MCP9808_DEV_ID;
            }
            else if(appData.i2c.transferStatus == I2C_TRANSFER_STATUS_ERROR){
                SYS_CONSOLE_PRINT( "I2C write MCP9808_REG_CONFIG error \r\n");
                appData.state = APP_CTRL_TURN_ON_MCP9808;
            }
            break;
        }
        /* MCP9808 read device ID */
        case APP_CTRL_READ_MCP9808_DEV_ID:
        {
            /* Schedule MCP9808 device ID reading */
            appData.i2c.transferStatus = I2C_TRANSFER_STATUS_IN_PROGRESS;
                if(i2cReadReg(MCP9808_I2C_ADDRESS, MCP9808_REG_DEVICE_ID, 2))
                    appData.state = APP_CTRL_WAIT_MCP9808_DEV_ID;
            break;
        }
        /* MCP9808 wait read device ID */
        case APP_CTRL_WAIT_MCP9808_DEV_ID:
        {
            /* MCP9808 device ID reading operation done */
            if(appData.i2c.transferStatus == I2C_TRANSFER_STATUS_SUCCESS){
                i2cReadRegComp(MCP9808_I2C_ADDRESS, MCP9808_REG_DEVICE_ID);
                appData.state = APP_CTRL_READ_TEMP;
            }
            break;
        }
        /* MCP9808 wait for read ambient temperature */
        case APP_CTRL_WAIT_READ_TEMP:
        {
            /* MCP9808 Temperature reading operation done */
            if(appData.i2c.transferStatus == I2C_TRANSFER_STATUS_SUCCESS){
                i2cReadRegComp(MCP9808_I2C_ADDRESS, MCP9808_REG_TAMBIENT);
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }
        case APP_STATE_SERVICE_TASKS:
        {
            break;
        }


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }

	APP_MQTT_Tasks(); 
#ifdef SYS_MQTT_DEF_PUB_TOPIC_NAME		
	APP_CheckTimeOut(MQTT_PUB_TIMEOUT_CONST, g_lastPubTimeout);
#endif
}


/*******************************************************************************
 End of File
 */
