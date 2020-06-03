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

/**
    @brief Quadrature encoder initialization.
    @return
    */
void SOI_Init(void);

x_uint32_t qei_get_position();

x_direction_t qei_get_direction();

#endif