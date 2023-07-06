/**
14 точек (то есть 13 отрезков) диапазона температур.
Для каждого отрезка своя коррекция.
На переходах, для того чтобы убрать ступеньки, используем интерполяцию

Система нужна для предсказания работы прибора в части подсчета импульсов
вне зависимости от темп. в определенном диапазоне.
*/
#include "core/thermo.h"
#include "core/global.h"
#include "core/math_dsp.h"
#include "stdlib.h"

#define	TEMP_AVER_PERIOD	4 // e. number of seconds for average
#define TSENS_NUMB        4 //e. number of the temperature sensor used for the thermocompensation
#define TS_DELTA_NUMB     5 //e. number of the temperature sensor used for the thermocompensation

#define	TEMPERFILT_LEN	4 
#define	TEMPERFILT_SHIFT 2
#define TERMO_SCALE(t, n)  \
    mac_r(Device_blk.Str.Tmp_bias[n] << 16 , (t - 0x8000), Device_blk.Str.Tmp_scal[n])

int buffer_dT[TEMPERFILT_LEN];
long int smooth_dT;
float	TermoCompDelta_dNdT = 0.0;

THERMOCOMP_DATA dynamic_;
THERMOCOMP_DATA static_;

/******************************************************************************/
void thermo_Max_Saturation(unsigned *lvl, unsigned limit)
{
	if (*lvl>limit) *lvl = limit;	
}

/*****************************************************************************
расчет компенсирующего значения для подсчета импульсов 
*/
long thermo_CalcCompens(void)
{
	float x = static_.delta + TermoCompDelta_dNdT;
    return LONG_2_FRACT_14_18(x);
}

/******************************************************************************/
void thermo_DeltaRecalc(int temperature, THERMOCOMP_DATA* data) 
{
	int i = 0;

    if(temperature > data->temperature_array[TERMO_FUNC_SIZE - 1]) {
        temperature = data->temperature_array[TERMO_FUNC_SIZE - 1];
    }

    while( temperature > data->temperature_array[i] ) i++;

    data->delta = 
        data->dN_array[i] 
        - data->dNdT_array[i] * (float)( data->temperature_array[i] - temperature );   
} 


/******************************************************************************/
/**
    Инициализация-Расчет параметров 
*/
void dNdT_calc(int i, THERMOCOMP_DATA* data){
		
	int dT = 0;
    
    if(i<=0) goto fail;
    if(i>=TERMO_FUNC_SIZE) goto fail;
    
    dT = data->temperature_array[i] - data->temperature_array[i-1];
    if (dT == 0) {
        data->dNdT_array[i] = 0.0;
    } else {
        data->dNdT_array[i] = (data->dN_array[i] - data->dN_array[i-1]) / (float)dT;
    }
fail:;
    return;
}

/******************************************************************************/
void thermo_init()
{
    int i = 0;

    dynamic_.temperature_array = Device_blk.Str.TemperCoolIntDyn;
    dynamic_.dN_array = (float*)Device_blk.Str.ThermoCoolDelta;

    static_.temperature_array = Device_blk.Str.TemperIntDyn;
    static_.dN_array = (float*)Device_blk.Str.ThermoHeatDelta;
    /*	    
    for (i = 0; i < TERMO_FUNC_SIZE; i++)  {
        static_.dN_array[i] = .0e-2;//-7.0e-4 + 1.0e-4*i;
        dynamic_.dN_array[i] = 1.0e-4;//7.0e-6 - 1.0e-6*i;
		    static_.temperature_array[i] = -6000+1000*i; 
			  dynamic_.temperature_array[i] = -6000+1000*i;  
    }
	*/
    //расчет коэффициентов наклона для интерполяции  
    for (i = 1; i < TERMO_FUNC_SIZE; i++)  {		
        dNdT_calc(i, &dynamic_);
        dNdT_calc(i, &static_);			
    }
    
    //Filter
    //e. smooth average initialization 
    for (i = 0; i < TEMPERFILT_LEN; ++i)
        buffer_dT[i] = 0;   

    smooth_dT = 0;
}

/******************************************************************************/
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
	static int TS_sum = 0; //аккумулятор T_main
	static int TS_sum_delta = 0; //аккумулятор T_delta
	static int seconds_aver = 1;
	static int Temp_AverPrevDynCalc = -7000;
	static int Temp_Aver_prev = -7000;
	static int Temp_Aver_delta = 0;
	static int Temp_Aver = 0;
  static x_bool_t bSingleInit = _x_true;

	//e. conversion of temperature values on ADC output 
	//to range -32768 .. +32767 ( additional code; format 1.15 )
	Output.Str.Tmp_Out[TSENS_NUMB] = TERMO_SCALE(g_input.word.temp1, TSENS_NUMB);
    Output.Str.Tmp_Out[TS_DELTA_NUMB] = TERMO_SCALE(g_input.word.delta_t, TS_DELTA_NUMB);
 
	//e. 1 second elapsed
	if (g_gld.time_1_Sec == DEVICE_SAMPLE_RATE_uks) {
		  seconds_aver++;
	}

	//e. TEMP_AVER_PERIOD second elapsed 
	if (seconds_aver > TEMP_AVER_PERIOD) {
		 seconds_aver = 1;

		//e. save the previous mean temperature for 1 Sec
		Temp_Aver_prev = Temp_Aver;		 
		//e. calculating mean temperature for 1 Sec 
		Temp_Aver = TS_sum / (DEVICE_SAMPLE_RATE_HZ * TEMP_AVER_PERIOD); 
		Temp_Aver_delta = TS_sum_delta / (DEVICE_SAMPLE_RATE_HZ * TEMP_AVER_PERIOD); 
		//e. reset the sum for calculation of an mean
 		TS_sum = 0; 
        TS_sum_delta = 0;       

        if (bSingleInit){
            thermo_DeltaRecalc(Temp_Aver, &dynamic_);
            bSingleInit = _x_false;
        }
            
        if((Device_blk.Str.TermoMode)&&(!bSingleInit)){
            switch (Device_blk.Str.TermoMode & 0xF0) {
            case 0x00:
                  TermoCompDelta_dNdT = dynamic_.delta * 
                            (float)(thermo_MovAver_filter((Temp_Aver-Temp_Aver_prev)<<4));
            break;
                
            case 0x10:
                  TermoCompDelta_dNdT = dynamic_.delta * 
                            (float)(thermo_MovAver_filter(Temp_Aver_delta<<4));
            break;
                }
        }        
	}else{
        //accumul
        TS_sum += Output.Str.Tmp_Out[TSENS_NUMB];
        TS_sum_delta += Output.Str.Tmp_Out[TS_DELTA_NUMB];		
	}

	//e. calculation the mean temperature for 1 Sec for T4 and T5 sensors 
	//r. расчет средней за 1 секунду температуры датчиков T4, T5
	if (( abs(Temp_Aver - Temp_AverPrevDynCalc) > Device_blk.Str.DeltaTempRecalc)&&(!bSingleInit)){
		Temp_AverPrevDynCalc = Temp_Aver;
		
		thermo_DeltaRecalc(Temp_Aver, &static_);
	}

	// cyclic built-in test
	if ((Output.Str.Tmp_Out[TSENS_NUMB] < TS_MIN) 
	 || (Output.Str.Tmp_Out[TSENS_NUMB] > TS_MAX)) {
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