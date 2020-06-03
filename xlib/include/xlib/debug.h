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
#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include "types.h"

#define DEBUG_ON 1
	
#if defined(DEBUG_ON)
#define X_ASSERT(value) \
if(!value){\
printf("error assert param %s",#value);return _x_false;\
}
	
#else
#define X_ASSERT(value) 0
	
#endif

#endif //TYPES_H_INCLUDED