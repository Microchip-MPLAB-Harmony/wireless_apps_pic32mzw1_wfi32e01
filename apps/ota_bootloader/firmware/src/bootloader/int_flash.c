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

/********************************************************************************
  USART Driver Dynamic implementation.

  Company:
    Microchip Technology Inc.

  File Name:
    int_flash.c

  Summary:
    Source code for the Internal Flash Driver interface implementation.

  Description:
    This file contains the source code for the internal flash driver interface
    implementation
 *******************************************************************************/




// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
*/

#include "definitions.h"
#include <string.h>

typedef struct 
{
    uint32_t buf[NVM_FLASH_PAGESIZE/sizeof(uint32_t)];
}INT_FLASH_DATA;

static INT_FLASH_DATA  __attribute__((coherent, aligned(32))) int_flash;
static volatile bool OpDone = false;

//---------------------------------------------------------------------------
static void INT_Flash_EventHandler(uintptr_t context)
{
    OpDone = true;
}
//---------------------------------------------------------------------------
bool INT_Flash_Initialize(void)
{
    NVM_CallbackRegister(INT_Flash_EventHandler, (uintptr_t)NULL);
    return true;
}
//---------------------------------------------------------------------------
void INT_Flash_Open(void)
{
    NVM_Initialize();
}
//---------------------------------------------------------------------------
void INT_Flash_Close(void)
{
}
//---------------------------------------------------------------------------
void INT_Flash_Deinitialize(void)
{
}
//---------------------------------------------------------------------------
bool INT_Flash_Read(uint32_t addr, uint8_t* buf, uint32_t len)
{
    addr += NVM_FLASH_START_ADDRESS;
        
    return NVM_Read((uint32_t*)buf, len, addr);
}

//---------------------------------------------------------------------------
bool INT_Flash_Write(uint32_t addr, uint8_t* buf, uint32_t len)
{
    addr += NVM_FLASH_START_ADDRESS;
    
    while (len)
    {
        memcpy(int_flash.buf, buf, NVM_FLASH_ROWSIZE);
        OpDone = false;
        if (NVM_RowWrite(int_flash.buf, addr) == false)
        {
            break;
        }
        
        while(OpDone == false);
             
        buf  += NVM_FLASH_ROWSIZE;
        addr += NVM_FLASH_ROWSIZE;
        len  -= NVM_FLASH_ROWSIZE;
    }
    
    return (len == 0)? true: false;
}

//---------------------------------------------------------------------------
bool INT_Flash_Erase(uint32_t addr, uint32_t len)
{
    addr += NVM_FLASH_START_ADDRESS;
        
    while (len)
    {
        OpDone = false;
        
        if (NVM_PageErase(addr) == false)
        {
            break;
        }
        
        while(OpDone == false);
         
        addr += NVM_FLASH_PAGESIZE;
        len  -= NVM_FLASH_PAGESIZE;
    }

   return (len == 0)? true: false;
}
    
bool INT_Flash_Busy()
{
    return false;
}

//---------------------------------------------------------------------------
uint32_t INT_Flash_Capacity(void)
{
    return NVM_FLASH_SIZE;
}
