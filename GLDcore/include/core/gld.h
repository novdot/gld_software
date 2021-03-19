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
//Loop controls TODO
#define loop_is_closed(loop_bit) 	((g_gld.RgConA.word & (loop_bit)) == loop_bit)
#define close_loop(loop_bit) 		g_gld.RgConA.word |= loop_bit
#define open_loop(loop_bit) 		g_gld.RgConA.word &= ~(loop_bit)
#define open_all_loops()			g_gld.RgConA.word   = 0	
#define close_all_loops()			g_gld.RgConA.word   = 0xFFFF

/**/

#endif