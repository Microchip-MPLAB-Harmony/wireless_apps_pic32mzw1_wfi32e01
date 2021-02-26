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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "definitions.h"
#include "ota/ota.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

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
char url[100] = SYS_OTA_URL;
bool otaFwInProgress = false;
bool dev_connected_to_wifi = false;
SYS_STATUS status;
bool download_success = false;
bool ota_completed = false;


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
void WiFiServCallback (uint32_t event, void * data,void *cookie )
    {
         
        switch(event)
        {
            case SYS_WIFI_CONNECT:
            {
                
                SYS_CONSOLE_PRINT("Device CONNECTED \r\n");
                dev_connected_to_wifi = true;
                break;
            }
            case SYS_WIFI_DISCONNECT:
            {
                SYS_CONSOLE_PRINT("Device DISCONNECTED \r\n");
                break;
            }
            case SYS_WIFI_PROVCONFIG:
            {
                SYS_CONSOLE_PRINT("Received the Provisioning data \r\n");
                break;
            }
        }
    }

static int OTACallback(SYS_STATUS status)
{
    
    if (otaFwInProgress)
    {
        if(!((status == SYS_STATUS_READY)? 0:1))
            download_success = true;
        otaFwInProgress = false;
        ota_completed = true;
    }
    return 0;
}

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


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
    CRYPT_RNG_CTX * rng_context;
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    HTTP_Client_Init();
    OTA_Initialize();
    // Initialize PRNG for SYS_RANDOM_PseudoGet().
    rng_context = OSAL_Malloc(sizeof(CRYPT_RNG_CTX));
    if (rng_context != NULL)
    {
        if (CRYPT_RNG_Initialize(rng_context) == 0)
        {
            uint32_t rng;
            CRYPT_RNG_BlockGenerate(rng_context, (unsigned char*)&rng, sizeof(rng));
            srand(rng);
        }
        OSAL_Free(rng_context);
    }
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

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;


            if (appInitialized)
            {
                SYS_WIFI_CtrlMsg(sysObj.syswifi,SYS_WIFI_REGCALLBACK,WiFiServCallback,sizeof(uint8_t *));
                appData.state = APP_STATE_SERVICE_TASKS;
                
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            
            if(dev_connected_to_wifi == true)
            {
                
                appData.state = APP_STATE_OTA_START;
            }
            break;
        }
            
        case APP_STATE_OTA_START:
        {
            
        
            status = OTA_Start(url, OTACallback);
            SYS_CONSOLE_PRINT("Starting OTA with server: %s \n\r",url);
            if(status == SYS_STATUS_ERROR)
            {
                
                appData.state = APP_STATE_OTA_ERROR;
                break;
            }
            otaFwInProgress = true;
            appData.state = APP_STATE_WAIT_OTA_CMPLT;
            break;
        }
        
        case APP_STATE_WAIT_OTA_CMPLT:
        {
            if(ota_completed == true)
            {
                if(download_success == true)
                {
                    SYS_CONSOLE_PRINT("OTA process completed successfully please reset the device for loading new image\r\n");
                    appData.state = APP_STATE_OTA_SUCCESS;
                }
                else
                {
                    SYS_CONSOLE_PRINT("\n\r1) SERVER MAY BE BUSY or \n\r2) IMAGE NAME IS WRONG or \n\r3) SERVER ADDRESS MAY BE WRONG \r\nPlease press reset button to initiate OTA again\r\n");
                    appData.state = APP_STATE_OTA_ERROR;
                }
            }
            break;
        }
        
        case APP_STATE_OTA_SUCCESS:
        {
            appData.state = APP_STATE_OTA_COMPLETE;
            break;
            
        }
        
        case APP_STATE_OTA_ERROR:
        {
            SYS_CONSOLE_PRINT("OTA ERROR \r\n");
            appData.state = APP_STATE_OTA_COMPLETE;
            break;
            
        }
        
        case APP_STATE_OTA_COMPLETE:
        {
            
            break;
            
        }

        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
    OTA_Tasks();
}


/*******************************************************************************
 End of File
 */
