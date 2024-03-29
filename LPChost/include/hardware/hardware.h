#ifndef __HARWARE_H_INCLUDED 
#define __HARWARE_H_INCLUDED

#include "xlib/types.h"

#include "hardware/uart.h"
#include "hardware/pwm.h"
#include "hardware/qei.h"
#include "hardware/dac.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/memory.h"

//#define HOST4

/**
    @brief configure light-up signal (поджиг)
    discharge
    pulse
*/
void hardware_configure_lightup(void);
void hardware_lightup_on(void);
void hardware_lightup_off(void);

/**
    @brief свето
*/
void hardware_configure_backlight(void);
void hardware_backlight_on(void);
void hardware_backlight_off(void);


/**
    @brief управление контурами ГВЧ, СРП посредством ЦАП, АЦП
*/
void hardware_modulator(x_int32_t a_data);
void hardware_cplc(x_int32_t a_data);
void hardware_regul_data_init();
void hardware_regul_data_write(x_uint32_t a_ch, int*a_pExchangeErr, int a_reg);
void hardware_regul_data_read(int*a_pBuffer, int cnt, int*pExchangeErr);

/**
    @brief управление CS ADC,DAC
*/
void hardware_set_dac();
void hardware_reset_dac();
void hardware_set_adc();
void hardware_reset_adc();

/**
    @brief управление фоторезистором
*/
void hardware_photo_init(void);
void hardware_photo_exchange(int*pCntDif);
void hardware_photo_set(x_uint32_t Ph_A, x_uint32_t Ph_B);


/**
    @brief управление памятью
    @attention перед вызовом остановить все прерывания
*/
x_bool_t hardware_flash_read(x_uint32_t a_sector,x_uint32_t* a_pmemory, x_uint32_t a_cnt);
x_bool_t hardware_flash_write(x_uint32_t a_sector,x_uint8_t* a_pmemory, x_uint32_t a_size,x_uint32_t shift);
x_bool_t hardware_flash_erase(x_uint32_t a_sector, x_uint16_t a_size);
void hardware_flash_load_main();

x_bool_t hardware_flash_erase_f(x_uint8_t* a_file, x_uint16_t a_size);
x_bool_t hardware_flash_write_f(x_uint8_t* a_file, x_uint32_t a_size, x_uint8_t *buf, x_uint32_t shift);
/**
    @brief таймер
*/
void hardware_tim_init(x_uint32_t* mcs_cnt);
void hardware_tim_start();
void hardware_tim_stop();

#endif