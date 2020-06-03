/**
  ******************************************************************************
  * @file    config.h
  *
  * @brief
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
#ifndef __GLD_CONFIG_H_INCLUDED 
#define __GLD_CONFIG_H_INCLUDED

/**
Настройка скорости обмена командами по протоколу ASK_GLD
*/
//режим мегабит. скорость 921600 и не изменяется
//#define __CONFIG_COMMANDS_MBIT 
//режим по-умолчанию. переключение согласно протоколу
#define __CONFIG_COMMANDS_DEFAULT 

#if defined (__CONFIG_COMMANDS_MBIT)
#define CONFIG_COMMANDS_BAUDRATE 921600
#elif defined (__CONFIG_COMMANDS_DEFAULT)
#define CONFIG_COMMANDS_BAUDRATE 38400
#else
#error "Please define __CONFIG_COMMANDS_MBIT or __CONFIG_COMMANDS_DEFAULT!"
#endif

#endif
