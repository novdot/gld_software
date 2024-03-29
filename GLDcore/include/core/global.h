#ifndef __GLOBAL_H_INCLUDED 
#define __GLOBAL_H_INCLUDED

#include "core/types.h"
#include "core/const.h"
//#include "core/ring_buffer.h"
#include "xlib/ring_buffer.h"


/******************************************************************************/
/*
typedef struct gld_thermoDef{
    int	TermoCompens_Sum;//< накопление термокомпенсации
	int	StartTermoCompens; //начальная термокомпенсация
    x_uint8_t IsHeating;//< признак нагрева для зеркального прибора
    float dThermoHeatDeltaPer_dTermo[TERMO_FUNC_SIZE];
    float dThermoCoolDeltaPer_dTermo[TERMO_FUNC_SIZE];
    int	dFuncPer_dTermo[TERMO_FUNC_SIZE];
    int	Temp_Aver; //< the mean temperature for 1 Sec for T4 sensor 
    int WP_reset_heating;	//e. voltage of reset at heating
    int WP_reset_cooling;	//e. voltage of reset at cooling
}gld_thermo;
*/
typedef struct gld_cplcDef{
    int WP_DelaySin_Array[21];
    int16_t sin_func[PI_B3_MAX];
    int16_t sin_index;	
}gld_cplc;

#define GLD_RINGBUFFER_SIZE (1024)
#define GLD_BUF_SIZE (512)
#define RCV_BUF_SIZE (256)
typedef struct gld_cmdDef{
    x_uint8_t trm_cycl; //is cyclic bit from cmd
    x_uint8_t trm_cycl_sync; //бит для синхронизации выдаваемого первого пакета данных
    x_uint32_t trm_rate; //rate value from cmd
    x_uint32_t trm_rate_prev; //set rate to uart
    struct{
        x_ring_buffer_t ring_in;
        x_ring_buffer_t ring_out;
        uint8_t buf_in[GLD_RINGBUFFER_SIZE];
        uint8_t buf_out[GLD_RINGBUFFER_SIZE];
    }dbg;
    struct{
        x_ring_buffer_t ring_in;
        x_ring_buffer_t ring_out;
        uint8_t buf_in[GLD_RINGBUFFER_SIZE];
        uint8_t buf_out[GLD_RINGBUFFER_SIZE];
    }ask;
    x_uint8_t recieve_cmd[GLD_BUF_SIZE];
    x_uint8_t recieve_cmd_cpy[GLD_BUF_SIZE];
    union{
        struct{
            unsigned rx_cpy:1; //<copy recieve buffer
            unsigned tx_cycl:1; //<cyclic trasmittion packets
            unsigned reserve:6;
        }bit;
        x_uint8_t word;
    }flags;
    x_uint16_t recieve_cmd_size;
}gld_cmd;


#define REPER_MEANDR_CNT_PLS_MIN_INIT (0xFFFFFFFF)
#define RATE_VIBRO_1_DELAY_BUF_MAX (255)
typedef struct gld_pulsesDef{
    x_uint32_t Cnt_curr; //< value from qei. Only for RATE_REPER_OR_REFMEANDR
    x_uint32_t Curr_Cnt_Vib; //< value_Vib = diff between curent value and old
    int32_t	Dif_Curr_Vib; //< diff between curent value_Vib and old
    struct{
        x_uint32_t cnt_curr;
        x_uint32_t cnt_prev;
        x_uint32_t cnt_dif;
        x_uint32_t cnt_reduced;
        x_int32_t delay_buff[RATE_VIBRO_1_DELAY_BUF_MAX];
        x_int32_t idelay_curr;
        x_int32_t idelay_max;
    }vibro1; //RATE_VIBRO_1
    struct{
        x_uint32_t cnt_curr;
        x_uint32_t cnt_prev;
        x_int32_t cnt_dif;
        x_uint32_t cnt_pls;
        x_uint32_t cnt_mns;
        x_uint32_t halfQEIPeriod; //< period elapsed, we can calculate Cnt_Dif
        
        struct{
            union{
                x_int8_t word;
                struct{
                    unsigned measure:1;
                    unsigned ready:1;
                    unsigned reserve:6;
                }bit;
            }flags;
            /*
            int s; //тек угол
            int s_pls; // точка +
            int s_mns; // точка -
            int s_pls_prev; // пред точка +
            int s_mns_prev; // пред точка -
            x_uint32_t s_pls_delta; // модуль между соседними точками +
            x_uint32_t s_mns_delta; // модуль между соседними точками -
            */
            x_uint32_t cnt_pls_prev;
            x_uint32_t cnt_mns_prev;
        }curr_angle; //< структура проверки на симметрию текущего угла
        
        x_int32_t cnt_delta;
        x_uint32_t cnt_iter; //счетчик итераций между прерываниями
        x_uint32_t cnt_iter_sum; //счетчик итераций на выдачу с частотой Rate
        x_uint32_t cnt_int; //счетчик прерываний
        x_uint32_t cnt_int_sum; //счетчик прерываний на выдачу с частотой Rate
        x_uint32_t cnt_tc_prev; //TC0 previous
        union{
            struct{
                unsigned get_zero:1;
                unsigned get_peak:1;
                unsigned reserve:6;
            }bit;
            x_uint8_t word;
        }flags;
    }reper_meandr; //RATE_REPER_OR_REFMEANDR
}gld_pulses;

/**
    @brief Струтрура содержит глобальные поля
        параметры прибора
        буфферы и флаги обмена ASK_GLD
        буфферы и флаги контроля работы периферии
*/
typedef struct gld_globalDef{
    union{
        struct{
            unsigned Lo:4; //firmware ver Lo
            unsigned Hi:4; //firmware ver Hi
        }bit;
        x_uint8_t word;
    }version;
    union{
        struct{
            unsigned Lo:8; //serial Lo
            unsigned Hi:8; //serial Hi
        }bit;
        x_uint16_t word;
    }serial;
    
    //regs
    reg_RgConA RgConA;
    reg_RgConB RgConB;
    reg_valid valid;
    
    //dac and adc holders
    x_uint32_t nADCData[6];
    x_uint32_t nDACData[2];
    
    //gld_thermo thermo;
    gld_cplc cplc;
    
    //sip
    //e. current difference output for dithering control in LightUp mode and Dither regulator
    gld_pulses pulses;
    
    //math dsp
    unsigned int Vibro_Filter_Aperture;
    
    //tim
    x_int32_t time_1_Sec;//e. pseudosecond timer
    x_uint32_t time_Seconds; //seconds from power on
    struct{
        x_uint32_t work_period;//e. run period in cycles 
    }internal_latch;
    struct{
        union{
            x_uint8_t word;
            struct{
                unsigned en:1; //< признак настройки на внешнюю защелку
                unsigned reserve:7;
            }bit;
        }flags;            
    }ext_latch;
    
    //cmd
    gld_cmd cmd;
    
    //dbg
    struct{
        int val[24];
        int iteration;
        x_uint32_t counters_latch;
    }dbg_buffers;
    
    struct{
        union{
            struct{
                unsigned isLimInt: 1;//< get interrupt pulse was formed(full period)
                unsigned In_Flag: 1;//< meandr
                unsigned SwitchCntInq: 1;//<
                unsigned reserve: 5;
            }bit;
            x_uint8_t word;
        }flags;
        x_int16_t halfPulseCycleCnt; //< кол-во тактов для построения вибро меандра
    }dither;
}gld_global;

extern gld_global g_gld;

/**
*   @brief проинициализируем глобальную структуру
*/
void gld_global_init(void);

/******************************************************************************/
//TODO
extern x_uint32_t trm_num_byt;
extern x_uint32_t rcv_num_byt;
extern x_uint32_t rcv_Rdy;
//extern x_uint32_t trm_cycl;

extern x_uint8_t trm_buf[256];//64
extern x_uint8_t rcv_buf[256];
extern x_uint8_t rcv_copy[256];

extern x_uint32_t num_of_par;
extern x_uint16_t* addr_param[16]; //void*
extern x_uint32_t size_param[16];

extern x_uint32_t rcv_num_byt_old;
extern x_int32_t rcv_byt_copy;
extern x_uint32_t trm_ena;
extern x_int32_t rx_buf_copy; //e. is copying of present received packet needed

//command
//extern x_uint32_t blt_in_test;

//regs
extern x_uint32_t line_err;
extern x_uint32_t line_sts;
extern x_uint32_t SRgR;

//extern INPUT Input;
extern OUTPUT Output;

extern inputData g_input;

extern x_uint32_t CMD_Mode;
extern x_uint32_t CMD_Code;
extern int Device_Mode;

extern x_uint32_t Vibro_2_CountIn;

//GLD
extern TDEVICE_BLK Device_blk; //e. the BLOCK of VARIABLES, LOADED from FLASH-memory

extern x_uint32_t Valid_Data;

//extern x_uint32_t ser_num;

extern x_uint32_t BIT_number;
extern x_uint32_t Is_BIT; 

extern x_uint32_t start_Rq; //e. request for device starting
extern x_uint32_t stop_Rq; //e. request for device stopping
//extern x_uint32_t pulse_Rq; //e. request for light-up of laser 

//extern x_uint32_t RgConA; //e. main register of device control 
//extern x_uint32_t RgConB; //e. additional register of device control 

//CycleSync
extern x_uint32_t Sys_Clock; 

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

//e. run period in cycles 
//extern x_uint32_t wrk_period;
#endif
