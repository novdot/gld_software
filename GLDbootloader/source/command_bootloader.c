#include "bootloader/command_bootloader.h"
#include "hardware/hardware.h"
#include "core/global.h"

void command_ans_common0();
void command_ans_common1();
void command_ans_m_status();
/******************************************************************************/
void command_handle()
{
	x_uint32_t uCmdCode = 0;
    x_uint32_t uCmdCodeLong = 0;

    uCmdCode = (rcv_buf[2] & 0xFF) << 8;
    CMD_Code = uCmdCode | (rcv_buf[3] & 0xFF);
    
    //e. initialization of the flag of copying of receiving buffer
	rx_buf_copy = 1;
    
    switch(uCmdCode){
		case  CMD_M_PTR_R    :   command_cmd_M_PTR_R();  return;
		case  CMD_M_PTR_W    :   command_cmd_M_PTR_W();  return;
		case  CMD_M_DAT_R    :   command_cmd_M_DAT_R();  return;
		case  CMD_M_DAT_W    :   command_cmd_M_DAT_W();  return;
		case  CMD_M_BUF_R    :   command_cmd_M_BUF_R();  return;
		case  CMD_M_BUF_W    :   command_cmd_M_BUF_W();  return;                                                        
		case  CMD_M_CTL_R    :   command_cmd_M_CTL_R();  return;
		case  CMD_M_CTL_M    :   command_cmd_M_CTL_M();  return;
		case  CMD_M_FME_E    :   command_cmd_M_FME_E();  return;
		
		case  0x0f00    :   
		case  0x9900    :  
		case  CMD_M_SUBCMD1    :  
		case  CMD_M_SUBCMD2    :  
			uCmdCodeLong = uCmdCode | (rcv_buf[3] & 0xFF);
			break;

		default:
            line_sts = line_sts | CODE_ERR;
			return;
	}
	switch(uCmdCodeLong){
		case  CMD_WRK_PC     :   command_cmd_WRK_PC();   return;
		case  CMD_MAINT      :   command_cmd_MAINT();    return;
		case  CMD_M_JUMP     :   command_cmd_M_JUMP();   return;
		case  CMD_M_LOAD     :   command_cmd_M_LOAD();   return;
		case  CMD_M_CONF     :   command_cmd_M_CONF();   return;
		case  CMD_M_DCNF     :   command_cmd_M_DCNF();   return;
		case  CMD_M_CLEAR    :   command_cmd_M_CLEAR();  return;
		case  CMD_M_MIRR     :   command_cmd_M_MIRR();   return;
		case  CMD_M_TSIV1    :   command_cmd_M_TSIV1();  return;
		case  CMD_M_TSOV2    :	 command_cmd_M_TSOV2();	 return;
		
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
void command_cmd_WRK_PC()
{
	//nothing
}

/******************************************************************************/
void command_cmd_MAINT()
{
	//Включает режим монитора ГЛД и откладывает старт основной программы.
	
	//составляем ответ
	command_ans_m_status();
}

/******************************************************************************/
void command_cmd_M_JUMP()
{
	//run app
}

/******************************************************************************/
void command_cmd_M_LOAD()
{
	//load from mem
    //run app
}

/******************************************************************************/
void command_cmd_M_CONF()
{
	//config pld = nothing
}

/******************************************************************************/
void command_cmd_M_DCNF()
{
	//reset pld = nothing
}

/******************************************************************************/
void command_cmd_M_CLEAR()
{
    //очистка регистра ошибок
    line_err = 0;
	//составляем ответ
	command_ans_m_status();
}

/******************************************************************************/
void command_cmd_M_MIRR()
{
	//TODO
}

/******************************************************************************/
void command_cmd_M_TSIV1()
{
	command_ans_common0();
}

/******************************************************************************/
void command_cmd_M_TSOV2()
{
	command_ans_common0();
}

/******************************************************************************/
void command_cmd_M_PTR_R()
{
	command_utility_SetSpeedPeriod();         		  
	UART_SwitchSpeed(trm_rate);
}

/******************************************************************************/
void command_cmd_M_PTR_W()
{
	command_utility_SetSpeedPeriod();         		  
	UART_SwitchSpeed(trm_rate);
}

/******************************************************************************/
void command_cmd_M_DAT_R()
{
	command_utility_SetSpeedPeriod();         		  
	UART_SwitchSpeed(trm_rate);
}

/******************************************************************************/
void command_cmd_M_DAT_W()
{
	command_utility_SetSpeedPeriod();         		  
	UART_SwitchSpeed(trm_rate);
}

/******************************************************************************/
void command_cmd_M_BUF_R()
{
	//
}

/******************************************************************************/
void command_cmd_M_BUF_W()
{
	
}

/******************************************************************************/
void command_cmd_M_CTL_R()
{
	
}

/******************************************************************************/
void command_cmd_M_CTL_M()
{
	
}

/******************************************************************************/
void command_cmd_M_FME_E()
{
	//erase flash
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/**
    answer
*/
void command_ans_common0()
{
	num_of_par = 2;
	COMMAND_UTILITY_ANSWER_FIELD(0,&num_of_par,2);//TODO
	COMMAND_UTILITY_ANSWER_FIELD(1,&line_err,2);
	trm_ena = 1;
}

void command_ans_common1()
{
	num_of_par = 2;
	COMMAND_UTILITY_ANSWER_FIELD(0,&num_of_par,2);
	COMMAND_UTILITY_ANSWER_FIELD(1,&line_err,2);
	trm_ena = 1;
}

void command_ans_m_status()
{
	//num_of_par = 1;
	//COMMAND_UTILITY_ANSWER_FIELD(0,&blt_in_test,2);
	//trm_ena = 1;
}

/******************************************************************************/

void command_ans_WRK_PC()
{
	//nothing
}

/******************************************************************************/
void command_ans_MAINT()
{
	command_ans_m_status();
}

/******************************************************************************/
void command_ans_M_JUMP()
{
	
}

/******************************************************************************/
void command_ans_M_LOAD()
{
	
}

/******************************************************************************/
void command_ans_M_CONF()
{
	
}

/******************************************************************************/
void command_ans_M_DCNF()
{
	
}

/******************************************************************************/
void command_ans_M_CLEAR()
{
	command_ans_m_status();
}

/******************************************************************************/
void command_ans_M_MIRR()
{
	
}

/******************************************************************************/
void command_ans_M_TSIV1()
{
	
}

/******************************************************************************/
void command_ans_M_TSOV2()
{
	
}

/******************************************************************************/
void command_ans_M_PTR_R()
{
	
}

/******************************************************************************/
void command_ans_M_PTR_W()
{
	
}

/******************************************************************************/
void command_ans_M_DAT_R()
{
	
}

/******************************************************************************/
void command_ans_M_DAT_W()
{
	
}

/******************************************************************************/
void command_ans_M_BUF_R()
{
	
}

/******************************************************************************/
void command_ans_M_BUF_W()
{
	
}

/******************************************************************************/
void command_ans_M_CTL_R()
{
	
}

/******************************************************************************/
void command_ans_M_CTL_M()
{
	
}

/******************************************************************************/
void command_ans_M_FME_E()
{
	
}

/******************************************************************************/