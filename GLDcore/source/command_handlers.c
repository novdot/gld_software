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
#include "core/command_handlers.h"
#include "core/global.h"
#include "core/types.h"
#include "hardware/hardware.h"

#include "core/dither.h"
#include "core/ignit.h"

//TODO
//#include "InputOutput.h"
#include "CyclesSync.h"
#include "Parameters.h"


/******************************************************************************/
void command_handle(void)
{
    x_uint32_t uCmdCode = 0;
    x_uint32_t uCmdCodeLong = 0;

    uCmdCode = (rcv_buf[2] & 0xFF) << 8;
    CMD_Code = uCmdCode | (rcv_buf[3] & 0xFF);
    
    //e. initialization of the flag of copying of receiving buffer
	rx_buf_copy = 1;
    
    switch(uCmdCode){
        case CMD_DELTA_PS    : command_cmd_DELTA_PS();	return;
        case CMD_DELTA_BINS  : command_cmd_DELTA_BINS();	return;
        case CMD_DELTA_SF	 : command_cmd_DELTA_SF(); return;
        case CMD_DEV_MODE    : command_cmd_DEV_MODE(); return;
        case CMD_BIT_MODE    : command_cmd_BIT_MODE();	return;
        case CMD_RATE        : command_cmd_RATE();	return;
        case CMD_DELTA       : command_cmd_DELTA();	return;
        case CMD_D_PERIOD_W  : command_cmd_D_PERIOD_W();	return;

        case SUBCMD_M_STIMUL : command_subcmd_M_STIMUL();  return;
        case SUBCMD_M_RESET  : command_subcmd_M_RESET();  return;

        case SUBCMD_M_CTL_R  : command_subcmd_M_CTL_R();  return;
        case SUBCMD_M_CTL_M  : command_subcmd_M_CTL_M();  return;

        case SUBCMD_M_TMP_W  : command_subcmd_M_TMP_W();  return;
        case SUBCMD_M_TMP_R  : command_subcmd_M_TMP_R();  return;
        case SUBCMD_M_E5R_W  : command_subcmd_M_E5R_W();  return;
        case SUBCMD_M_ADC_R  : command_subcmd_M_ADC_R();  return;
        case SUBCMD_M_VIB_W  : command_subcmd_M_VIB_W();  return;
        case SUBCMD_M_CNT_R  : command_subcmd_M_CNT_R();  return;
        case SUBCMD_M_GPH_W  : command_subcmd_M_GPH_W();  return;
        case SUBCMD_M_FLG_R  : command_subcmd_M_FLG_R();  return;
        case SUBCMD_M_PARAM_W: command_subcmd_M_PARAM_W();  return;
        case SUBCMD_M_PARAM_R: command_subcmd_M_PARAM_R();  return;
        case SUBCMD_M_E5RA_W : command_subcmd_M_E5RA_W();  return;

        //check mask of cmd code
        case SUBCMD_M_RATE_MASK  : 
            uCmdCodeLong = uCmdCode | (rcv_buf[3] & 0x1F);
            break;

        case CMD_MAINT_MASK  : 
        case SUBCMD_M_MASK  :
            uCmdCodeLong = uCmdCode | (rcv_buf[3] & 0xFF);
            break;
        default:
            line_sts = line_sts | CODE_ERR;
            return;
    }
    
    switch(uCmdCodeLong){
        case CMD_MAINT       :  command_cmd_MAINT(); return;

        case SUBCMD_M_CLEAR  :  command_subcmd_M_CLEAR(); return;
        case SUBCMD_M_MIRR   :  command_subcmd_M_MIRR(); return;
        case SUBCMD_M_LDPAR_F:  command_subcmd_M_LDPAR_F(); return;
        case SUBCMD_M_LDPAR_D:  command_subcmd_M_LDPAR_D(); return;
        case SUBCMD_M_START  :  command_subcmd_M_START(); return;
        case SUBCMD_M_STOP   :  command_subcmd_M_STOP(); return;
        case SUBCMD_M_PULSE  :  command_subcmd_M_PULSE(); return;

        case SUBCMD_M_RATE1  :  command_subcmd_M_RATE1(); return;
        case SUBCMD_M_RATE2  :  command_subcmd_M_RATE2(); return;
        case SUBCMD_M_RATE3  :  command_subcmd_M_RATE3(); return;
        case SUBCMD_M_RATE7  :  command_subcmd_M_RATE7(); return;
        case SUBCMD_M_RATE5K :  command_subcmd_M_RATE5K(); return;
        
        default: 
            line_sts = line_sts | MODE_ERR;
			return;
    }
}

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
void command_cmd_DELTA_PS_EXEC()
{
    static void * paramTable[11] = {
        &(Output.Str.F_ras) 
        , &(Output.Str.HF_reg)
        , &(Output.Str.T_Vibro)
        , &(Output.Str.L_Vibro)
        , &(Output.Str.WP_reg)
        , &(Output.Str.Tmp_Out[0])
        , &(Output.Str.Tmp_Out[1])
        , &(Output.Str.Tmp_Out[2])
        , &(Output.Str.Tmp_Out[3])
        , &(Output.Str.Tmp_Out[4])
        , &(Output.Str.Tmp_Out[5])
    };
	static uint32_t val, paramTmpWord;
	static uint32_t * ptr;
	static uint32_t index = 0;
		   
    // high byte
	if ((index & 1) == 0){
		ptr = (uint32_t*)paramTable[index >> 1];
		val = *ptr;
        // move it to low byte of word
		paramTmpWord = val >> 8; 
	} else {
		paramTmpWord = val;
	}
	paramTmpWord &= 0xFF;
	paramTmpWord |= index << 8;
	
    // reset all bits of status word
	//Valid_Data = 0;
    //g_gld.valid.word = 0;
    Valid_Data = 0;
 
    index++;
	if (index > 21) {
		index = 0;
	}
    
    command_ans_DELTA_PS_EXEC(paramTmpWord);
    
	return;
}
/******************************************************************************/	
void command_cmd_DELTA_BINS()
{
	//e. set in the additional register of device control the mode of work with 
	//dither counters  and the filter of moving average
	g_gld.RgConB.word = RATE_VIBRO_1;
	command_utility_SetSpeedPeriod(); 
	UART_SwitchSpeed(trm_rate);
	CMD_Mode = 4;
	// reset all bits of status word
    //g_gld.valid.word = 0;
    Valid_Data = 0;
	
	command_ans_DELTA_BINS();
	return;
}	
/******************************************************************************/
void command_cmd_DELTA_SF()
{
	//e. time for data transfer has come, we work only with dither counters 
	//dither counters  and the filter of moving average
	g_gld.RgConB.word = RATE_VIBRO_1;
	// reset all bits of status word
    //g_gld.valid.word = 0;	
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
            //PLC_reg - 0
            //преобразов до доп кода и сдвиг
            Output.Str.WP_reg = (data-2048)<<4;
            break;
        case 2:
            //Dither - 2 DS regulator ДУП
            //Output.Str. = data;
            break;
        case 3:
            //HFO_reg - 3
            Output.Str.HF_reg = (data-2048)<<4;
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
	Output.Str.T_Vibro = (rcv_buf[4] << 8) | (rcv_buf[5] & 0xFF); //period 
	Output.Str.L_Vibro= (rcv_buf[6] << 8) | (rcv_buf[7] & 0xFF); //pulse width 
    //Output.Str.L_Vibro /= 2;
    
    /*if( Output.Str.L_Vibro<(Output.Str.T_Vibro/100 + 1) ){
        Output.Str.L_Vibro = Output.Str.T_Vibro/100 + 1;
    }*/
    
    Device_blk.Str.VB_N = Output.Str.T_Vibro; 
    Device_blk.Str.VB_tau = Output.Str.L_Vibro;
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
	hardware_photo_out(Device_blk.Str.Gain_Ph_A, Device_blk.Str.Gain_Ph_B);
	
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
	if (!g_gld.RgConB.word) {
	    g_gld.RgConB.word = RATE_VIBRO_1;
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
    
    g_gld.RgConB.word =  RATE_REPER_OR_REFMEANDR;
    SwitchRefMeandInt(RATE_REPER_OR_REFMEANDR);
    
    // reset all bits of status word
    //g_gld.valid.word = 0;
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
    
    g_gld.RgConB.word = RATE_REPER_OR_REFMEANDR;
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
    //e. and set the answer transfer rate and its periodicity
	command_utility_SetSpeedPeriod();
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
void command_ans_DELTA_PS_EXEC(x_uint32_t paramTmpWord)
{
    num_of_par = 2;
    COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint32_t*)&Output.Str.PS_dif,2);
    COMMAND_UTILITY_ANSWER_FIELD(1,&paramTmpWord,2);
    trm_ena = 1; 
}

/******************************************************************************/
void command_ans_DELTA_PS()
{
}

/******************************************************************************/
void command_ans_DELTA_BINS()
{
	num_of_par = 2;        			//e. 2 parameters output 
	COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint32_t*)&Output.Str.BINS_dif,4);
	//COMMAND_UTILITY_ANSWER_FIELD(1,&(g_gld.valid.word),1);
	COMMAND_UTILITY_ANSWER_FIELD(1,&(Valid_Data),1);
	//e. allow operation of the transmitter
	trm_ena = 1;        			 
}

/******************************************************************************/
void command_ans_DELTA_SF()
{
	num_of_par = 5;
	COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint32_t*)&Output.Str.SF_dif,8);
	COMMAND_UTILITY_ANSWER_FIELD(1,&Out_main_cycle_latch,4);
	COMMAND_UTILITY_ANSWER_FIELD(2,&Out_T_latch,2);
	COMMAND_UTILITY_ANSWER_FIELD(3,(x_uint32_t*)&Output.Str.WP_reg,2);	
	COMMAND_UTILITY_ANSWER_FIELD(4,(x_uint32_t*)&Output.Str.Tmp_Out,12);
	trm_ena = 1;        			//e. allow operation of the transmitter
}
/******************************************************************************/
void command_ans_DEV_MODE()
{
	num_of_par = 2; //e. 2 parameters output
	//e. address of the counter mode register (intenal latch, external latch, etc.)
	COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint32_t*)&Device_Mode,2);
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
		COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint32_t*)&g_gld.RgConA.word,2);
	} else {
		COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint32_t*)&g_gld.RgConB.word,2);
	}

	trm_ena = 1;                 //e. allow operation of the transmitter of line
}
/******************************************************************************/
void command_ans_M_CTL_M()
{
	x_uint16_t * ptr;
	x_uint32_t bit_numb;
	
	num_of_par = 2;        
	COMMAND_UTILITY_ANSWER_FIELD(0,&CMD_Code,2);
	COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint32_t*)ptr,2);   
	
	//e. is main control register needed?
	if ((CMD_Code & (1 << 4)) == 0) {
		ptr = &(g_gld.RgConA.word);
	} else {
		//e. otherwise - load the address of the addititonal register
		ptr = &(g_gld.RgConB.word);
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
    //фотоприемник А
	COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint32_t*)&(g_input.word.in1),2);
    //фотоприемник Б
	COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint32_t*)&(g_input.word.in2),2);
    //ВЧ АМ-детектора
	COMMAND_UTILITY_ANSWER_FIELD(2,(x_uint32_t*)&(g_input.word.hf_out),2);
    //НЧ АМ-детектора
	COMMAND_UTILITY_ANSWER_FIELD(3,(x_uint32_t*)&(g_input.word.wp_sel),2);
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
	COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint32_t*)&(Output.Str.Cnt_Pls),2);
	COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint32_t*)&(Output.Str.Cnt_Mns),2);
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
	addr_param[0] =  (x_uint32_t*)&rcv_copy;         //e. set the address of the receiving buffer 

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
    
	
    COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint32_t*)&(Output.Str.Cnt_Pls),2);
    COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint32_t*)&(Output.Str.Cnt_Mns),2);
    COMMAND_UTILITY_ANSWER_FIELD(2,(x_uint32_t*)&(Output.Str.Cnt_Dif),2);
    COMMAND_UTILITY_ANSWER_FIELD(3,(x_uint32_t*)&(Output.Str.F_ras),2);//частота расщипления
    COMMAND_UTILITY_ANSWER_FIELD(4,(x_uint32_t*)&(Output.Str.HF_reg),2);
    COMMAND_UTILITY_ANSWER_FIELD(5,(x_uint32_t*)&(Output.Str.HF_dif),2);
    COMMAND_UTILITY_ANSWER_FIELD(6,&(Output.Str.T_Vibro),2);
    COMMAND_UTILITY_ANSWER_FIELD(7,&(Output.Str.T_VB_pll),2);
    COMMAND_UTILITY_ANSWER_FIELD(8,&(Output.Str.L_Vibro),2);
    COMMAND_UTILITY_ANSWER_FIELD(9,(x_uint32_t*)&(g_input.word.hf_out),2);
    COMMAND_UTILITY_ANSWER_FIELD(10,(x_uint32_t*)&(Output.Str.WP_reg),2);
    COMMAND_UTILITY_ANSWER_FIELD(11,(x_uint32_t*)&(Output.Str.WP_pll),2); //phase detector
    COMMAND_UTILITY_ANSWER_FIELD(12,(x_uint32_t*)&(Output.Str.Tmp_Out),12); //темпер;
    COMMAND_UTILITY_ANSWER_FIELD(13,(x_uint32_t*)&(Output.Str.WP_scope1),4); //резерв;
	
    trm_ena = 1;        //allow operation of the transmitter 
} 
/******************************************************************************/
void command_ans_M_RATE2()
{
    num_of_par = 2;     //2 parameters output
	
    COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint32_t*)&(Output.Str.Cnt_Pls),2);
    COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint32_t*)&(Output.Str.Cnt_Mns),2);
    
    trm_ena = 1;
} 
/******************************************************************************/
void command_ans_M_RATE3()
{
    num_of_par = 2;     //e. 2 parameters output
			
    COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint32_t*)&(Output.Str.WP_scope1),2);
    COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint32_t*)&(Output.Str.WP_scope2),2);
    
    trm_ena = 1;        //e. allow operation of the transmitter
} 

/******************************************************************************/
void command_ans_M_RATE4()
{
    num_of_par = 4;  
			
    COMMAND_UTILITY_ANSWER_FIELD(0,0,16);
    COMMAND_UTILITY_ANSWER_FIELD(1,0,32);
    COMMAND_UTILITY_ANSWER_FIELD(2,(x_uint32_t*)&(Output.Str.HF_reg),2);
    COMMAND_UTILITY_ANSWER_FIELD(3,(x_uint32_t*)&(Output.Str.WP_reg),2);
    
    //e. allow operation of the transmitter
    trm_ena = 1; 
} 

/******************************************************************************/
void command_ans_M_RATE7()
{
    num_of_par = 5; 
    
    COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint32_t*)&(Output.Str.WP_Phase_Det_Array),16);
    COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint32_t*)&(Output.Str.WP_sin_Array),16);
    COMMAND_UTILITY_ANSWER_FIELD(2,(x_uint32_t*)&(Output.Str.WP_reg),2);
    COMMAND_UTILITY_ANSWER_FIELD(3,(x_uint32_t*)&(Output.Str.WP_pll),2);
    COMMAND_UTILITY_ANSWER_FIELD(4,(x_uint32_t*)&(Output.Str.HF_reg),2);
    
    trm_ena = 1;
} 
/******************************************************************************/
void command_ans_M_RATE5K()
{
}
/******************************************************************************/