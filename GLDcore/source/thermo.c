#include "core/thermo.h"
#include "core/global.h"
#include "core/const.h"

#include "math.h"

//TODO
#include "mathDSP.h"

#define	TEMP_AVER_PERIOD	4 // e. number of seconds for average

/******************************************************************************/
void Max_Saturation(unsigned *lvl, unsigned limit)
{
	if (*lvl>limit) *lvl = limit;	
}

/******************************************************************************/
int StaticTermoCompens(int temperature)
{
    //e. the value of the thermocompensation for 1 device period (100 uSec) 
	float TermoCompens_Curr; 
	int i, t;					
		
	t = temperature;
    if(t > Device_blk.Str.TemperInt[TERMO_FUNC_SIZE - 1]){
        t = Device_blk.Str.TemperInt[TERMO_FUNC_SIZE - 1];
    }
    
    i = 0;
    while( t > Device_blk.Str.TemperInt[i] ) i++;

    //r. Tmp_Out[5] = i;
    TermoCompens_Curr = 
        Device_blk.Str.TermoFunc[i] 
        - g_gld.thermo.dFuncPer_dTermo[i] * (float)( Device_blk.Str.TemperInt[i] - t );
			
	return TermoCompens_Curr;
}

/******************************************************************************/
int DynamicDeltaCalc() 
{
	int i, t = 0;
    int	TermoCompDelta = 0; 

	t = g_gld.thermo.Temp_Aver;

    if (g_gld.thermo.IsHeating){
		if(t > Device_blk.Str.TemperIntDyn[TERMO_FUNC_SIZE - 1])
		{
			t = Device_blk.Str.TemperIntDyn[TERMO_FUNC_SIZE - 1];
		}
	
		i = 0;
	   	while( t > Device_blk.Str.TemperIntDyn[i] ) i++;
	
    	TermoCompDelta = ( 
            Device_blk.Str.ThermoHeatDelta[i] 
            - g_gld.thermo.dThermoHeatDeltaPer_dTermo[i] * (float)( Device_blk.Str.TemperIntDyn[i] - t ) 
            );
    }else{
		if(t > Device_blk.Str.TemperCoolIntDyn[TERMO_FUNC_SIZE - 1])
		{
			t = Device_blk.Str.TemperCoolIntDyn[TERMO_FUNC_SIZE - 1];
		}
	
		i = 0;
	   	while( t > Device_blk.Str.TemperCoolIntDyn[i] ) i++;
	
    	TermoCompDelta = ( 
            Device_blk.Str.ThermoCoolDelta[i] 
            - g_gld.thermo.dThermoCoolDeltaPer_dTermo[i] * (float)( Device_blk.Str.TemperCoolIntDyn[i] - t ) 
            );
    }  
	return TermoCompDelta; 
} 

/******************************************************************************/
void clc_ThermoSensors(void)	
{
	unsigned i;
	static int TS_sum = 0;
	static int seconds_aver = 0, TenSeconds = 0;
	static int Temp_AverPrevDynCalc = -7000;
	static int StartRdy = 1;
	static int PrevTemp = -7000;
    static int TempEvolution = 0;
    int	StartTermoCompens = 0; //н6ачальная термокомпенсация

	/*for (i=0; i<2; i++)
	{
		//e. conversion of temperature values on ADC output 
		//e. to range -32768 .. +32767 ( additional code; format 1.15 )
        /*Output.Str.Tmp_Out[i] = mac_r(Device_blk.Str.Tmp_bias[i] << 16,
												(Input.StrIn.Tmp_in[i] - 0x8000), 
												Device_blk.Str.Tmp_scal[i]);
		Output.Str.Tmp_Out[i+4] = Input.StrIn.Tmp_in[i];																	
	}*/
    //#NDA 20200304 temp
    Output.Str.Tmp_Out[0] = g_input.word.temp1 /2;
    Output.Str.Tmp_Out[1] = g_input.word.hf_out /2;
    Output.Str.Tmp_Out[2] = g_input.word.wp_sel /2;
    Output.Str.Tmp_Out[3] = g_input.word.delta_t /2;
    
    Output.Str.Tmp_Out[4] = g_input.word.in1 /2;
    Output.Str.Tmp_Out[5] = g_input.word.in2 /2;

    //e. 1 second elapsed
	if (time_1_Sec == DEVICE_SAMPLE_RATE_uks) {
		seconds_aver++;
#if !defined DEBUG
		//ResetToResetInSeconds++;
#endif
	}

    //e. TEMP_AVER_PERIOD second elapsed 
	if (seconds_aver > TEMP_AVER_PERIOD) {
		seconds_aver = 0;
		TenSeconds++;
        //e. save the previous mean temperature for 1 Sec
		PrevTemp = g_gld.thermo.Temp_Aver;		 
        //e. calculating mean temperature for 1 Sec 
		g_gld.thermo.Temp_Aver = TS_sum / (DEVICE_SAMPLE_RATE_HZ * TEMP_AVER_PERIOD); 

		if (g_gld.thermo.Temp_Aver > PrevTemp){
			TempEvolution++;
		}else if (g_gld.thermo.Temp_Aver < PrevTemp){
			TempEvolution--;
		}

        //e. reset the sum for calculation of an mean
		TS_sum = 0; 
	}else{
		TS_sum += Output.Str.Tmp_Out[TSENS_NUMB];		
	}
	
    // 10 * TEMP_AVER_PERIOD = 40
	if (TenSeconds == 10) {
		TenSeconds = 0;
		if (TempEvolution > 0){
			g_gld.thermo.IsHeating = 1;
		}else if (TempEvolution < 0){
			g_gld.thermo.IsHeating = 0;
		}
		TempEvolution = 0;
	}	

	//e. single calculaiton of some device parameters 
    //(measurement on the VALID_START_SEC  second after start)
	if (StartRdy){
		if (TenSeconds > VALID_START_4SEC){

			StartRdy = 0;	
            //TempOfReset = Temp_Aver;
			//	Device_blk.Str.WP_scl >>= 1;
          	//	Device_blk.Str.HF_scl >>= 1;

            //e. static thermocompensation is enable
			if ((Device_blk.Str.TermoMode != TERMO_OFF) && \
				(Device_blk.Str.TermoMode != TERMO_ON_DYNAMIC_ONLY) && \
				(Device_blk.Str.TermoMode != TERMO_ON_DYNAMIC_ONLY_NUMB_OFF)) 		
            {
                //e. calculation of the static component of the thermocompensation	
                //e. starting temperature of the device    
                StartTermoCompens = StaticTermoCompens(g_gld.thermo.Temp_Aver); 			
            } 			
			DynamicDeltaCalc();
			
			//e. calculation of range for dither drive frequency, 
            //depending on starting temperature
            //DithFreqRangeCalc();
				
			//e. voltage of reset at heating 
			g_gld.thermo.WP_reset_heating = CPL_reset_calc(
                Device_blk.Str.WP_reset
                , Device_blk.Str.K_WP_rst_heating
                , g_gld.thermo.Temp_Aver
                , Device_blk.Str.TemperNormal
            );
			//e. voltage of reset at cooling 
			g_gld.thermo.WP_reset_cooling = CPL_reset_calc(
                Device_blk.Str.WP_reset2
                , Device_blk.Str.K_WP_rst_cooling
                , g_gld.thermo.Temp_Aver
                , Device_blk.Str.TemperNormal
            );		
		}
	}	

    //e. calculation the mean temperature for 1 Sec for T4 and T5 sensors 
	if ( abs(g_gld.thermo.Temp_Aver - Temp_AverPrevDynCalc) > Device_blk.Str.DeltaTempRecalc){
		Temp_AverPrevDynCalc = g_gld.thermo.Temp_Aver;
		DynamicDeltaCalc();
	}

	// cyclic built-in test
	if ( (Output.Str.Tmp_Out[4] < TS_MIN) || (Output.Str.Tmp_Out[4] > TS_MAX) || (Output.Str.Tmp_Out[5] < TS_MIN) || (Output.Str.Tmp_Out[5] > TS_MAX) )
	{
		Valid_Data |= THERMO_RANGE_ERROR;
	}

	if ( abs( Output.Str.Tmp_Out[4] - Output.Str.Tmp_Out[5]) > TS_DIFF_MAX)
	{
		Valid_Data |= THERMO_DIFF_ERROR;
	}		
} // clc_ThermoSensors


/******************************************************************************/
void DithFreqRangeCalc(void)
{
	unsigned int min_level, max_level;
	int delta_VB_N;
	
	delta_VB_N = mult_r(
        Device_blk.Str.K_vb_tu >> DITH_VBN_SHIFT
        , (g_gld.thermo.Temp_Aver - Device_blk.Str.TemperNormal));
	min_level = VB_Nmin0 + delta_VB_N;
	max_level = VB_Nmax0 + delta_VB_N;
	// maximum saturation for unsigned levels  
    // min should be always less then max_level by 1
	Max_Saturation(&min_level, ((unsigned int)0xFFFF >> DITH_VBN_SHIFT)-1);
	Max_Saturation(&max_level, ((unsigned int)0xFFFF >> DITH_VBN_SHIFT));
	Device_blk.Str.VB_Nmin = min_level << DITH_VBN_SHIFT;
	Device_blk.Str.VB_Nmax = max_level << DITH_VBN_SHIFT;
}
