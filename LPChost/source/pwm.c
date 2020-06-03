#include "hardware/pwm.h"
#include "lpc17xx.h"

/******************************************************************************/
void pwm_init(int a_VB_N,int a_VB_tau)
{
    // Turn On MCPWM PCLK
    LPC_SC->PCONP |= 0x00020000; 
    //CLK=12.5MHz
    LPC_SC->PCLKSEL1 |= 0xC0000000; 
    /* P1.25,1.26 as PhA_vibro; P1.28,1.29 as PhB_vibro*/
    LPC_PINCON->PINSEL3 &= ~(0x3CF<<18); 											  
    LPC_PINCON->PINSEL3 |= (0x145 << 18) |(1<<6)|(1<<12);
    
    //e. initial time counter of channel 0
    LPC_MCPWM->TC0 = 0;	
    
    //e. period of the dither drive
    LPC_MCPWM->LIM0 = (a_VB_N*MULT_7680_12500)>>SHIFT_7680_12500;  
    //e. set LPC_MCPWM->MAT0 for defineteness
    LPC_MCPWM->MAT0 = (a_VB_N*MULT_7680_12500)>>SHIFT_7680_12500;
    
    //e. pulse width of the PhA dither drive
    LPC_MCPWM->MAT2 = (a_VB_tau*MULT_7680_12500)>>SHIFT_7680_12500;
    //e. pulse width of the PhB dither drive  at first time
    LPC_MCPWM->MAT1 = ((a_VB_N - a_VB_tau)*MULT_7680_12500)>>SHIFT_7680_12500;

    //e. reset dead timer register
    LPC_MCPWM->DT &= ~0x3FF; 
    //e. enable lim0 interrupt
    LPC_MCPWM->INTEN_SET = 1;	

    //e. set AC mode (Pha, PhB periods are set by LIM0 )
    LPC_MCPWM->CON_SET |= 1<<30;
    //start PWM channel 0,1,2
    LPC_MCPWM->CON_SET |= (1<<8) |1 |(1<<16); 
    LPC_MCPWM->CON_SET |= 0x00060606;
}

/******************************************************************************/
void pwm_set_period(int a_nPeriod)
{
    LPC_MCPWM->LIM0 = (a_nPeriod*MULT_7680_12500)>>SHIFT_7680_12500;  
}
/******************************************************************************/
x_bool_t pwm_is_pulse_was_formed()
{
    if (LPC_MCPWM->INTF & 0x0001) {	 
        LPC_MCPWM->INTF_CLR |= 0x0001;
        return _x_true;
	}
    return _x_false;
}
/******************************************************************************/
x_bool_t pwm_pulse_calc(int a_T_Vibro, int a_L_Vibro, int a_Vibro_2_CountIn, x_bool_t a_bIsSwitchInq)
{
    //LPC_MCPWM->LIM0 = (a_L_Vibro*MULT_7680_12500)>>SHIFT_7680_12500;
    //LPC_MCPWM->MAT0 = (a_L_Vibro*MULT_7680_12500)>>SHIFT_7680_12500;
    
    if (LPC_MCPWM->MAT2 > LPC_MCPWM->MAT1) {
        //inquiry cycle duration must be changed
        if (a_bIsSwitchInq) {
            LPC_PWM1->MR0 = (a_T_Vibro*a_Vibro_2_CountIn)>>SHIFT_C_7680_12500; 		
            //e. enable updating of register
            LPC_PWM1->LER = (1<<0);
        }
        LPC_MCPWM->MAT1 = (a_T_Vibro*MULT_7680_12500)>>SHIFT_7680_12500;
        LPC_MCPWM->MAT2 = ((a_T_Vibro - a_L_Vibro)*MULT_7680_12500)>>SHIFT_7680_12500;  
        return _x_true;
    } else {
        LPC_MCPWM->MAT2 = (a_T_Vibro*MULT_7680_12500)>>SHIFT_7680_12500;
        LPC_MCPWM->MAT1 = ((a_T_Vibro - a_L_Vibro)*MULT_7680_12500)>>SHIFT_7680_12500;
        return _x_false;	 
    }
}
/******************************************************************************/
void pwm_on(void)
{
    LPC_MCPWM->CON_SET = 1<<8;	  //start vibro dither
}
/******************************************************************************/
void pwm_off(void)
{
    LPC_MCPWM->CON_CLR = 1<<8;	  //stop vibro dither 
}
/******************************************************************************/
/******************************************************************************/