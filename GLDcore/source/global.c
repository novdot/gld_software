#include "core/global.h"
#include "core/const.h"

#include <stdint.h>

//TODO
#include "core/gld.h"

//
gld_global g_gld;

/******************************************************************************/
void gld_global_init(void)
{
    g_gld.time_1_Sec = DEVICE_SAMPLE_RATE_uks; 
    g_gld.time_Seconds = 0;    
    
    g_gld.cmd.trm_rate = 0;
    g_gld.cmd.trm_rate_prev = 0;
    
    
    g_gld.dbg_buffers.iteration = 999;
}

/******************************************************************************/
//command
x_uint32_t rcv_num_byt;
x_uint32_t rcv_Rdy;

x_uint8_t rcv_buf[256];//64
x_uint8_t rcv_copy[256];

x_int32_t rx_buf_copy;
x_int32_t rcv_byt_copy;
x_uint32_t rcv_num_byt_old;

x_uint32_t trm_num_byt;
x_uint32_t trm_ena;
x_uint32_t num_of_par;
x_uint8_t trm_buf[256];//

x_uint16_t* addr_param[16];
x_uint32_t size_param[16];
x_uint32_t line_err;
x_uint32_t line_sts;

x_uint32_t CMD_Mode = 0;
x_uint32_t CMD_Code; //e. full code of the last recieved command 

int Device_Mode = DM_INT_10KHZ_LATCH;

x_uint32_t blt_in_test;

//dither
x_uint32_t In_Flag = 0;

//exchange
//INPUT Input;
OUTPUT Output;

inputData g_input;

//e. system register of the ADSP card 
x_uint32_t 	SRgR;  
x_uint32_t 	trm_cycl;

x_uint32_t Vibro_2_CountIn;

//x_uint32_t 	RgConA; //e. main register of device control 
//x_uint32_t 	RgConB; //e. additional register of device control

x_uint32_t	Valid_Data; //e. the byte-attribute of authentic data, accumulating errors from request to request  

TDEVICE_BLK Device_blk; //e. union for device constants
x_uint32_t	ser_num; //e. device serial number

x_uint32_t stop_Rq = 0;
//x_uint32_t gyro_Sts = 0;
//x_uint32_t start_Rq = 1;
//x_uint32_t start_Go = 0;
//x_uint32_t pulse_Rq = 0;      
//x_uint32_t pulse_Go = 0;

x_uint32_t BIT_number;
x_uint32_t Is_BIT = 0;

//CycleSync
x_uint32_t 	Sys_Clock;      //e. counter of system ticks
 

x_uint32_t	count;
x_int32_t	PrevPeriod = 0;
volatile x_uint32_t	Latch_Rdy = 0;

x_int32_t LatchPhase = INT32_MAX;
x_uint32_t Ext_Latch_ResetEnable = 1;
volatile x_uint32_t data_Rdy = 0;

x_uint32_t main_cycle_latch = 0; 
x_uint32_t Out_main_cycle_latch; //e. counter of main cycles between external latch pulse appearence
x_uint32_t T_latch, Out_T_latch, temp_T_latch;

x_uint32_t wrk_period;

