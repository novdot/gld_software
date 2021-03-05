/**
  ******************************************************************************
  * @file    gld.h
  *
  * @brief   
      hfo - контур стабилизации мощности накачки лазера
      - контур регулированния коэффициента передачи усилителя фотоприемника
      cplc - контур регулирования длины оптического периметра периметра 
      ignit - система управления поджигом лазера
      dither - система управления виброприводом
      - система вычитания виброподставки
      
      command - система команд прибора ГЛД
      
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
#ifndef __GLD_H_INCLUDED 
#define __GLD_H_INCLUDED

#include "command.h"
#include "cplc.h"
#include "dither.h"
#include "hfo.h"
#include "thermo.h"
#include "ignit.h"

#include "sync.h"
#include "exchange.h"

//TODO
//#include "CntrlGLD.h"

/**
    @brief device starting process
*/
void gld_start(void);
x_bool_t gld_is_started(void);
void gld_set_start_request(x_bool_t flag);

/**
    @brief procedure of GLD stopping
*/
void gld_stop(void);
x_bool_t gld_is_stop_request(void);
void gld_set_stop_request(x_bool_t flag);

/**
    @brief prestart control of GLD parameters
*/
void gld_status(void);
/**
    @brief Output modes
*/
void gld_output(void);

void gld_control(void);
/**
    @brief управление контурами
*/
void gld_loop_laser_set(x_switch_t flag);
void gld_loop_hf_reg_set(x_switch_t flag);
void gld_loop_ri_reg_set(x_switch_t flag);
void gld_loop_wp_reg_set(x_switch_t flag);
void gld_loop_wp_sin_set(x_switch_t flag);
void gld_loop_vb_tau_set(x_switch_t flag);
void gld_loop_vb_freq_set(x_switch_t flag);
void gld_loop_gld_reg_set(x_switch_t flag);
void gld_loop_all_regs_set(x_switch_t flag);
/**/
x_switch_t gld_loop_is_laser();
x_switch_t gld_loop_is_hf_reg();
x_switch_t gld_loop_is_ri_reg();
x_switch_t gld_loop_is_wp_reg();
x_switch_t gld_loop_is_wp_sin();
x_switch_t gld_loop_is_vb_tau();
x_switch_t gld_loop_is_vb_freq();
x_switch_t gld_loop_is_gld_reg();
/**/
//Loops bits
#define  LASER_ON        (0x0001)  		//< switch on/off laser-up
#define  HF_REG_ON       (0x0002)  		//< switch on/off the HFO regulator
#define  RI_REG_ON       (0x0004)  		//< switch on/off the DS power regulator
#define  WP_REG_ON       (0x0008)  		//< a mask of switch on/off of the CPLC regulator
#define  WP_SIN_ON       (0x0010)  		//< switch on/off search signal of the CPLC
#define  VB_TAU_ON       (0x0020)  		//< switch on/off amplitude regulator of the dither drive
#define  VB_FREQ_ON      (0x0040)  		//< switch on/off frequency regulator of the dither drive
#define  GLD_ON          (0x0080)  		//< switch on/off all GLD

//Loop controls
#define loop_is_closed(loop_bit) 	((RgConA & (loop_bit)) == loop_bit)
#define close_loop(loop_bit) 		RgConA |= loop_bit
#define open_loop(loop_bit) 		RgConA &= ~(loop_bit)
#define open_all_loops()			RgConA   = 0	
#define close_all_loops()			RgConA   = 0xFFFF

	// Status word errors bits
#define	OUT_FREQ_ERROR		(0x8000)
#define	DITH_FREQ_ERROR		(0x4000)
#define	HFO_VOLT_ERROR		(0x2000)
#define	THERMO_RANGE_ERROR	(0x1000)
#define	THERMO_DIFF_ERROR	(0x0800)
/**/

#endif