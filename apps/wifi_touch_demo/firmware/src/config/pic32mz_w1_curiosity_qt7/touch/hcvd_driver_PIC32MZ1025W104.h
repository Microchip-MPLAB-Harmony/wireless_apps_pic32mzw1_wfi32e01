
/*******************************************************************************
  Touch Library

  Company:
    Microchip Technology Inc.

  File Name:
    hcvd_driver_PIC32MZ1025W104.h

  Summary:
    QTouch Modular Library

  Description:
    API for Acquisition module - PIC32MZW/HCVD
	
*******************************************************************************/	

/*******************************************************************************
Copyright (c) Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS  WITHOUT  WARRANTY  OF  ANY  KIND,
EITHER EXPRESS  OR  IMPLIED,  INCLUDING  WITHOUT  LIMITATION,  ANY  WARRANTY  OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A  PARTICULAR  PURPOSE.
IN NO EVENT SHALL MICROCHIP OR  ITS  LICENSORS  BE  LIABLE  OR  OBLIGATED  UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,  BREACH  OF  WARRANTY,  OR
OTHER LEGAL  EQUITABLE  THEORY  ANY  DIRECT  OR  INDIRECT  DAMAGES  OR  EXPENSES
INCLUDING BUT NOT LIMITED TO ANY  INCIDENTAL,  SPECIAL,  INDIRECT,  PUNITIVE  OR
CONSEQUENTIAL DAMAGES, LOST  PROFITS  OR  LOST  DATA,  COST  OF  PROCUREMENT  OF
SUBSTITUTE  GOODS,  TECHNOLOGY,  SERVICES,  OR  ANY  CLAIMS  BY  THIRD   PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE  THEREOF),  OR  OTHER  SIMILAR  COSTS.
*******************************************************************************/

/*============================================================================
Filename : hcvd_driver_PIC32MZ1025W104 : QTouch Modular Library
Purpose : Acquisition module - hcvd_driver_PIC32MZ1025W104/HCVD
------------------------------------------------------------------------------
------------------------------------------------------------------------------
Revision 0.1 - New build 
Revision 0.2 - removed all the timer code for interrupt workaround
Revision 1.0 - Baselining revision for release
Revision 1.1 - QTMODLIB-205: Fixed pointer casting issue
               QTMODLIB-206: Fixed CVD Timing register value assignment
               QTMODLIB-207: Assigned the correct value to CVDADC register
============================================================================*/

#ifndef __HCVD_PIC32MZ__
#define __HCVD_PIC32MZ__

/*----------------------------------------------------------------------------
include files
----------------------------------------------------------------------------*/

#include <xc.h>
#include "qtm_acq_pic32mzw_0x003e_api.h"

/*----------------------------------------------------------------------------
manifest constants
----------------------------------------------------------------------------*/

/* Module #38 */

#define QTM_MODULE_ID_PIC32MZW_ACQ        0x003eu

/* Version 1.1:  */ 

#define QTM_MODULE_VERSION                  0x11u

/* CSD Limit is 7 bits, otherwise overflow at acquisition status bit */
#define NUM_BITS_CSD                          7u
#define NODE_CSD_MAX  ((1u << NUM_BITS_CSD) - 1u)
#define CSD_MIN_OFFSET                        0u

/* Calibration target +/- signal */
#define CC_CAL_PRECISION 75u
#define TAU_CAL_PRECISION 10u

/* Proportional signal loss allowed when testing auto-tune */
#define NUM_TAU_OPTIONS   4u
#define CHARGE_2_TAU    276u
#define CHARGE_3_TAU    102u
#define CHARGE_4_TAU     37u
#define CHARGE_5_TAU     14u

/* Max limit on reasonable cal  */
#define MAX_CX_2_TAU     CHARGE_2_TAU

/* ADC Properties */
#define ADC_MAX_READ 4095u
#define ADC_MID_READ (ADC_MAX_READ >> 1u)
#define ADC_SAMPLE_HOLD_CAP 5u

#define MUTL_START_CCCAL      0x00u
#define SELFCAP_START_CCCAL   0x00u


#define QTM_ACQ_SEQUENTIAL 1u
#define QTM_ACQ_WINDOWCOMP 2u

#define NULL_POINTER			0u

#define TIMEOUT_OVERHEAD        2
#endif /* __HCVD_PIC32MZ__ */
