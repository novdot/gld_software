
#ifndef __HARDWARE_DAC_H_INCLUDED__
#define __HARDWARE_DAC_H_INCLUDED__

#include "xlib/types.h"

#define LPC_DAC_VOLT_MAX (2.8)
//опроник операционного усилителя
#define MOD_AMPL_VOLT_IN_REF (2.0)
#define MOD_AMPL_VAL_IN_REF (MOD_CONVERT_VOLT2VAL(1.78))

#define LPC_DAC_SIN_AMP_MAX (LPC_DAC_VOLT_MAX-MOD_AMPL_VOLT_IN_REF)*2

//Vout = VALUE * ((VREFP - VREFN)/1024) + VREFN
#define MOD_CONVERT_VOLT2VAL(voltage) \
    voltage*1024/3


/**
    @brief DAC functions
*/
void hardware_dac_init(void);
void hardware_dac_send(x_uint32_t a_data);

#endif //__HARDWARE_DAC_H_INCLUDED__
