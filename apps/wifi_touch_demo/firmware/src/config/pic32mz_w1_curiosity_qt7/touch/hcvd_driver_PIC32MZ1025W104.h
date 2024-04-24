
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
Copyright (C) [2023], Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
************************************************************************************/

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

#ifndef HCVD_DRIVER_PIC32MZ1025W104
#define HCVD_DRIVER_PIC32MZ1025W104

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

/* Version 1.2:  */ 

#define QTM_MODULE_VERSION                  0x12u

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

#define TIMEOUT_OVERHEAD        2
#endif /* HCVD_DRIVER_PIC32MZ1025W104 */
