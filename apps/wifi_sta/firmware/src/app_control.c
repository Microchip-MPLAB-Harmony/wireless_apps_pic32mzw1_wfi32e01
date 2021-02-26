/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_control.c

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

#include "app_control.h"
#include "app_driver.h"
#include "system_config.h"
#include "system_definitions.h"

#define READ_WRITE_SIZE         (NVM_FLASH_PAGESIZE/2)
#define BUFFER_SIZE             (READ_WRITE_SIZE / sizeof(uint32_t))
#define APP_FLASH_ADDRESS       0x900fE000 

static void WLANCMDProcessing(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

static const SYS_CMD_DESCRIPTOR    WLANCmdTbl[]=
{
    {"wlan",     WLANCMDProcessing,              ": WLAN MAC commands processing"},
};

static volatile bool xferDone = false;
static uint32_t nvmDataBuff[BUFFER_SIZE] CACHE_ALIGN;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_CONTROL_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_CONTROL_DATA app_controlData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void nvmEventHandler(uintptr_t context)
{
    xferDone = true;
}

static void nvmConfigInit(void)
{
    NVM_CallbackRegister(nvmEventHandler, (uintptr_t) NULL);
    return;
}

static void nvmPopulateBuffer(WLAN_CONFIG_DATA *wlanConfig)
{
    memset(nvmDataBuff, 0, BUFFER_SIZE); //Clear the buffer first
    memcpy((void*) nvmDataBuff, (const void*) wlanConfig, sizeof (WLAN_CONFIG_DATA)); //copy data into the template buffer.
}

static int nvmWriteConfig(WLAN_CONFIG_DATA *wlanConfig)
{
    uint32_t address = APP_FLASH_ADDRESS;
    uint8_t *writePtr = (uint8_t *) nvmDataBuff;
    uint32_t i = 0;

    while (NVM_IsBusy() == true);

    if (!NVM_PageErase(address)) {
        SYS_CONSOLE_PRINT("Failed NVM erase @ %x \r\n", address);
    }
    while (xferDone == false);
    xferDone = false;
    nvmPopulateBuffer(wlanConfig);

    for (i = 0; i < READ_WRITE_SIZE; i += NVM_FLASH_ROWSIZE) {
        /* Program a row of data */
        if (!NVM_RowWrite((uint32_t *) writePtr, address)) {
            SYS_CONSOLE_PRINT("Failed NVM ROW write @ %x \r\n", address);
        }

        while (xferDone == false);

        xferDone = false;

        writePtr += NVM_FLASH_ROWSIZE;
        address += NVM_FLASH_ROWSIZE;
    }
    return 0;
}

static bool nvmReadConfig(WLAN_CONFIG_DATA *wlanConfig)
{
    NVM_Read(nvmDataBuff, sizeof (WLAN_CONFIG_DATA), APP_FLASH_ADDRESS);
    
    memcpy((void*) wlanConfig, (void*) nvmDataBuff, sizeof (WLAN_CONFIG_DATA));
    return true;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_CONTROL_Initialize ( void )

  Remarks:
    See prototype in app_control.h.
 */

void APP_CONTROL_Initialize ( void )
{
    
    nvmConfigInit();
    if (nvmReadConfig(&app_controlData.wlanConfig)) 
    {

        if((app_controlData.wlanConfig.ssidLength > 0) && (app_controlData.wlanConfig.ssidLength < (SSID_LENGTH+1)))
        {
            app_controlData.wlanConfigValid = true;
            SYS_CONSOLE_MESSAGE("WLAN Config read from NVM\r\n");
        }
        else
        {
            app_controlData.wlanConfigValid = false;
            SYS_CONSOLE_MESSAGE("No WLAN Config in NVM\r\n");
        }
    } 
    else 
    {
        app_controlData.wlanConfigValid = false;
        SYS_CONSOLE_MESSAGE("NVM read fail\r\n");
    }
    
    if (!SYS_CMD_ADDGRP(WLANCmdTbl, sizeof(WLANCmdTbl)/sizeof(*WLANCmdTbl), "wlan", ": WLAN commands"))
    {
        SYS_ERROR(SYS_ERROR_ERROR, "Failed to create WLAN Commands\r\n");
    }
    
    app_controlData.state = APP_CONTROL_STATE_INIT;
}


/******************************************************************************
  Function:
    void APP_CONTROL_Tasks ( void )

  Remarks:
    See prototype in app_control.h.
 */

void APP_CONTROL_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( app_controlData.state )
    {
        /* Application's initial state. */
        case APP_CONTROL_STATE_INIT:
        {
            
            app_controlData.state = APP_CONTROL_STATE_SERVICE_TASKS;
            
            break;
        }

        case APP_CONTROL_STATE_SERVICE_TASKS:
        {

            break;
        }
        default:
        {
            break;
        }
    }
}

static void WLANCMDProcessing(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if (argc < 2)
    {
        return;
    }
    
    if(!strcmp("set", argv[1]))
    {
        if (argc < 3)
        {
            SYS_CONSOLE_MESSAGE("usage: wlan set config <ssid> <ssid_length> <channel> <open | wpa2 | wpam | wpa3 | wpa3m | wep> <password>\r\n");
            SYS_CONSOLE_MESSAGE("usage: wlan set regdomain <reg_domain_name>\r\n");
            return;
        }
        
        if (!strcmp("config", argv[2]))
        {
            if (argc < 7)
            {
                SYS_CONSOLE_MESSAGE("usage: wlan set config <ssid> <ssid_length> <channel> <open | wpa2 | wpam | wpa3 | wpa3m | wep> <password>\r\n");
            return;
        }
        else
        {
                char *ssid = argv[3];
                char *authMode = argv[6];
            char *password;
                unsigned char ssidLength = strtoul(argv[4],0,10);
                unsigned char channel = strtoul(argv[5],0,10);
                        
                if (8 == argc)
            {
                    password = argv[7];
            }
                else if (7 == argc)
            {
                password = "  ";
            }
            else 
            {
                    SYS_CONSOLE_MESSAGE("usage: wlan set config <ssid> <ssid_length> <channel> <open | wpa2 | wpam | wpa3 | wpa3m | wep> <password>\r\n");
                return;
            }
            
            if(ssidLength > SSID_LENGTH)
            {
                SYS_CONSOLE_MESSAGE("SSID too long");
                return;
            }
            else
            {
                app_controlData.wlanConfig.ssidLength = ssidLength;
                memset(app_controlData.wlanConfig.ssid, 0, SSID_LENGTH);
                memcpy(app_controlData.wlanConfig.ssid, ssid, ssidLength);
            }
            
                if (channel > 13)
                {
                SYS_CONSOLE_MESSAGE("Invalid channel number");
                return;
            }
            else
            {
                app_controlData.wlanConfig.channel = channel;
            }
            
            if ((!strcmp(authMode, "open")) || (!strcmp(authMode, "OPEN"))) 
            {
                app_controlData.wlanConfig.authMode = OPEN;
            } 
            else if ((!strcmp(authMode, "wpa2")) || (!strcmp(authMode, "WPA2"))) 
            {
                app_controlData.wlanConfig.authMode = WPA2;
            } 
            else if ((!strcmp(authMode, "wpam")) || (!strcmp(authMode, "WPAM"))) 
            {
                app_controlData.wlanConfig.authMode = WPAWPA2MIXED;
            } 
            else if ((!strcmp(authMode, "wpa3")) || (!strcmp(authMode, "WPA3"))) 
            {
                app_controlData.wlanConfig.authMode = WPA3;
            }
            else if ((!strcmp(authMode, "wpa3m")) || (!strcmp(authMode, "WPA3M"))) 
            {
                app_controlData.wlanConfig.authMode = WPA2WPA3MIXED;
            }
            else if ((!strcmp(authMode, "wep")) || (!strcmp(authMode, "WEP"))) 
            {
                app_controlData.wlanConfig.authMode = WEP;
            } 
            else 
            {
                SYS_CONSOLE_MESSAGE("Invalid Auth mode \r\n Supported auth modes: <open | wpa2 | wpam | wpa3 | wpa3m | wep> \r\n");
                return;
            }

            if(app_controlData.wlanConfig.authMode != WEP)
            {
                if(strlen(password) > PASSWORD_LENGTH)
                {
                    SYS_CONSOLE_MESSAGE("Password too long\r\n");
                    return;
                }
                memset(app_controlData.wlanConfig.password, 0, PASSWORD_LENGTH+1);
                memcpy(app_controlData.wlanConfig.password, password, strlen(password));
            }
            else
            {
                char* WEPIdx;
                char* WEPKey;

                WEPIdx = strtok(password, "*");

                if (NULL == WEPIdx)
                {
                    SYS_CONSOLE_MESSAGE("Invalid WEP parameter\r\n");
                    return;
                }

                WEPKey = strtok(NULL, "\0");

                if (NULL == WEPKey)
                {
                    SYS_CONSOLE_MESSAGE("Invalid WEP parameter\r\n");
                    return;
                }

                app_controlData.wlanConfig.wepIdx = strtol(WEPIdx, NULL, 0);
                memcpy(app_controlData.wlanConfig.wepKey, (unsigned char *)WEPKey, strlen(WEPKey));
            }
            
            app_controlData.wlanConfigValid = true;
            }
        }
        else if (!strcmp("regdomain", argv[2]))
        {
            int length;
            
            if (argc < 4)
            {
                SYS_CONSOLE_MESSAGE("usage: wlan set regdomain <name>\r\n");
                return;
            }
            else
            {
                length = strlen(argv[3]);
                
                if (length < 7)
                {
                    memset(app_controlData.regDomName, 0, 7);
                    strcpy(app_controlData.regDomName, argv[3]);
                    app_controlData.regDomChanged = true;
                }
            }
        }
    }
    else if(!strcmp("connect", argv[1]))
    {
        if(app_controlData.wlanConfigValid == true)
        {
            app_controlData.wlanConfigChanged = true;
        }
        else
        {
            SYS_CONSOLE_MESSAGE("Entered WLAN configuration is Invalid\r\n");
        }
    }
    else if(!strcmp("config", argv[1]))
    {
        if (argc < 3)
        {
            return;
        }
        
        if(!strcmp("save", argv[2]))
        {
            if(app_controlData.wlanConfigValid == true)
            {
                app_controlData.wlanConfigChanged = true;
                nvmWriteConfig(&app_controlData.wlanConfig);
            }
            else
            {
                SYS_CONSOLE_MESSAGE("Entered WLAN configuration is Invalid\r\n");
            }
        }
    }
    else if(!strcmp("scan", argv[1]))
    {
        if(argc<5)
        {
            SYS_CONSOLE_MESSAGE("usage: wlan scan <active | passive> <channel> <scan time in ms>\r\n");
            SYS_CONSOLE_MESSAGE("EX: wlan scan active 1 200 - Runs active scan on channel 1 for 200ms\r\n");
            SYS_CONSOLE_MESSAGE("EX: wlan scan passive 6 120 - Runs passive scan on channel 6 for 120ms\r\n");
            SYS_CONSOLE_MESSAGE("Note: Setting channel to '0' scans all channels and scan time of 0 uses default values\r\n");
            
            return;
        }
        else
        {
            if((!strcmp("active", argv[2])) || (!strcmp("passive", argv[2])))
            {
                unsigned char channel;
                uint16_t time;
                SCAN_TYPE scanType;
                
                channel  = strtoul(argv[3],0,10);
                time     = strtoul(argv[4],0,10);
                if(!strcmp("active", argv[2]))
                {
                    scanType = ACTIVE;
                }
                else
                {
                    scanType = PASSIVE;
                }
                APP_Scan(channel, scanType, time);
            }
        }
    }
    else if(!strcmp("get", argv[1]))
    {
        if (argc < 3)
        {
            SYS_CONSOLE_MESSAGE("wlan get rssi - display the rssi of the current association\r\n");
            SYS_CONSOLE_MESSAGE("wlan get regdomain <all | current> - display the set regulatory domain\r\n");
            return;
        }
        
        if(!strcmp("rssi", argv[2]))
        {
            APP_RSSIGet();
        }
        else if (!strcmp("regdomain", argv[2]))
        {
            if (argc < 4)
            {
                SYS_CONSOLE_MESSAGE("wlan get regdomain <all | current> - display the all or set regulatory domain\r\n");
                return;
            }
            
            if ((!strcmp("all", argv[3])) || (!strcmp("current", argv[3])))
            {
                uint8_t regDomainSelect;
                if (!strcmp("all", argv[3]))
                {
                    regDomainSelect = 0;
                }
                else
                {
                    regDomainSelect = 1;
                }
                APP_RegDomainGet(regDomainSelect);
            }
        }
    }
}


/*******************************************************************************
 End of File
 */
