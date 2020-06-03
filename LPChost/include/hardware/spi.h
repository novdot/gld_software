#ifndef _SPI_H_INCLUDED_
#define _SPI_H_INCLUDED_

#include "xlib/types.h"

#define HALFWORDS_FOR_ADCs		3

#define WRITE_DAC0 0x00000030 //write data to DAC 0
#define WRITE_DAC1 0x00000031 //write data to DAC 1

#define	PIN_DAC_CS (1<<23) //DAC identifer
#define PIN_ADC_CS (1<<16) //ADC identifer

void spi_init();
void spi_read(int* a_pArrayIn,int a_nCount,int*a_pExchangeErr);
void spi_write(int* a_nDataOut,int a_nCount,int*a_pExchangeErr);

void spi_set_cs(x_uint32_t data);
void spi_reset_cs(x_uint32_t data);

#endif