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

/*****************************************************************************
[in] temperature - new mean temperature for 1 Sec from T4
[in] data - static holder
[out] data->temperature_array - update by temperature

*/
void thermo_DeltaRecalc(int temperature, THERMOCOMP_DATA* data) 
{
	int i = 0;

    if(temperature > data->temperature_array[TERMO_FUNC_SIZE - 1]) {
        temperature = data->temperature_array[TERMO_FUNC_SIZE - 1];
    }

    while( temperature > data->temperature_array[i] ) i++;

    data->delta = 
        data->dN_array[i].fdata 
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
        data->dNdT_array[i] = (data->dN_array[i].fdata - data->dN_array[i-1].fdata) / (float)dT;
    }
fail:;
    return;
}

/******************************************************************************/
#include "hardware/uart.h"
void thermo_init()
{
    int ifun = 0;
    int i = 0;
    x_uint8_t dbg[64];

    dynamic_.temperature_array = Device_blk.Str.TemperCoolIntDyn;
    //dynamic_.dN_array_hi = (float*)Device_blk.Str.ThermoCoolDelta;
    //dynamic_.dN_array_lo = (float*)Device_blk.Str.ThermoHeatDelta_;
    for(ifun=0;ifun<TERMO_FUNC_SIZE;ifun++){
        if(ifun<(TERMO_FUNC_SIZE/2)){
            dynamic_.dN_array[ifun].udata = 
                (((Device_blk.Str.ThermoCoolDelta[(ifun+0)*2]<<16)&0xffff0000) 
                + ((Device_blk.Str.ThermoCoolDelta[(ifun+1)*2])&0x0000ffff));
        }else{
            dynamic_.dN_array[ifun].udata = 
                (((Device_blk.Str.ThermoCoolDelta_[(ifun+0-(TERMO_FUNC_SIZE/2))*2]<<16)&0xffff0000) 
                + ((Device_blk.Str.ThermoCoolDelta_[(ifun+1-(TERMO_FUNC_SIZE/2))*2])&0xffff));
        }
    }
    
    static_.temperature_array = Device_blk.Str.TemperIntDyn;
    //static_.dN_array_hi = (float*)Device_blk.Str.ThermoHeatDelta;
    //static_.dN_array_lo = (float*)Device_blk.Str.ThermoHeatDelta_;
    for(ifun=0;ifun<TERMO_FUNC_SIZE;ifun++){
        if(ifun<(TERMO_FUNC_SIZE/2)){
            static_.dN_array[ifun].udata = 
                (((Device_blk.Str.ThermoHeatDelta[(ifun+0)*2]<<16)&0xffff0000) 
                + ((Device_blk.Str.ThermoHeatDelta[(ifun+1)*2])&0x0000ffff));
        }else{
            static_.dN_array[ifun].udata = 
                (((Device_blk.Str.ThermoHeatDelta_[(ifun+0-(TERMO_FUNC_SIZE/2))*2]<<16)&0xffff0000) 
                + ((Device_blk.Str.ThermoHeatDelta_[(ifun+1-(TERMO_FUNC_SIZE/2))*2])&0xffff));
        }
        
        /*DBG4(&g_gld.cmd.dbg.ring_out,dbg,64,"%d static_:%f = %u %u\n\r"
            ,ifun
            ,static_.dN_array[ifun].fdata
        ,Device_blk.Str.ThermoHeatDelta[ifun]
        ,Device_blk.Str.ThermoHeatDelta_[ifun]
        );*/
    }
    /**
    for (i = 0; i < TERMO_FUNC_SIZE; i++)  {
        static_.dN_array[i].fdata = -7.0e-4 + (1.0e-4)*i;//-7.0e-4 + 1.0e-4*i;
        dynamic_.dN_array[i].fdata = 7.0e-6 - (1.0e-6)*i;//7.0e-6 - 1.0e-6*i;
    }
	/**/
    //расчет коэффициентов наклона для интерполяции  
    for (ifun = 1; ifun < TERMO_FUNC_SIZE; ifun++)  {		
        dNdT_calc(ifun, &dynamic_);
        dNdT_calc(ifun, &static_);			
    }
    
    //Filter
    //e. smooth average initialization 
    for (ifun = 0; ifun < TEMPERFILT_LEN; ++ifun)
        buffer_dT[ifun] = 0;   

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

#define TERMO_SCALE(t, n)  \
    mac_r(Device_blk.Str.Tmp_bias[n] << 16 , (t - 0x8000), Device_blk.Str.Tmp_scal[n])

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
    int i = 0;
    x_uint8_t dbg[64];

	//e. conversion of temperature values on ADC output 
	//to range -32768 .. +32767 ( additional code; format 1.15 )
	Output.Str.Tmp_Out[TSENS_NUMB] = TERMO_SCALE(g_input.word.temp1, TSENS_NUMB);
    Output.Str.Tmp_Out[TS_DELTA_NUMB] = TERMO_SCALE(g_input.word.delta_t, TS_DELTA_NUMB);
 
	//e. 1 second elapsed
	if (g_gld.time_1_Sec == DEVICE_SAMPLE_RATE_uks) {
		  seconds_aver++;
	}

	//e. TEMP_AVER_PERIOD second elapsed 
	//e. calculation the mean temperature for 1 Sec for T4 and T5 sensors 
	//r. расчет средней за 1 секунду температуры датчиков T4, T5
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
        /*DBG3(&g_gld.cmd.dbg.ring_out,dbg,64,"TermoCompDelta:%f delta:%f Temp_Aver:%d\n\r"
            ,TermoCompDelta_dNdT
            ,dynamic_.delta
            ,Temp_Aver
        );   */     
	}else{
        //accumul
        TS_sum += Output.Str.Tmp_Out[TSENS_NUMB];
        TS_sum_delta += Output.Str.Tmp_Out[TS_DELTA_NUMB];		
	}

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