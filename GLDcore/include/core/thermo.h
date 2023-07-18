/**
    @brief Thermo.
    Термокомпенсация. для режима DELTA_PS коррекция переменной PS_dif на TermoCompens_Sum
*/

#include "core/types.h"
#ifndef __THERMO_H_INCLUDED 
#define __THERMO_H_INCLUDED

 typedef struct {
	x_int32_t* temperature_array;
	//float* dN_array_hi;
	//float* dN_array_lo;
     union{
         float fdata;
         x_uint32_t udata;
     }dN_array[TERMO_FUNC_SIZE];
	//float dN_array[TERMO_FUNC_SIZE];
	float dNdT_array[TERMO_FUNC_SIZE];
	float delta;
} THERMOCOMP_DATA;
 
void thermo_Max_Saturation(unsigned *lvl, unsigned limit);

/**
    @brief thermo_init.
    @return void
*/
void thermo_init();

/**
    @brief thermo_CalcCompens.
    calculation of the termocompensation for 1 device period (100 uSec) 
    расчет термокомпенсации за один период прибора (100 мкс)
	@return 
*/
long thermo_CalcCompens();

/**
    @brief thermo_DeltaRecalc
calculation the addition termocompensation for current temperature
расчет добавки термокомпенсации для текущей температуры
    @return 
*/
void thermo_DeltaRecalc(int temperature, THERMOCOMP_DATA* data);

/**
    @brief clc_ThermoSensors.
Procedure of calculating of the normalized temperaturre vector	 
*/
void thermo_clc_ThermoSensors(void);

#endif