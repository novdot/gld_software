/**
    @brief Thermo.
    Термокомпенсация.
*/
#ifndef __THERMO_H_INCLUDED 
#define __THERMO_H_INCLUDED


void Max_Saturation(unsigned *lvl, unsigned limit);

/**
    @brief StaticTermoCompens.
    calculation the static component of the termocompensation for 1 measure period (100 uSec)
    @return 
*/
int StaticTermoCompens(int temperature);  

/**
    @brief DynamicDeltaCalc.
calculation the addition termocompensation for 1 reset
    @return Thermocompensation addition
*/
int DynamicDeltaCalc(); 

/**
    @brief clc_ThermoSensors.
Procedure of calculating of the normalized temperaturre vector	 
*/
void clc_ThermoSensors(void);

/**
    @brief DithFreqRangeCalc
calculation of range of the division factor for the dither drive frequency, 
depending on current temperature 
*/
void DithFreqRangeCalc(void);



#endif