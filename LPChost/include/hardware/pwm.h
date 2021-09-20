#ifndef _PWM_H_INCLUDED
#define _PWM_H_INCLUDED

#include "xlib/types.h"

#define 	SHIFT_7680_12500		15	  //e. 14 digits for 7680 to 12500 clock converting and 1 division digit  
#define		SHIFT_C_7680_12500		11	  //
#define		DITH_VB_TAU_SHIFT       2
#define 	MULT_7680_12500			26667

#define MCPWM_VAL2CODE(val)\
    ((val)*MULT_7680_12500)>>SHIFT_7680_12500
    
void pwm_init(int a_VB_N,int a_VB_tau);
void pwm_set(int a_nPeriod,int a_nPulse);
x_bool_t pwm_is_pulse_was_formed();
x_bool_t pwm_pulse_calc(
    int a_T_Vibro
    , int a_L_Vibro
    , int a_Vibro_2_CountIn
    , x_bool_t a_bIsSwitchInq
    );
void pwm_on(void);
void pwm_off(void);
    
#endif
