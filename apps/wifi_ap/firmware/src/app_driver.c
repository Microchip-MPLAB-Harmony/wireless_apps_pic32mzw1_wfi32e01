/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_driver.c

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

#include "app_driver.h"
#include "wdrv_pic32mzw_client_api.h"
#include "tcpip/tcpip_manager.h"
#include "tcpip/src/tcpip_manager_control.h"
#include "tcpip/dhcps.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

typedef struct wifiConfiguration 
{
    WDRV_PIC32MZW_AUTH_CONTEXT authCtx;
    WDRV_PIC32MZW_BSS_CONTEXT bssCtx;
} wifiConfig;

SYS_TIME_HANDLE timeHandle;
static wifiConfig g_wifiConfig;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_DRIVER_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void APP_TimerCallback(uintptr_t context)
{
    APP_APShowConnectedDevices();
}

static bool APP_BSSFindNotifyCallback(DRV_HANDLE handle, uint8_t index, uint8_t ofTotal, WDRV_PIC32MZW_BSS_INFO *pBSSInfo)
{
    WDRV_PIC32MZW_BSS_INFO bssInfo;
    
    if(ofTotal == 0)
    {
        SYS_CONSOLE_MESSAGE("APP: No AP Found Rescan\r\n");
        return true;
    }
        
    if(index == 1)
    {
        SYS_CONSOLE_PRINT("#%02d\r\n", ofTotal);
        SYS_CONSOLE_PRINT(">>#  RI  Sec  Recommend CH BSSID             SSID\r\n");
        SYS_CONSOLE_PRINT(">>#      Cap  Auth Type\r\n>>#\r\n");
    }
    
    if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_BSSFindGetInfo(appData.wdrvHandle, &bssInfo))
    {
        SYS_CONSOLE_PRINT(">>%02d %d 0x%02x ", index, bssInfo.rssi, bssInfo.secCapabilities);

        switch (bssInfo.authTypeRecommended)
        {
            case WDRV_PIC32MZW_AUTH_TYPE_OPEN:
            {
                SYS_CONSOLE_PRINT("OPEN     ");
                break;
            }

            case WDRV_PIC32MZW_AUTH_TYPE_WEP:
            {
                SYS_CONSOLE_PRINT("WEP");
                break;
            }

            case WDRV_PIC32MZW_AUTH_TYPE_WPAWPA2_PERSONAL:
            {
                SYS_CONSOLE_PRINT("WPA/2 PSK");
                break;
            }

            case WDRV_PIC32MZW_AUTH_TYPE_WPA2_PERSONAL:
            {
                SYS_CONSOLE_PRINT("WPA2 PSK ");
                break;
            }
#ifdef WDRV_PIC32MZW_WPA3_SUPPORT
            case WDRV_PIC32MZW_AUTH_TYPE_WPA2WPA3_PERSONAL:
            {
                SYS_CONSOLE_PRINT("SAE/PSK  ", 9);
                break;
            }

            case WDRV_PIC32MZW_AUTH_TYPE_WPA3_PERSONAL:
            {
                SYS_CONSOLE_PRINT("SAE      ", 9);
                break;
            }
#endif
            default:
            {
                SYS_CONSOLE_PRINT("Not Avail");
                break;
            }
        }

        SYS_CONSOLE_PRINT(" %02d %02X:%02X:%02X:%02X:%02X:%02X %.*s\r\n", bssInfo.ctx.channel,
            bssInfo.ctx.bssid.addr[0], bssInfo.ctx.bssid.addr[1], bssInfo.ctx.bssid.addr[2],
            bssInfo.ctx.bssid.addr[3], bssInfo.ctx.bssid.addr[4], bssInfo.ctx.bssid.addr[5],
            bssInfo.ctx.ssid.length, bssInfo.ctx.ssid.name);
    }

    return true;
}

static void APP_APNotifyCallback(DRV_HANDLE handle, WDRV_PIC32MZW_ASSOC_HANDLE associationHandle, WDRV_PIC32MZW_CONN_STATE currentState)
{
    WDRV_PIC32MZW_MAC_ADDR macAddr;
    WDRV_PIC32MZW_AssocPeerAddressGet(associationHandle, &macAddr);
    
    if (WDRV_PIC32MZW_CONN_STATE_CONNECTED == currentState)
    {
        timeHandle = SYS_TIME_CallbackRegisterMS(APP_TimerCallback, (uintptr_t)0, 500, SYS_TIME_SINGLE);
        SYS_CONSOLE_PRINT("Wifi State :: CONNECTED :: %02X:%02X:%02X:%02X:%02X:%02X\r\n", macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);
    }
    else if (WDRV_PIC32MZW_CONN_STATE_DISCONNECTED == currentState)
    {
        SYS_CONSOLE_PRINT("Wifi State :: DISCONNECTED :: %02X:%02X:%02X:%02X:%02X:%02X\r\n", macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);
        TCPIP_DHCPS_LeaseEntryRemove(appData.netHandle, (TCPIP_MAC_ADDR*) macAddr.addr);
    }
}

static void APP_RegDomainSetCallback(DRV_HANDLE handle, uint8_t index, uint8_t ofTotal, bool isCurrent, const WDRV_PIC32MZW_REGDOMAIN_INFO *const pRegDomInfo)
{
    if ((1 != index) || (1 != ofTotal) || (false == isCurrent) || (NULL == pRegDomInfo) || (0 == pRegDomInfo->regDomainLen))
    {
        SYS_CONSOLE_MESSAGE("APP Error: Unable to set the Regulatory domain\r\n");
    }
    else
    {
        appData.state = APP_WIFI_CONFIG;
        SYS_CONSOLE_MESSAGE("APP: Regulatory domain set successfully\r\n");
    }
}

static void APP_RegDomainGetCallback(DRV_HANDLE handle, uint8_t index, uint8_t ofTotal, bool isCurrent, const WDRV_PIC32MZW_REGDOMAIN_INFO *const pRegDomInfo)
{
    if (0 == ofTotal)
    {
        if ((NULL == pRegDomInfo) || (0 == pRegDomInfo->regDomainLen))
        {
            SYS_CONSOLE_MESSAGE("APP: No Regulatory Domains Defined\r\n");
        }
        else
        {
            SYS_CONSOLE_MESSAGE("APP Error: Getting regulatory domain\r\n");
        }
    }
    else if (NULL != pRegDomInfo)
    {
        if (1 == index)
        {
            SYS_CONSOLE_MESSAGE("#.   CC      Ver Status\r\n");
        }
        SYS_CONSOLE_PRINT("%02d: [%-6s] %d.%d %s\r\n", index, pRegDomInfo->regDomain, pRegDomInfo->version.major, pRegDomInfo->version.minor, isCurrent ? "Active" : "");
    }
    else
    {
        SYS_CONSOLE_MESSAGE("APP Error: Getting regulatory domain\r\n");
    }
}

static void APP_RfMacConfigStatus(void)
{
    WDRV_PIC32MZW_RF_MAC_CONFIG rfMacConfig;
    char status[100] = {0};
    
    if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_InfoRfMacConfigGet(appData.wdrvHandle, &rfMacConfig))
    {
        if (true != rfMacConfig.powerOnCalIsValid)
        {
            strcat(status, "Power ON calibration, ");
        }
        
        if (true != rfMacConfig.factoryCalIsValid)
        {
            strcat(status, "Factory calibration, ");
        }
        
        if (true != rfMacConfig.gainTableIsValid)
        {
            strcat(status, "Regulatory domain, ");
        }
        
        if (true != rfMacConfig.macAddressIsValid)
        {
            strcat(status, "MAC address, ");
        }
        
        if (NULL != *status)
        {
            appData.isRfMacConfigValid = false;
            SYS_CONSOLE_PRINT("APP Error: %s - configurations missing\r\n",status);
        }
        else
        {
            appData.isRfMacConfigValid = true;
            SYS_CONSOLE_PRINT("APP: RF and MAC configurations are set successfully\r\n");
        }
    }
}

void APP_Scan(uint8_t channel, SCAN_TYPE scanType, uint16_t scanTime)
{
    if ((channel > 13) && (channel < 255))
    {
        SYS_CONSOLE_MESSAGE("APP: Invalid channel \r\n");
        return;
    }
    
    SYS_CONSOLE_PRINT("APP: channel: %d scanType: %d\r\n",channel, scanType);
    
    if(scanType == ACTIVE)
    {
        if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSFindSetScanParameters(appData.wdrvHandle, scanTime, 0))
        {
            SYS_CONSOLE_MESSAGE("APP Error: updating Scan Parameters\r\n");
            return;
        }
    }
    else
    {
        if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSFindSetScanParameters(appData.wdrvHandle, 0, scanTime))
        {
            SYS_CONSOLE_MESSAGE("APP Error: updating Scan Parameters\r\n");
            return;
        }
    }

    if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSFindFirst(appData.wdrvHandle, channel, scanType, APP_BSSFindNotifyCallback))
    {
        SYS_CONSOLE_MESSAGE("APP Error: scan fail\r\n");
    }

    return;
}

bool APP_APStart()
{
    if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_APStart(appData.wdrvHandle, &g_wifiConfig.bssCtx, &g_wifiConfig.authCtx, &APP_APNotifyCallback))
    {
        SYS_CONSOLE_MESSAGE("APP: AP started\r\n");
        appData.isApServiceStarted = true;
        app_controlData.isAPServiceSuspended = false;
        appData.state = APP_WIFI_IDLE;
        return true;
    }
    else
    {
        appData.state = APP_WIFI_ERROR;
        return false;
    }
}

bool APP_APStop()
{
    if(appData.isApServiceStarted == true)
    {
        if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_APStop(appData.wdrvHandle))
        {
            SYS_CONSOLE_MESSAGE("APP: AP Stop\r\n");
            appData.isApServiceStarted = false;
            app_controlData.isAPServiceSuspended = true;
            return true;
        }
    }
    
    return false;
}

void APP_APShowConnectedDevices()
{
    int numDhcpsPoolEntries;
    numDhcpsPoolEntries = TCPIP_DHCPS_GetPoolEntries(appData.netHandle, DHCP_SERVER_POOL_ENTRY_IN_USE);

    if (numDhcpsPoolEntries > 0)
    {
        TCPIP_DHCPS_LEASE_HANDLE dhcpsLease = 0;
        TCPIP_DHCPS_LEASE_ENTRY dhcpsLeaseEntry;
        SYS_CONSOLE_MESSAGE("--------------------------------------------\r\n");
        SYS_CONSOLE_MESSAGE("           Connected devices summary\r\n");
        SYS_CONSOLE_MESSAGE("--------------------------------------------\r\n\r\n");
        SYS_CONSOLE_MESSAGE("MAC Address              IP Address\r\n");

        do
        {
            dhcpsLease = TCPIP_DHCPS_LeaseEntryGet(appData.netHandle, &dhcpsLeaseEntry, dhcpsLease);

            if (NULL != dhcpsLease)
            {
                char ipAddr[20];
                char macAddr[20];
                
                TCPIP_Helper_MACAddressToString(&dhcpsLeaseEntry.hwAdd, macAddr, sizeof(macAddr));
                TCPIP_Helper_IPAddressToString(&dhcpsLeaseEntry.ipAddress, ipAddr, sizeof(ipAddr));

                SYS_CONSOLE_PRINT("%s", macAddr);
                SYS_CONSOLE_PRINT("        %s\r\n", ipAddr);
            }
        }
        while (NULL != dhcpsLease);
    }
    else
    {
        SYS_CONSOLE_MESSAGE("No device is connected\r\n");
    }
}

void APP_RegDomainGet(uint8_t regDomainSelect)
{
    if (0 == regDomainSelect)
    {
        if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_RegDomainGet(appData.wdrvHandle, WDRV_PIC32MZW_REGDOMAIN_SELECT_ALL, APP_RegDomainGetCallback))
        {
            SYS_CONSOLE_MESSAGE("APP Error: Getting regulatory domain\r\n");
        }
    }
    else
    {
        if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_RegDomainGet(appData.wdrvHandle, WDRV_PIC32MZW_REGDOMAIN_SELECT_CURRENT, APP_RegDomainGetCallback))
        {
            SYS_CONSOLE_MESSAGE("APP Error: Getting regulatory domain\r\n");
        }
    }
}

static bool APP_WIFI_Config() 
{    
    WIFI_AUTH_MODE authMode = (WIFI_AUTH_MODE)app_controlData.wlanConfig.authMode;
    uint8_t *ssid           = (uint8_t *)app_controlData.wlanConfig.ssid;
    uint8_t ssidLength      = app_controlData.wlanConfig.ssidLength;
    uint8_t *password       = (uint8_t *)app_controlData.wlanConfig.password;
    uint8_t passwordLength  = strlen((char *)password);

    SYS_CONSOLE_PRINT("APP: SSID is %.*s \r\n",ssidLength, ssid);
    if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSCtxSetSSID(&g_wifiConfig.bssCtx, ssid, ssidLength)) 
    {
        SYS_CONSOLE_PRINT("SSID set fail \r\n");
        return false;
    }
    
    if(WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSCtxSetChannel(&g_wifiConfig.bssCtx, app_controlData.wlanConfig.channel))
    {
        SYS_CONSOLE_PRINT("channel %d \r\n", app_controlData.wlanConfig.channel);
        return false;
    }
    
    if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSCtxSetSSIDVisibility(&g_wifiConfig.bssCtx, 1))
    {
        SYS_CONSOLE_MESSAGE("Set visibility fail \r\n");
        return false;
    }

    
    switch (authMode) 
    {
        case OPEN:
        {
            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetOpen(&g_wifiConfig.authCtx)) 
            {
                SYS_CONSOLE_MESSAGE("APP: Unable to set Authentication\r\n");
                return false;
            }
            break;
        }
        case WPA2:
        {
            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiConfig.authCtx, (uint8_t *)password, passwordLength, WDRV_PIC32MZW_AUTH_TYPE_WPA2_PERSONAL)) 
            {
                SYS_CONSOLE_MESSAGE("APP: Unable to set authentication to WPA2\r\n");
                return false;
            }
            break;
        }
        case WPAWPA2MIXED:
        {
            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiConfig.authCtx, password, passwordLength, WDRV_PIC32MZW_AUTH_TYPE_WPAWPA2_PERSONAL)) 
            {
                SYS_CONSOLE_MESSAGE("APP: Unable to set authentication to WPAWPA2 MIXED\r\n");
                return false;
            }
            break;
        }
#ifdef WDRV_PIC32MZW_WPA3_SUPPORT
        case WPA3:
        {
            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiConfig.authCtx, password, passwordLength, WDRV_PIC32MZW_AUTH_TYPE_WPA3_PERSONAL)) 
            {
                SYS_CONSOLE_MESSAGE("APP: Unable to set authentication to WPA3 \r\n");
                return false;
            }
            break;
        }
        case WPA2WPA3MIXED:
        {
            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiConfig.authCtx, password, passwordLength, WDRV_PIC32MZW_AUTH_TYPE_WPA2WPA3_PERSONAL)) 
            {
                SYS_CONSOLE_MESSAGE("APP: Unable to set authentication to WPA2WPA3 MIXED \r\n");
                return false;
            }
            break;
        }
#endif
        case WEP:
        {
            uint8_t *wepKey        = app_controlData.wlanConfig.wepKey;
            uint8_t wepKeyLength  = strlen((char *)app_controlData.wlanConfig.wepKey);
            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetWEP(&g_wifiConfig.authCtx, app_controlData.wlanConfig.wepIdx, wepKey, wepKeyLength))
            {
                SYS_CONSOLE_MESSAGE("APP: Unable to set authentication to WEP \r\n");
                return false;
            }
            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSharedKey(&g_wifiConfig.authCtx, true))
            {
                SYS_CONSOLE_MESSAGE("APP: Unable to Enable shared key authentication \r\n");
                return false;
            }
            break;
        }
        default:
        {
            SYS_CONSOLE_PRINT("APP: Unknown Authentication type\r\n");
            return false;
        }
    }

    if (false == TCPIP_DHCPS_Enable(appData.netHandle))
    {
        return false;
    }
    return true;
}



// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_DRIVER_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_DRIVER_Initialize ( void )
{
    appData.state                   = APP_STATE_INIT;
    appData.wdrvHandle              = DRV_HANDLE_INVALID;
    appData.netHandle               = TCPIP_STACK_NetHandleGet(TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX0);
    
    WDRV_PIC32MZW_BSSCtxSetDefaults(&g_wifiConfig.bssCtx);
    WDRV_PIC32MZW_AuthCtxSetDefaults(&g_wifiConfig.authCtx);
    
    SYS_CONSOLE_MESSAGE("APP: Initialization Successful\r\n");
}


/******************************************************************************
  Function:
    void APP_DRIVER_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_DRIVER_Tasks ( void )
{
    SYS_STATUS tcpipStat;
    bool status;

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            SYS_STATUS sysStatus;
            
            sysStatus = WDRV_PIC32MZW_Status(sysObj.drvWifiPIC32MZW1);
            
            if (SYS_STATUS_READY == sysStatus)
            {
                appData.state = APP_STATE_WDRV_INIT_READY;
            }
            else if (SYS_STATUS_READY_EXTENDED == sysStatus)
            {
                if (WDRV_PIC32MZW_SYS_STATUS_RF_CONF_MISSING == WDRV_PIC32MZW_StatusExt(sysObj.drvWifiPIC32MZW1))
                {
                    /* Continue to initialisation state to allow application to set reg domain from command */
                    appData.state = APP_STATE_WDRV_INIT_READY;
                }
            }
            break;
        }
        
        case APP_STATE_WDRV_INIT_READY:
        {
            appData.wdrvHandle = WDRV_PIC32MZW_Open(0, 0);
            
            if (DRV_HANDLE_INVALID != appData.wdrvHandle) 
            {
                appData.state = APP_TCPIP_WAIT_FOR_TCPIP_INIT;
            }
            break;
        }

        case APP_TCPIP_WAIT_FOR_TCPIP_INIT:
        {
            tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);

            if (tcpipStat < 0)
            {
                SYS_CONSOLE_MESSAGE( "TCP/IP stack initialization failed!\r\n" );
                appData.state = APP_TCPIP_ERROR;
            }
            else if(SYS_STATUS_READY == tcpipStat)
            {
                appData.state = APP_WIFI_RF_MAC_CONFIG;
            }
            break;
        }
        
        case APP_WIFI_RF_MAC_CONFIG:
        {
            APP_RfMacConfigStatus();
            
            if (true == appData.isRfMacConfigValid)
            {
                appData.state = APP_WIFI_CONFIG;
            }
            else
            {
                appData.state = APP_WIFI_ERROR;
            }
            break;
        }
        
        case APP_WIFI_CONFIG:
        {
            if(app_controlData.wlanConfigValid) 
            {
                status = APP_WIFI_Config();
                
                if(status)
                {
                    appData.state = APP_WIFI_AP_START;
                }
                
                app_controlData.wlanConfigValid = 0;
            }
            else
            {
                appData.state = APP_WIFI_IDLE;
            }
            
            break;
        }
        
        case APP_WIFI_AP_START:
        {
            APP_APStart();
            break;
        }
        
        case APP_WIFI_IDLE:
        {
            if(app_controlData.wlanConfigChanged) 
            {
                appData.state = APP_WIFI_CONFIG;
                app_controlData.wlanConfigChanged = false;
            }
            break;
        }
        
        case APP_WIFI_ERROR:
        {
            if (true == app_controlData.regDomChanged)
            {
                WDRV_PIC32MZW_RegDomainSet(appData.wdrvHandle, app_controlData.regDomName, APP_RegDomainSetCallback);
                app_controlData.regDomChanged = false;
            }
            
            if (false == appData.isRfMacConfigValid)
            {
                if (WDRV_PIC32MZW_SYS_STATUS_RF_READY == WDRV_PIC32MZW_StatusExt(appData.wdrvHandle))
                {
                    appData.isRfMacConfigValid = true;
                    appData.state = APP_WIFI_CONFIG;
                }
            }
            break;
        }
        
        case APP_TCPIP_ERROR:
        {
            break;
        }
        
        default:
        {
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */