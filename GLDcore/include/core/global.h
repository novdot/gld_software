#ifndef __GLOBAL_H_INCLUDED 
#define __GLOBAL_H_INCLUDED

#include "core/types.h"
#include "core/const.h"

/******************************************************************************/
//e.   device operation modes
#define  DM_INT_10KHZ_LATCH				1		//e. mode of internal latch 10 kHz 		
#define  DM_INT_LATCH_DELTA_PS			2
#define	 DM_INT_LATCH_DELTA_BINS		3
#define  DM_EXT_LATCH_DELTA_PS_PULSE	4		//e. mode of external latch with output of the Delta_PS command by pulse 
#define  DM_EXT_LATCH_DELTA_BINS_PULSE	6		//e. mode of external latch with output of the Delta_BINS command by pulse
#define  DM_EXT_LATCH_DELTA_SF_PULSE	7		//e. mode of Delta Scale factor 

/******************************************************************************/
/**
    @brief Струтрура содержит глобальные поля
        параметры прибора
        буфферы и флаги обмена ASK_GLD
        буфферы и флаги контроля работы периферии
*/
typedef struct gld_globalDef{
    //regs
    reg_RgConA RgConA;
    reg_RgConB RgConB;
    reg_valid valid;
    
    //dac and adc holders
    x_uint16_t nADCData[6];
    x_uint16_t nDACData[2];
    
}gld_global;

extern gld_global g_gld;

/******************************************************************************/
//TODO
extern x_uint32_t  trm_num_byt;
extern x_uint32_t  rcv_num_byt;
extern x_uint32_t  rcv_Rdy;

extern x_uint8_t trm_buf[256];//64
extern x_uint8_t rcv_buf[256];
extern x_uint8_t rcv_copy[256];

extern x_uint32_t num_of_par;
extern x_uint32_t* addr_param[16]; //void*
extern x_uint32_t size_param[16];
extern x_uint32_t trm_rate;

extern x_uint32_t rcv_num_byt_old;
extern x_int32_t rcv_byt_copy;
extern x_uint32_t trm_ena;

extern x_uint32_t line_err;
extern x_uint32_t line_sts;
extern x_int32_t rx_buf_copy;


//extern INPUT Input;
extern OUTPUT Output;

extern inputData g_input;

extern x_uint32_t CMD_Mode;
extern x_uint32_t CMD_Code;
extern int Device_Mode;

extern x_uint32_t SRgR;
extern x_uint32_t trm_cycl;

extern x_uint32_t Vibro_2_CountIn;

//GLD
extern TDEVICE_BLK Device_blk; //e. the BLOCK of VARIABLES, LOADED from FLASH-memory

extern x_uint32_t Valid_Data;

extern x_uint32_t ser_num;

extern int VB_Nmin0;  //e. minimum of the output value of a regulator of the period for the Device_blk.TemperNormal temperature
extern int VB_Nmax0;  //e. maximum of the output value of a regulator of the period for the Device_blk.TemperNormal 

extern x_uint32_t BIT_number;
extern x_uint32_t Is_BIT; 

extern x_uint32_t start_Rq; //e. request for device starting
extern x_uint32_t stop_Rq; //e. request for device stopping
//extern x_uint32_t pulse_Rq; //e. request for light-up of laser 

extern x_uint32_t RgConA; //e. main register of device control 
extern x_uint32_t RgConB; //e. additional register of device control 

//CycleSync
extern x_uint32_t Sys_Clock; 
extern x_int32_t time_1_Sec;

extern x_int32_t PrevPeriod;	
extern x_uint32_t Ext_Latch_ResetEnable;
extern volatile x_uint32_t Latch_Rdy;
extern volatile x_uint32_t data_Rdy;
extern x_int32_t LatchPhase;
extern x_uint32_t PeriodElapsed;
extern x_uint32_t count;
extern x_uint32_t main_cycle_latch; 
extern x_uint32_t Out_main_cycle_latch; //e. counter of main cycles between external latch pulse appearence
extern x_uint32_t T_latch, Out_T_latch, temp_T_latch;

//dither
extern x_uint32_t In_Flag;

//command
extern x_uint32_t blt_in_test;

//e. run period in cycles 
extern x_uint32_t wrk_period;
#endif
