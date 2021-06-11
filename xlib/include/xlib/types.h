/**
  ******************************************************************************
  * @file    types.h
  *
  * @brief   xlib
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
#ifndef XLIB_TYPES_H_INCLUDED
#define XLIB_TYPES_H_INCLUDED

typedef enum x_bool_tDef{ 
	_x_false = 0
	, _x_true = 1
} x_bool_t;

typedef enum x_direction_tDef{ 
	_x_plus = 0
	, _x_minus = 1
} x_direction_t;

typedef enum x_switch_tDef{ 
	_x_off = 0
	, _x_on = 1
} x_switch_t;

typedef unsigned char x_uint8_t;
typedef unsigned short int x_uint16_t;
typedef unsigned long int x_uint32_t;

typedef char x_int8_t;
typedef short int x_int16_t;
typedef long int x_int32_t;

typedef float x_float_t;
typedef double x_double_t;

#define X_Lo_b16(word) (x_uint8_t)(0x00FF & word)
#define X_Hi_b16(word) (x_uint8_t)((0xFF00 & word) >> 8)

#define X_CONVERT_DIGIT2ASCII(byte) (byte + 48)


#endif //TYPES_H_INCLUDED