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
    @brief модулятор
*/
void hardware_modulator(x_uint32_t a_data);

/**
    @brief управление контурами ГВЧ, СРП
*/
void hardware_regul_data_init();
void hardware_regul_data_write(x_uint32_t flag, int*pExchangeErr, int a_HF_reg, int a_WP_reg);
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
void hardware_photo_out(x_uint32_t Ph_A, x_uint32_t Ph_B);


/**
    @brief управление памятью
    @attention перед вызовом остановить все прерывания
*/
void hardware_flash_read(x_uint32_t a_sector,x_uint8_t* a_pmemory);
void hardware_flash_write(x_uint32_t a_sector,x_uint8_t* a_pmemory);
void hardware_flash_erase(x_uint32_t a_sector);
void hardware_flash_load_main();

/**
    @brief таймер
*/
void hardware_tim_init(x_uint32_t* mcs_cnt);
void hardware_tim_start();
void hardware_tim_stop();

#endif