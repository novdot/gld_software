/**
  ******************************************************************************
  * @file ignit.h
  *
  * @brief ignit - система управления поджигом лазера
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
#ifndef __GLD_IGNIT_H_INCLUDED 
#define __GLD_IGNIT_H_INCLUDED

#include "core/types.h"

/**
    @brief gld initialize
*/
void ignit_init(void);

/**
    @brief full gld light-up cycle
*/
gld_error_t ignit_start(void);

/**
    @brief Check is requested ignition
*/
x_bool_t ignit_is_request(void);

/**
    @brief Check is requested ignition
*/
void ignit_set_request(x_bool_t flag);

/**
    @brief Check is ignition cycle on work
*/
x_bool_t ignit_is_started(void);

/**
    @brief Check is ignition cycle on work
*/
x_bool_t ignit_is_busy(void);

extern inputData g_input;

#endif //__GLD_IGNIT_H_INCLUDED