#include "core/cplc.h"
#include "hardware/hardware.h"
#include "core/global.h"
#include "core/gld.h"
#include "core/math_dsp.h"

#include <math.h>
#include <stdlib.h>

//TODO
#include "CyclesSync.h"
//#include "ThermoCalc.h"

#define  PLC_SHIFT				(6) //(6) 	
#define	 PLC_PHASE_DET_SHIFT	(18) //for analog output

#define  PLC_RESET_THRESHOLD 	(-3276)//(-3276) //e. correspond to the voltage +1.2 Volts
#define	 WP_REG32MAX_SATURATION (32767 << PLC_SHIFT)
#define	 WP_REG32MIN_SATURATION (-32767 << PLC_SHIFT)
#define	 WP_REG32MIN_NEW_SATURATION (PLC_RESET_THRESHOLD << PLC_SHIFT)

int MaxDelayPLC;
int phase_Digital;
int WP_reg32 = 0;
int16_t g_sin_func[100];		
/******************************************************************************/
int PLC_MeanderDelay(int flag)
{
	static int poz_counter = 0, neg_counter = 0, flg_delay;

	if (Device_blk.Str.WP_ref == 0) {
		return (flag);
	}
	
	//e. check whether delay exceeds the greatest possible value
    if (Device_blk.Str.WP_ref > MaxDelayPLC) { 
        Device_blk.Str.WP_ref = MaxDelayPLC; 
    }
	
    //e. outgoing poz_sin_flag flag, which delayed by the WP_ref
	if (flag) {
  		neg_counter = 0;
  		poz_counter++;
	} else {
  		poz_counter = 0;
  		neg_counter++;
	}
	if (poz_counter == Device_blk.Str.WP_ref) { flg_delay = 1; }
	if (neg_counter == Device_blk.Str.WP_ref) { flg_delay = 0; }
	return (flg_delay);
}
/******************************************************************************/
/**
*   @breif интегрирование выхода ФД контура СРП для технологического вывода
*/
int WP_PhaseDetectorRate(int a_nPhaseDetInput, int a_nIntegrateTime) 
{

	static int SampleAndHoldOut = 0;
	static int WP_PhasDet_integr = 0;//, WP_PhasDetector = 0;
	
    //проверяем если накопилась секунда - вернем проинтегрированное значение контура
	if (a_nIntegrateTime >= DEVICE_SAMPLE_RATE_uks) {
		SampleAndHoldOut = (int)(WP_PhasDet_integr >> PLC_PHASE_DET_SHIFT);  
		WP_PhasDet_integr = 0;
	} else {	
        //время интеграции не накопилось - суммируем
		WP_PhasDet_integr += a_nPhaseDetInput;
	}
	return (SampleAndHoldOut);
}
/******************************************************************************/
void calc_sin_func()
{
	int i = 0;
    
    //проверим коэф частоты
    //if demanded frequency of PLC modulator 
    if ((Device_blk.Str.PI_b3>PI_B3_MAX)||(Device_blk.Str.PI_b3<PI_B3_MIN)){	
        //is more then 1kHz or less them 100Hz, set f=250Hz
	    Device_blk.Str.PI_b3 = PI_B3_CONST;
    }
     
    //e. synthesis of PLC scan signal
    for (i = 0; i<Device_blk.Str.PI_b3; i++) {
       // g_sin_func[i] = (x_int16_t)(5958*sin((float)i*2.0*PI/(float)Device_blk.Str.PI_b3));
        g_sin_func[i] = (x_int16_t)(32000*sin((float)i*2.0*PI/(float)Device_blk.Str.PI_b3));
    }
}

/******************************************************************************/
void cplc_init(void)
{
	int i = 0;
    //char dbg[32];
    Output.Str.WP_reg = Device_blk.Str.WP_start;
	   WP_reg32 = Output.Str.WP_reg<<PLC_SHIFT;
		   
    calc_sin_func();
    
    //e. calculation of filter coefficients for PLC		
	init_BandPass( 1.0/(float)Device_blk.Str.PI_b3, 10.0/(float)(DEVICE_SAMPLE_RATE_HZ), PLC);	

    //e. max expected delay for phase detector output = period of modulator
	MaxDelayPLC = Device_blk.Str.PI_b3;

    //init worms arrays
    for (i=0; i<7; i++){
		Output.Str.WP_Phase_Det_Array[i] = 0;
    }
    for (i=0; i<21; i++){
		g_gld.cplc.WP_DelaySin_Array[i]	=0;
    }
    for (i=0; i<=7; i++){
		Output.Str.WP_sin_Array[i] = Output.Str.WP_sin_Array[i+1];
    }
}

/******************************************************************************/
//процедура предварительной обработки для контура СРП
void cplc_regulator(void)
{
    int poz_sin_flag = 0;   	 	   
    int poz_sin_flag_delayed = 0;
    
    //e. output of the phase detector of the CPLC (in a digital kind)
    int WP_Phase_Det = 0; 
    int i =0;
    
    //static int cpld_iscl = 0;
    //static int cpld_phase_accum = 0;
    
    //create meandr
    if (Output.Str.WP_sin > 0) {
        poz_sin_flag = 0;
    } else {
        poz_sin_flag = 1;
    }
    
	//e. band-pass filter for the CPLC regulator 
    WP_Phase_Det = PLC_PhaseDetFilt(g_input.word.wp_sel);
	
	if (WP_Phase_Det > 0) {
  		phase_Digital = 1;
	} else {
  		phase_Digital = -1;
	}
    
    //worms arrays
    //if (Device_blk.TermoMode ==0)
    for (i=0; i<7; i++){
		Output.Str.WP_Phase_Det_Array[i] = Output.Str.WP_Phase_Det_Array[i+1];
    }
		Output.Str.WP_Phase_Det_Array[7] = WP_Phase_Det;
    
    for (i=0; i<21; i++){
		g_gld.cplc.WP_DelaySin_Array[i] = g_gld.cplc.WP_DelaySin_Array[i+1];
    }
	g_gld.cplc.WP_DelaySin_Array[20] = Output.Str.WP_sin;
	
	for (i=0; i<=7; i++){
		Output.Str.WP_sin_Array[i] = Output.Str.WP_sin_Array[i+1];
    }
	//Output.Str.WP_sin_Array[7] = g_gld.cplc.WP_DelaySin_Array[20-Device_blk.Str.WP_ref];
	Output.Str.WP_sin_Array[7] = g_gld.cplc.WP_DelaySin_Array[Device_blk.Str.WP_ref];		
    //определяем задержку
	// from this WP_Phase_Det - modulated signal like LIM_DIG
	poz_sin_flag_delayed = PLC_MeanderDelay(poz_sin_flag);
		
//	Output.Str.WP_scope1 = poz_sin_flag_delayed;
//	Output.Str.WP_scope2 = phase_Digital;
		
	if(poz_sin_flag_delayed) {
		WP_Phase_Det = -WP_Phase_Det; 
		phase_Digital = -phase_Digital;
	}  
    
    /*cpld_phase_accum += phase_Digital;

	cpld_iscl++;
    if(cpld_iscl == 10){
        WP_reg32 = L_mac(WP_reg32, cpld_phase_accum, Device_blk.Str.WP_scl );
        cpld_iscl = 0;
        cpld_phase_accum = 0;
    }*/
    WP_reg32 = L_mac(WP_reg32, phase_Digital, Device_blk.Str.WP_scl );
    
    // e.working with the range +15 ... -15 V;
    Saturation(WP_reg32, WP_REG32MAX_SATURATION, WP_REG32MIN_SATURATION);  
    
	if ( loop_is_closed(WP_REG_ON) ) {
        //enable auto regulation
        //e. the regulator loop is closed 
        //e. we use as controlling - voltages of the integrator
		Output.Str.WP_reg = (int)(WP_reg32 >> PLC_SHIFT);		
	} else {
        //handle regulation
        //e. the regulator loop is open
        //e. set the previous value of the WP_reg
		WP_reg32 = Output.Str.WP_reg << PLC_SHIFT;
		
	}
	
 // Output.Str.WP_scope1 = Output.Str.WP_reg;
//	Output.Str.WP_scope2 = phase_Digital;
	
	//e. integartion of output of the PD of the CPLC regulator for the technological output on the Rate command
	Output.Str.WP_pll = WP_PhaseDetectorRate( WP_Phase_Det, g_gld.time_1_Sec); 
}

/******************************************************************************/
int cplc_calc_modulator(void)
{
	static int index = 0;
	int val = 0;  
    float perc = 0.0;
	
    //e. check modulator amplitude
    if (Device_blk.Str.PI_a4 == 0) {
        Device_blk.Str.PI_a4 = PI_A4_CONST;
    }
    if (Device_blk.Str.PI_a4 > PI_A4_MAX) {
        Device_blk.Str.PI_a4 = PI_A4_MAX;
    }
    //e. current array index
	index++;
	if (index >= Device_blk.Str.PI_b3)
        index = 0;
	
	//e. calculate output value
    //val = mult(g_sin_func[index], Device_blk.Str.PI_a4)+ INT16_MAX;
    
    perc = (float)Device_blk.Str.PI_a4/(float)PI_A4_MAX;
    perc *= (float)g_sin_func[index];
    val = (int)perc;
    
    //e. 
    hardware_modulator(val + INT16_MAX);
		 
	return (val);
} 


/******************************************************************************/
