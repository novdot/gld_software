/**
  ******************************************************************************
  * @file    types.h
  *
  * @brief   core
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
#ifndef GLD_TYPES_H_INCLUDED
#define GLD_TYPES_H_INCLUDED

#include "core/const.h"
#include "xlib/types.h"

//e. error code
typedef enum   
{
	_gld_error_ok = 0
    ,_gld_wrg_wait
    ,_gld_error_no_ignit
} gld_error_t;

//e. thermocompensation modes 
typedef enum _TERMO_MODE   
{
	TERMO_OFF						//e. thermocompensation is switched off  
	,TERMO_ON					    //e. thermocompensation is switched on  
    ,TERMO_ON_NUMB_OFF			    //e. thermocompensation is switched on, number resets (debug mode) 
	,TERMO_ON_STATIC_ONLY		    //e. only static thermocompensation is switched on  
	,TERMO_ON_DYNAMIC_ONLY		    //e. only dynamic thermocompensation is switched on  
	,TERMO_ON_STATIC_ONLY_NUMB_OFF	//e. static thermocompensation is switched on, number resets (debug mode) 
	,TERMO_ON_DYNAMIC_ONLY_NUMB_OFF	//e. dynamic thermocompensation is switched on, number resets (debug mode) 
} TERMO_MODE;

//e. variants of control points for scope 
typedef enum _WATCH_MODE   
{
	VB_DELAY_MEANDER			//e. the delayed on the VB_phs meander 
	,VB_PHASE_DETECTOR			//e. output of the PD of the dither drive 
	,VB_PHASE_DETECTOR_1_SEC	//e. output of integral of the PD of the dither drive for 1 Sec 
	,VB_INTEGRATOR_40T			//e. output of integral of the PD of the dither drive for 40 periods 
	,WP_PHASE_DETECTOR  			//e. output of the PD of the CPLC regulator 
} WATCH_MODE;

/**
    @brief the BLOCK of VARIABLES, LOADED from FLASH-memory
    Address (parameter number in the block = 0..255 )
*/
#define VARIABLE_COUNT (171)
//#define VARIABLE_SIZE (VARIABLE_COUNT*sizeof(int))
typedef union {
    int Array[VARIABLE_COUNT];
    struct {   					
        int My_Addres;      //e. 0 - device own address 
 
        //e. parameters of HFO regulator    
        int HF_ref;    		//e. 1 - value of the reference 
        int HF_scl;         //e. 2 - the gain factor (1.15)    
        int HF_min;         //e. 3 - minimum of the output value on the regulator DAC (appropriate to maximal voltage on the HFO)
        int HF_max;         //e. 4 - maximum of the output value on the regulator DAC (appropriate to minimal voltage on the HFO)

        //e. parameters of the regulator of the DS power regulator 
        int RI_ref;         //e. 5 - value of the reference 
        int RI_scl;         //e. 6 - the gain factor (1.15) 

        //e. parameters of the CPLC regulator 
        int WP_ref;         //e. 7 -  value of the reference
        int WP_scl;         //e. 8 - the gain factor (1.15)
        int WP_mdy;         //e. 9 - value of the reset delay
        int WP_rup;         //e. 10 - upper value of DAC adjustment (appropriate to minimal voltage on the heater)                                    
        int WP_rdw;         //e. 11 - lower value of the DAC adjustment (appropriate to maximal voltage on the heater) 
	
        //e. parameters of the dither drive regulator of the GLD 
        int VB_phs;             //e. 12 - the phase delay parameter of the dither drive PLL 
        int VB_scl;             //e. 13 - the gain factor (1.15) of the dither drive PLL  
        unsigned int VB_N;      //e. 14 - divider for dither drive period (defines dither period) 
        unsigned int VB_Nmin;   //e. 15 - minimum of the output value of the oscillation period regulator
        unsigned int VB_Nmax;   //e. 16 - maximum of the output value of the oscillation period regulator

        int VB_Fdf_Hi;      //e. 17 - adjusted output frequency (H) 
        unsigned int VB_Fdf_Lo;      //e. 18 - (L) (double precision)                      

        int VB_Fsc;         //e. 19 - the gain factor of the frequency regulator             
        unsigned int VB_Tmin;        //e. 20 - the gain factor of the frequency regulator 
        unsigned int VB_Tmax;        //e. 21 - maximum of the output value of the regulator 
        unsigned int VB_tau;         //e. 22 - pulse width of the dither drive (without noise)
        int VBN_Tzd;        //e. 23 - adjusted noise period 
        int VBN_Ran;        //e. 24 - range of the random component of noise                                
        int VBN_k;          //e. 25 - adjusted noise constant 
        
        //e. parameters of processing of accurate data  
        int PI_Fsc; //e.  26 - the S_ds scale parameter
        int PI_Fb0; //e.  27 - the B_ds zero shift of the DS 
        int PI_scl; //e.  28 - the Scale scale coefficient of the gyro 
        int PI_bia; //e.  29 - the Bias zero shift of the gyro 

        //e. coefficients of the temperature correction  
        int PI_a0; //  30
        int PI_a1; //  31
        int PI_a2; //  32
        int PI_a3; //  33 
        int PI_a4; //  34 PLC Mod ampl
        int PI_a5; //  35
        int PI_a6; //  36
        int PI_b1; //  37
        int PI_b2; //  38
        int PI_b3; //  39 PLC Mod freq
        int PI_b4; //  40
        int PI_b5; //  41 gain of HF ADC
        int PI_b6; //  42

        //e. parameters of normalization of the temperature sensors 
        int Tmp_bias[6]; //e. 43 - an array of shifts of the 0..5 temperature sensors 
        int Tmp_scal[6]; //e. 49 - an array of the scale coefficients of temperature sensors
        int WP_reset;    //e. 55 - initial position of the CPLC regulator (after reset)

        //e. gain factor of photodetector channels 
        unsigned int Gain_Ph_A; //e. 56 - initial gain factor of the A channel of photodetector
        unsigned int Gain_Ph_B; //e. 57 - initial gain factor of the B channel of photodetector

        //e. switch of the source of loading GLD variables block
        int Header_Word;			//e. 58 - flash sector validity header  	
        int LoadFlash_enable;		//e. 59 - source loading flag: 1 - load from the flash , 0 - load default parameters (factory)
        
        unsigned int Device_SerialNumber;		//e. 60 - serial number of the device
        int Reserved0;       //e. 61 - not used
        int	TermoMode;	//e  62 - device operation mode (with thermocompenstion, without it, debug )
        //e. addition for the piecewise-linear termocorrection 
        int TemperInt[TERMO_FUNC_SIZE];	//e. 63 
        int TermoFunc[TERMO_FUNC_SIZE];	//e. 77
        int WP_reset2;          //e. 91 - voltages of CPLC regulator reset at cooling 	//e. parameters setting termocompensation parameters in dynamics (at heating and cooling)
        int Reserved1;        //e. 92 - not used
        int Reserved2;        //e. 93 - not used
        int K_vb_tu; 			//e. 94 - slope of dependence of the VB_N division factor from temperature (as though Hz/degree, but in relative units ) 
        int TemperNormal; 		//e. 95 -temperature for which the VB_N division factor of the dither drive is set 
        int K_WP_rst_heating; 	//r. 96 - 
        int K_WP_rst_cooling; 	//e. 97 - slope of dependence of a reset voltage at cooling from temperature (as though Volt/degree, but in relative units of DAC and temperature sensors)
        int WP_transition_step; //e. 98 - step of change of a heater voltage at resetting 
        int Reserved3;          //e. 99 -not used
        int HF_scl_2;           //e. 100 -
        int TemperIntDyn[TERMO_FUNC_SIZE]; //e. 114 - 
        int ThermoHeatDelta[TERMO_FUNC_SIZE]; //e. 128 - 
        int DeltaTempRecalc;    //e. temperature delta for dynamic thermocompensation recalculation(in relative units)
        int TemperCoolIntDyn[TERMO_FUNC_SIZE]; // e. 143 - 
        int ThermoCoolDelta[TERMO_FUNC_SIZE];  //e. 157 - 
    } Str;
} TDEVICE_BLK;

/**
    @brief output exchange buffer
*/
typedef union {
    //int ArrayOut[31];
    struct {
        int	WP_reg; //e. PLC output voltage
        int WP_sin; //e. the signal of CPLC search - sine 250 Hz  
        int	WP_pll; //e. the XOR output of the PD of the CPLC regulator (phase detector)
        int HF_reg; //e. HFO output voltage
        int HF_dif;
        int WP_Phase_Det_Array[8]; //e. arrays for "worms" output
        int WP_sin_Array[8];
        int	WP_scope1;
        int WP_scope2; //e. temporary variables for Rate3 monitoring
        x_uint32_t F_ras; //e. really output frequency, divided by 16  (for transmission) 		    
        x_uint32_t L_Vibro; //e. period of oscillation of the dither drive 
        x_uint32_t T_Vibro; //e. pulse width of the dither drive 
        x_uint32_t T_VB_pll; //e. PD output of the frequency regulator of the dither drive 
        int PS_dif; //
        int BINS_dif;
        int Cnt_Dif;
        int Cnt_Pls;
        int Cnt_Mns;
        x_int16_t Tmp_Out[6];
        __int64 SF_dif;
    } Str;	 
} OUTPUT;

/**
    @brief input exchange buffer
*/
/*typedef union {
    int ArrayIn[4];
    struct {		  							
        int Tmp_in[2];
        int HF_out;	
        int WP_sel;			       	
    }StrIn;
} INPUT; */

typedef union {
    int array[6];
    struct {		  							
        int temp1;//< termo1
        int in1;//< currency PhA
        int in2;//< currency PhB	
        int delta_t;//< termo2 + uterm
        int hf_out; //< signal value AmL
        int wp_sel;	//< signal value AmH		       	
    }word;
} inputData;

/**
    @brief RgConA - регистр управления A, main register of device control
*/
typedef union{
    struct{
        unsigned Las: 1; //< 0- выключить лазер
        unsigned HFG: 1; //< 0 - выключить контур ГВЧ;
        unsigned Rlr: 1; //< 0 – выключить контур мощности ДУП;
        unsigned WPr: 1; //< 0 – выключить контур СРП;
        unsigned WPs: 1; //< 0 - выключить сигнал поиска СРП
        unsigned VBT: 1; //< 0 – выключить контур амплитуды вибропривода;
        unsigned VBF: 1; //< 0 – выключить контур частоты ВП
        unsigned GLD: 1; //< 0 – выключить все контуры ГЛД
        unsigned : 8;
    }bit;
    x_uint16_t word;
}reg_RgConA;

/**
    @brief RgConB- дополнительный регистр управления B, additional register of device control
*/
typedef union{
    struct{
        unsigned Vib0: 1; //< 1- включить сглаживающую фильтрацию вибросчетчиков лазер;
        unsigned Vib1: 1; //< 1 – включить контур компенсации виброподставки
        unsigned RM: 1; //< 1 – включить защелкивание счетчиков по восстановленному меандру
        unsigned : 13;
    }bit;
    x_uint16_t word;
}reg_RgConB;
#define RATE_REPER_OR_REFMEANDR 	0x0000		//e. difference of general counters (not dither counters), latched by Reper or by Sign Meander 
#define RATE_VIBRO_1	 			0x0001      //e. difference of dither counters after the filter of moving average


/**
    @brief Назначение битов байта достоверности
*/
typedef union{
    struct{
        unsigned : 3;
        unsigned thermo_delta_more: 1;
        unsigned thermo_out_of_range: 1;
        unsigned fho_voltage_more: 1;
        unsigned frq_dither_out_of_range: 1;
        unsigned frq_splitted_less_normal: 1;
    }bit;
    x_uint8_t word;
}reg_valid;

/**
    @brief B.I.T - регистр самотестирования;
*/

/**
    @brief Line_Err - регистр ошибок линии
*/
typedef union{
    struct{
        unsigned : 1;
        unsigned lcc: 1;
        unsigned siz: 1;
        unsigned : 3;
        unsigned stp: 1;
        unsigned str: 1;
        unsigned cd: 1;
        unsigned ce: 1;
        unsigned me: 1;
        unsigned pe: 1;
        unsigned : 4;
    }bit;
    x_uint16_t word;
}reg_line_err;

#endif