#include "stdint.h"

#include "core/gld.h"
#include "core/global.h"

void DAC_ADC_Exchange_Init(void);
void DAC_ADC_Exchange(void);
void ADC_Input(void);

//int  clc_WP_sin(void);
//void clc_PLC(void);
//void init_PLC(void);

void clc_Pulses(void);
void SOI_Init(void);

void clc_ThermoSensors(void);

void Out_G_photo(unsigned, unsigned);
void G_Photo_Init( void );
void G_Photo_Exchange(void);

//endof file
