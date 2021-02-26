
/*******************************************************************************
  Touch Library

  Company:
    Microchip Technology Inc.

  File Name:
    hcvd_driver_PIC32MZ1025W104.c

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
============================================================================*/

#include "hcvd_driver_PIC32MZ1025W104.h"
#include "definitions.h" 
#include <math.h>

#if (__XC32_VERSION <2500)
/* this is a workaround fix for compiler */
asm(".section .__vector_offset_BF8107E0_data,data,keep,address(0xBF8107E0)");
asm(".word __vector_offset_168");
#endif

/* Library state */
static const uint8_t CVD_TX_PINS[] = {
    GPIO_PIN_RB7,
    GPIO_PIN_RB6,
    GPIO_PIN_RB5,
    GPIO_PIN_RB4,
    GPIO_PIN_RB3,
    GPIO_PIN_RB2,
    GPIO_PIN_RB1,
    GPIO_PIN_RC9,
    GPIO_PIN_RC10,
    GPIO_PIN_RC11,
    GPIO_PIN_RC12,
    GPIO_PIN_RC13,
    GPIO_PIN_RC14,
    GPIO_PIN_RC15,
    GPIO_PIN_RK12,
    GPIO_PIN_RK13,
    GPIO_PIN_RK14,
    GPIO_PIN_RA0,
    GPIO_PIN_RC0,
    GPIO_PIN_RC1,
    GPIO_PIN_RC2,
    GPIO_PIN_RC3,
    GPIO_PIN_RC4,
    GPIO_PIN_RC5
};

static const uint8_t CVD_RX_PINS[] = {
    GPIO_PIN_NONE,
    GPIO_PIN_RB1,
    GPIO_PIN_RB2,
    GPIO_PIN_RB3,
    GPIO_PIN_RB4,
    GPIO_PIN_RB5,
    GPIO_PIN_RB6,
    GPIO_PIN_RB7,
    GPIO_PIN_RB8,
    GPIO_PIN_RB9,
    GPIO_PIN_RB10,
    GPIO_PIN_RB13,
    GPIO_PIN_RB14,
    GPIO_PIN_RA15,
    GPIO_PIN_RA14,
    GPIO_PIN_RA13,
    GPIO_PIN_RA12,
    GPIO_PIN_RA10,
    GPIO_PIN_RK3,
    GPIO_PIN_RK2
};

static const uint8_t OVERSAMPLING_LUT[7] = {
    0, 1, 3, 7, 15, 31, 63
};

static uint8_t* CVD_Y_REG[16] = {
    (uint8_t*) ((uint8_t*) & CVDRX0),
    (uint8_t*) ((uint8_t*) & CVDRX0 + 1),
    (uint8_t*) ((uint8_t*) & CVDRX0 + 2),
    (uint8_t*) ((uint8_t*) & CVDRX0 + 3),
    (uint8_t*) ((uint8_t*) & CVDRX1),
    (uint8_t*) ((uint8_t*) & CVDRX1 + 1),
    (uint8_t*) ((uint8_t*) & CVDRX1 + 2),
    (uint8_t*) ((uint8_t*) & CVDRX1 + 3),
    (uint8_t*) ((uint8_t*) & CVDRX2),
    (uint8_t*) ((uint8_t*) & CVDRX2 + 1),
    (uint8_t*) ((uint8_t*) & CVDRX2 + 2),
    (uint8_t*) ((uint8_t*) & CVDRX2 + 3),
    (uint8_t*) ((uint8_t*) & CVDRX3),
    (uint8_t*) ((uint8_t*) & CVDRX3 + 1),
    (uint8_t*) ((uint8_t*) & CVDRX3 + 2),
    (uint8_t*) ((uint8_t*) & CVDRX3 + 3),
};


static uint8_t* CVD_X_REG[16] = {
    (uint8_t*) ((uint8_t*) & CVDTX0),
    (uint8_t*) ((uint8_t*) & CVDTX0 + 1),
    (uint8_t*) ((uint8_t*) & CVDTX0 + 2),
    (uint8_t*) ((uint8_t*) & CVDTX0 + 3),
    (uint8_t*) ((uint8_t*) & CVDTX1),
    (uint8_t*) ((uint8_t*) & CVDTX1 + 1),
    (uint8_t*) ((uint8_t*) & CVDTX1 + 2),
    (uint8_t*) ((uint8_t*) & CVDTX1 + 3),
    (uint8_t*) ((uint8_t*) & CVDTX2),
    (uint8_t*) ((uint8_t*) & CVDTX2 + 1),
    (uint8_t*) ((uint8_t*) & CVDTX2 + 2),
    (uint8_t*) ((uint8_t*) & CVDTX2 + 3),
    (uint8_t*) ((uint8_t*) & CVDTX3),
    (uint8_t*) ((uint8_t*) & CVDTX3 + 1),
    (uint8_t*) ((uint8_t*) & CVDTX3 + 2),
    (uint8_t*) ((uint8_t*) & CVDTX3 + 3),
};

/* Container for tau constant scale factors */
static const uint16_t tau_calc[NUM_TAU_OPTIONS] = {CHARGE_2_TAU, CHARGE_3_TAU, CHARGE_4_TAU, CHARGE_5_TAU};

/* Measurement channel */
static uint16_t current_measure_channel = 0u;

/* Sample delay for frequency hopping */
static uint8_t sample_delay;

/* Library state */
static uint8_t touch_seq_lib_state = TOUCH_STATE_NULL;

/* Acquisition settings pointer */
qtm_acquisition_control_t* qtm_acquisition_control_working_set_ptr;

/* Raw data pointer */
uint32_t(*qtm_raw_data_measurements_ptr);

/* Track between sequential and event system measurements */
uint8_t qtm_which_mode_current = QTM_ACQ_SEQUENTIAL;

/* Prototypes*/
/*============================================================================
static void (*cvd_seq_measure_complete_pointer) (void);
------------------------------------------------------------------------------
Purpose: Measure complete callback pointer
Input  : Pointer to user callback function
Output : none
Notes  : none
============================================================================*/
static void (*cvd_seq_measure_complete_pointer) (void);

/*============================================================================
static void cvd_config_io_pins(uint32_t cvd_lines,uint8_t direction, const uint8_t* pin_table)
------------------------------------------------------------------------------
Purpose: Checks which X and Y pins are used, sets to output low
Input  : Bitfield: x_lines | y_lines, where 1 = Used by CVD, 
         direction: specify whether the pin is input or output, 1 input , 0 for output
         and pin table specific the index for the i/o pin for the port control
Output : none
Notes  : none
============================================================================*/
static void cvd_config_io_pins(uint32_t cvd_lines, uint8_t direction, const uint8_t pin_table[]);

/*============================================================================
static uint8_t select_next_channel_to_measure(void);
------------------------------------------------------------------------------
Purpose: Select the next channel for measuring
Input  : none
Output : current_measure_channel set to next channel number
Notes  : none
============================================================================*/
static uint8_t select_next_channel_to_measure(void);

/*============================================================================
static void qtm_measure_node(uint16_t channel_number);
------------------------------------------------------------------------------
Purpose: Configures the CVD for the selected channel + starts measurement
Input  : channel number
Output : none
Notes  : none
============================================================================*/
static void qtm_measure_node(uint16_t channel_number);


/*============================================================================
static uint8_t check_cvd_busy(void)
------------------------------------------------------------------------------
Purpose: Checks is a measurement is in progress
Input  : none
Output : 0 = Not busy / 1 = Busy
Notes  : none
============================================================================*/
static uint8_t check_cvd_busy(void);

/*============================================================================
static uint8_t charge_share_test(uint16_t which_sensor_node, uint16_t new_measured_signal);
------------------------------------------------------------------------------
Purpose: Checked whether the signal taken with reduced timing shows a drop in sensor apparent capacitance
Input  : (Calibrated signal + comp cap), new signal to check for undercharging
Output : 0 = OK, 1 = Undercharged
Notes  : Hard coded limit on % reduction in apparent Cx
============================================================================*/
static uint8_t charge_share_test(uint16_t which_sensor_node, uint16_t new_measured_signal);

/*============================================================================
static uint32_t get_cvd_result(void)
------------------------------------------------------------------------------
Purpose: Captures CVD RESULT register
Input  : -
Output : RESULT
Notes  : none
============================================================================*/
static uint32_t get_cvd_result(void);

/*============================================================================
static uint8_t qtm_load_group_config(void)
------------------------------------------------------------------------------
Purpose: Captures CVD RESULT register
Input  : -
Output : RESULT
Notes  : none
============================================================================*/
static uint8_t qtm_load_group_config(qtm_acquisition_control_t* qtm_acq_control_pointer);

/*============================================================================
void qtm_cvd_clear_interrupt(void)
------------------------------------------------------------------------------
Purpose:  Clears the eoc/wcomp interrupt bits
Input  : none
Output : none
Notes  : none
============================================================================*/
void qtm_cvd_clear_interrupt(void);

/*============================================================================
static uint8_t check_cvd_busy(void)
------------------------------------------------------------------------------
Purpose: Checks is a measurement is in progress
Input  : none
Output : 0 = Not busy / 1 = Busy
Notes  : none
============================================================================*/
static uint8_t check_cvd_busy(void)
{
    uint8_t return_busy_state = 0u;

    if (0u != CVDSD0C3bits.SD0EN)
    {
        return_busy_state = 1u;
    }
    return return_busy_state;
}

/*============================================================================
static uint8_t charge_share_test(uint16_t which_sensor_node, uint16_t new_measured_signal);
------------------------------------------------------------------------------
Purpose: Checked whether the signal taken with reduced timing shows a drop in sensor apparent capacitance
Input  : Calibrated signal, new signal to check for undercharging
Output : 0 = OK, 1 = Undercharged
Notes  : Hard coded limit on % reduction in apparent Cx
============================================================================*/
static uint8_t charge_share_test(uint16_t which_sensor_node, uint16_t new_measured_signal)
{
    uint16_t signal_diff;
    uint8_t cal_tau_x_target;
    uint8_t need_further_cal;

    if (qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[which_sensor_node].node_acq_signals == new_measured_signal)
    {
        /* Signal unchanged - no need further checks */
        need_further_cal = 0u;
    }
    else
    {
        /* Signal difference */
        if (qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[which_sensor_node].node_acq_signals < new_measured_signal)
        {
            signal_diff = new_measured_signal - qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[which_sensor_node].node_acq_signals;
        }
        else
        {
            signal_diff = qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[which_sensor_node].node_acq_signals - new_measured_signal;
        }

        /* Signal_diff represents 2 x Delta-C */
        signal_diff = (uint16_t) (signal_diff >> 1u);

        if (signal_diff >= MAX_CX_2_TAU)
        {
            /* Signal miss exceeds 2 Tau at max comp cap -> Definitely NG, no point calculating */
            need_further_cal = 1u;
        }
        else if (signal_diff <= TAU_CAL_PRECISION)
        {
            /* Noise floor at >5< counts to avoid extra calibration cycling */
            need_further_cal = 0u;
        }

        else
        {
            /* compare to the allowed tolerance */
            cal_tau_x_target = ((qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->calib_option_select & CAL_CHRG_TIME_MASK) >> CAL_CHRG_TIME_POS);

            if (signal_diff <= tau_calc[cal_tau_x_target])
            {
                /* Loss in Cx is within tuning limit */
                need_further_cal = 0u;
            }
            else
            {
                /* Too much Cx reduction */
                need_further_cal = 1u;
            }
        } /* Signal miss < max (MAX_CX_2_TAU) */
    } /* Non-zero signal miss */

    /* 0 = OK, 1 = NG */
    return need_further_cal;
}
/*============================================================================
touch_ret_t qtm_calibrate_sensor_node(qtm_acquisition_control_t* qtm_acq_control_ptr, uint16_t qtm_which_node_number)
------------------------------------------------------------------------------
Purpose:  Marks a sensor node for calibration
Input  :  Node configurations pointer, node (channel) number
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_calibrate_sensor_node(qtm_acquisition_control_t* qtm_acq_control_ptr, uint16_t qtm_which_node_number)
{
  uint8_t c_temp_calc = 0u;
  
  touch_ret_t node_calibrate_t_status = TOUCH_SUCCESS;
  if(qtm_acq_control_ptr == NULL)
  {
    /* Not assigned */
    node_calibrate_t_status = TOUCH_INVALID_POINTER;
  }
  else if(qtm_which_node_number >= (qtm_acq_control_ptr->qtm_acq_node_group_config->num_sensor_nodes))
  {
    node_calibrate_t_status = TOUCH_INVALID_INPUT_PARAM;
  }
  else
  {
    c_temp_calc = qtm_acq_control_ptr->qtm_acq_node_group_config->acq_sensor_type;
    /* Self or mutual - to decide starting CCCAL */
    if(NODE_MUTUAL == c_temp_calc)
    {
      qtm_acq_control_ptr->qtm_acq_node_data[qtm_which_node_number].node_comp_caps = MUTL_START_CCCAL;
    }
    else if((NODE_SELFCAP == c_temp_calc)||(NODE_SELFCAP_SHIELD == c_temp_calc))
    {
      qtm_acq_control_ptr->qtm_acq_node_data[qtm_which_node_number].node_comp_caps = SELFCAP_START_CCCAL;
    }
    else
    {
      node_calibrate_t_status = TOUCH_INVALID_INPUT_PARAM;
    }
    
    /* CAL Flag */
    qtm_acq_control_ptr->qtm_acq_node_data[qtm_which_node_number].node_acq_status |= NODE_CAL_REQ;
    
  }
  return node_calibrate_t_status;
}

/*============================================================================
touch_ret_t qtm_cvd_init_acquisition_module(qtm_acquisition_control_t* qtm_acq_control_ptr)
------------------------------------------------------------------------------
Purpose: Initialize the HCVD and ADC & Assign pins
Input  : pointer to acquisition set
Output : touch_ret_t: TOUCH_SUCCESS or INVALID_PARAM
Notes  : qtm_hcvd_init_acquisition_module module must be called ONLY once with a pointer to each config set
============================================================================*/
touch_ret_t qtm_cvd_init_acquisition_module(qtm_acquisition_control_t* qtm_acq_control_ptr)
{
    touch_ret_t touch_return_param = TOUCH_SUCCESS;
    uint32_t which_xpins_cvd = 0u;
    uint32_t which_ypins_cvd = 0u;
    uint16_t counter = 0u;

    if (qtm_acq_control_ptr == NULL_POINTER)
    {
        touch_return_param = TOUCH_INVALID_POINTER;
    }
    else
    {

        for (counter = 0u; counter < qtm_acq_control_ptr->qtm_acq_node_group_config->num_sensor_nodes; counter++)
        {
            /* All nodes use the same structure, node_xmask should be 0 for Self-cap */
            which_xpins_cvd |= qtm_acq_control_ptr->qtm_acq_node_config[counter].node_xmask;
            which_ypins_cvd |= qtm_acq_control_ptr->qtm_acq_node_config[counter].node_ymask;
        }

        if (which_ypins_cvd == 0u)
        {
            /* No valid sensor nodes */
            touch_return_param = TOUCH_INVALID_INPUT_PARAM;
        }

        else
        {
            if (TOUCH_STATE_NULL == touch_seq_lib_state)
            {
                /* ADC Reset & Initialize */
                touch_seq_lib_state = TOUCH_STATE_INIT;
            }
            cvd_config_io_pins(which_ypins_cvd, 0, CVD_RX_PINS);
            cvd_config_io_pins(which_xpins_cvd, 0, CVD_TX_PINS);
        }
    } 
    return touch_return_param;

} /* cvd_init_settings(...) */

static touch_ret_t node_process(uint8_t node)
{
    touch_ret_t touch_return = TOUCH_SUCCESS;
    uint32_t measured_signal;
    uint16_t cal_sig_delta;
    uint8_t node_state;
    uint8_t delta_pol = 0u;
    uint8_t scale_down;
    uint8_t charge_share_check;
    uint8_t which_cal_option;
    uint8_t this_node_oversampling = 0u;
    uint8_t temp_var_mask = 0u;
    uint16_t comp_cap_test_val = 0u;

    measured_signal = (uint32_t) (qtm_raw_data_measurements_ptr[node]);
    node_state = (qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_acq_status & NODE_STATUS_MASK) >> NODE_STATUS_POS;
    this_node_oversampling = (qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[node].node_oversampling);

    switch (node_state)
    {
    case (NODE_MEASURE):
    {
        temp_var_mask = NODE_GAIN_DIG(qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[node].node_gain);

        /* Scale down according to digital gain & accumulation settings */
        if (this_node_oversampling > temp_var_mask)
        {
            scale_down = this_node_oversampling - temp_var_mask;
        }
        else
        {
            scale_down = 0u;
        }

        /* Scale down according to analog gain(ADC resolution)settings */
        scale_down += 2 - NODE_GAIN_ANA(qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[node].node_gain);

        qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_acq_signals = (uint16_t) (measured_signal >> scale_down);
    }
        break;
    case (NODE_CC_CAL):
    {
        /* Calibration -> Scale down to ADC range => Number of samples */
        scale_down = this_node_oversampling;
        measured_signal = (uint16_t) (measured_signal >> scale_down);
        qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_acq_signals = measured_signal;

        /* Comp cap settings used for this measurement */
        comp_cap_test_val = qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_comp_caps;

        /* Calculate Delta-C */
        if (measured_signal > ADC_MAX_READ)
        {
            cal_sig_delta = measured_signal - ADC_MAX_READ;
            delta_pol = 1u;
        }
        else
        {
            cal_sig_delta = ADC_MAX_READ - measured_signal;
            delta_pol = 0u;
        }
        /* Delta is doubled by differential measurement (mutual) OR by reduced selfcap */
        cal_sig_delta = (cal_sig_delta >> 1u);

        /* Adjust for self or mutual */
        if (qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->acq_sensor_type == NODE_MUTUAL)
        {
            if (0u == delta_pol)
            {
                delta_pol = 1u;
            }
            else
            {
                delta_pol = 0u;
            }
        }
        else
        {

        }

        /* Check if Comp cap is at max but too small */
        if (delta_pol == 1u)
        {
            if (comp_cap_test_val == 0x0007u)
            {
                /* Can't calibrate CCC, use Max */
                cal_sig_delta = 0u;
                qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_acq_status = (uint8_t) NODE_ENABLED;

            }
        }
        else
        {
            /* calibration is complete */
            cal_sig_delta = 0u;
            qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_acq_status = (uint8_t) NODE_ENABLED;
        }

        /* Latest measurement within target ? */
        if (cal_sig_delta < CC_CAL_PRECISION)
        {
            which_cal_option = (qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->calib_option_select & CAL_OPTION_MASK);
            /* Is this a re-cal after time parameter (Tau) tuning ? */
            if (TAU_CAL_DONE == (qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_acq_status & TAU_CAL_DONE))
            {
                /* Calibration auto-tune of hardware config already completed */
                qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_acq_status = NODE_ENABLED;
            }
            else if (NODE_CAL_ERROR == (qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_acq_status & NODE_CAL_ERROR))
            {
                /* No Tau cal for NODE_CAL_ERROR */
            }
            else if (CAL_AUTO_TUNE_CSD == which_cal_option)
            {
                /* Configure CSD - Take a reading with maximum CSD first */
                qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[node].node_csd = NODE_CSD_MAX;
                qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_acq_status = (uint8_t) ((NODE_CSD_CAL << NODE_STATUS_POS) | NODE_ENABLED);
            }
            else if (CAL_AUTO_TUNE_PRSC == which_cal_option)
            {
                /* clock pre-scaler is not implemented on the PIC32MZ */
                qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_acq_status = NODE_ENABLED;
            }
            else
            {
                /* No calibration auto-tune of hardware config */
                qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_acq_status = NODE_ENABLED;
            }

            /* Write result to node status array */
            qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_comp_caps = (uint16_t) (comp_cap_test_val & 0x0007u);
        }
        else
        {
            /* Modify the comp cap setting according to delta */
            comp_cap_test_val++;
            /* Write result to node status array */
            qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_comp_caps = (uint16_t) (comp_cap_test_val & 0x0007u);
        }
    }
        break;
    case (NODE_CSD_CAL):
    {
        /* CSD tuning - Comp cap calibrated with largest CSD, then CSD reduced to a level showing little / no loss in charge transfer */
        /* Scale measurement down to (0-1023) */
        scale_down = this_node_oversampling;
        measured_signal = (measured_signal >> scale_down);

        /* If this measurement is the first of CSD sequence (i.e. Max CSD) record it and prepare for the next */
        if (qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[node].node_csd == NODE_CSD_MAX)
        {
            /* Store the measured signal */
            qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_acq_signals = measured_signal;

            /* Set highest bit to 0 for next measurement */
            qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[node].node_csd &= ~(1u << (NUM_BITS_CSD - 1u));
        }
        else
        {
            /* Find the lowest bit position containing 0 */
            /* Note: Variable re-use scale_down as search index delta_pol as bit position */
            for (scale_down = 0u; scale_down < NUM_BITS_CSD; scale_down++)
            {
                if (0u == ((qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[node].node_csd) & (1u << scale_down)))
                {
                    /* This bit */
                    delta_pol = scale_down;
                    scale_down = NUM_BITS_CSD;
                }
                else
                {
                    delta_pol = 0u;
                }
            }

            /* Convert delta to (apparent) reduction in sensor capacitance */
            /* Variable re-use cal_sig_delta */
            charge_share_check = charge_share_test(node, measured_signal);
            if (0u == charge_share_check)
            {
                /* Good - Bit stays cleared */
            }
            else
            {
                /* No good - Set bit */
                qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[node].node_csd |= 1u << delta_pol;
            }

            if (delta_pol > 0u)
            {
                delta_pol--;
                scale_down = ((qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[node].node_csd) & (uint8_t) ~(1u << delta_pol));
                qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[node].node_csd = scale_down;
            }
            else
            {
                /* Done - Set node to 'Measure' mode */
                qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[node].node_acq_status = NODE_ENABLED;
            }
        }
        /* Save bit position */

    }
        break;
    case (NODE_PRSC_CAL):
    {
        /* No prescaler for PIC32MZ */
    }
        break;
        /* /Prescaler tuning */
    default:
    {
        /* Shouldn't be here */
        while (1)
        {
        }
    }
        break;
    }

    return touch_return;
}

/*============================================================================
touch_ret_t qtm_acquisition_process(void)
------------------------------------------------------------------------------
Purpose: Signal capture and processing
Input  : (Measured signals, config)
Output : TOUCH_SUCCESS or TOUCH_CAL_ERROR
Notes  : none
============================================================================*/
touch_ret_t qtm_acquisition_process(void)
{
    touch_ret_t touch_return = TOUCH_SUCCESS;
    /* Called from Result complete */
    uint16_t measured_nodes;

    if (TOUCH_STATE_NULL == touch_seq_lib_state)
    {
        touch_return = TOUCH_INVALID_LIB_STATE;
    }
    else
    {
        for (measured_nodes = 0u; measured_nodes < (qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->num_sensor_nodes); measured_nodes++)
        {
            node_process(measured_nodes);
        }
        /* Allow next acquisition start */
        touch_seq_lib_state = TOUCH_STATE_READY;
    } /* Not NULL */

    return touch_return;
}

/*============================================================================
static void cvd_config_io_pins(uint32_t cvd_lines,uint8_t direction, const uint8_t* pin_table)
------------------------------------------------------------------------------
Purpose: Checks which X and Y pins are used, sets to output low
Input  : Bitfield: x_lines | y_lines, where 1 = Used by CVD, 
         direction: specify whether the pin is input or output, 1 input , 0 for output
         and pin table specific the index for the i/o pin for the port control
Output : none
Notes  : none
============================================================================*/
static void cvd_config_io_pins(uint32_t cvd_lines, uint8_t direction, const uint8_t pin_table[])
{
    uint8_t local_count_var = 0u;

    for (local_count_var = 0u; local_count_var < 32u; local_count_var++)
    {
        /* XY Lines */
        if (0u == (cvd_lines & (uint32_t) ((uint32_t) 1u << local_count_var)))
        {
            /* This XY pin not used */
        }
        else
        {
            if (direction)
            {
                GPIO_PinInputEnable(pin_table[local_count_var]);
            }
            else
            {
                GPIO_PinOutputEnable(pin_table[local_count_var]);
                GPIO_PinClear(pin_table[local_count_var]);
            }

        } /* Enabled XY Line */
    } /* Count to NUM_CVD_XY_LINES */
}

/*============================================================================
static uint8_t select_next_channel_to_measure(void);
------------------------------------------------------------------------------
Purpose: Select the next channel for measuring
Input  : none
Output : current_measure_channel set to next channel number
Notes  : none
============================================================================*/
static uint8_t select_next_channel_to_measure(void)
{
    uint8_t sequence_complete = 2u;
    uint16_t next_channel;

    next_channel = current_measure_channel;
    do
    {
        if (next_channel < qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->num_sensor_nodes)
        {
            /* More channels in the group */
            if (0u == (qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[next_channel].node_acq_status & NODE_ENABLED))
            {
                /* This channel disabled */
                next_channel++;
            }
            else
            {
                /* This channel next */
                sequence_complete = 0u;
                current_measure_channel = next_channel;
            }
        }
        else
        {
            /* No more channels */
            sequence_complete = 1u;
            current_measure_channel = 0u;
        }
    }
    while (2u == sequence_complete);

    return sequence_complete;
}

/*============================================================================
touch_ret_t qtm_cvd_qtlib_assign_signal_memory(uint32_t* qtm_signal_raw_data_ptr)
------------------------------------------------------------------------------
Purpose: Assign raw signals pointer to array defined in application code
Input  : pointer to raw data array
Output : touch_ret_t: TOUCH_SUCCESS or INVALID_POINTER
Notes  : none
============================================================================*/
touch_ret_t qtm_cvd_qtlib_assign_signal_memory(uint32_t* qtm_signal_raw_data_ptr)
{
    touch_ret_t touch_return_this = TOUCH_SUCCESS;
    if (qtm_signal_raw_data_ptr == NULL)
    {
        touch_return_this = TOUCH_INVALID_POINTER;
    }
    else
    {
        qtm_raw_data_measurements_ptr = qtm_signal_raw_data_ptr;
    }
    return touch_return_this;
}

/*============================================================================
static uint32_t get_cvd_result(void)
------------------------------------------------------------------------------
Purpose: Captures CVD RESULT register
Input  : -
Output : RESULT
Notes  : none
============================================================================*/
static uint32_t get_cvd_result(void)
{
    uint32_t touch_measurement_capture;
    /* the offset is to make sure the result of CVD is always positive 
       so that all the math afterwards will be unsigned math */
    uint32_t result_offset;

    result_offset = ADC_MAX_READ << \
 (qtm_acquisition_control_working_set_ptr->\
 qtm_acq_node_config[current_measure_channel].node_oversampling);

    touch_measurement_capture = result_offset + CVDRES0H - CVDRES0L;
    //touch_measurement_capture = result_offset + CVDRES0Dbits.DELTA;
    return touch_measurement_capture;
}
/*============================================================================
static uint8_t qtm_load_group_config(void)
------------------------------------------------------------------------------
Purpose: Load node group configuration
Input  : -
Output : RESULT
Notes  : none
============================================================================*/
static uint8_t qtm_load_group_config(qtm_acquisition_control_t* qtm_acq_control_pointer)
{
    uint8_t param_ok_status = TOUCH_SUCCESS;


    /* CVD Common settings */
    CVDCONbits.SDHOLD = 1u;
    if (qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->acq_sensor_type == NODE_MUTUAL)
    {
        CVDSD0C3bits.SD0SELF = 0u;
        CVDSD0C3bits.SD0MUT = 1u;
    }
    else if (qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->acq_sensor_type == NODE_SELFCAP)
    {
        CVDSD0C3bits.SD0SELF = 1u;
        CVDSD0C3bits.SD0MUT = 0u;
    }
    else if (qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->acq_sensor_type == NODE_SELFCAP_SHIELD)
    {
        CVDSD0C3bits.SD0SELF = 1u;
        CVDSD0C3bits.SD0MUT = 1u;
    }
    else
    {
        /* Neither Mutual or Self configured - invalid */
        param_ok_status = TOUCH_INVALID_INPUT_PARAM;
    }

    if (param_ok_status == TOUCH_SUCCESS)
    {

        CVDADC = 0x0000000B;
        CVDSD0C3bits.SD0BUF = 1u;

        if (qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->freq_option_select < FREQ_SEL_SPREAD)
        {
            sample_delay = qtm_acquisition_control_working_set_ptr->qtm_acq_node_group_config->freq_option_select; /* Sampling delay */

        }
        else
        {
            /* Spread spectrum selected */
            sample_delay = (uint8_t) (rand()&0x07u);
        }
    }

    /* Use PCLCK2 for the CVD core */
    CVDCONbits.CLKSEL = 0b00u;
    CVDCONbits.SDHOLD = 0u;
    return param_ok_status;
}

/*============================================================================
static void qtm_measure_node(uint16_t channel_number);
------------------------------------------------------------------------------
Purpose: Configures the cvd for the selected channel + starts measurement
Input  : channel number
Output : none
Notes  : none
============================================================================*/
static void qtm_measure_node(uint16_t channel_number)
{
    uint8_t temp_var_mask_here = 0u;
    uint8_t xy_counter;
    uint8_t num_x, num_y;

    /* Disable CVD */
    CVDCONbits.SDHOLD = 1u;
    /* XSEL and YSEL => Bitfields */

    num_x = 0u;
    num_y = 0u;
    xy_counter = 0u;

    /* configure the Y line as input */
    cvd_config_io_pins(qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].node_ymask, 1u, CVD_RX_PINS);

    for (temp_var_mask_here = 0u; temp_var_mask_here < sizeof (CVD_RX_PINS); temp_var_mask_here++)
    {
        if (0u == ((qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].node_ymask) & (1u << temp_var_mask_here)))
        {
        }
        else
        {
            *CVD_Y_REG[xy_counter++] = temp_var_mask_here;
            num_y++;
        }
    }

    xy_counter = 0;
    for (temp_var_mask_here = 0u; temp_var_mask_here < sizeof (CVD_TX_PINS); temp_var_mask_here++)
    {
        if (0u == ((qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].node_xmask) & (1u << temp_var_mask_here)))
        {
        }
        else
        {
            *CVD_X_REG[xy_counter++] = temp_var_mask_here;
            num_x++;
        }
    }

    if (num_x == 0u)
    {
        num_x = 1u;
    }
    
    /* number of X and Y */
    CVDSD0C2bits.SD0RXSTRIDE1 = 0b0011 & (num_y - 1u);
    CVDSD0C2bits.SD0RXSTRIDE2 = (num_y - 1u) >> 2u;
    CVDSD0C2bits.SD0TXSTRIDE1 = 0b0011 & (num_x - 1u);
    CVDSD0C2bits.SD0TXSTRIDE2 = (num_x - 1u) >> 2u;

    CVDSD0C2bits.SD0TXBEG = 0u;
    CVDSD0C2bits.SD0TXEND = num_x - 1u;

    CVDSD0C2bits.SD0RXBEG = 0u;
    CVDSD0C2bits.SD0RXEND = num_y - 1u;

    /* Compensation cap */
    CVDSD0C3bits.CVDEN = 1u;
    CVDSD0C3bits.CVDCPL = qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[channel_number].node_comp_caps;
    /* Calibration requested? */
    if (0u == (qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[channel_number].node_acq_status & NODE_CAL_REQ))
    {
    }
    else
    {
        /* Calibration requested */
        /* Set node status to calibrate comp cap, clear CAL request flag */
        qtm_acquisition_control_working_set_ptr->qtm_acq_node_data[channel_number].node_acq_status = (uint8_t) ((NODE_CC_CAL << NODE_STATUS_POS) | NODE_ENABLED);
    }

    /* Always use 12 bit ADC resolution, scale down in the node process */
    CVDADCbits.SELRES = 0b11u;

    temp_var_mask_here = qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].node_csd;

    if (temp_var_mask_here < (uint8_t) (NODE_CSD_MAX - CSD_MIN_OFFSET))
    {
        CVDSD0C3bits.SD0CHGTIME = (temp_var_mask_here + CSD_MIN_OFFSET);
        CVDSD0C3bits.SD0ACQTIME = (temp_var_mask_here + CSD_MIN_OFFSET);
    }
    else
    {
        CVDSD0C3bits.SD0CHGTIME = NODE_CSD_MAX;
        CVDSD0C3bits.SD0ACQTIME = NODE_CSD_MAX;
    }

    /* Oversampling - Digital (2^n) */
    CVDSD0C1bits.SD0OVRSAMP = OVERSAMPLING_LUT[qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[channel_number].node_oversampling];

    /* SD0OVRTIME = 1; SD0POLTIME = 1; These registers cannot be 0, otherwise, CVD doesn't work */
    CVDSD0T2 = 0x00010100;

    /* Apply additional delay for the frequency hopping */
    CVDSD0T2bits.SD0CONTIME = 20u + (sample_delay << 2u);

    /* Enable CVD */
    CVDSD0C3bits.SD0EN = 0b01u;
    CVDCONbits.SDHOLD = 0u;
    CVDCONbits.ON = 1u;

    CVDCONbits.CVDIEN = 1;
    
    CVDSD0C3bits.SD0IEN = 1;
    IFS5 &=~(0x00000100);   //clear the interrupt flag
    IEC5 |=0x00000100;  //enable the interrupt 

    /* Start Measurement */
    CVDCONbits.SWTRIG = 1u;
}

/*============================================================================
touch_ret_t qtm_cvd_start_measurement_seq(qtm_acquisition_control_t* qtm_acq_control_pointer, void (*measure_complete_callback) (void));
------------------------------------------------------------------------------
Purpose:  Loads touch configurations for first channel and start,
Input  :  Node configurations pointer, measure complete callback pointer
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_cvd_start_measurement_seq(qtm_acquisition_control_t* qtm_acq_control_pointer, void (*measure_complete_callback) (void))
{
    touch_ret_t param_ok_status = TOUCH_SUCCESS;
    uint8_t sequence_complete = 0u;

    if (measure_complete_callback == NULL)
    {
        param_ok_status = TOUCH_INVALID_POINTER;
    }
    else if (qtm_acq_control_pointer == NULL)
    {
        param_ok_status = TOUCH_INVALID_POINTER;
    }
    else if (touch_seq_lib_state == TOUCH_STATE_NULL)
    {
        param_ok_status = TOUCH_INVALID_LIB_STATE;
    }
    else if (touch_seq_lib_state == TOUCH_STATE_BUSY)
    {
        param_ok_status = TOUCH_ACQ_INCOMPLETE;
    }
    else if (0u != check_cvd_busy())
    {
        param_ok_status = TOUCH_ACQ_INCOMPLETE;
    }
    else if ((qtm_acq_control_pointer->qtm_acq_node_group_config->cvd_interrupt_priority) < 1u)
    {
        param_ok_status = TOUCH_INVALID_INPUT_PARAM;
    }
    else if ((qtm_acq_control_pointer->qtm_acq_node_group_config->cvd_interrupt_priority) > 3u)
    {
        param_ok_status = TOUCH_INVALID_INPUT_PARAM;
    }
    else
    {

        /* Attach pointers */
        qtm_acquisition_control_working_set_ptr = qtm_acq_control_pointer;
        cvd_seq_measure_complete_pointer = measure_complete_callback;

        /* Load group config */
        qtm_load_group_config(qtm_acquisition_control_working_set_ptr);

        /* Load config for first enabled sensor channel */
        current_measure_channel = 0u;
        sequence_complete = select_next_channel_to_measure();
        if (0u == sequence_complete)
        {
            touch_seq_lib_state = TOUCH_STATE_BUSY;
            qtm_measure_node(current_measure_channel);
        }
        else
        {
            param_ok_status = TOUCH_INVALID_INPUT_PARAM;
            touch_seq_lib_state = TOUCH_STATE_READY;
        }
    }

    return param_ok_status;
}

/*============================================================================
void qtm_pic32_cvd_handler_eoc(void)
------------------------------------------------------------------------------
Purpose:  Captures  the  measurement,  starts  the  next  or  End  Of  Sequence  handler
Input    :  none
Output  :  none
Notes    :  none
============================================================================*/
void qtm_pic32_cvd_handler_eoc(void)
{    
    uint8_t sequence_complete;
    
    IFS5 &=~(0x00000100);   //clear the interrupt flag
    qtm_raw_data_measurements_ptr[current_measure_channel] = get_cvd_result();
    CVDCONbits.ON = 0u;
    cvd_config_io_pins(qtm_acquisition_control_working_set_ptr->qtm_acq_node_config[current_measure_channel].node_ymask, 0u, CVD_RX_PINS);

    switch (qtm_which_mode_current)
    {
    case (QTM_ACQ_SEQUENTIAL):
    {
        /* Sequential mode - capture data and continue sequence */
        current_measure_channel++;
        sequence_complete = select_next_channel_to_measure();
        if (0u == sequence_complete)
        {
            /* More nodes to measure */
            qtm_measure_node(current_measure_channel);
            touch_seq_lib_state = TOUCH_STATE_BUSY;
        }
        else
        {
            /* Sequence complete */
            cvd_seq_measure_complete_pointer();
        }
    }
        break;

    case (QTM_ACQ_WINDOWCOMP):
        /* Not implemented */
        break;

    default:
    {
        /* Error ??? */
    }
        break;
    }

}

/*============================================================================
touch_ret_t qtm_enable_sensor_node(qtm_acquisition_control_t* qtm_acq_control_ptr, uint16_t qtm_which_node_number)
------------------------------------------------------------------------------
Purpose:  Enables a sensor node for measurement
Input  :  Node configurations pointer, node (channel) number
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_enable_sensor_node(qtm_acquisition_control_t* qtm_acq_control_ptr, uint16_t qtm_which_node_number)
{
    touch_ret_t node_enable_t_status = TOUCH_SUCCESS;
    if (qtm_acq_control_ptr == NULL)
    {
        /* Not assigned */
        node_enable_t_status = TOUCH_INVALID_POINTER;
    }
    else if (qtm_which_node_number >= (qtm_acq_control_ptr->qtm_acq_node_group_config->num_sensor_nodes))
    {
        node_enable_t_status = TOUCH_INVALID_INPUT_PARAM;
    }
    else
    {
        qtm_acq_control_ptr->qtm_acq_node_data[qtm_which_node_number].node_acq_status = NODE_ENABLED;
    }
    return node_enable_t_status;
}

/*============================================================================
void qtm_cvd_last_measure_is_complete(void)
------------------------------------------------------------------------------
Purpose:  check if the CVD measurement is compelete or not.
Input    :  none
Output  :  uint8_t: 0-> incomplete, 1-> complete
Notes    :  none
============================================================================*/
uint8_t qtm_cvd_last_measure_is_complete(void)
{
    uint8_t return_is_complete = 1u;

    if (0u != CVDSD0C3bits.SD0EN)
    {
        return_is_complete = 0u;
    }
    return return_is_complete;
}

/*============================================================================
void qtm_cvd_clear_interrupt(void)
------------------------------------------------------------------------------
Purpose:  Clears the eoc/wcomp interrupt bits
Input    :  none
Output  :  none
Notes    :  none
============================================================================*/
void qtm_cvd_clear_interrupt(void)
{
    /* Clear the interrupt flags */
    IFS5 &= ~(0x00000100); //clear the interrupt flag
}

/*============================================================================
void qtm_cvd_de_init(void)
------------------------------------------------------------------------------
Purpose: Clear CVD Pin registers, set TOUCH_STATE_NULL
Input  : none
Output : none
Notes  : none
============================================================================*/
void qtm_cvd_de_init(void)
{
    touch_seq_lib_state = TOUCH_STATE_NULL;
    CVDCONbits.ON = 0u;
}

/*============================================================================
uint16_t qtm_pic32mzw_acq_module_get_id(void);
------------------------------------------------------------------------------
Purpose: Returns the module ID
Input  : none
Output : Module ID
Notes  : none
============================================================================*/
uint16_t qtm_pic32mzw_acq_module_get_id(void)
{
    return QTM_MODULE_ID_PIC32MZW_ACQ;
}

/*============================================================================
uint8_t qtm_pic32mzw_acq_module_get_version(void);
------------------------------------------------------------------------------
Purpose: Returns the module Firmware version
Input  : none
Output : Module ID - Upper nibble major / Lower nibble minor
Notes  : none
============================================================================*/
uint8_t qtm_pic32mzw_acq_module_get_version(void)
{
    return QTM_MODULE_VERSION;
}
