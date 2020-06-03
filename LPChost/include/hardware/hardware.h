#ifndef __HARWARE_H_INCLUDED 
#define __HARWARE_H_INCLUDED

#include "xlib/types.h"

#include "hardware/uart.h"
#include "hardware/pwm.h"
#include "hardware/qei.h"
#include "hardware/dac.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"

/**
configure light-up signal
поджиг
*/
void hardware_configure_lightup(void);
void hardware_lightup_on(void);
void hardware_lightup_off(void);

/**
свето
*/
void hardware_configure_backlight(void);
void hardware_backlight_on(void);
void hardware_backlight_off(void);

/**
вибро1,вибро2
*/
void hardware_configure_vibro(void);
void hardware_vibro1_on(void);
void hardware_vibro1_off(void);
void hardware_vibro2_on(void);
void hardware_vibro2_off(void);

void hardware_modulator(x_uint32_t a_data);

/**
    @brief управление контурами ГВЧ, СРП
*/
void hardware_write_regul_data(x_uint32_t flag, int*pExchangeErr, int a_HF_reg, int a_WP_reg);


/**
    @brief управление ADC,DAC
*/
void SetDAC();
void ResetDAC();
void SetADC();
void ResetADC();

#endif