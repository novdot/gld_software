/**
14 точек (то есть 13 отрезков) диапазона температур.
Для каждого отрезка своя коррекция.
На переходах, для того чтобы убрать ступеньки, используем интерполяцию

Система нужна для предсказания работы прибора в части подсчета импульсов
вне зависимости от темп. в определенном диапазоне.
*/
#include "core/thermo.h"
#include "core/global.h"
#include "core/const.h"
#include "core/math_dsp.h"

#include "math.h"
#include "stdlib.h"

#define	TEMP_AVER_PERIOD	4 // e. number of seconds for average
#define TSENS_NUMB          4 //e. number of the temperature sensor used for the thermocompensation
#define TS_DELTA_NUMB          5 //e. number of the temperature sensor used for the thermocompensation

float	g_nTermoCompDelta = 0; 
float	g_nTermoCompDelta_dNdT = 0; 

#define	TEMPERFILT_LEN	4 
#define	TEMPERFILT_SHIFT 2
int buffer_dT[TEMPERFILT_LEN];
long int smooth_dT;
/******************************************************************************/
void thermo_Max_Saturation(unsigned *lvl, unsigned limit)
{
	if (*lvl>limit) *lvl = limit;	
}


/******************************************************************************/
float thermo_StaticTermoCompens(int temperature)
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
float thermo_DynamicTermoCompens(void)
{
	float x = g_nTermoCompDelta + g_nTermoCompDelta_dNdT;
    return LONG_2_FRACT_14_18(x);
    
    //return (g_nTermoCompDelta + g_nTermoCompDelta_dNdT);
}

/******************************************************************************/
void thermo_DynamicDeltaCalc() 
{
	int i, t = 0;

	t = g_gld.thermo.Temp_Aver;

    //if (g_gld.thermo.IsHeating){
		if(t > Device_blk.Str.TemperIntDyn[TERMO_FUNC_SIZE - 1]) {
			t = Device_blk.Str.TemperIntDyn[TERMO_FUNC_SIZE - 1];
		}
	
		i = 0;
	   	while( t > Device_blk.Str.TemperIntDyn[i] ) i++;
	
    	g_nTermoCompDelta = ( 
            Device_blk.Str.ThermoHeatDelta[i] 
            - g_gld.thermo.dThermoHeatDeltaPer_dTermo[i] * (float)( Device_blk.Str.TemperIntDyn[i] - t ) 
            );
    /*}else{
		if(t > Device_blk.Str.TemperCoolIntDyn[TERMO_FUNC_SIZE - 1]) {
			t = Device_blk.Str.TemperCoolIntDyn[TERMO_FUNC_SIZE - 1];
		}
	
		i = 0;
	   	while( t > Device_blk.Str.TemperCoolIntDyn[i] ) i++;
	
    	TermoCompDelta = ( 
            Device_blk.Str.ThermoCoolDelta[i] 
            - g_gld.thermo.dThermoCoolDeltaPer_dTermo[i] * (float)( Device_blk.Str.TemperCoolIntDyn[i] - t ) 
            );
    }  */
} 

/******************************************************************************/
void thermo_DithFreqRangeCalc(void)
{
	unsigned int min_level = 0, max_level = 0;
	static unsigned int VB_Nmin0 = 0, VB_Nmax0 = 0;
	
	int delta_VB_N;
	
	if(VB_Nmin0==0){
		VB_Nmin0 = Device_blk.Str.VB_Nmin;
		VB_Nmax0 = Device_blk.Str.VB_Nmax;
	}
	
	delta_VB_N = mult_r(
        Device_blk.Str.K_vb_tu >> DITH_VBN_SHIFT
        , (g_gld.thermo.Temp_Aver - Device_blk.Str.TemperNormal)
    );
	min_level = VB_Nmin0 + delta_VB_N;
	max_level = VB_Nmax0 + delta_VB_N;
	// maximum saturation for unsigned levels  
    // min should be always less then max_level by 1
	thermo_Max_Saturation(&min_level, ((unsigned int)0xFFFF >> DITH_VBN_SHIFT)-1);
	thermo_Max_Saturation(&max_level, ((unsigned int)0xFFFF >> DITH_VBN_SHIFT));
	Device_blk.Str.VB_Nmin = min_level << DITH_VBN_SHIFT;
	Device_blk.Str.VB_Nmax = max_level << DITH_VBN_SHIFT;
}

/******************************************************************************/
void thermo_init()
{
    int i = 0;
    int i_buff = 0;
    int dT, dT_dyn;
	
//r.x	TermoCompHeatingDelta = -0.001000; //r. коэфф., при котором за 1 сек накапливается 10 импульс
//r.x	TermoCompCoolingDelta = +0.001000; //r. коэфф., при котором за 1 сек накапливается 10 импульс
	
    //test
    Device_blk.Str.TermoMode = TERMO_ON;
    for (i = 1; i < TERMO_FUNC_SIZE; i++)  {
        Device_blk.Str.TermoFunc[i] = 100*i;
        Device_blk.Str.ThermoHeatDelta[i] = 100*i;
        Device_blk.Str.ThermoCoolDelta[i] = 0*i;
		Device_blk.Str.TemperInt[i] = 100*i; 
    }
    
	Device_blk.Str.TermoFunc[0] = (Device_blk.Str.TermoFunc[0]);
    Device_blk.Str.ThermoHeatDelta[0] = (Device_blk.Str.ThermoHeatDelta[0]);
    Device_blk.Str.ThermoCoolDelta[0] = (Device_blk.Str.ThermoCoolDelta[0]);
	for (i = 1; i < TERMO_FUNC_SIZE; i++)  {
		Device_blk.Str.TermoFunc[i] = (Device_blk.Str.TermoFunc[i]);
		Device_blk.Str.ThermoHeatDelta[i] = (Device_blk.Str.ThermoHeatDelta[i]);
		Device_blk.Str.ThermoCoolDelta[i] = (Device_blk.Str.ThermoCoolDelta[i]);
		dT = Device_blk.Str.TemperInt[i] - Device_blk.Str.TemperInt[i-1]; 
		if (dT == 0) {
            //r. защита от деления на 0
            g_gld.thermo.dFuncPer_dTermo[i] = 0.0; 
        } else {	
			g_gld.thermo.dFuncPer_dTermo[i] = (Device_blk.Str.TermoFunc[i] - Device_blk.Str.TermoFunc[i-1]) / (float)dT;
        }
        
		dT_dyn = Device_blk.Str.TemperIntDyn[i] - Device_blk.Str.TemperIntDyn[i-1];
		if (dT_dyn == 0) {
            g_gld.thermo.dThermoHeatDeltaPer_dTermo[i] = 0.0;
        } else {
            g_gld.thermo.dThermoHeatDeltaPer_dTermo[i] = 
                (Device_blk.Str.ThermoHeatDelta[i] - Device_blk.Str.ThermoHeatDelta[i-1]) 
                / (float)dT_dyn;
        }
        
		dT_dyn = Device_blk.Str.TemperCoolIntDyn[i] - Device_blk.Str.TemperCoolIntDyn[i-1];       
		if (dT_dyn == 0) {
            g_gld.thermo.dThermoCoolDeltaPer_dTermo[i] = 0.0;
        } else {
            g_gld.thermo.dThermoCoolDeltaPer_dTermo[i] = 
                (Device_blk.Str.ThermoCoolDelta[i] - Device_blk.Str.ThermoCoolDelta[i-1]) 
                / (float)dT_dyn;
        }
    }
    
    //Filter
    buffer_dT[0] = 0;	
    
    //e. smooth average initialization 
    //r. инициализация температурного фильтра скользящего среднего
    for (i_buff = 1; i_buff < TEMPERFILT_LEN; i_buff++) buffer_dT[i_buff] = 0;
    
    smooth_dT = 0;//Temp_Aver2 << TEMPERFILT_SHIFT;

}
/******************************************************************************/
int thermo_MovAver_filter_init(int Temp_Aver)
{
    int t = Temp_Aver;
    int i_buff = 0;

    if(t > Device_blk.Str.TemperCoolIntDyn[TERMO_FUNC_SIZE - 1]) {
        t = Device_blk.Str.TemperCoolIntDyn[TERMO_FUNC_SIZE - 1];
    }

    while( t > Device_blk.Str.TemperCoolIntDyn[i_buff] ) i_buff++;

    Device_blk.Str.Reserved2 = 
        Device_blk.Str.ThermoCoolDelta[i_buff] 
        - g_gld.thermo.dThermoCoolDeltaPer_dTermo[i_buff] * ( Device_blk.Str.TemperCoolIntDyn[i_buff] - t );
}
/******************************************************************************/
//moving average for termocorrection
int thermo_MovAver_filter(int Input)
{
    static unsigned i_dT = 0;

    smooth_dT -= buffer_dT[i_dT];
    buffer_dT[i_dT] = Input;
    smooth_dT += Input;

    i_dT++;
    i_dT &= (TEMPERFILT_LEN-1);

    //shift on additional 6 bits for smoothing 2^TEMPERFILT_SHIFT = TEMPERFILT_LENGTH 
    return ((int)(smooth_dT>>TEMPERFILT_SHIFT));	
} 
/******************************************************************************/
void thermo_clc_ThermoSensors(void)	
{
	//unsigned i = 0
	static int TS_sum = 0; //аккумулятор T_main
	static int TS_sum_delta = 0; //аккумулятор T_delta
	static int seconds_aver = 0, TenSeconds = 0;
	static int Temp_AverPrevDynCalc = -7000;
	static x_bool_t StartRdy = _x_true;
	static int Temp_Aver_prev = -7000;
	static int TempEvolution = 0;
    static x_bool_t bSingleInit = _x_true;

	//e. conversion of temperature values on ADC output 
	//to range -32768 .. +32767 ( additional code; format 1.15 )
	Output.Str.Tmp_Out[TSENS_NUMB] = mac_r(Device_blk.Str.Tmp_bias[TSENS_NUMB] << 16
								, (g_input.word.temp1 - 0x8000)
								, Device_blk.Str.Tmp_scal[TSENS_NUMB]
                                );
    
	Output.Str.Tmp_Out[TS_DELTA_NUMB] = mac_r(Device_blk.Str.Tmp_bias[TS_DELTA_NUMB] << 16
                                , (g_input.word.delta_t - 0x8000)
                                , Device_blk.Str.Tmp_scal[TS_DELTA_NUMB]
                                );
    

    //TODO for tests
	//Output.Str.Tmp_Out[TSENS_NUMB] = 1000;
	
	//e. 1 second elapsed
	if (g_gld.time_1_Sec == DEVICE_SAMPLE_RATE_uks) {
		seconds_aver++;
	}

	//e. TEMP_AVER_PERIOD second elapsed 
	if (seconds_aver > TEMP_AVER_PERIOD) {
		seconds_aver = 0;
		TenSeconds++;
		//e. save the previous mean temperature for 1 Sec
		Temp_Aver_prev = g_gld.thermo.Temp_Aver;		 
		//e. calculating mean temperature for 1 Sec 
		//r. вычисляем среднюю температуру за секунду
		g_gld.thermo.Temp_Aver = TS_sum / (DEVICE_SAMPLE_RATE_HZ * TEMP_AVER_PERIOD); 

		/*if (g_gld.thermo.Temp_Aver > Temp_Aver_prev){
			TempEvolution++;
		}else if (g_gld.thermo.Temp_Aver < Temp_Aver_prev){
			TempEvolution--;
		}*/
        
        if(TenSeconds>2){
            if(bSingleInit){
                thermo_MovAver_filter_init(g_gld.thermo.Temp_Aver);
                bSingleInit = _x_false;
            }
        }
        
        if(
            (Device_blk.Str.TermoMode!=TERMO_OFF)
            &&(bSingleInit==_x_false)
        ){
            switch (Device_blk.Str.TermoMode & 0xF0) {
                case 0x00:
                    g_nTermoCompDelta_dNdT = Device_blk.Str.Reserved2 * 
                        (float)(thermo_MovAver_filter((g_gld.thermo.Temp_Aver-Temp_Aver_prev)<<4));
                break;
            
                case 0x10:
                    //for TS_DELTA_NUMB
                    /*g_nTermoCompDelta_dNdT = Device_blk.Str.Reserved2 * 
                        (float)(thermo_MovAver_filter(Temp_Aver2<<4));*/
                break;
            }
        }        

		//e. reset the sum for calculation of an mean
		TS_sum = 0; 
        TS_sum_delta = 0;
	}else{
		TS_sum += Output.Str.Tmp_Out[TSENS_NUMB];
        TS_sum_delta += Output.Str.Tmp_Out[TS_DELTA_NUMB];		
	}

	// 10 * TEMP_AVER_PERIOD = 40
	/*if (TenSeconds == 10) {
		TenSeconds = 0;
		if (TempEvolution > 0){
			g_gld.thermo.IsHeating = 1;
		}else if (TempEvolution < 0){
			g_gld.thermo.IsHeating = 0;
		}
		TempEvolution = 0;
	}*/

	//e. single calculaiton of some device parameters 
	//(measurement on the VALID_START_SEC  second after start)
	if (StartRdy==_x_true){
		if (TenSeconds > VALID_START_4SEC){
			StartRdy = _x_false;	
			//TempOfReset = Temp_Aver;
			Device_blk.Str.WP_scl >>= 1;
			Device_blk.Str.HF_scl >>= 1;

			//e. static thermocompensation is enable
			if ((Device_blk.Str.TermoMode != TERMO_OFF) && \
					(Device_blk.Str.TermoMode != TERMO_ON_DYNAMIC_ONLY) && \
					(Device_blk.Str.TermoMode != TERMO_ON_DYNAMIC_ONLY_NUMB_OFF)) {
				//e. calculation of the static component of the thermocompensation	
				//e. starting temperature of the device    
				g_gld.thermo.StartTermoCompens = thermo_StaticTermoCompens(g_gld.thermo.Temp_Aver); 			
			} 			
			thermo_DynamicDeltaCalc();

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
	//r. расчет средней за 1 секунду температуры датчиков T4, T5
	if ( abs(g_gld.thermo.Temp_Aver - Temp_AverPrevDynCalc) > Device_blk.Str.DeltaTempRecalc){
		Temp_AverPrevDynCalc = g_gld.thermo.Temp_Aver;
		thermo_DynamicDeltaCalc();
	}

	// cyclic built-in test
	if ( 
			(Output.Str.Tmp_Out[TSENS_NUMB] < TS_MIN) 
			|| (Output.Str.Tmp_Out[TSENS_NUMB] > TS_MAX) 
	) {
		Valid_Data |= THERMO_RANGE_ERROR;
	}else{
		Valid_Data &= ~THERMO_RANGE_ERROR;
	}

	if ( Output.Str.Tmp_Out[TS_DELTA_NUMB] > TS_DIFF_MAX ) {
		Valid_Data |= THERMO_DIFF_ERROR;
	}else{
		Valid_Data &= ~THERMO_DIFF_ERROR;
	}		
}

