/**
  ******************************************************************************
  * @file    const.h
  *
  * @brief   core
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
#ifndef GLD_CONST_H_INCLUDED
#define GLD_CONST_H_INCLUDED

//e. constants for the piecewise-linear thermocompensation 
#define TERMO_FUNC_SIZE    14 //e. amount of the points of the table function of thermocompensation 
#define MAX_ORDER           9 //e. maximal value of order for the thermocompensation coefficients 
#define TSENS_NUMB          4 //e. number of the temperature sensor used for the thermocompensation

#define	DEVICE_SAMPLE_RATE_HZ		10000		//e. sampling frequency 10 kHz 
#define	DEVICE_SAMPLE_RATE_uks		100000000		//e. sampling frequency 10 kHz

#define CONST_PARAMS_FLASH_START 0x40000

#endif