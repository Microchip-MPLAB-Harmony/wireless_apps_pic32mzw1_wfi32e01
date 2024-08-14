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
#include "app_control.h"
#include "wdrv_pic32mzw_client_api.h"
#include "tcpip/tcpip_manager.h"

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
static TCPIP_EVENT_HANDLE TCPIP_event_handle;
static wifiConfig g_wifiConfig;
static WDRV_PIC32MZW_ASSOC_HANDLE drvAssocHandle;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void APP_TcpipStack_EventHandler(TCPIP_NET_HANDLE hNet, TCPIP_EVENT event, const void *fParam)
{
    const char *netName = TCPIP_STACK_NetNameGet(hNet);
    if (event & TCPIP_EV_CONN_ESTABLISHED) 
    {
        if (TCPIP_DHCP_IsEnabled(hNet) == false)
        {
            TCPIP_DHCP_Enable(hNet);
        }
    }
    else if(event & TCPIP_EV_CONN_LOST)
    {
        TCPIP_DHCP_Disable(hNet);
    }
    else
    {
        SYS_CONSOLE_PRINT("APP: %s Unknown event = %d\r\n", netName, event);
    }
}

void APP_TcpipDhcp_EventHandler(TCPIP_NET_HANDLE hNet, TCPIP_DHCP_EVENT_TYPE evType, const void* param)
{
    switch(evType)
    {
        case DHCP_EVENT_BOUND:
        {
            TCPIP_DHCP_INFO DhcpInfo;
            if(TCPIP_DHCP_InfoGet(hNet, &DhcpInfo))
            {
                SYS_CONSOLE_PRINT("APP: IP address = %d.%d.%d.%d \r\n", 
                        DhcpInfo.dhcpAddress.v[0], DhcpInfo.dhcpAddress.v[1], DhcpInfo.dhcpAddress.v[2], DhcpInfo.dhcpAddress.v[3]);
            }
            break;
        }
        case DHCP_EVENT_CONN_ESTABLISHED:
        {
            SYS_CONSOLE_MESSAGE("APP: Connection to the DHCP server established\r\n");
            break;
        }
        case DHCP_EVENT_CONN_LOST:
        {
            SYS_CONSOLE_MESSAGE("APP: Connection to the DHCP server lost\r\n");
            break;
        }
        default:
            break;
    }
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

static void APP_RSSICallback(DRV_HANDLE handle, WDRV_PIC32MZW_ASSOC_HANDLE assocHandle, int8_t rssi)
{
    SYS_CONSOLE_PRINT("APP: RSSI %d\r\n", rssi);
}

static void APP_RegDomainSetCallback(DRV_HANDLE handle, uint8_t index, uint8_t ofTotal, bool isCurrent, const WDRV_PIC32MZW_REGDOMAIN_INFO *const pRegDomInfo)
{
    if ((1 != index) || (1 != ofTotal) || (false == isCurrent) || (NULL == pRegDomInfo) || (0 == pRegDomInfo->regDomainLen))
    {
        SYS_CONSOLE_MESSAGE("APP Error: Unable to set the Regulatory domain\r\n");
    }
    else
    {
        appData.state = APP_TCPIP_WAIT_FOR_TCPIP_INIT;
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
        
        if ('\0' != *status)
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

void APP_Scan(uint8_t channel, SCAN_TYPE scanType)
{
    if ((channel > 13) && (channel < 255))
    {
        SYS_CONSOLE_MESSAGE("APP: Invalid channel \r\n");
        return;
    }
    
    SYS_CONSOLE_PRINT("APP: channel: %d scanType: %d\r\n",channel, scanType);
    
    if (255 == channel)
    {
        channel = WDRV_PIC32MZW_CID_ANY;
    }

    if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSFindFirst(appData.wdrvHandle, channel, scanType, NULL, APP_BSSFindNotifyCallback))
    {
        SYS_CONSOLE_MESSAGE("APP Error: scan fail\r\n");
    }

    return;
}

void APP_ScanOptions(uint8_t numSlots, uint8_t activeSlotTime, uint16_t passiveScanTime, uint8_t numProbes, int8_t stopOnFirst)
{
    
    if (stopOnFirst > 1)
    {
        SYS_CONSOLE_MESSAGE("APP Error: updating stop on first parameter");
        return;
    }
    
    if (0 == stopOnFirst)
    {
        if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSFindSetScanMatchMode(appData.wdrvHandle, WDRV_PIC32MZW_SCAN_MATCH_MODE_FIND_ALL))
        {
            SYS_CONSOLE_MESSAGE("APP Error: updating scan match mode\r\n");
            return;
        }
    }
    else if (1 == stopOnFirst)
    {
        if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSFindSetScanMatchMode(appData.wdrvHandle, WDRV_PIC32MZW_SCAN_MATCH_MODE_STOP_ON_FIRST))
        {
            SYS_CONSOLE_MESSAGE("APP Error: updating scan match mode\r\n");
            return;
        }
    }

    if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSFindSetScanParameters(appData.wdrvHandle, numSlots, activeSlotTime, passiveScanTime, numProbes))
    {
        SYS_CONSOLE_MESSAGE("APP Error: updating scan parameters\r\n");
        return;
    }
}

void APP_ScanSSIDList(uint8_t channel, uint8_t numSSIDs, char *ssid)
{
    int i;
    char *pssid;
    WDRV_PIC32MZW_SSID_LIST ssidList[SSID_LIST_LENGTH];
    
    if (numSSIDs > SSID_LIST_LENGTH)
    {
        SYS_CONSOLE_MESSAGE("APP: Invalid length of SSID list \r\n");
        return;
    }
    
    if ((channel > 13) && (channel < 255))
    {
        SYS_CONSOLE_MESSAGE("APP: Invalid channel \r\n");
        return;
    }
    
    if (255 == channel)
    {
        channel = WDRV_PIC32MZW_CID_ANY;
    }
    
    for (i=0; i<numSSIDs; i++)
    {
        pssid = &ssid[i*(SSID_LENGTH+1)];
        ssidList[i].ssid.length = strlen(pssid);
        
        if ((0 == ssidList[i].ssid.length) || (ssidList[i].ssid.length > SSID_LENGTH))
        {
            SYS_CONSOLE_MESSAGE("APP: SSID too long \r\n");
            return;
        }

        memcpy(ssidList[i].ssid.name, pssid, ssidList[i].ssid.length);

        ssidList[i].pNext = &ssidList[i+1];
    }

    ssidList[numSSIDs-1].pNext = NULL;
    
    if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSFindFirst(appData.wdrvHandle, channel, true, &ssidList[0], APP_BSSFindNotifyCallback))
    {
        SYS_CONSOLE_MESSAGE("APP Error: scan fail \r\n");
    }
}

void APP_RSSIGet()
{
    if(!appData.isConnected)
    {
        SYS_CONSOLE_MESSAGE("APP: Device not connected\r\n");
        return;
    }
    
    if (WDRV_PIC32MZW_STATUS_RETRY_REQUEST != WDRV_PIC32MZW_AssocRSSIGet(drvAssocHandle, NULL, APP_RSSICallback))
    {
        return;
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
    bool ret = true;
    
    WIFI_AUTH_MODE authMode = (WIFI_AUTH_MODE)app_controlData.wlanConfig.authMode;
    uint8_t *ssid           = (uint8_t *)app_controlData.wlanConfig.ssid;
    uint8_t ssidLength      = app_controlData.wlanConfig.ssidLength;
    uint8_t *password       = (uint8_t *)app_controlData.wlanConfig.password;
    uint8_t passwordLength  = strlen((char *)password);

    SYS_CONSOLE_PRINT("APP: SSID is %.*s \r\n",ssidLength, ssid);
    
    if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSCtxSetSSID(&g_wifiConfig.bssCtx, ssid, ssidLength)) 
    {
        SYS_CONSOLE_PRINT("APP: SSID set fail \r\n");
        return false;
    }
    
    if(WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSCtxSetChannel(&g_wifiConfig.bssCtx, app_controlData.wlanConfig.channel))
    {
        SYS_CONSOLE_PRINT("APP: channel set fail %d \r\n", app_controlData.wlanConfig.channel);
        return false;
    }
    
    switch (authMode) 
    {
        case OPEN:
        {
            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetOpen(&g_wifiConfig.authCtx)) 
            {
                SYS_CONSOLE_MESSAGE("APP: Unable to set Authentication\r\n");
                ret = false;
            }
            break;
        }
        case WPA2:
        {
            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiConfig.authCtx, (uint8_t *)password, passwordLength, WDRV_PIC32MZW_AUTH_TYPE_WPA2_PERSONAL)) 
            {
                SYS_CONSOLE_MESSAGE("APP: Unable to set authentication to WPA2\r\n");
                ret = false;
            }
            break;
        }
        case WPAWPA2MIXED:
        {
            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiConfig.authCtx, password, passwordLength, WDRV_PIC32MZW_AUTH_TYPE_WPAWPA2_PERSONAL)) 
            {
                SYS_CONSOLE_MESSAGE("APP: Unable to set authentication to WPAWPA2 MIXED\r\n");
                ret = false;
            }
            break;
        }
#ifdef WDRV_PIC32MZW_WPA3_SUPPORT
        case WPA3:
        {
            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiConfig.authCtx, password, passwordLength, WDRV_PIC32MZW_AUTH_TYPE_WPA3_PERSONAL)) 
            {
                SYS_CONSOLE_MESSAGE("APP: Unable to set authentication to WPA3 \r\n");
                ret = false;
            }
            break;
        }
        case WPA2WPA3MIXED:
        {
            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiConfig.authCtx, password, passwordLength, WDRV_PIC32MZW_AUTH_TYPE_WPA2WPA3_PERSONAL)) 
            {
                SYS_CONSOLE_MESSAGE("APP: Unable to set authentication to WPA2WPA3 MIXED \r\n");
                ret = false;
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
                ret = false;
            }
            
            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSharedKey(&g_wifiConfig.authCtx, true))
            {
                SYS_CONSOLE_MESSAGE("APP: Unable to Enable shared key authentication \r\n");
                ret = false;
            }
            break;
        }
        default:
        {
            ret = false;
        }
    }
    return ret;
}

static void WIFI_ConnectCallback(DRV_HANDLE handle, WDRV_PIC32MZW_ASSOC_HANDLE assocHandle, WDRV_PIC32MZW_CONN_STATE currentState) 
{
    switch (currentState) 
    {
        case WDRV_PIC32MZW_CONN_STATE_DISCONNECTED:
        {
            appData.isConnected = false;
            SYS_CONSOLE_PRINT("APP: WiFi Disconnected\r\n" );
            break;
        }
            
        case WDRV_PIC32MZW_CONN_STATE_CONNECTED:
        {
            appData.isConnected = true;
            SYS_CONSOLE_PRINT("APP: WiFi Connected\r\n" );
            break;
        }
        
        case WDRV_PIC32MZW_CONN_STATE_FAILED:
        {
            appData.isConnected = false;
            SYS_CONSOLE_PRINT("APP: WiFi Connection Failed\r\n" );
            break;
        }
        
        case WDRV_PIC32MZW_CONN_STATE_CONNECTING:
        {
            appData.isConnected = false;
            SYS_CONSOLE_PRINT("APP: WiFi Connecting\r\n" );
            break;
    	}
        
        case WDRV_PIC32MZW_CONN_STATE_ROAMED:
        {
            /*** TO DO ***/
            break;
        }
        
        case WDRV_PIC32MZW_CONN_STATE_RECONNECTED:
        {
            /*** TO DO ***/
            break;
        }
    }
    drvAssocHandle = assocHandle;
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
    WDRV_PIC32MZW_BSSCtxSetDefaults(&g_wifiConfig.bssCtx);
    WDRV_PIC32MZW_AuthCtxSetDefaults(&g_wifiConfig.authCtx);
    appData.state = APP_STATE_INIT;
    
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
    TCPIP_NET_HANDLE netH;
    int i, nNets;

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
                appData.state = APP_WIFI_RF_MAC_CONFIG;
            }
            break;
        }
        
        case APP_WIFI_RF_MAC_CONFIG:
        {
            APP_RfMacConfigStatus();
            
            if (true == appData.isRfMacConfigValid)
            {
                appData.state = APP_TCPIP_WAIT_FOR_TCPIP_INIT;
            }
            else
            {
                appData.state = APP_WIFI_ERROR;
            }
            break;
        }
        
        case APP_TCPIP_WAIT_FOR_TCPIP_INIT:
        {
            tcpipStat = TCPIP_STACK_Status(sysObj.tcpip);

            if (tcpipStat < 0) 
            {
                SYS_CONSOLE_MESSAGE( "APP: TCP/IP stack initialization failed!\r\n" );
                appData.state = APP_TCPIP_ERROR;
            }
            else if (SYS_STATUS_READY == tcpipStat)
            {
                nNets = TCPIP_STACK_NumberOfNetworksGet();
                for (i = 0; i < nNets; i++)
                {
                    netH = TCPIP_STACK_IndexToNet(i);
                    TCPIP_event_handle = TCPIP_STACK_HandlerRegister(netH, TCPIP_EV_CONN_ALL, APP_TcpipStack_EventHandler, NULL);
                    TCPIP_DHCP_HandlerRegister(netH, APP_TcpipDhcp_EventHandler, NULL);
                }
                appData.state = APP_WIFI_CONFIG;
            }
            break;
        }
        
        case APP_WIFI_CONFIG:
        {
            if(app_controlData.wlanConfigValid) 
            {
                status = APP_WIFI_Config();
                
                if (status) 
                {
                    appData.state = APP_WIFI_CONNECT;
                } 
                else 
                {
                    SYS_CONSOLE_MESSAGE("APP: Failed connecting to Wi-Fi\r\n" );
                    appData.state = APP_WIFI_ERROR;
                }
                app_controlData.wlanConfigValid = false;
            }
            else
            {
                appData.state = APP_WIFI_IDLE;   
            }
            
            break;
        }
        
        case APP_WIFI_CONNECT:
        {
            if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_BSSConnect(appData.wdrvHandle, &g_wifiConfig.bssCtx, &g_wifiConfig.authCtx, WIFI_ConnectCallback)) 
            {
                appData.state = APP_WIFI_IDLE;
            }
            break;
        }
        
        case APP_WIFI_IDLE:
        {
            if(app_controlData.wlanConfigChanged)
            {
                WDRV_PIC32MZW_STATUS status = WDRV_PIC32MZW_STATUS_OK;
                
                if(appData.isConnected == true)
                {
                    status = WDRV_PIC32MZW_BSSDisconnect(appData.wdrvHandle);
                }
                
                if(status != WDRV_PIC32MZW_STATUS_OK)
                {
                    appData.state = APP_WIFI_ERROR;
                }
                else
                {
                    appData.state = APP_WIFI_CONFIG;
                }
                
                app_controlData.wlanConfigChanged = false;
            }
            if(app_controlData.regDomChanged)
            {
                SYS_CONSOLE_MESSAGE("APP: calling WDRV_PIC32MZW_RegDomainSet from APP_WIFI_IDLE\r\n" );
                WDRV_PIC32MZW_RegDomainSet(appData.wdrvHandle, app_controlData.regDomName, APP_RegDomainSetCallback);
                app_controlData.regDomChanged = false;
            }
            break;
        }
        
        case APP_TCPIP_ERROR:
        {
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
                    appData.state = APP_TCPIP_WAIT_FOR_TCPIP_INIT;
                }
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
 End of File
 */
