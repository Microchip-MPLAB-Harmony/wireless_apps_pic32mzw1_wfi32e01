// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2020-2021 released Microchip Technology Inc.  All rights reserved.

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
/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    int_flash.h

  Summary:
    Internal Flash Driver Interface definition.
  
  Description:
    Internal Flash Driver Interface definition.

*******************************************************************************/



#ifndef __INC_INT_FLASH_H__
#define __INC_INT_FLASH_H__

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
*/

//#include "driver/driver_common.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif



// *****************************************************************************
/* Function:
    INT_Flash_Initialize(void);
    
  Summary:
    Initializes internal flash driver instance.
    
  Description:
    This function initializes the external flash driver instance. 
*/
bool INT_Flash_Initialize(void);

// *****************************************************************************
/* Function:
    INT_Flash_Open(void);
    
  Summary:
    Open internal flash driver.
    
  Description:
    This function initializes the external flash driver instance. 
*/
bool INT_Flash_Open(void);


// *****************************************************************************
/* Function:
    INT_Flash_Close(void);
    
  Summary:
    Close internal flash driver.
    
  Description:
    This function initializes the external flash driver instance. 
*/
void INT_Flash_Close(void);


//*************************************************************************
/* Function:
    
    DRV_CLIENT_STATUS INT_Flash_Write
    (
        uint32_t addr,
        uint8_t* buf,
        uint32_t len
    )
    
  Summary:
    Write blocks of data starting from the specified block start address.
  
  Description:
    This function schedules a non-blocking write operation for writing blocks
    of data into flash memory.
    
  Note:
    addr must be specified by the logical address (0xB000000 ~ 0xB0100000)

  Returns:
    DRV_CLIENT_STATUS_READY - Indicates that the driver completes the request.
    DRV_CLIENT_STATUS_BUSY - Indicates that the driver is start processing request.
    DRV_CLIENT_STATUS_ERROR - Indicates that the driver failed the request.
*/
bool INT_Flash_Write(uint32_t addr, uint8_t* buf, uint32_t len);

//*************************************************************************
/* Function:
    
    DRV_CLIENT_STATUS INT_Flash_Read
    (
        uint32_t addr,
        uint8_t* buf,
        uint32_t len
    )
    
  Summary:
    Read blocks of data starting from the specified block start address.
  
  Description:
    This function schedules a non-blocking read operation.

  Note:
    addr must be specified by the logical address (0xB000000 ~ 0xB0100000)
 
  Returns:
    DRV_CLIENT_STATUS_READY - Indicates that the driver completes the request.
    DRV_CLIENT_STATUS_BUSY - Indicates that the driver is start processing request.
    DRV_CLIENT_STATUS_ERROR - Indicates that the driver failed the request.
*/
bool INT_Flash_Read(uint32_t addr, uint8_t* buf, uint32_t len);


//*************************************************************************
/* Function:
    
    DRV_CLIENT_STATUS INT_Flash_Erase
    (
        uint32_t addr,
        uint32_t pages
    )
    
  Summary:
    Erase blocks of data starting from the specified block start address.
  
  Description:
    This function schedules a non-blocking erase operation.

  Note:
    addr must be specified by the logical address (0xB000000 ~ 0xB0100000)

  Returns:
    DRV_CLIENT_STATUS_READY - Indicates that the driver completes the request.
    DRV_CLIENT_STATUS_BUSY - Indicates that the driver is start processing request.
    DRV_CLIENT_STATUS_ERROR - Indicates that the driver failed the request.
*/
bool INT_Flash_Erase(uint32_t addr, uint32_t pages);


//*************************************************************************
/* Function:
    DRV_CLIENT_STATUS INT_Flash_ClientStatus(void)
    
  Summary:
    Gets the current status of the Internal Flash Driver module.
  
  Description:
    This function provides the current status of the External Flash Driver module.
  
  Returns:
    SYS_STATUS_READY - Indicates that the driver is ready and accept requests
                       for new operations
    SYS_STATUS_BUSY - Indicates that the driver is still processing request.
    DRV_CLIENT_STATUS_ERROR - Indicates that the driver failed the request.
*/
//DRV_CLIENT_STATUS INT_Flash_ClientStatus(void);
bool INT_Flash_Busy(void);

//*************************************************************************
/* Function:
    uint32_t INT_Flash_Capacity(void)
    
  Summary:
    Get the flash device capacity.
  
  Description:
    Get the flash device capacity.
  
  Returns:
    
*/
uint32_t INT_Flash_Capacity(void);

#ifdef __cplusplus
}
#endif
#endif //__INC_INT_FLASH_H__
