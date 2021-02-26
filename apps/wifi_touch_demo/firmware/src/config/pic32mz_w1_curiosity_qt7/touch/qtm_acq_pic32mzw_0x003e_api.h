
/*******************************************************************************
  Touch Library

  Company:
    Microchip Technology Inc.

  File Name:
    qtm_acq_pic32mzw_0x003e_api.h

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


#ifndef __TOUCH_API_PIC32MZW_ACQ_H__
#define __TOUCH_API_PIC32MZW_ACQ_H__

/* Include files */
#include <stdint.h>
#include <stddef.h>
#include "qtm_common_components_api.h"

/* Calibration auto-tuning options */
#define CAL_OPTION_MASK 0x03u

#define CAL_AUTO_TUNE_NONE 0u
#define CAL_AUTO_TUNE_RSEL 1u 
#define CAL_AUTO_TUNE_PRSC 2u
#define CAL_AUTO_TUNE_CSD 3u

/* Timing auto-cal target */
#define CAL_CHRG_TIME_MASK 0x30u
#define CAL_CHRG_TIME_POS  4u

#define CAL_CHRG_2TAU 0u
#define CAL_CHRG_3TAU 1u
#define CAL_CHRG_4TAU 2u
#define CAL_CHRG_5TAU 3u

#define RSEL_MAX_OPTION                 RSEL_VAL_200
#define PRSC_MAX_OPTION                 PRSC_DIV_SEL_32

/* X line bit position - CVDT line */
#define X_NONE 0u
#define X(n) ((uint32_t)(1u << (n)))

/* Y line bit position - CVDR line */
#define Y(n) ((uint32_t)(1u << (n)))  

/* Extract Analog / Digital Gain */
#define NODE_GAIN_ANA(m) (uint8_t)(((m) & 0xF0u) >> 4u)
#define NODE_GAIN_DIG(m) (uint8_t)((m) & 0x0Fu)

/* Combine Analog / Digital Gain */
#define NODE_GAIN(a,d) (uint8_t)(((a) << 4u)|(d))


typedef enum tag_filter_level_t 
{
  FILTER_LEVEL_1,
  FILTER_LEVEL_2,
  FILTER_LEVEL_4,
  FILTER_LEVEL_8,
  FILTER_LEVEL_16,
  FILTER_LEVEL_32,
  FILTER_LEVEL_64
}
filter_level_t;

/* Touch library GAIN setting */
typedef enum tag_gain_t 
{
  GAIN_1,
  GAIN_2,
  GAIN_4,
  GAIN_8,
  GAIN_16
}
gain_t;

/**
* HCVD acquisition frequency delay setting.

* inserts "n" CVD state machine clock cycles between consecutive measurements 
* e.g.  FREQ_HOP_SEL_14 setting inserts 14 CVD clock cycles.
OR
* FREQ_SEL_SPREAD automatically sweeps through all 15 delay values during the series of measurements.
*/
typedef enum tag_freq_config_sel_t 
{
  FREQ_SEL_0,
  FREQ_SEL_1,
  FREQ_SEL_2,
  FREQ_SEL_3,
  FREQ_SEL_4,
  FREQ_SEL_5,
  FREQ_SEL_6,
  FREQ_SEL_7,
  FREQ_SEL_8,
  FREQ_SEL_9,
  FREQ_SEL_10,
  FREQ_SEL_11,
  FREQ_SEL_12,
  FREQ_SEL_13,
  FREQ_SEL_14,
  FREQ_SEL_15,
  FREQ_SEL_SPREAD
}
freq_config_sel_t;

/*----------------------------------------------------------------------------
*     Structure Declarations
*----------------------------------------------------------------------------*/

/* Acquisition module PIC32MZW Sequential */
typedef struct
{
  uint32_t node_xmask;		/* Selects the X Pins for this node */
  uint32_t node_ymask;		/* Selects the Y Pins for this node */
  uint8_t node_csd;             /* Charge share delay */
  uint8_t node_rsel_prsc;       /* Bits 7:4 = Resistor, Bits 3:0  Prescaler */
  uint8_t node_gain;            /* Bits 7:4 = Analog gain, Bits 3:0 = Digital gain */
  uint8_t node_oversampling;	/* Accumulator setting  */
}qtm_acq_pic32mzw_node_config_t;

/* Node run-time data - Defined in common api as it will be used with all acquisition modules */

/* Node group configuration */
typedef struct
{
  uint16_t num_sensor_nodes;            /* Number of sensor nodes */
  uint8_t acq_sensor_type;              /* Self or mutual sensors */
  uint8_t calib_option_select;          /* Hardware tuning: XX | TT 3/4/5 Tau | X | XX None/RSEL/PRSC/CSD */
  uint8_t freq_option_select;           /* SDS or ASDV setting */ 
  uint8_t cvd_interrupt_priority;       /* Runtime priority of CVD interrupt */  
} qtm_acq_node_group_config_t;

/* Container structure for sensor group */
typedef struct
{
  qtm_acq_node_group_config_t (*qtm_acq_node_group_config);
  qtm_acq_pic32mzw_node_config_t (*qtm_acq_node_config);
  qtm_acq_node_data_t (*qtm_acq_node_data);
} qtm_acquisition_control_t;

typedef struct 
{
  qtm_acquisition_control_t* qtm_acq_control;
  uint16_t auto_scan_node_number;
  uint8_t auto_scan_node_threshold;
  uint8_t auto_scan_trigger;  
}qtm_auto_scan_config_t;

/*----------------------------------------------------------------------------
* prototypes
*----------------------------------------------------------------------------*/

/* Library prototypes */
/*============================================================================
touch_ret_t qtm_acquisition_process(void)
------------------------------------------------------------------------------
Purpose: Signal capture and processing
Input  : (Measured signals, config)
Output : touch_ret_t
Notes  : Called by application after 'touch_measure_complete_callback'
============================================================================*/
touch_ret_t qtm_acquisition_process(void);

/*============================================================================
touch_ret_t qtm_cvd_init_acquisition_module(qtm_acquisition_control_t* qtm_acq_control_ptr);
------------------------------------------------------------------------------
Purpose: Initialize the hcvd & Assign pins
Input  : pointer to acquisition set
Output : touch_ret_t: TOUCH_SUCCESS or INVALID_PARAM
Notes  : qtm_cvd_init_acquisition_module module must be called ONLY once with a pointer to each config set
============================================================================*/
touch_ret_t qtm_cvd_init_acquisition_module(qtm_acquisition_control_t* qtm_acq_control_ptr);

/*============================================================================
touch_ret_t qtm_cvd_qtlib_assign_signal_memory(uint32_t* qtm_signal_raw_data_ptr);
------------------------------------------------------------------------------
Purpose: Assign raw signals pointer to array defined in application code
Input  : pointer to raw data array
Output : touch_ret_t: TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t qtm_cvd_qtlib_assign_signal_memory(uint32_t* qtm_signal_raw_data_ptr);

/* Scan configuration */
/*============================================================================
touch_ret_t qtm_enable_sensor_node(qtm_acquisition_control_t* qtm_acq_control_ptr, uint16_t qtm_which_node_number);
------------------------------------------------------------------------------
Purpose:  Enables a sensor node for measurement
Input  :  Node configurations pointer, node (channel) number
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_enable_sensor_node(qtm_acquisition_control_t* qtm_acq_control_ptr, uint16_t qtm_which_node_number);

/*============================================================================
touch_ret_t qtm_calibrate_sensor_node(qtm_acquisition_control_t* qtm_acq_control_l_ptr, uint16_t which_node_number)
------------------------------------------------------------------------------
Purpose:  Marks a sensor node for calibration
Input  :  Node configurations pointer, node (channel) number
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_calibrate_sensor_node(qtm_acquisition_control_t* qtm_acq_control_ptr, uint16_t qtm_which_node_number);

/* Measurement start - sequence or windowcomp */

/*============================================================================
touch_ret_t qtm_cvd_start_measurement_seq(qtm_acquisition_control_t* qtm_acq_control_pointer, void (*measure_complete_callback) (void));
------------------------------------------------------------------------------
Purpose:  Loads touch configurations for first channel and start,  
Input  :  Node configurations pointer, measure complete callback pointer
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_cvd_start_measurement_seq(qtm_acquisition_control_t* qtm_acq_control_pointer, void (*measure_complete_callback) (void));

/*============================================================================
void qtm_cvd_set_timer_period_function(void (*timer_period_function_ptr) (uint16_t period))
------------------------------------------------------------------------------
Purpose:  set the timer period function 
Input  :  function pointer of the timer period function
Output :  
Notes  :
============================================================================*/
void qtm_cvd_set_timer_period_function(void (*timer_period_function_ptr) (uint16_t period));

/*============================================================================
void qtm_cvd_set_timer_start_function(void (*timer_start) (void))
------------------------------------------------------------------------------
Purpose:  set the timer start function 
Input  :  function pointer of the timer start function
Output :  
Notes  :
============================================================================*/
void qtm_cvd_set_timer_start_function(void (*timer_start) (void));

/*============================================================================
void qtm_cvd_set_timer_stop_function(void (*timer_stop) (void))
------------------------------------------------------------------------------
Purpose:  set the timer start function 
Input  :  function pointer of the timer start function
Output :  
Notes  :
============================================================================*/
void qtm_cvd_set_timer_stop_function(void (*timer_stop) (void));

/*============================================================================
void qtm_cvd_de_init(void)
------------------------------------------------------------------------------
Purpose: Clear hcvd Pin registers, set TOUCH_STATE_NULL
Input  : none
Output : none
Notes  : none
============================================================================*/
void qtm_cvd_de_init(void);

/*============================================================================
uint16_t qtm_pic32mzw_acq_module_get_id(void);
------------------------------------------------------------------------------
Purpose: Returns the module ID
Input  : none
Output : Module ID
Notes  : none
============================================================================*/
uint16_t qtm_pic32mzw_acq_module_get_id(void);

/*============================================================================
uint8_t qtm_pic32mzw_acq_module_get_version(void);
------------------------------------------------------------------------------
Purpose: Returns the module Firmware version
Input  : none
Output : Module ID - Upper nibble major / Lower nibble minor
Notes  : none
============================================================================*/
uint8_t qtm_pic32mzw_acq_module_get_version(void);

/*============================================================================
void qtm_cvd_last_measure_is_complete(void)
------------------------------------------------------------------------------
Purpose:  check if the CVD measurement is compelete or not.
Input    :  none
Output  :  uint8_t: 0-> incomplete, 1-> complete
Notes    :  none
============================================================================*/
uint8_t qtm_cvd_last_measure_is_complete(void);

/*============================================================================
void qtm_cvd_clear_interrupt(void)
------------------------------------------------------------------------------
Purpose:  Clears the eoc/wcomp interrupt bits
Input    :  none
Output  :  none
Notes    :  none
============================================================================*/
void qtm_cvd_clear_interrupt(void);

/*============================================================================
void qtm_pic32_cvd_handler_eoc(void)
------------------------------------------------------------------------------
Purpose:  Captures  the  measurement,  starts  the  next  or  End  Of  Sequence  handler
Input    :  none
Output  :  none
Notes    :  none
============================================================================*/
void qtm_pic32_cvd_handler_eoc(void);

#endif    /* __TOUCH_API_PIC32MZW_ACQ_H__ */
