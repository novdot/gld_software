  #include "ThermoCalc.h"
  #include "CyclesSync.h"
  #include "CntrlGLD.h"
  #include "mathDSp.h"
  
#include "core/global.h"

#define debug
#define	TEMP_AVER_PERIOD	4 // e. number of seconds for average
		 int		TermoCompens_Sum = 0;
unsigned int 	IsHeating;
		 //int 	Tmp_Out[NUM_OF_THERMOSENS];
		 int    dThermoHeatDeltaPer_dTermo[TERMO_FUNC_SIZE];
		 int	dThermoCoolDeltaPer_dTermo[TERMO_FUNC_SIZE];
		 int	dFuncPer_dTermo[TERMO_FUNC_SIZE];
		 int	TermoCompDelta;
		 int	Temp_Aver; //e. the mean temperature for 1 Sec for T4 sensor //r. средняя температура за 1 секунду для датчика T4
		 int	TempEvolution = 0;
	     int	StartTermoCompens = 0; 	//e. initial thermocompensation (in XXX seconds after start ) //r. начальная термокомпенсация (через ХХХ секунд после старта)

extern 	 int WP_reset_heating;	//e. voltage of reset at heating //r. напряжение сброса при нагревании
extern 	 int WP_reset_cooling;	//e. voltage of reset at cooling //r. напряжение сброса при охлаждении

__inline Max_Saturation(unsigned *lvl, unsigned limit)
{
	if (*lvl>limit) *lvl = limit;	
}


/******************************************************************************
** Function name:		StaticTermoCompens
**
** Descriptions: Procedure of 		
**
** parameters:	None		
** Returned value:	None	
** 
******************************************************************************/
int StaticTermoCompens(int temperature) //e. calculation the static component of the termocompensation for 1 measure period (100 uSec) //r. расчет статической составляющей термокомпенсации за один период прибора (100 мкс)
{

	float TermoCompens_Curr; //e. the value of the thermocompensation for 1 device period (100 uSec) //r. величина термокомпенсации за один период прибора (100 мкс)

	int i, t;					
						
		//r. используем только термодатчик TSENS_NUMB
		//r. для новых термодатчиков: T4, для старых: T1
		//r. и используем кусочно-непрерывную термокомпенсацию
	
		//r. Tmp_Out[TSENS_NUMB] = 8960; //-2560; //5120; //8000; // -2600; //-5000;

		t = temperature;
		if(t > Device_blk.Str.TemperInt[TERMO_FUNC_SIZE - 1])
		{
			t = Device_blk.Str.TemperInt[TERMO_FUNC_SIZE - 1];
		}
		
		i = 0;
    	while( t > Device_blk.Str.TemperInt[i] ) i++;

    	//r. Tmp_Out[5] = i;
    	TermoCompens_Curr = Device_blk.Str.TermoFunc[i] - dFuncPer_dTermo[i] * (float)( Device_blk.Str.TemperInt[i] - t );

		//r. Отладка
		//r. TermoCompens_Curr = 1.111111125; // отладка
		//r.TermoCompens_Curr = // 0.25; // за 1 сек накапливается 2500 импульсов
		/*r.
							//0.000100; // коэфф., при котором за 1 сек накапливается 1 импульс
							//1.0001; // коэфф., при котором за 1 сек накапливается 10001 импульс
							// 0.000125; // коэфф., при котором за 1 сек накапливается 1.25 импульса (за 100 выводится 122 имп.???)
							// 0.000105; // за 100 сек д.накапливаться 105 имп., накапл. 103???
		*/							

	//	TermoCompens_Curr = LONG_2_FRACT_14_18(TermoCompens_Curr);  //r. TermoCompens_Curr переводим в формат 14.18

		return TermoCompens_Curr;
} // StaticTermoCompens

/******************************************************************************
** Function name:		DynamicDeltaCalc
**
** Descriptions: Procedure of 		
**
** parameters:	None		
** Returned value:	Thermocompensation addition	
** 
******************************************************************************/
int DynamicDeltaCalc() //e. calculation the addition termocompensation for 1 reset //r. расчет добавки термокомпенсации на одно обнуление
{

	int i, t;

	t = Temp_Aver;

    if (IsHeating)
    {
		if(t > Device_blk.Str.TemperIntDyn[TERMO_FUNC_SIZE - 1])
		{
			t = Device_blk.Str.TemperIntDyn[TERMO_FUNC_SIZE - 1];
		}
	
		i = 0;
	   	while( t > Device_blk.Str.TemperIntDyn[i] ) i++;
	
    	TermoCompDelta = ( Device_blk.Str.ThermoHeatDelta[i] - dThermoHeatDeltaPer_dTermo[i] * (float)( Device_blk.Str.TemperIntDyn[i] - t ) );
    }
    else
    {
		if(t > Device_blk.Str.TemperCoolIntDyn[TERMO_FUNC_SIZE - 1])
		{
			t = Device_blk.Str.TemperCoolIntDyn[TERMO_FUNC_SIZE - 1];
		}
	
		i = 0;
	   	while( t > Device_blk.Str.TemperCoolIntDyn[i] ) i++;
	
    	TermoCompDelta = ( Device_blk.Str.ThermoCoolDelta[i] - dThermoCoolDeltaPer_dTermo[i] * (float)( Device_blk.Str.TemperCoolIntDyn[i] - t ) );
    }  
	return TermoCompDelta; 

} // DynamicDeltaCalc

/******************************************************************************
** Function name:		clc_ThermoSensors
**
** Descriptions: Procedure of calculating of the normalized temperaturre vector		
**
** parameters:	None		
** Returned value:	None	
** 
******************************************************************************/
void clc_ThermoSensors(void)	
{
	unsigned i;
	static int TS_sum = 0;
	static int seconds_aver = 0, TenSeconds = 0;
	static int Temp_AverPrevDynCalc = -7000;
	static int StartRdy = 1;
	static int PrevTemp = -7000;

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
	if (time_1_Sec == DEVICE_SAMPLE_RATE_uks) 
	{
		seconds_aver++;
#if !defined DEBUG
		ResetToResetInSeconds++;
#endif
	}

	if (seconds_aver > TEMP_AVER_PERIOD) //e. TEMP_AVER_PERIOD second elapsed //r. истекли TEMP_AVER_PERIOD секунд
	{
		seconds_aver = 0;
		TenSeconds++;
		PrevTemp = Temp_Aver;		//e. save the previous mean temperature for 1 Sec //r. запоминаем предыдущую среднюю температуру за секунду
		Temp_Aver = TS_sum / (DEVICE_SAMPLE_RATE_HZ * TEMP_AVER_PERIOD); //e. calculating mean temperature for 1 Sec //r. вычисляем среднюю температуру за секунду

		if (Temp_Aver > PrevTemp)
		{
			TempEvolution++;
		}
		if (Temp_Aver < PrevTemp)
		{
			TempEvolution--;
		}

		TS_sum = 0; //e. reset the sum for calculation of an mean //r. обнуляем сумму для вычисления среднего
	}
	else
	{
		TS_sum += Output.Str.Tmp_Out[TSENS_NUMB];		
	}
	
	if (TenSeconds == 10) // 10 * TEMP_AVER_PERIOD = 40
	{
		TenSeconds = 0;
		if (TempEvolution > 0)
		{
			IsHeating = 1;
		}
		
		if (TempEvolution < 0)
		{
			IsHeating = 0;
		}
		TempEvolution = 0;
	}	

	//e. single calculaiton of some device parameters (measurement on the VALID_START_SEC  second after start) //r. однократный расчет некоторых параметров прибора (измерение на VALID_START_SEC секунде после старта)
	if (StartRdy)
	{
		if (TenSeconds > VALID_START_4SEC)
		{

			StartRdy = 0;	//r. самоблокировка, поэтому больше сюда не заходим
			//TempOfReset = Temp_Aver; //r.x Temp5_Aver;
			//	Device_blk.Str.WP_scl >>= 1;
          	//	Device_blk.Str.HF_scl >>= 1;

			if ((Device_blk.Str.TermoMode != TERMO_OFF) && \
				(Device_blk.Str.TermoMode != TERMO_ON_DYNAMIC_ONLY) && \
				(Device_blk.Str.TermoMode != TERMO_ON_DYNAMIC_ONLY_NUMB_OFF)) //e. static thermocompensation is enable //r. статическая термокомпенсация включена
 			{
				//e. calculation of the static component of the thermocompensation //r. расчет статической термокомпенсации числа
				StartTermoCompens = StaticTermoCompens(Temp_Aver); //e. starting temperature of the device //r. стартовая температуры прибора
 			} 			
			DynamicDeltaCalc();
			
			//e. calculation of range for dither drive frequency, depending on starting temperature //r. расчет границ для частоты вибропривода, зависящего от стартовой температуры
		//	DithFreqRangeCalc();

			//r. коэффициенты для полосового фильтра квази ДУП не пересчитываем: считаем, что полоса фильтра заведомо шире

								
			//e. voltage of reset at heating //r. напряжение сброса при нагревании
			WP_reset_heating = CPL_reset_calc(Device_blk.Str.WP_reset, Device_blk.Str.K_WP_rst_heating, Temp_Aver, Device_blk.Str.TemperNormal);
			//e. voltage of reset at cooling //r. напряжение сброса при охлаждении
			WP_reset_cooling = CPL_reset_calc(Device_blk.Str.WP_reset2, Device_blk.Str.K_WP_rst_cooling, Temp_Aver, Device_blk.Str.TemperNormal);		
		}
	}	

	if ( abs(Temp_Aver - Temp_AverPrevDynCalc) > Device_blk.Str.DeltaTempRecalc)
	{
		Temp_AverPrevDynCalc = Temp_Aver;
		DynamicDeltaCalc();
	}//e. calculation the mean temperature for 1 Sec for T4 and T5 sensors //r. расчет средней за 1 секунду температуры датчиков T4, T5

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


/******************************************************************************
** Function name:		clc_PLC
**
** Descriptions: Procedure of initial processing for the CPLC regulator 		
**
** parameters:	None		
** Returned value:	None	
** 
******************************************************************************/
void DithFreqRangeCalc(void) //e. calculation of range of the division factor for the dither drive frequency, depending on current temperature //r. расчет границ коэффициента деления для частоты вибропривода, зависящих от текущей температуры
{

	unsigned int min_level, max_level;
	int delta_VB_N;
	
	delta_VB_N = mult_r(Device_blk.Str.K_vb_tu >> DITH_VBN_SHIFT, (Temp_Aver - Device_blk.Str.TemperNormal)); //r.200;
	//r. !!! сделать суммирование с насыщением, а затем сдвиг
	min_level = VB_Nmin0 + delta_VB_N;
	max_level = VB_Nmax0 + delta_VB_N;
	// maximum saturation for unsigned levels
	Max_Saturation(&min_level, ((unsigned int)0xFFFF >> DITH_VBN_SHIFT)-1);  // min should be always less then max_level by 1
	Max_Saturation(&max_level, ((unsigned int)0xFFFF >> DITH_VBN_SHIFT));
	Device_blk.Str.VB_Nmin = min_level << DITH_VBN_SHIFT;
	Device_blk.Str.VB_Nmax = max_level << DITH_VBN_SHIFT;

} // DithFreqRange_calc
