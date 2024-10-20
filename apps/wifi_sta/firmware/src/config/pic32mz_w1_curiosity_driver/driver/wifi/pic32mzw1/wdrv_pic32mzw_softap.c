/*******************************************************************************
  PIC32MZW Driver Soft-AP Implementation

  File Name:
    wdrv_pic32mzw_softap.c

  Summary:
    PIC32MZW wireless driver Soft-AP implementation.

  Description:
    Provides an interface to create and manage a Soft-AP.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2020-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <string.h>

#include "wdrv_pic32mzw.h"
#include "wdrv_pic32mzw_common.h"
#include "wdrv_pic32mzw_softap.h"
#include "wdrv_pic32mzw_cfg.h"

// *****************************************************************************
// *****************************************************************************
// Section: PIC32MZW Driver Soft-AP Implementation
// *****************************************************************************
// *****************************************************************************

DRV_PIC32MZW_11I_MASK DRV_PIC32MZW_Get11iMask
(
    WDRV_PIC32MZW_AUTH_TYPE authType,
    WDRV_PIC32MZW_AUTH_MOD_MASK authMod
);

//*******************************************************************************
/*
  Function:
    WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_APStart
    (
        DRV_HANDLE handle,
        const WDRV_PIC32MZW_BSS_CONTEXT *const pBSSCtx,
        const WDRV_PIC32MZW_AUTH_CONTEXT *const pAuthCtx,
        const WDRV_PIC32MZW_BSSCON_NOTIFY_CALLBACK pfNotifyCallback
    )

  Summary:
    Starts an instance of Soft-AP.

  Description:
    Using the defined BSS and authentication contexts with an optional HTTP
      provisioning context (socket mode only) this function creates and starts
      a Soft-AP instance.

  Remarks:
    See wdrv_pic32mzw_softap.h for usage information.

*/

WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_APStart
(
    DRV_HANDLE handle,
    const WDRV_PIC32MZW_BSS_CONTEXT *const pBSSCtx,
    const WDRV_PIC32MZW_AUTH_CONTEXT *const pAuthCtx,
    const WDRV_PIC32MZW_BSSCON_NOTIFY_CALLBACK pfNotifyCallback
)
{
    WDRV_PIC32MZW_DCPT *pDcpt = (WDRV_PIC32MZW_DCPT *)handle;
    DRV_PIC32MZW_WIDCTX wids;
    uint8_t i, channel;
    DRV_PIC32MZW_11I_MASK dot11iInfo = 0;
    OSAL_CRITSECT_DATA_TYPE critSect;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_PIC32MZW_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_PIC32MZW_STATUS_NOT_OPEN;
    }

    if ((NULL == pBSSCtx) && (NULL == pAuthCtx))
    {
        /* Allow callback to be set/changed, but only if not trying to change
         BSS/Auth settings. */
        pDcpt->pCtrl->pfConnectNotifyCB = pfNotifyCallback;
        return WDRV_PIC32MZW_STATUS_OK;
    }

    /* Ensure RF and MAC is configured */
    if (WDRV_PIC32MZW_RF_MAC_MIN_REQ_CONFIG != (pDcpt->pCtrl->rfMacConfigStatus & WDRV_PIC32MZW_RF_MAC_MIN_REQ_CONFIG))
    {
        return WDRV_PIC32MZW_STATUS_RF_MAC_CONFIG_NOT_VALID;
    }

    /* Ensure the BSS context is valid. */
    if (false == WDRV_PIC32MZW_BSSCtxIsValid(pBSSCtx, false))
    {
        return WDRV_PIC32MZW_STATUS_INVALID_CONTEXT;
    }

    /* NULL authentication context is OK - no encryption. */
    if (NULL != pAuthCtx)
    {
        /* Ensure the authentication context is valid. */
        if (false == WDRV_PIC32MZW_AuthCtxIsValid(pAuthCtx))
        {
            return WDRV_PIC32MZW_STATUS_INVALID_CONTEXT;
        }

        /* Convert authentication type to an 11i bitmap. */
        dot11iInfo = DRV_PIC32MZW_Get11iMask(
                pAuthCtx->authType,
                pAuthCtx->authMod & ~WDRV_PIC32MZW_AUTH_MOD_STA_TD);
    }

    channel = pBSSCtx->channel;

    if(!((1<<(channel-1)) & pDcpt->pCtrl->regulatoryChannelMask24))
    {
        return WDRV_PIC32MZW_STATUS_INVALID_ARG;
    }

    /* Ensure PIC32MZW is not already configured for Soft-AP. */
    if (false != pDcpt->pCtrl->isAP)
    {
        return WDRV_PIC32MZW_STATUS_REQUEST_ERROR;
    }

    /* Ensure PIC32MZW is not connected or attempting to connect. */
    if (WDRV_PIC32MZW_CONN_STATE_DISCONNECTED != pDcpt->pCtrl->connectedState)
    {
        return WDRV_PIC32MZW_STATUS_REQUEST_ERROR;
    }

    /* Indicate that the dot11i settings are intended for AP mode. */
    dot11iInfo |= DRV_PIC32MZW_AP;

    /* Allocate memory for the WIDs. */
    DRV_PIC32MZW_MultiWIDInit(&wids, 512);

    /* Switch to AP mode (1). */
    DRV_PIC32MZW_MultiWIDAddValue(&wids, DRV_WIFI_WID_SWITCH_MODE, 1);

    /* Enable or disable broadcast SSID based on cloaked flag. */
    DRV_PIC32MZW_MultiWIDAddValue(&wids, DRV_WIFI_WID_BCAST_SSID, pBSSCtx->cloaked ? 1 : 0);

    /* Set transmit rate to auto-rate. */
    DRV_PIC32MZW_MultiWIDAddValue(&wids, DRV_WIFI_WID_CURRENT_TX_RATE, 0);

    /* Set SSID. */
    DRV_PIC32MZW_MultiWIDAddData(&wids, DRV_WIFI_WID_SSID, pBSSCtx->ssid.name, pBSSCtx->ssid.length);

    /* Set channel. */
    DRV_PIC32MZW_MultiWIDAddValue(&wids, DRV_WIFI_WID_USER_PREF_CHANNEL, channel);

    /* Set 11i info as derived above. */
    DRV_PIC32MZW_MultiWIDAddValue(&wids, DRV_WIFI_WID_11I_SETTINGS, (int)dot11iInfo);

    /* Set credentials for whichever authentication types are enabled. */
    if (
            (dot11iInfo & DRV_PIC32MZW_PRIVACY)
        &&  !(dot11iInfo & DRV_PIC32MZW_RSNA_MASK)
    )
    {
        /* Set WEP credentials. */
        DRV_PIC32MZW_MultiWIDAddValue(&wids, DRV_WIFI_WID_KEY_ID, pAuthCtx->authInfo.WEP.idx-1);
        DRV_PIC32MZW_MultiWIDAddData(&wids, DRV_WIFI_WID_WEP_KEY_VALUE, pAuthCtx->authInfo.WEP.key, pAuthCtx->authInfo.WEP.size);
    }

    if (dot11iInfo & DRV_PIC32MZW_11I_PSK)
    {
        /* Set PSK credentials. */
        DRV_PIC32MZW_MultiWIDAddData(&wids, DRV_WIFI_WID_11I_PSK,
                pAuthCtx->authInfo.personal.password,
                pAuthCtx->authInfo.personal.size);
    }
    if (dot11iInfo & DRV_PIC32MZW_11I_SAE)
    {
        /* Set SAE credentials. */
        DRV_PIC32MZW_MultiWIDAddData(&wids, DRV_WIFI_WID_RSNA_PASSWORD,
                pAuthCtx->authInfo.personal.password,
                pAuthCtx->authInfo.personal.size);
    }

    /* Set 11g compatibility mode 1 (2). */
    DRV_PIC32MZW_MultiWIDAddValue(&wids, DRV_WIFI_WID_11G_OPERATING_MODE, 2);

    /* Set Ack policy: Normal Ack (0). */
    DRV_PIC32MZW_MultiWIDAddValue(&wids, DRV_WIFI_WID_ACK_POLICY, 0);

    /* Set 11n enabled (1). */
    DRV_PIC32MZW_MultiWIDAddValue(&wids, DRV_WIFI_WID_11N_ENABLE, 1);

    critSect = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    /* Write the WIDs. */
    if (false == DRV_PIC32MZW_MultiWIDWrite(&wids))
    {
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

        DRV_PIC32MZW_MultiWIDDestroy(&wids);

        return WDRV_PIC32MZW_STATUS_CONNECT_FAIL;
    }

    pDcpt->pCtrl->pfConnectNotifyCB = pfNotifyCallback;
    pDcpt->pCtrl->isAP              = true;

    for (i=0; i<WDRV_PIC32MZW_NUM_ASSOCS; i++)
    {
        pDcpt->pCtrl->assocInfoAP[i].handle = DRV_HANDLE_INVALID;
        pDcpt->pCtrl->assocInfoAP[i].peerAddress.valid = false;
        pDcpt->pCtrl->assocInfoAP[i].assocID = -1;
        pDcpt->pCtrl->assocInfoAP[i].transitionDisable =
                            (dot11iInfo & DRV_PIC32MZW_11I_TD) ? true : false;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

    return WDRV_PIC32MZW_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_APStop(DRV_HANDLE handle)

  Summary:
    Stops an instance of Soft-AP.

  Description:
    Stops an instance of Soft-AP.

  Remarks:
    See wdrv_pic32mzw_softap.h for usage information.

*/

WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_APStop(DRV_HANDLE handle)
{
    WDRV_PIC32MZW_DCPT *pDcpt = (WDRV_PIC32MZW_DCPT *)handle;
    DRV_PIC32MZW_WIDCTX wids;
    OSAL_CRITSECT_DATA_TYPE critSect;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_PIC32MZW_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_PIC32MZW_STATUS_NOT_OPEN;
    }

    /* Ensure operation mode is really Soft-AP. */
    if (false == pDcpt->pCtrl->isAP)
    {
        return WDRV_PIC32MZW_STATUS_REQUEST_ERROR;
    }

    DRV_PIC32MZW_MultiWIDInit(&wids, 16);

    /* Switch to STA mode (0) */
    DRV_PIC32MZW_MultiWIDAddValue(&wids, DRV_WIFI_WID_SWITCH_MODE, 0);

    /* Clear the SSID value */
    DRV_PIC32MZW_MultiWIDAddString(&wids, DRV_WIFI_WID_SSID, "\0");

    critSect = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    /* Write the wids. */
    if (false == DRV_PIC32MZW_MultiWIDWrite(&wids))
    {
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

        DRV_PIC32MZW_MultiWIDDestroy(&wids);

        return WDRV_PIC32MZW_STATUS_REQUEST_ERROR;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

    return WDRV_PIC32MZW_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_APRekeyIntervalSet
    (
        DRV_HANDLE handle,
        const uint32_t interval
    )

  Summary:
    Configures the group re-key interval used when operating in Soft-AP mode

  Description:
    The re-key interval specifies how much time must elapse before a group re-key
    is initiated with connected stations.

  Remarks:
    See wdrv_pic32mzw_softap.h for usage information.

*/

WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_APRekeyIntervalSet(
    DRV_HANDLE handle,
    const uint32_t interval
)
{
    WDRV_PIC32MZW_DCPT *const pDcpt = (WDRV_PIC32MZW_DCPT *const)handle;
    DRV_PIC32MZW_WIDCTX wids;
    OSAL_CRITSECT_DATA_TYPE critSect;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt))
    {
        return WDRV_PIC32MZW_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if ((false == pDcpt->isOpen) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_PIC32MZW_STATUS_NOT_OPEN;
    }

    /* Ensure driver handle is valid */
    if (DRV_HANDLE_INVALID == pDcpt->pCtrl->handle)
    {
        return WDRV_PIC32MZW_STATUS_INVALID_ARG;
    }

    /*Sanity check the interval */
    if (interval < DRV_PIC32MZW_AP_REKEY_MIN_PERIOD)
    {
        return WDRV_PIC32MZW_STATUS_INVALID_ARG;
    }

    /* Allocate memory for the WIDs. */
    DRV_PIC32MZW_MultiWIDInit(&wids, 64);

    /* Configfure group rekey period */
    DRV_PIC32MZW_MultiWIDAddValue(&wids, DRV_WIFI_WID_REKEY_PERIOD, interval);

    critSect = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    /* Write the wids. */
    if (false == DRV_PIC32MZW_MultiWIDWrite(&wids))
    {
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

        DRV_PIC32MZW_MultiWIDDestroy(&wids);

        return WDRV_PIC32MZW_STATUS_REQUEST_ERROR;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, critSect);

    return WDRV_PIC32MZW_STATUS_OK;
}
