/*******************************************************************************
 System Interrupts File

  Company:
    Microchip Technology Inc.

  File Name:
    interrupt.c

  Summary:
    Interrupt vectors mapping

  Description:
    This file maps all the interrupt vectors to their corresponding
    implementations. If a particular module interrupt is used, then its ISR
    definition can be found in corresponding PLIB source file. If a module
    interrupt is not used, then its ISR implementation is mapped to dummy
    handler.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "configuration.h"
#include "interrupts.h"
#include "definitions.h"



// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************


/* All the handlers are defined here.  Each will call its PLIB-specific function. */
// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector declarations
// *****************************************************************************
// *****************************************************************************
void EXTERNAL_0_Handler (void);
void TIMER_1_Handler (void);
void TIMER_3_Handler (void);
void FLASH_CONTROL_Handler (void);
void RTCC_Handler (void);
void UART1_FAULT_Handler (void);
void UART1_RX_Handler (void);
void UART1_TX_Handler (void);
void I2C1_BUS_Handler (void);
void I2C1_MASTER_Handler (void);
void CHANGE_NOTICE_A_Handler (void);
void RFSMC_Handler (void);
void RFMAC_Handler (void);
void RFTM0_Handler (void);
void CRYPTO1_Handler (void);
void CRYPTO1_FAULT_Handler (void);


// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector definitions
// *****************************************************************************
// *****************************************************************************
void __ISR(_EXTERNAL_0_VECTOR, ipl2SRS) EXTERNAL_0_Handler (void)
{
    EXTERNAL_0_InterruptHandler();
}

void __ISR(_TIMER_1_VECTOR, ipl1SRS) TIMER_1_Handler (void)
{
    TIMER_1_InterruptHandler();
}

void __ISR(_TIMER_3_VECTOR, ipl1SRS) TIMER_3_Handler (void)
{
    TIMER_3_InterruptHandler();
}

void __ISR(_FLASH_CONTROL_VECTOR, ipl1SRS) FLASH_CONTROL_Handler (void)
{
    NVM_InterruptHandler();
}

void __ISR(_RTCC_VECTOR, ipl1SRS) RTCC_Handler (void)
{
    RTCC_InterruptHandler();
}

void __ISR(_UART1_FAULT_VECTOR, ipl1SRS) UART1_FAULT_Handler (void)
{
    UART1_FAULT_InterruptHandler();
}

void __ISR(_UART1_RX_VECTOR, ipl1SRS) UART1_RX_Handler (void)
{
    UART1_RX_InterruptHandler();
}

void __ISR(_UART1_TX_VECTOR, ipl1SRS) UART1_TX_Handler (void)
{
    UART1_TX_InterruptHandler();
}

void __ISR(_I2C1_BUS_VECTOR, ipl1SRS) I2C1_BUS_Handler (void)
{
    I2C1_BUS_InterruptHandler();
}

void __ISR(_I2C1_MASTER_VECTOR, ipl1SRS) I2C1_MASTER_Handler (void)
{
    I2C1_MASTER_InterruptHandler();
}

void __ISR(_CHANGE_NOTICE_A_VECTOR, ipl1SRS) CHANGE_NOTICE_A_Handler (void)
{
    CHANGE_NOTICE_A_InterruptHandler();
}

void __ISR(_RFSMC_VECTOR, ipl1SRS) RFSMC_Handler (void)
{
    WDRV_PIC32MZW_TasksRFSMCISR();
}

void __ISR(_RFMAC_VECTOR, ipl1SRS) RFMAC_Handler (void)
{
    WDRV_PIC32MZW_TasksRFMACISR();
}

void __ISR(_RFTM0_VECTOR, ipl1SRS) RFTM0_Handler (void)
{
    WDRV_PIC32MZW_TasksRFTimer0ISR();
}

void __ISR(_CRYPTO1_VECTOR, ipl1SRS) CRYPTO1_Handler (void)
{
    DRV_BA414E_InterruptHandler();
}

void __ISR(_CRYPTO1_FAULT_VECTOR, ipl1SRS) CRYPTO1_FAULT_Handler (void)
{
    DRV_BA414E_ErrorInterruptHandler();
}




/*******************************************************************************
 End of File
*/
