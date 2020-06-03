/**
  ******************************************************************************
  * @file    i2c.h
  *
  * @brief   hardware driver
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
#ifndef HARDWARE_I2C_H_INCLUDED
#define HARDWARE_I2C_H_INCLUDED

#include "xlib/types.h"

#define I2C0

#define I2C_IDLE              1
#define I2C_BUSY              0

#define I2CONSET_I2EN       0x0000040  /* I2C Control Set Register */
#define I2CONSET_AA         0x0000004
#define I2CONSET_SI         0x0000008
#define I2CONSET_STO        0x0000010
#define I2CONSET_STA        0x0000020

#define I2CONCLR_AAC        0x0000004  /* I2C Control clear Register */
#define I2CONCLR_SIC        0x0000008
#define I2CONCLR_STAC       0x0000020
#define I2CONCLR_I2ENC      0x0000040

#define I2SCLH_SCLH			0x00000080  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL			0x00000080  /* I2C SCL Duty Cycle Low Reg */

#define I2C_WRITELENGTH		0x00000006	/*Buffer length*/
#define A_ADDRESS			0x0000005E	/*address of slave potentiometer (table 6-2 of pot's manual)*/
#define B_ADDRESS			0x0000005C	/*address of slave potentiometer (table 6-2 of pot's manual)*/
#define WRITE_CMD			0x00000000	/*address of wiper0 register in memory map*/

void i2c_init(void);
void i2c_read(int *cnt_dif);
void i2c_write(x_uint32_t Ph_A, x_uint32_t Ph_B);

#endif