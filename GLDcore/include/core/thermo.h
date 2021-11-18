/**
    @brief Thermo.
    Термокомпенсация. для режима DELTA_PS коррекция переменной PS_dif на TermoCompens_Sum
*/
#ifndef __THERMO_H_INCLUDED 
#define __THERMO_H_INCLUDED

/**
    @brief thermo_init.
    @return void
*/
void thermo_init();

void thermo_Max_Saturation(unsigned *lvl, unsigned limit);

/**
    @brief StaticTermoCompens.
    calculation the static component of the termocompensation for 1 measure period (100 uSec)
    расчет статической составляющей термокомпенсации за один период прибора (100 мкс)
	@return 
*/
//int thermo_StaticTermoCompens(int temperature);  
float thermo_StaticTermoCompens(int temperature);  

/**
    @brief DynamicTermoCompens.
    calculation of the termocompensation for 1 device period (100 uSec) 
    расчет термокомпенсации за один период прибора (100 мкс)
	@return 
*/
//int thermo_DynamicTermoCompens(void);
float thermo_DynamicTermoCompens(void);

/**
    @brief DynamicDeltaCalc.
calculation the addition termocompensation for 1 reset
расчет добавки термокомпенсации на одно обнуление
    @return 
*/
void thermo_DynamicDeltaCalc(); 

/**
    @brief clc_ThermoSensors.
Procedure of calculating of the normalized temperaturre vector	 
*/
void thermo_clc_ThermoSensors(void);

/**
    @brief DithFreqRangeCalc
calculation of range of the division factor for the dither drive frequency, 
depending on current temperature 
*/
void thermo_DithFreqRangeCalc(void);



#endif