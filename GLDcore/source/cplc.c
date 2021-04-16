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

#define  PLC_RESET_THRESHOLD 	(-3276) //e. correspond to the voltage +1.2 Volts
#define	 WP_REG32MAX_SATURATION (32767 << PLC_SHIFT)
#define	 WP_REG32MIN_NEW_SATURATION (PLC_RESET_THRESHOLD << PLC_SHIFT)
#define  WP_TMP_THRESHOLD		(7) //e. temperature threshold, defining heats up or cool down the device

int WP_reg32;
//int WP_reset_heating; //e. voltage of reset at heating
//int WP_reset_cooling; //e. voltage of reset at cooling
int MaxDelayPLC;
float g_sin_func[100];
int phase_Digital;

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
	if (poz_counter == Device_blk.Str.WP_ref) { flg_delay = 0; }
	if (neg_counter == Device_blk.Str.WP_ref) { flg_delay = 1; }
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
	if (a_nIntegrateTime == DEVICE_SAMPLE_RATE_uks) {
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
    float temp = 0.0;
    
    //проверим коэф частоты
    //if demanded frequency of PLC modulator 
    if ((Device_blk.Str.PI_b3>PI_B3_MAX)||(Device_blk.Str.PI_b3<PI_B3_MIN)){	
        //is more then 1kHz or less them 100Hz, set f=250Hz
	    Device_blk.Str.PI_b3 = PI_B3_CONST;
    }
    
    //проверим коэф амплитуды
    if (Device_blk.Str.PI_a4 == 0) {
		Device_blk.Str.PI_a4 = PI_A4_CONST;
	}
    
    
    //e. synthesis of PLC scan signal
    for (i = 0; i<Device_blk.Str.PI_b3; i++) {
        //generate sin points
        temp = sin((float)i*2.0*PI/(float)Device_blk.Str.PI_b3);

        //move up from -1..+1 to 0..+2
        temp += 1.0;
        
        g_sin_func[i] = ( temp );
        
        //convert from 0..+2 to 0..0xFFFF
        //наложим амплитуду - в процессе исполнения
        //sin_func[i] = (int)( temp*Device_blk.Str.PI_a4);
    }
}


/******************************************************************************/
void cplc_init(void)
{
	int i;
	//e. voltage of reset at heating should not 
    //e.exceed a limit of the upper threshold of the CPLC
	if (Device_blk.Str.WP_reset < PLC_RESET_THRESHOLD) {
		Device_blk.Str.WP_reset = PLC_RESET_THRESHOLD + 1;
	}
    //e. WP_reg start voltage is (WP_rup - WP_rdw)/2	
	Output.Str.WP_reg = (Device_blk.Str.WP_rup + Device_blk.Str.WP_rdw) >> 1; 
	WP_reg32 = Output.Str.WP_reg<<PLC_SHIFT;
		   
    calc_sin_func();
    
    //e. calculation of filter coefficients for PLC		
	init_BandPass( 1.0/(float)Device_blk.Str.PI_b3, 10.0/(float)(DEVICE_SAMPLE_RATE_HZ), PLC);	

    //e. during fist 10 seconds after start we state Device_blk.Str.WP_scl = 2*Device_blk.Str.WP_scl
	Device_blk.Str.WP_scl <<=  1; 

    //e. max expected delay for phase detector output
	MaxDelayPLC = Device_blk.Str.PI_b3>>1;

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
void cplc_regulator(void)
{
    static int is_zeroing = 0;
    static int zero_delay = 0;
    int poz_sin_flag = 0;   	 	   
    int poz_sin_flag_delayed = 0;
    static int plc_reset32 = 0;
    //e. output of the phase detector of the CPLC (in a digital kind)
    int WP_Phase_Det = 0; 
    int i =0;
    
    //e. state of linear transition at reset of the CPLC regulator
	static enum {
        FINISHED, 		//e. linear transition is completed
        TRANS_HEATING,  //e. transition is perfromed at heating
        TRANS_COOLING	//e. transition is perfromed at cooling 
    } plc_transiton = FINISHED; 
    
	if (Output.Str.WP_sin >= 32768) {
		poz_sin_flag = 0;
	} else {
		poz_sin_flag = 1;
	}
    
	//e. band-pass filter for the CPLC regulator 
    WP_Phase_Det = PLC_PhaseDetFilt(g_input.word.wp_sel);
	
	if (WP_Phase_Det >0) {
  		phase_Digital = 1;
	} else {
  		phase_Digital = -1;
	}
    
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
	Output.Str.WP_sin_Array[7] = g_gld.cplc.WP_DelaySin_Array[20-Device_blk.Str.WP_ref];
    
	// from this WP_Phase_Det - modulated signal like LIM_DIG
	poz_sin_flag_delayed = PLC_MeanderDelay(poz_sin_flag);

	if(poz_sin_flag_delayed) {
		WP_Phase_Det = -WP_Phase_Det; 
		phase_Digital = -phase_Digital;
	}  
	// from this WP_Phase_Det - demodulated signal like LIDEM_DIG
		
    //e. it is not time for reset 
	if (!is_zeroing) 	{
        //e. there is no reset
        if ((WP_reg32 > (Device_blk.Str.WP_rup << PLC_SHIFT)) && g_gld.thermo.IsHeating){
            //e. there is heating
            is_zeroing = 1;
            //e. voltage of reset at heating 
            g_gld.thermo.WP_reset_heating = CPL_reset_calc(
                Device_blk.Str.WP_reset
                , Device_blk.Str.K_WP_rst_heating
                , g_gld.thermo.Temp_Aver
                , Device_blk.Str.TemperNormal
                );
	  			
	  			plc_transiton = TRANS_HEATING;
	  			plc_reset32 = g_gld.thermo.WP_reset_heating << PLC_SHIFT;;

				Device_blk.Str.HF_scl = Device_blk.Str.HF_scl_2;
	  		} else if ((WP_reg32 < (Device_blk.Str.WP_rdw << PLC_SHIFT)) && !g_gld.thermo.IsHeating)	{
                //e. cooling
	  			is_zeroing = 1;
				//e. voltage of reset at cooling 
				g_gld.thermo.WP_reset_cooling = CPL_reset_calc(
                    Device_blk.Str.WP_reset2
                    , Device_blk.Str.K_WP_rst_cooling
                    , g_gld.thermo.Temp_Aver
                    , Device_blk.Str.TemperNormal
                    );
  			
	  			plc_transiton = TRANS_COOLING;
	  			plc_reset32 = g_gld.thermo.WP_reset_cooling << PLC_SHIFT;

				Device_blk.Str.HF_scl = Device_blk.Str.HF_scl_2;
			} else { 
                //e. thresholds are not exceeded, normal operation of regulator 
				WP_reg32 = L_mac(WP_reg32, phase_Digital, Device_blk.Str.WP_scl );
            }

	} else { 
        //e. flag is set (1) - reset mode
  			
		if (plc_transiton != FINISHED) {
			if (plc_transiton == TRANS_HEATING) {
				
				WP_reg32 = L_sub(WP_reg32, Device_blk.Str.WP_transition_step); // WP_reg32 -= Device_blk.Str.WP_transition_step;
				if (WP_reg32 < plc_reset32) {
		  			zero_delay = 0;
		  			plc_transiton = FINISHED; //r.false;
	  				WP_reg32 = plc_reset32;
				}
			} else {
                // plc_transiton == TRANS_COOLING
				WP_reg32 = L_add(WP_reg32, Device_blk.Str.WP_transition_step); // WP_reg32 += Device_blk.Str.WP_transition_step;
				if (WP_reg32 > plc_reset32) {
		  			zero_delay = 0;
		  			plc_transiton = FINISHED; //r.false;
	  				WP_reg32 = plc_reset32;
				}
			}
		} else {
            if (zero_delay < Device_blk.Str.WP_mdy) {
                zero_delay++;
            } else {
                //e. resetting was completed 
                is_zeroing = 0;
            }
        }
	}
    //e. the minimum corresponds to a small negative number, appropriate to PLC_RESET_THRESHOLD
    Saturation(WP_reg32, WP_REG32MAX_SATURATION, WP_REG32MIN_NEW_SATURATION);  
    
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

	//e. integartion of output of the PD of the CPLC regulator for the technological output on the Rate command
	Output.Str.WP_pll = WP_PhaseDetectorRate( WP_Phase_Det, g_gld.time_1_Sec); 
}

/******************************************************************************/
int cplc_calc_modulator(void)
{
	static int index = 0;
    int val = 0;
    int val_noise = 0;
	index++;
    
    //TODO for test temporarly recalculate sin in main loop
    //calc_sin_func();
    
    //modulator ampl
    if (Device_blk.Str.PI_a4 == 0) {
		Device_blk.Str.PI_a4 = PI_A4_CONST;
	}

    //current array index
	if (index >= Device_blk.Str.PI_b3)
		index = 0;

    val = (int)(g_sin_func[index]*Device_blk.Str.PI_a4) 
        + (PI_A4_MAX - Device_blk.Str.PI_a4);
    
    //modulator noise
    /*if(0){
        val_noise = Device_blk.Str.PI_a4*rand();
        val *= val_noise;
        if (val == 32767){
            val -=(val << 1);
        }
    }*/
    
    //Modulator
    hardware_modulator(val);
		 
	return (val);
} 


/******************************************************************************/
