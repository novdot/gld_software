#ifndef __COMMAND_HANDLERS_H_INCLUDED 
#define __COMMAND_HANDLERS_H_INCLUDED

#include "xlib/types.h"

/******************************************************************************/
#define  CMD_DELTA_PS    	0xa000  //e. the code of the B_DeltaPS command 
#define  CMD_DELTA_BINS  	0xb000  //e. the code of the B_DeltaBINS command
#define  CMD_DELTA_SF	  	0xb200  //e. the code of the B_DeltaSF command 
#define  CMD_DEV_MODE    	0xA500  //e. the code of the Device_Mode command
#define  CMD_BIT_MODE    	0xA700  //e. the code of the B.I.T. command 
#define  CMD_RATE        	0x0000  //e. the code of the Rate command  
#define  CMD_DELTA       	0x0f00  //e. the code of the Delta command
#define  CMD_D_PERIOD_W  	0xf000  //e. the code of the D_Period_W command 

#define  CMD_MAINT_MASK     0x9900
#define  CMD_MAINT       	0x9999  //e. the code of the Maintenance mode command

#define  SUBCMD_M_STIMUL    0x0a00  //e. the code of the Stimul command 
#define  SUBCMD_M_RESET     0x8000  //e. software device reset(restart) 

#define  SUBCMD_M_MASK      0xda00
#define  SUBCMD_M_CLEAR     0xda00  //e. reset of the register of errors of line 485 
#define  SUBCMD_M_MIRR      0xda01  //e. return previous contents of the receiver buffer 
#define  SUBCMD_M_LDPAR_F   0xda02  //e. load GLD parameters from Flash-memory 
#define  SUBCMD_M_LDPAR_D   0xda03  //e. set GLD parameters by procedure of initialization 
#define  SUBCMD_M_START     0xda04  //e. switch on the GLD 
#define  SUBCMD_M_STOP      0xda40  //e. switch off the GLD 
#define  SUBCMD_M_PULSE     0xda0a  //e. generation of the light-up pulse 
                                                                      
#define  SUBCMD_M_CTL_R     0xd900  //e. reading the control register of the device 
#define  SUBCMD_M_CTL_M     0xd800  //e. modification of bit of the control register 

#define  SUBCMD_M_TMP_W     0xe000  //e. writing to the AD7714 circuit (temperature sensors) 
#define  SUBCMD_M_TMP_R     0xe100  //e. reading from the AD7714 circuit (temperature sensors)
#define  SUBCMD_M_E5R_W     0xe200  //e. writing to the register of a mode of the Elio5 card 
#define  SUBCMD_M_ADC_R     0xe300  //e. reading data from the ADC of the Elio5 card 
#define  SUBCMD_M_VIB_W     0xe400  //e. loading of the oscillation period of the dither drive
#define  SUBCMD_M_CNT_R     0xe500  //e. reading data from the counters of the Elio5 card 
#define  SUBCMD_M_GPH_W     0xe600  //e. loading gain factors of photodetector channels 
#define  SUBCMD_M_FLG_R     0xe700  //e. reading data from the register of input flags
#define  SUBCMD_M_PARAM_W   0xe800  //e. writing working parameter to the data memory
#define  SUBCMD_M_PARAM_R   0xe900  //e. reading working parameter of the GLD from the data memory
#define  SUBCMD_M_E5RA_W    0xea00  //e. writing in 1st additional (A) register of a mode of the Elio5 card  

#define SUBCMD_M_RATE_MASK  0xdd00 //e. delivery of a stack of parameters M_Rate (Rate2, Rate3) 
#define SUBCMD_M_RATE       0xdd00 //e. delivery of a stack of parameters M_Rate (Rate2, Rate3) 
#define	SUBCMD_M_RATE1      0xdd01          
#define	SUBCMD_M_RATE2      0xdd02  
#define	SUBCMD_M_RATE3      0xdd03 
#define	SUBCMD_M_RATE7      0xdd07
#define SUBCMD_M_RATE5K 	0xdd14

#define SUBCMD_SYSTEMRESET 	0xEF00

/******************************************************************************/
void command_cmd_DELTA_PS();
void command_cmd_DELTA_PS_EXEC();	
void command_cmd_DELTA_BINS();
void command_cmd_DELTA_SF();
void command_cmd_DEV_MODE();
void command_cmd_BIT_MODE();
void command_cmd_RATE();
void command_cmd_DELTA();
void command_cmd_D_PERIOD_W();
void command_subcmd_M_STIMUL();
void command_subcmd_M_RESET();
void command_subcmd_M_CTL_R();
void command_subcmd_M_CTL_M();
void command_subcmd_M_TMP_W();
void command_subcmd_M_TMP_R();
void command_subcmd_M_E5R_W();
void command_subcmd_M_ADC_R();
void command_subcmd_M_VIB_W();
void command_subcmd_M_CNT_R();
void command_subcmd_M_GPH_W();
void command_subcmd_M_FLG_R();
void command_subcmd_M_PARAM_W();
void command_subcmd_M_PARAM_R();
void command_subcmd_M_E5RA_W();
void command_cmd_MAINT();
void command_subcmd_M_CLEAR();
void command_subcmd_M_MIRR();
void command_subcmd_M_LDPAR_F();
void command_subcmd_M_LDPAR_D();
void command_subcmd_M_START();
void command_subcmd_M_STOP();
void command_subcmd_M_PULSE();
void command_subcmd_M_RATE1();
void command_subcmd_M_RATE2();
void command_subcmd_M_RATE3();
void command_subcmd_M_RATE7();
void command_subcmd_M_RATE5K();

void command_ans_common(void);
void command_ans_device_status(void);
void command_ans_DELTA_PS_EXEC(x_uint32_t* paramTmpWord);
void command_ans_DELTA_PS();
void command_ans_DELTA_BINS();
void command_ans_DELTA_SF();
void command_ans_DEV_MODE();
void command_ans_BIT_MODE();
void command_ans_RATE();
void command_ans_DELTA();
void command_ans_D_PERIOD_W();
void command_ans_M_STIMUL();
void command_ans_M_RESET();
void command_ans_M_CTL_R();
void command_ans_M_CTL_M();
void command_ans_M_TMP_W();
void command_ans_M_TMP_R();
void command_ans_M_E5R_W();
void command_ans_M_ADC_R();
void command_ans_M_VIB_W();
void command_ans_M_CNT_R();
void command_ans_M_GPH_W();
void command_ans_M_FLG_R();
void command_ans_M_PARAM_W();
void command_ans_M_PARAM_R();
void command_ans_M_E5RA_W();
void command_ans_MAINT();
void command_ans_M_CLEAR();
void command_ans_M_MIRR();
void command_ans_M_LDPAR_F();
void command_ans_M_LDPAR_D();
void command_ans_M_START();
void command_ans_M_STOP();
void command_ans_M_PULSE();
void command_ans_M_RATE1();
void command_ans_M_RATE2();
void command_ans_M_RATE3();
void command_ans_M_RATE4();
void command_ans_M_RATE7();
void command_ans_M_RATE5K();

#endif