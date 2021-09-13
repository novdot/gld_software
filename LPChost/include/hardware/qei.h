/**
  ******************************************************************************
  * @file    qei.h
  *
  * @brief   Quadrature encoder
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
#ifndef __HARDWARE_QEI_H_INCLUDED 
#define __HARDWARE_QEI_H_INCLUDED

#include "core/types.h"
#include "lpc17xx.h"

#define  MAX_QEI_CNT				2147483646	//e .limit of input counter	 (INT32_MAX - 1)
/**
    @brief Quadrature encoder initialization.
    @return
    */
void SOI_Init(void);

x_uint32_t qei_get_position();

x_direction_t qei_get_direction();

#endif