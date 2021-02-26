 /*******************************************************************************
 Source file for the Net Pres Encryption glue functions to work with Harmony


  Summary:


  Description:

*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/


#include "net_pres_enc_glue.h"
#include "net_pres/pres/net_pres_transportapi.h"
#include "net_pres/pres/net_pres_certstore.h"

NET_PRES_EncProviderObject net_pres_EncProviderStreamClient0 = 
{
    .fpInit =    NET_PRES_EncProviderStreamClientInit0,
    .fpDeinit =  NET_PRES_EncProviderStreamClientDeinit0,
    .fpOpen =    NET_PRES_EncProviderStreamClientOpen0,
    .fpConnect = NET_PRES_EncProviderClientConnect0,
    .fpClose =   NET_PRES_EncProviderConnectionClose0,
    .fpWrite =   NET_PRES_EncProviderWrite0,
    .fpWriteReady =   NET_PRES_EncProviderWriteReady0,
    .fpRead =    NET_PRES_EncProviderRead0,
    .fpReadReady = NET_PRES_EncProviderReadReady0,
    .fpPeek =    NET_PRES_EncProviderPeek0,
    .fpIsInited = NET_PRES_EncProviderStreamClientIsInited0,
    .fpOutputSize = NET_PRES_EncProviderOutputSize0,
    .fpMaxOutputSize = NET_PRES_EncProviderMaxOutputSize0,
};
	
	
	

		
bool NET_PRES_EncProviderStreamClientInit0(NET_PRES_TransportObject * transObject)
{
    //TODO: Enter in code to initialize the provider
    return false;
}
bool NET_PRES_EncProviderStreamClientDeinit0(void)
{
    //TODO: Enter in code to deinitialize the provider
    return false;
}
bool NET_PRES_EncProviderStreamClientOpen0(uintptr_t transHandle, void * providerData)
{
    //TODO: Enter in code to open a connection with the provider
    return false;
}
bool NET_PRES_EncProviderStreamClientIsInited0(void)
{
    //TODO: Enter in code to open a connection with the provider
    return false;
}
NET_PRES_EncSessionStatus NET_PRES_EncProviderClientConnect0(void * providerData)
{
    //TODO: Enter in code to Connect a connection through the provider
    return NET_PRES_ENC_SS_FAILED;
}
NET_PRES_EncSessionStatus NET_PRES_EncProviderConnectionClose0(void * providerData)
{
    //TODO: Enter in code to close a connection through provider
    return false;
}
int32_t NET_PRES_EncProviderWrite0(void * providerData, const uint8_t * buffer, uint16_t size)
{
    //TODO: Enter in  code to write data through the provider
    return 0;
}
uint16_t NET_PRES_EncProviderWriteReady0(void * providerData, uint16_t reqSize, uint16_t minSize)
{
    //TODO: Enter in  code to check write ready through the provider
    return 0;
}
int32_t NET_PRES_EncProviderRead0(void * providerData, uint8_t * buffer, uint16_t size)
{
    //TODO: Enter in  code to read data from the provider
    return 0;
}

int32_t NET_PRES_EncProviderReadReady0(void * providerData)
{
    //TODO: Enter in  code to read data from the provider
    return 0;
}
        
int32_t NET_PRES_EncProviderPeek0(void * providerData, uint8_t * buffer, uint16_t size)
{
    //TODO: Enter in  code to peek at data held by provider
    return 0;
}
int32_t NET_PRES_EncProviderOutputSize0(void * providerData, int32_t inSize)
{
    //TODO: Enter in  code to get the provider output size
    return 0;
}
int32_t NET_PRES_EncProviderMaxOutputSize0(void * providerData)
{
    //TODO: Enter in  code to get the provider max output size
    return 0;
}
