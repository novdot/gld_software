/**
  ******************************************************************************
  * @file    dither.c
  *
  * @brief   Вибропривод
  *
  * @author  Дмитрий Новиков novikov@elektrooptika.ru
  *
  ******************************************************************************
  * @attention
  *
  * в разработке
  *
  * <h2><center>&copy; 2020 Электрооптика</center></h2>
  ******************************************************************************
  */
#include "core/dither.h"
#include "hardware/hardware.h"

#include "core/global.h"

//TODO
#include "mathDSP.h"
#include "CyclesSync.h"
#include "CntrlGLD.h"
#include "stdlib.h"
#include "sip.h"

x_int32_t RI_diff; //e.input signal of "recovery" APS
x_int32_t MaxDelay;
x_int32_t VB_tau_Ins; //e. local value of the Tau regulator  


int32_t VB_Nmin0; //e. minimum of the output value of a regulator of the period for the Device_blk.Str.TemperNormal temperature
int32_t VB_Nmax0; //e. maximum of the output value of a regulator of the period for the Device_blk.Str.TemperNormal 

uint32_t SwitchCntInq = 0;

int32_t accum_error = 0;
int32_t ph_error = 0;
int32_t accum_error_old = 0;
int32_t PhaseShift;
int32_t temp2;
int32_t temp3;
/******************************************************************************/
void init_Dither_reg()
{
  init_VibroReduce();
  Device_blk.Str.VB_N = 29538;
  VibroDither_Init();
  VibroDither_SwitchOn();
  init_BandPass(1.0/(float)Vibro_Filter_Aperture, 100.0/(float)DEVICE_SAMPLE_RATE_HZ, DUP);	
  //e. maximal delay of the meander of the dither drive
  MaxDelay = Vibro_Filter_Aperture >> 1; 
  CounterIquiryCycle_Init((Device_blk.Str.VB_N*Vibro_2_CountIn)>>SHIFT_C_7680_12500);  
}

/******************************************************************************/
void VibroDither_Init()
{
    pwm_init(Device_blk.Str.VB_N,Device_blk.Str.VB_tau);
    
    VB_tau_Ins = Device_blk.Str.VB_tau;

    Output.Str.L_Vibro = Device_blk.Str.VB_tau;  //to update the period and pulse duration for displaying
    Output.Str.T_Vibro = Device_blk.Str.VB_N;  
    return;
}
/******************************************************************************/
void clc_Noise_regulator(void)
{
    int temp;
	static x_uint32_t Flag = 0;
	static int PeriodCount = 0, Tnoise = 0;

	if ( PeriodCount >= Tnoise ){
		PeriodCount = 0;
		srand(Device_blk.Str.VB_N);
		Tnoise = add( Device_blk.Str.VBN_Tzd, mult_r(Device_blk.Str.VBN_Ran, rand())); 
		//e. calculation +dF/-dF 
        if ( Flag ) {
			temp = Device_blk.Str.VBN_k;
		} else {
			temp = -Device_blk.Str.VBN_k;
		}

		Device_blk.Str.VB_tau = add(VB_tau_Ins, (mult_r( VB_tau_Ins, temp ) << 1)); // VB_tau = VB_tau_Ins + VB_tau_Ins * temp; with saturation
		Saturation(Device_blk.Str.VB_tau, Device_blk.Str.VB_Tmax, Device_blk.Str.VB_Tmin);	//e. checking upper and lower levels of control range                      	
		Flag = !Flag;                
	} else {
		PeriodCount++;
	}
}
/******************************************************************************/
void clc_OutFreq_regulator(void)
{
    static int out_freq_sum = 0;
    static int temp = 0;
    static int temp_pure = 0;

    if(Dif_Curr_Vib>0)
        out_freq_sum += Dif_Curr_Vib;
    else
        out_freq_sum -= Dif_Curr_Vib;

    //e. second has elapsed, fix the output frequency value 
    if (time_1_Sec == DEVICE_SAMPLE_RATE_uks) {
        //e. the regulator loop is closed
        if (loop_is_closed(VB_TAU_ON)) {
            temp = Device_blk.Str.VB_Fdf_Hi << 16;
            temp |= Device_blk.Str.VB_Fdf_Lo;
            //memorize pure temo before 
            //temp_pure = temp;
            // (out_freq_sum - temp) with saturation, then >> 3
            temp = L_sub(out_freq_sum, temp) >> 3;
            // error saturation if error is out of range
            Saturation(temp, 32767, -32768);
            //e. scaling and summing with rounding and saturation 
            VB_tau_Ins = mac_r( 
                    VB_tau_Ins << (16 - DITH_VB_TAU_SHIFT)
                    , temp
                    , Device_blk.Str.VB_Fsc 
                    ); // << DITH_VB_TAU_SHIFT; 
            //e. reduction the VB_Err value to 16 digits (arithmetic right shift to 3 digits) 

            //e. checking upper and lower levels in sign range
            Saturation(
                VB_tau_Ins
                , (int)Device_blk.Str.VB_Tmax >> DITH_VB_TAU_SHIFT
                , (int)Device_blk.Str.VB_Tmin >> DITH_VB_TAU_SHIFT
            );	
            VB_tau_Ins <<= DITH_VB_TAU_SHIFT;
        }

        //e. once more divide output frequency by 2, in order to coincide with frequency meter 
        Output.Str.F_ras = out_freq_sum >> 5;
        //e. reset accumulated values for next cycle of measurement
        out_freq_sum = 0;
        
        // cyclic built-in test
        // if output frequency is less than 3/4 of nominal then data is invalid
        if (Output.Str.F_ras < ((temp >> 7)*3)) {
        //if (Output.Str.F_ras < (((int)(temp_pure >> 5)/3)*2))  {
            Valid_Data |= OUT_FREQ_ERROR;
        } else {
            Valid_Data &= ~OUT_FREQ_ERROR;
        } 
    }

    clc_Noise_regulator();

    //e.  is stabilization regulator switched on?  
    if ( loop_is_closed(VB_TAU_ON) ) {
        //e. otherwise, load new value of pulse width of the dither drive
        Output.Str.L_Vibro = Device_blk.Str.VB_tau; 
    }
}

/******************************************************************************/
void clc_Dith_regulator(void)
{	
    static int dith_period = 0;
    RI_diff = DUP_Filt(Dif_Curr_Vib<<2);

	if (RI_diff >= 0)
        ph_error = 1;	   			
	else	
        ph_error = 0;

    //vibro pulse has been formed
    if(pwm_is_pulse_was_formed()==_x_true) {
        if( pwm_pulse_calc( 
                Output.Str.T_Vibro
                ,  Output.Str.L_Vibro
                ,  Vibro_2_CountIn
                ,  SwitchCntInq
            )==_x_true) {
            if (SwitchCntInq) {
                SwitchCntInq = 0;
            }
            In_Flag = 0;
        } else {
            In_Flag = 1;
            dith_period++; 
        }
    }
    //e. outgoing of the delayed menader signal 
    temp3 = VB_MeanderDelay(In_Flag, Device_blk.Str.VB_phs, MaxDelay); 
    temp2 = ( ( temp3 ^ ph_error ) << 1 ) - 1; //e. the PD XOR analog out (-1..+1, since const=1) 
    accum_error += temp2; 

    //e. outgoing of the integrated for 1 Sec analog signal of the PD of the dither drive 
	Output.Str.T_VB_pll = VB_PhaseDetectorRate(temp2, time_1_Sec); 
    //e. checking status of the dith_period counter  
    if ( dith_period > DITHER_REG_PERIOD ) {  
        dith_period = 0; //e. 40 periods - resetting the counter of dither drive periods                     
        //e. scaling and summing with rounding and saturation 
        if ( loop_is_closed(VB_FREQ_ON) ) {	
            Device_blk.Str.VB_N = mac_r(
                    Device_blk.Str.VB_N << (16 - DITH_VBN_SHIFT)
                    , -accum_error
                    , Device_blk.Str.VB_scl
                ) << DITH_VBN_SHIFT;
               
            //e. checking upper and lower levels of control range
            Saturation(Device_blk.Str.VB_N, Device_blk.Str.VB_Nmax, Device_blk.Str.VB_Nmin);	                
            //e. resetting the _VB_Uab40 sum 
            accum_error = 0;
        }
    }

    //e. it was, check the activation of the stabilization regulator 
    if ( loop_is_closed(VB_FREQ_ON) ) {
        Output.Str.T_Vibro = Device_blk.Str.VB_N;  
        //e. has switched on, load calculated values of period 
        pwm_set_period(Output.Str.T_Vibro);
        //e. enable loading counter inquiry timer at the next vibro halfperiod
        SwitchCntInq = 1;  
    }
	// cyclic built-in test
	if (
        (Output.Str.T_Vibro > Device_blk.Str.VB_Nmax) 
        || (Output.Str.T_Vibro < Device_blk.Str.VB_Nmin)
    ) {
		Valid_Data |= DITH_FREQ_ERROR;
	}
}

/******************************************************************************/
void VibroDither_Set()
{
    Device_blk.Str.VB_N = Output.Str.T_Vibro; 
    pwm_set_period(Output.Str.T_Vibro);
    //to enable inquiry timer reloading
    SwitchCntInq = 1;	 
}
/******************************************************************************/
void VibroDither_SwitchOn()
{
    pwm_on();
}
/******************************************************************************/
void VibroDither_SwitchOff()
{
    pwm_off();
}
/******************************************************************************/
int VB_MeanderDelay(int VB_Meander, int Delay100uS, int MaxDly)
{
	static int poz_counter = 0, neg_counter = 0, flg_delay;
	
	if (Delay100uS == 0) {
		return (VB_Meander);
	}
	
	if (Delay100uS > 0) {
		if (Delay100uS > MaxDly) { Delay100uS = MaxDly; }
        //e. outgoing WP_flg flag, which delayed by the WP_ref 
		if (VB_Meander){
	  		neg_counter = 0;
	  		poz_counter++;
		} else {
	  		poz_counter = 0;
	  		neg_counter++;
		}
		if (poz_counter == Delay100uS) { flg_delay = 1; }
		if (neg_counter == Delay100uS) { flg_delay = 0; }
	}else{
		Delay100uS = -Delay100uS;
		if (Delay100uS > MaxDly) { Delay100uS = MaxDly; }
        //e. outgoing WP_flg flag, which delayed by the WP_ref
		if (VB_Meander) {
	  		neg_counter = MaxDly + 1;
	  		poz_counter--;
		} else {
	  		poz_counter = MaxDly + 1;
	  		neg_counter--;
		}
		if (poz_counter == Delay100uS) { flg_delay = 0; }
		if (neg_counter == Delay100uS) { flg_delay = 1; }
	}
	return (flg_delay);
}
/******************************************************************************/
int VB_PhaseDetectorRate(int PhaseDetInput, int IntegrateTime) 
{
	static int SampleAndHoldOut = 0, VB_PhasDet_integr = 0;
	
	if (IntegrateTime == DEVICE_SAMPLE_RATE_uks) {
		SampleAndHoldOut = VB_PhasDet_integr;
		VB_PhasDet_integr = 0;
	} else {	
		VB_PhasDet_integr += PhaseDetInput;
	}
	return (SampleAndHoldOut);
}
/******************************************************************************/
/******************************************************************************/
