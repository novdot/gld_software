/**
  ******************************************************************************
  * @file    command_handlers.с
  *
  * @brief   Обработчики команд
    В зависимости от типа команды прочитать параметры,
    заполнить буффер сообщений к отправке
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
#include "core/command.h"
#include "core/global.h"
#include "core/types.h"
#include "hardware/hardware.h"

#include "core/dither.h"
#include "core/ignit.h"

//TODO
#include "InputOutput.h"
#include "CyclesSync.h"
#include "Parameters.h"

/******************************************************************************/
//e procedure of set of rate and periodicity of answer 
void command_utility_SetSpeedPeriod(void)
{
	if ((rcv_buf[3] & 0x0080) != 0) //e. is periodic data transmission needed? 
	{
		trm_cycl = 1;		//e. yes, set present flag 
	}
	else
	{  
		trm_cycl = 0;		//e. no, reset present flag 
	}
	
	SRgR &= 0xffcf;				//e. clear the bit of transfer rate
	trm_rate = (rcv_buf[3] >> 1) & 0x0030;
	SRgR |= trm_rate; 			//e. set present transfer rate
} // SetSpeedPeriod

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void command_cmd_DELTA_PS()
{
	//e. and set the answer transfer rate and its periodicity 
	command_utility_SetSpeedPeriod();         		  
	UART_SwitchSpeed(trm_rate);
	//e. work with internal latch
	if (Device_Mode < 4)	 
	Device_Mode = DM_INT_LATCH_DELTA_PS;
	else
	Device_Mode = DM_EXT_LATCH_DELTA_PS_PULSE;
	CMD_Mode = 1;
	
	return;
}
/******************************************************************************/	
void command_cmd_DELTA_BINS()
{
	//e. set in the additional register of device control the mode of work with 
	//dither counters  and the filter of moving average
	RgConB = RATE_VIBRO_1;
	command_utility_SetSpeedPeriod(); 
	UART_SwitchSpeed(trm_rate);
	CMD_Mode = 4;
	// reset all bits of status word
	Valid_Data = 0;
	
	command_ans_DELTA_BINS();
	return;
}	
/******************************************************************************/
void command_cmd_DELTA_SF()
{
	//e. time for data transfer has come, we work only with dither counters 
	//dither counters  and the filter of moving average
	RgConB = RATE_VIBRO_1;
	// reset all bits of status word
	Valid_Data = 0;		

	command_ans_DELTA_SF();
	return;
} 
/******************************************************************************/
void command_cmd_DEV_MODE()
{
	CMD_Mode = 3;
	//e. read the byte of command parameter from the receiver buffer
	//e. and write it to the counter mode register
	Device_Mode = rcv_buf[3] & 0x00ff;
	//e. periodic data transmission is not needed
	trm_cycl = 0;
	
	command_ans_DEV_MODE();
	return;
} 
/******************************************************************************/
void command_cmd_BIT_MODE()
{
	x_uint8_t temp;
	CMD_Mode = 6;       //e. set the value of the Delta mode
	
	temp = rcv_buf[3] & 0x000f & (~((rcv_buf[3] & 0x00f0) >> 4));
	if (temp == 1) {
		Is_BIT = 1;
		BIT_number = (long)(rcv_buf[4] & 0x00FF) << 24;
		BIT_number |= (long)(rcv_buf[5] & 0x00FF) << 16;
		BIT_number |= (long)(rcv_buf[6] & 0x00FF) << 8;
		BIT_number |= (long)(rcv_buf[7] & 0x00FF);
	} else {
		Is_BIT = 0;
	}
	trm_cycl = 0;	//e. periodic data transmission is not needed
	
	command_ans_DEV_MODE();
	return;
}	
/******************************************************************************/
void command_cmd_RATE()
{
	return;
}
/******************************************************************************/
void command_cmd_DELTA()
{
	//e. set the value of the Delta mode
	CMD_Mode = 2; 
	return;      
}	
/******************************************************************************/
void command_cmd_D_PERIOD_W()
{
	return;
}	
/******************************************************************************/
void command_subcmd_M_STIMUL()
{
	x_uint8_t chan;
	x_uint32_t data;
	//e. WP_PHASE_DETECTOR allocate a mode of display for a control point of an scope
	//ScopeMode = 4; 
	//e. extracting the number of stimulus
    //HFO_reg - 3, PLC_reg - 0
	chan = (int)rcv_buf[3] & 0x0007; 
	//chan = CMD_Code & 0x0007;     
    data = ( (rcv_buf[4]<<8) | rcv_buf[5] ) >>4 ;
    //Output.ArrayOut[chan] = (((int)rcv_buf[4] << 8) | (int)rcv_buf[5])-0x8000;
    switch(chan) {
        case 0:
            Output.Str.WP_reg = data;
            break;
        case 2:
            break;
        case 3:
            Output.Str.HF_reg = data;
            break;
        default:
            //check flags bit in stimul cmd
            if ((chan & (1 << 2)) != 0) {}
            break;
    }
		 	
	//e. to estimate: whether the answer is required 
	if ((CMD_Code & (1 << 7)) == 0) return;
	
	CMD_Code &= 0xff00;			//e. clear bits of errors 
	
	command_ans_common();
	return;
}
/******************************************************************************/
void command_subcmd_M_RESET()
{
	while(1);
}  
/******************************************************************************/
void command_subcmd_M_CTL_R()
{
	//e. clear in it bit of errors and byte number
	CMD_Code &= 0xff10;           
	command_ans_M_CTL_R();
	return;
}  
/******************************************************************************/
void command_subcmd_M_CTL_M()
{
	command_ans_M_CTL_M();
	return;
}  
/******************************************************************************/
void command_subcmd_M_TMP_W()
{
	//TODO
	command_ans_common();
	return;
} 
/******************************************************************************/
void command_subcmd_M_TMP_R()
{
	//TODO
	return;
}  
/******************************************************************************/
void command_subcmd_M_E5R_W()
{
	//TODO
	return;
} 
/******************************************************************************/
void command_subcmd_M_ADC_R()
{
	command_ans_M_ADC_R();
	return;
}  
/******************************************************************************/
void command_subcmd_M_VIB_W()
{
	Output.Str.T_Vibro = (rcv_buf[4] << 8) | (rcv_buf[5] & 0xFF); //e. new variable of the period 
	Output.Str.L_Vibro= (rcv_buf[6] << 8) | (rcv_buf[7] & 0xFF); //e. new variable of the pulse width 
	VibroDither_Set();       //e. and output its value to period registers on card
	trm_cycl = 0;      //e. periodic data transmission is not needed
	
	command_ans_common();
	return;
} 
/******************************************************************************/
void command_subcmd_M_CNT_R()
{
	command_utility_SetSpeedPeriod(); //e. set the answer transfer rate and its periodicity
	UART_SwitchSpeed(trm_rate);
	
	command_ans_M_CNT_R();
	return;
} 
/******************************************************************************/
void command_subcmd_M_GPH_W()
{
	//e. read from the receiver buffer the value of the gain factor of the A,B channel
	Device_blk.Str.Gain_Ph_A = rcv_buf[4];    
	Device_blk.Str.Gain_Ph_B = rcv_buf[5];

	//e. display these values to digital potentiometers 
	Out_G_photo(Device_blk.Str.Gain_Ph_A, Device_blk.Str.Gain_Ph_B);
	
	trm_cycl = 0;      //e. periodic data transmission is not needed
	command_ans_common();
	return;
}  
/******************************************************************************/
void command_subcmd_M_FLG_R()
{
	command_ans_M_FLG_R();
	return;
}

/******************************************************************************/
void command_subcmd_M_PARAM_W()
{
	int * ptr = 0;
	
	ptr = (int *)&Device_blk.Str.My_Addres; // pointer to Parameters block
	ptr += rcv_buf[3]; // calculate offset
	*ptr = (rcv_buf[4] << 8) | (rcv_buf[5] & 0xFF); // write new parameter value

	trm_cycl = 0; //e. periodic data transmission is not needed 
	
	command_ans_common();
	return;
}

/******************************************************************************/
void command_subcmd_M_PARAM_R()
{
	command_ans_M_PARAM_R();
	return;
}

/******************************************************************************/
void command_subcmd_M_E5RA_W()
{
	//TODO
	return;
}
/******************************************************************************/
void command_cmd_MAINT()
{
	if (!RgConB) {
	    RgConB = RATE_VIBRO_1;
		//e. disable interrupt from referense meander
		SwitchRefMeandInt(RATE_VIBRO_1);   
	}
	//e. yes, switch on present mode 
	CMD_Mode = 5;	
	//e. output the status stack of registers 
	command_ans_device_status();          
	//e. mode of internal latch 10 kHz /
	if (Device_Mode != DM_INT_10KHZ_LATCH) {
	   Device_Mode = DM_INT_10KHZ_LATCH;
	   trm_ena = 0;
	} else {
		UART_SwitchSpeed(trm_rate);
	}
	return;
}    
/******************************************************************************/
void command_subcmd_M_CLEAR()
{
	line_err = 0; //e. cleaning the error register 
	command_ans_device_status(); //e. preparing for trabsfer of the device status
	UART_SwitchSpeed(trm_rate);
	return;
} 
/******************************************************************************/
void command_subcmd_M_MIRR()
{
	rx_buf_copy = 0; //e. yes, forbid copying of command on saving previous
	command_ans_M_MIRR(); //e. prepare transfer of the receiving buffer copy 
	return;
}  
/******************************************************************************/
void command_subcmd_M_LDPAR_F()
{
	//e. load the GLD parameters from the flash-memory
	LoadFlashParam(FromFLASH);
	blt_in_test = ((uint32_t)FIRMWARE_VER << 8) | (Device_blk.Str.Device_SerialNumber & 0x00FF);
	//Init_software();
	trm_cycl = 0; //e. periodic data transmission is not needed 
	
	command_ans_common();
	return;
}
/******************************************************************************/
void command_subcmd_M_LDPAR_D()
{
	LoadFlashParam(ByDefault);
	//Init_software();
	trm_cycl = 0;

	command_ans_common();
	return;	
}
/******************************************************************************/
void command_subcmd_M_START()
{
	//start_Rq = 1;      	//e. set the flag of the GLD switch on request
	//TODO
    ignit_set_request(_x_true);
    trm_cycl = 0;      	//e. periodic data transmission is not needed 
	command_ans_common();
	return;	
}
/******************************************************************************/
void command_subcmd_M_STOP()
{
	stop_Rq = 1;      	//e. set the flag of the GLD switch off request
	trm_cycl = 0;      	//e. periodic data transmission is not needed 
	command_ans_common();
	return;	
}
/******************************************************************************/
void command_subcmd_M_PULSE()
{
	//pulse_Rq = 1;      	//e. set the flag of the GLD switch on request
	ignit_set_request(_x_true);
    trm_cycl = 0;      	//e. periodic data transmission is not needed 
	command_ans_common();
	return;	
}

/******************************************************************************/
void command_subcmd_M_RATE1()
{
    command_utility_SetSpeedPeriod();
    UART_SwitchSpeed(trm_rate);
    
    RgConB =  RATE_REPER_OR_REFMEANDR;
    SwitchRefMeandInt(RATE_REPER_OR_REFMEANDR);
    
    // reset all bits of status word
    Valid_Data = 0;
    
    //e. load needed length of working period 1
    wrk_period = 25000000; 
    
    command_ans_M_RATE1();
    SetIntLatch(wrk_period);
}
/******************************************************************************/
void command_subcmd_M_RATE2()
{
    command_utility_SetSpeedPeriod();
    UART_SwitchSpeed(trm_rate);
    
    RgConB = RATE_REPER_OR_REFMEANDR;
    //e. enable interrupt from ref. meander
    SwitchRefMeandInt(RATE_REPER_OR_REFMEANDR);
    
    //e. frequency of output = fvibro 
    wrk_period = 0;     

    command_ans_M_RATE2();
    SetIntLatch(wrk_period);
}
/******************************************************************************/
void command_subcmd_M_RATE3()
{
    command_utility_SetSpeedPeriod();
    UART_SwitchSpeed(trm_rate);
    
    wrk_period = 2500;
    
    command_ans_M_RATE3();
    SetIntLatch(wrk_period);
}
/******************************************************************************/
void command_subcmd_M_RATE7()
{
    command_utility_SetSpeedPeriod();
    UART_SwitchSpeed(trm_rate);
    
    wrk_period = 20000;
    
    command_ans_M_RATE7();
    SetIntLatch(wrk_period);
}
/******************************************************************************/
void command_subcmd_M_RATE5K()
{
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
#define COMMAND_UTILITY_ANSWER_FIELD(index,ptr_addr,size) \
	addr_param[index] = ptr_addr; \
	size_param[index] = size;

/******************************************************************************/
void command_ans_common(void)
{
	//prepare of the standart answer
	num_of_par = 1;             //e. total amount parameters in aswer - 1 
	COMMAND_UTILITY_ANSWER_FIELD(0,&CMD_Code,2);
	trm_ena = 1;              	//e. allow operation of the transmitter of the device
}

/******************************************************************************/
void command_ans_device_status(void)
{
	command_utility_SetSpeedPeriod();        //e. and set the answer transfer rate and its periodicity 
	num_of_par = 2;
	//e. the register address of the self-testing result
	COMMAND_UTILITY_ANSWER_FIELD(0,&blt_in_test,2);
	//e. address of the register of errors of line
	COMMAND_UTILITY_ANSWER_FIELD(1,&ser_num,2);

	trm_rate = 0;       //e. set the transfer rate to the 38400 bauds
	trm_cycl = 0;       //e. forbid cyclic transmission of the parameter 
	trm_ena = 1;        //e. allow operation of the transmitter
}

/******************************************************************************/
void command_ans_DELTA_PS()
{
}

/******************************************************************************/
void command_ans_DELTA_BINS()
{
	num_of_par = 2;        			//e. 2 parameters output 
	COMMAND_UTILITY_ANSWER_FIELD(0,&Output.Str.BINS_dif,4);
	COMMAND_UTILITY_ANSWER_FIELD(1,&Valid_Data,1);
	//e. allow operation of the transmitter
	trm_ena = 1;        			 
}

/******************************************************************************/
void command_ans_DELTA_SF()
{
	num_of_par = 5;
	COMMAND_UTILITY_ANSWER_FIELD(0,&Output.Str.SF_dif,8);
	COMMAND_UTILITY_ANSWER_FIELD(1,&Out_main_cycle_latch,4);
	COMMAND_UTILITY_ANSWER_FIELD(2,&Out_T_latch,2);
	COMMAND_UTILITY_ANSWER_FIELD(3,&Output.Str.WP_reg,2);	
	COMMAND_UTILITY_ANSWER_FIELD(4,&Output.Str.Tmp_Out,12);
	trm_ena = 1;        			//e. allow operation of the transmitter
}
/******************************************************************************/
void command_ans_DEV_MODE()
{
	num_of_par = 2; //e. 2 parameters output
	//e. address of the counter mode register (intenal latch, external latch, etc.)
	COMMAND_UTILITY_ANSWER_FIELD(0,&Device_Mode,2);
	//e. address of the mode register of the processor card 
	COMMAND_UTILITY_ANSWER_FIELD(1,&SRgR,2);
	trm_cycl = 0; //e. forbid cyclic transmission of the parameter 
	trm_ena = 1; //e. allow operation of the transmitter
}
/******************************************************************************/
void command_ans_BIT_MODE()
{
}
/******************************************************************************/
void command_ans_RATE()
{
}
/******************************************************************************/
void command_ans_DELTA()
{
}
/******************************************************************************/
void command_ans_D_PERIOD_W()
{
}
/******************************************************************************/
void command_ans_M_STIMUL()
{
}
/******************************************************************************/
void command_ans_M_RESET()
{
}
/******************************************************************************/
void command_ans_M_CTL_R()
{
	num_of_par = 2; //e. 2 parameters transfer
	
	COMMAND_UTILITY_ANSWER_FIELD(0,&CMD_Code,2);
	if ((rcv_buf[3] & (1 << 4)) == 0) {
		COMMAND_UTILITY_ANSWER_FIELD(1,&RgConA,2);
	} else {
		COMMAND_UTILITY_ANSWER_FIELD(1,&RgConB,2);
	}

	trm_ena = 1;                 //e. allow operation of the transmitter of line
}
/******************************************************************************/
void command_ans_M_CTL_M()
{
	uint32_t * ptr;
	uint32_t bit_numb;
	
	num_of_par = 2;        
	COMMAND_UTILITY_ANSWER_FIELD(0,&CMD_Code,2);
	COMMAND_UTILITY_ANSWER_FIELD(1,ptr,2);   
	
	//e. is main control register needed?
	if ((CMD_Code & (1 << 4)) == 0) {
		ptr = &RgConA;
	} else {
		//e. otherwise - load the address of the addititonal register
		ptr = &RgConB;
	}
	
	//e. extract the number of the changeable bit
	bit_numb = CMD_Code & 0x000f;     
	
	if ((CMD_Code & (1 << 7)) == 0) {
		*ptr &= ~(1 << bit_numb); 	// yes, clear bit
	}else{
		*ptr |= 1 << bit_numb;		// no, set bit
	}
	
	CMD_Code &= 0xff10; //e. clear in command bit of errors and byte number 
	trm_cycl = 0; //e. forbid cyclic transmission of the parameter
	trm_ena = 1; //e. allow operation of the transmitter of line 
}
/******************************************************************************/
void command_ans_M_TMP_W()
{
}
/******************************************************************************/
void command_ans_M_TMP_R()
{
}
/******************************************************************************/
void command_ans_M_E5R_W()
{
}
/******************************************************************************/
void command_ans_M_ADC_R()
{
	//e. set the answer transfer rate and its periodicity 
	command_utility_SetSpeedPeriod(); 
	UART_SwitchSpeed(trm_rate);
	
	num_of_par = 4;     		//e. 4 parameters output
	COMMAND_UTILITY_ANSWER_FIELD(0,0,2);
	COMMAND_UTILITY_ANSWER_FIELD(1,0,2);
	COMMAND_UTILITY_ANSWER_FIELD(2,0,2);
	COMMAND_UTILITY_ANSWER_FIELD(3,&(g_input.word.hf_out),2);
	trm_ena = 1;
}
/******************************************************************************/
void command_ans_M_VIB_W()
{
}
/******************************************************************************/
void command_ans_M_CNT_R()
{
	num_of_par = 2;     		//e. 2 parameters output
	COMMAND_UTILITY_ANSWER_FIELD(0,&(Output.Str.Cnt_Pls),2);
	COMMAND_UTILITY_ANSWER_FIELD(1,&(Output.Str.Cnt_Mns),2);
	trm_ena = 1;        		//e. allow operation of the transmitter
}
/******************************************************************************/
void command_ans_M_GPH_W()
{
}
/******************************************************************************/
void command_ans_M_FLG_R()
{
	num_of_par = 1; //e. 1 parameter output
	COMMAND_UTILITY_ANSWER_FIELD(0,&In_Flag,2);
	trm_ena = 1; //e. allow operation of the transmitter
}
/******************************************************************************/
void command_ans_M_PARAM_W()
{
}
/******************************************************************************/
void command_ans_M_PARAM_R()
{
	num_of_par = 1;	
	COMMAND_UTILITY_ANSWER_FIELD(0,(void*)(&Device_blk.Str.My_Addres + rcv_buf[3]),2);
	trm_ena = 1; 
}
/******************************************************************************/
void command_ans_M_E5RA_W()
{
}
/******************************************************************************/
void command_ans_MAINT()
{
}  
/******************************************************************************/
void command_ans_M_CLEAR()
{
}
/******************************************************************************/
void command_ans_M_MIRR()
{
	num_of_par = 1;     //e. 1 parameter output 
	addr_param[0] =  &rcv_copy;         //e. set the address of the receiving buffer 

	//e. multiply the number of copied words on 2, since each 
	//e. will be trasferred in two steps: first zero, and then 
	size_param[0] = rcv_byt_copy << 1; // multiplay by 2
	if (size_param[0] >= 64) {
		size_param[0] = 64;		//e. maximal amount - no more than double length of the copy buffer 
	}
	trm_rate = 0;       //e. set the transfer rate to the 38400 bauds
	trm_cycl = 0;       //e. forbid cyclic transmission of the parameter
	trm_ena = 1;        //e. allow operation of the transmitter
}
/******************************************************************************/
void command_ans_M_LDPAR_F()
{
	
}
/******************************************************************************/
void command_ans_M_LDPAR_D()
{
}
/******************************************************************************/
void command_ans_M_START()
{
}
/******************************************************************************/
void command_ans_M_STOP()
{
}
/******************************************************************************/
void command_ans_M_PULSE()
{
}
/******************************************************************************/
void command_ans_M_RATE1()
{
    num_of_par = 14;
	
    COMMAND_UTILITY_ANSWER_FIELD(0,&(Output.Str.Cnt_Pls),2);
    COMMAND_UTILITY_ANSWER_FIELD(1,&(Output.Str.Cnt_Mns),2);
    COMMAND_UTILITY_ANSWER_FIELD(2,&(Output.Str.Cnt_Dif),2);
    COMMAND_UTILITY_ANSWER_FIELD(3,&(Output.Str.F_ras),2);
    COMMAND_UTILITY_ANSWER_FIELD(4,&(Output.Str.HF_reg),2);
    COMMAND_UTILITY_ANSWER_FIELD(5,&(Output.Str.HF_dif),2);
    COMMAND_UTILITY_ANSWER_FIELD(6,&(Output.Str.T_Vibro),2);
    COMMAND_UTILITY_ANSWER_FIELD(7,&(Output.Str.T_VB_pll),2);
    COMMAND_UTILITY_ANSWER_FIELD(8,&(Output.Str.L_Vibro),2);
    COMMAND_UTILITY_ANSWER_FIELD(9,&(g_input.word.hf_out),2);
    COMMAND_UTILITY_ANSWER_FIELD(10,&(Output.Str.WP_reg),2);
    COMMAND_UTILITY_ANSWER_FIELD(11,&(Output.Str.WP_pll),2);
    COMMAND_UTILITY_ANSWER_FIELD(12,&(Output.Str.Tmp_Out),12);
    COMMAND_UTILITY_ANSWER_FIELD(13,&(Output.Str.WP_scope1),4);
	
    trm_ena = 1;        //e. allow operation of the transmitter 
} 
/******************************************************************************/
void command_ans_M_RATE2()
{
    num_of_par = 2;     //e. 2 parameters output
	
    COMMAND_UTILITY_ANSWER_FIELD(0,&(Output.Str.Cnt_Pls),2);
    COMMAND_UTILITY_ANSWER_FIELD(1,&(Output.Str.Cnt_Mns),2);
    
    trm_ena = 1;
} 
/******************************************************************************/
void command_ans_M_RATE3()
{
    num_of_par = 2;     //e. 2 parameters output
			
    COMMAND_UTILITY_ANSWER_FIELD(0,&(Output.Str.WP_scope1),2);
    COMMAND_UTILITY_ANSWER_FIELD(1,&(Output.Str.WP_scope2),2);
    
    trm_ena = 1;        //e. allow operation of the transmitter
} 

/******************************************************************************/
void command_ans_M_RATE4()
{
    num_of_par = 4;  
			
    COMMAND_UTILITY_ANSWER_FIELD(0,0,16);
    COMMAND_UTILITY_ANSWER_FIELD(1,0,32);
    COMMAND_UTILITY_ANSWER_FIELD(2,&(Output.Str.HF_reg),2);
    COMMAND_UTILITY_ANSWER_FIELD(3,&(Output.Str.WP_reg),2);
    
    //e. allow operation of the transmitter
    trm_ena = 1; 
} 

/******************************************************************************/
void command_ans_M_RATE7()
{
    num_of_par = 5; 
    
    COMMAND_UTILITY_ANSWER_FIELD(0,&(Output.Str.WP_Phase_Det_Array),16);
    COMMAND_UTILITY_ANSWER_FIELD(1,&(Output.Str.WP_sin_Array),16);
    COMMAND_UTILITY_ANSWER_FIELD(2,&(Output.Str.WP_reg),2);
    COMMAND_UTILITY_ANSWER_FIELD(3,&(Output.Str.WP_pll),2);
    COMMAND_UTILITY_ANSWER_FIELD(4,&(Output.Str.HF_reg),2);
    
    trm_ena = 1;
} 
/******************************************************************************/
void command_ans_M_RATE5K()
{
}
/******************************************************************************/