/*******************************************************************************
  MPLAB Harmony Sample Library Module Interface Header

  Company:
    Microchip Technology Inc.

  File Name:
    imagestore.h

  Summary:
    Define Image Store Interface

  Description:
    Define Image Store Interface
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
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
// DOM-IGNORE-END

#ifndef __INC_IMAGESTORE_H__
#define __INC_IMAGESTORE_H__

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
*/

//#include "system_config.h"
//#include "system_definitions.h"
#include "definitions.h"

#ifdef __cplusplus
extern "C" {
#endif
    
//#define IMAGESTORE_BLOCK_SIZE (FLASH_SECTOR_SIZE)
// *****************************************************************************
// *****************************************************************************
// Section: ClientInterface Routines
// *****************************************************************************
// *****************************************************************************
/*  These functions can be called by clients to this module to utilise the 
    functionality it provides.
*/


// *****************************************************************************
/* Function:
    IMAGESTORE_Initialize(void);
    
  Summary:
    Initializes Image Store Module instance.
    
  Description:
    Initializes Image Store Module instance.
*/
bool IMAGESTORE_Initialize(void);


//*************************************************************************
/* Function:
    
    DRV_CLIENT_STATUS IMAGESTORE_Open(void)
    
  Summary:
    Open IMAGESTORE interface.
  
  Description:
    Open IMAGESTORE interface.
  
  Returns:
    DRV_CLIENT_STATUS_READY
    DRV_CLIENT_STATUS_ERROR
*/
bool IMAGESTORE_Open(void);


//*************************************************************************
/* Function:
    
    DRV_CLIENT_STATUS IMAGESTORE_Close(void)
    
  Summary:
    Close IMAGESTORE interface.
  
  Description:
    Close IMAGESTORE interface.
  
*/
void IMAGESTORE_Close(void);


//*************************************************************************
/* Function:
    
    DRV_CLIENT_STATUS IMAGESTORE_Write
    (
        uint32_t slot
        uint32_t offset,
        uint8_t* buf,
        uint32_t len
    )
    
  Summary:
    Write blocks of data starting from the specified address in the slot.
  
  Description:
    This function schedules a non-blocking write operation for writing blocks
    of data into flash memory.
  
  Returns:
    DRV_CLIENT_STATUS_READY - Indicates that the driver completes the request.
    DRV_CLIENT_STATUS_BUSY - Indicates that the driver is start processing request.
    DRV_CLIENT_STATUS_ERROR - Indicates that the driver failed the request.
*/
bool IMAGESTORE_Write(uint32_t slot, uint32_t offset, void *buf, uint32_t len);


//*************************************************************************
/* Function:
    
    DRV_CLIENT_STATUS IMGSTORE_Read
    (
        uint32_t slot,
        uint32_t offset
        uint8_t* buf,
        uint32_t len
    )
    
  Summary:
    Read data from the specified address in the slot.
  
  Description:
    This function schedules a non-blocking read operation.
  
  Returns:
    DRV_CLIENT_STATUS_READY - Indicates that the driver completes the request.
    DRV_CLIENT_STATUS_BUSY - Indicates that the driver is start processing request.
    DRV_CLIENT_STATUS_ERROR - Indicates that the driver failed the request.
*/
bool IMAGESTORE_Read(uint32_t slot, uint32_t offset, void *buf, uint32_t len);

//*************************************************************************
/* Function:
    
    DRV_CLIENT_STATUS IMGSTORE_Erase
    (
        uint32_t slot,
    )
    
  Summary:
    Erase the specified slot.
  
  Description:
    This function schedules a non-blocking erase operation.
  
  Returns:
    DRV_CLIENT_STATUS_READY - Indicates that the driver completes the request.
    DRV_CLIENT_STATUS_BUSY - Indicates that the driver is start processing request.
    DRV_CLIENT_STATUS_ERROR - Indicates that the driver failed the request.
*/
bool IMAGESTORE_Erase(uint32_t slot);

//*************************************************************************
/* Function:
    
    DRV_CLIENT_STATUS IMGSTORE_EraseAll(
        
    )
    
  Summary:
    Erase the specified slot.
  
  Description:
    This function schedules a non-blocking erase operation.
  
  Returns:
    DRV_CLIENT_STATUS_READY - Indicates that the driver completes the request.
    DRV_CLIENT_STATUS_BUSY - Indicates that the driver is start processing request.
    DRV_CLIENT_STATUS_ERROR - Indicates that the driver failed the request.
*/
bool IMAGESTORE_EraseAll(void);


//*************************************************************************
/* Function:
    DRV_CLIENT_STATUS IMAGESTORE_ClientStatus(void)
    
  Summary:
    Gets the current status of the Image Store module.
  
  Description:
    This function provides the current status of the Image Store module.
  
  Returns:
    SYS_STATUS_READY - Indicates that the driver is ready and accept requests
                       for new operations
    SYS_STATUS_BUSY - Indicates that the driver is still processing request.
    DRV_CLIENT_STATUS_ERROR - Indicates that the driver failed the request.
*/
//DRV_CLIENT_STATUS IMAGESTORE_ClientStatus(void);
bool IMAGESTORE_Busy(void);

// *****************************************************************************
#define IMAGESTORE_FORCE_INLINE
#ifdef IMAGESTORE_FORCE_INLINE
#include "ota_config.h"
#include "ext_flash.h"

#define IMAGESTORE_Initialize()\
    EXT_Flash_Initialize()

#define IMAGESTORE_ClientStatus()\
    EXT_Flash_ClientStatus()

#define IMAGESTORE_Open()\
    EXT_Flash_Open()

#define IMAGESTORE_Close()\
    EXT_Flash_Close()

#define IMAGESTORE_Read(slot, offset, buf, len)\
    EXT_Flash_Read(IMAGESTORE_ADDR + ((slot) * IMAGESTORE_SLOT_SIZE) + offset, (uint8_t*)(buf), len)

#define IMAGESTORE_Write(slot, offset, buf, len)\
    EXT_Flash_Write(IMAGESTORE_ADDR + ((slot) * IMAGESTORE_SLOT_SIZE) + offset, (uint8_t*)(buf), len)

#define IMAGESTORE_Erase(slot)\
    EXT_Flash_Erase(IMAGESTORE_ADDR + ((slot) * IMAGESTORE_SLOT_SIZE), IMAGESTORE_SLOT_SIZE)

#define IMAGESTORE_EraseAll()\
    EXT_Flash_Erase(IMAGESTORE_ADDR, ((IMAGESTORE_NUM_SLOT) * IMAGESTORE_SLOT_SIZE))

#define IMAGESTORE_Capacity()\
    EXT_Flash_Capacity()

#define IMAGESTORE_Busy()\
    EXT_Flash_Busy()
#endif //IMAGESTORE_FORCE_INLINE

#ifdef __cplusplus
}
#endif
#endif //__INC_IMAGESTORE_H__
