#include "bootloader/command_bootloader.h"
#include "bootloader/global.h"
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
	//Включает режим монитора ГЛД и откладывает старт основной программы. TODO
	
	//составляем ответ
	command_ans_m_status();
}

/******************************************************************************/
void command_cmd_M_JUMP()
{
	//run app TODO
}

/******************************************************************************/
void command_cmd_M_LOAD()
{
	//load from mem TODO
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
    bootloader_paramsField params;
    x_uint32_t data = 0;
    
    //set answer speed
	command_utility_SetSpeedPeriod();         		  
	UART_SwitchSpeed(trm_rate);
    //читаем что за указатель к нам пришел
    params.word = rcv_buf[3]&0x1F;
    
    //запишем в нужный контейнер
    switch(params.bit.code) {
        case BOOTLOADER_PTR_CODE_JUMP : 
            data = g_bootloader.ptr.nPtrJump; 
        break;
        case BOOTLOADER_PTR_CODE_BUF  : 
            data = g_bootloader.ptr.nPtrBuf; 
        break;
        case BOOTLOADER_PTR_CODE_EXE  : 
            data = g_bootloader.ptr.nPtrExe; 
        break;
        case BOOTLOADER_PTR_CODE_DATA : 
            data = g_bootloader.ptr.nPtrData; 
        break;    
        case BOOTLOADER_PTR_CODE_IO   : 
            data = g_bootloader.ptr.nPtrIo; 
        break; 
        case BOOTLOADER_PTR_CODE_FLASH:
            data = g_bootloader.ptr.nPtrFlash; 
        break;   
        case BOOTLOADER_PTR_CODE_DEV1 : 
            data = g_bootloader.ptr.nPtrDev1; 
        break;
        case BOOTLOADER_PTR_CODE_DEV2 : 
            data = g_bootloader.ptr.nPtrDev2; 
        break;
    }
    
    //формируем ответ
    command_ans_M_PTR_R(data);
}

/******************************************************************************/
void command_cmd_M_PTR_W()
{
	bootloader_paramsField params;
    x_uint32_t data;
    
    //set answer speed
	command_utility_SetSpeedPeriod();         		  
	UART_SwitchSpeed(trm_rate);
    //читаем что за указатель к нам пришел
    params.word = rcv_buf[3]&0x1F;
    
    //read param value
    data = (rcv_buf[4]<<16) + (rcv_buf[5]<<8) + (rcv_buf[6]<<0);
    //save in g_bootloader
    switch(params.bit.code) {
        case BOOTLOADER_PTR_CODE_JUMP : 
            g_bootloader.ptr.nPtrJump = data; 
        break;
        case BOOTLOADER_PTR_CODE_BUF  : 
            g_bootloader.ptr.nPtrBuf = data; 
        break;
        case BOOTLOADER_PTR_CODE_EXE  : 
            g_bootloader.ptr.nPtrExe = data; 
        break;
        case BOOTLOADER_PTR_CODE_DATA : 
            g_bootloader.ptr.nPtrData = data; 
        break;    
        case BOOTLOADER_PTR_CODE_IO   : 
            g_bootloader.ptr.nPtrIo = data; 
        break; 
        case BOOTLOADER_PTR_CODE_FLASH: 
            g_bootloader.ptr.nPtrFlash = data; 
        break;   
        case BOOTLOADER_PTR_CODE_DEV1 : 
            g_bootloader.ptr.nPtrDev1 = data; 
        break;
        case BOOTLOADER_PTR_CODE_DEV2 : 
            g_bootloader.ptr.nPtrDev2 = data; 
        break;
    }
    command_ans_common();
    //формируем ответ
    //command_ans_M_PTR_W();
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
	//TODO erase flash
    command_ans_common1();
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/**
    answer
*/
void command_ans_common(void)
{
	//prepare of the standart answer
	num_of_par = 1;             //e. total amount parameters in aswer - 1 
	COMMAND_UTILITY_ANSWER_FIELD(0,&CMD_Code,2);
	trm_ena = 1;              	//e. allow operation of the transmitter of the device
}
/******************************************************************************/
void command_ans_common0()
{
	num_of_par = 2;
	COMMAND_UTILITY_ANSWER_FIELD(0,&num_of_par,2);//TODO адрес первого параметра
	COMMAND_UTILITY_ANSWER_FIELD(1,&line_err,2);
	trm_ena = 1;
}

/******************************************************************************/
void command_ans_common1()
{
    //take cmd 
    g_bootloader.cmd.nCmdCodeH = (rcv_buf[2]&0xFF);
    g_bootloader.cmd.nCmdCodeL = (rcv_buf[3]&0xFF);
    //clear error code
    g_bootloader.cmd.nCmdCodeL &= 0x1F;//TODO add error code
    
	num_of_par = 2;
	COMMAND_UTILITY_ANSWER_FIELD(0,&g_bootloader.cmd.nCmdCodeH,1);
	COMMAND_UTILITY_ANSWER_FIELD(1,&g_bootloader.cmd.nCmdCodeL,1);
	trm_ena = 1;
}

/******************************************************************************/
void command_ans_m_status()
{
	num_of_par = 2;
	COMMAND_UTILITY_ANSWER_FIELD(0,&blt_in_test,2);
    COMMAND_UTILITY_ANSWER_FIELD(1,&line_err,2);
	trm_ena = 1;
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
	//
}

/******************************************************************************/
void command_ans_M_TSIV1()
{
	//
}

/******************************************************************************/
void command_ans_M_TSOV2()
{
	//
}

/******************************************************************************/
int par0 = 0x55;
int par1 = 0x01;
int par2 = 0x02;
int par3 = 0x03;

void command_ans_M_PTR_R(x_uint32_t data)
{
    //hardware_backlight_on();
    g_bootloader.cmd.nCmdCodeH = (rcv_buf[2]&0xFF);
    g_bootloader.cmd.send_data_ptr[0] = (data>>0)&0xff;
    g_bootloader.cmd.send_data_ptr[1] = (data>>8)&0xff;
    g_bootloader.cmd.send_data_ptr[2] = (data>>16)&0xff;
    
    g_bootloader.cmd.nCmdCodeH = 0x55;
    g_bootloader.cmd.send_data_ptr[0] = 0x01;
    g_bootloader.cmd.send_data_ptr[1] = 0x02;
    g_bootloader.cmd.send_data_ptr[2] = 0x03;
    
    num_of_par = 4;
	COMMAND_UTILITY_ANSWER_FIELD(0,&(g_bootloader.cmd.nCmdCodeH),1);
	COMMAND_UTILITY_ANSWER_FIELD(1,&(g_bootloader.cmd.send_data_ptr[0]),1);
	COMMAND_UTILITY_ANSWER_FIELD(2,&(g_bootloader.cmd.send_data_ptr[1]),1);
	COMMAND_UTILITY_ANSWER_FIELD(3,&(g_bootloader.cmd.send_data_ptr[2]),1);
	trm_ena = 1;
}

/******************************************************************************/
void command_ans_M_PTR_W()
{
    g_bootloader.cmd.nCmdCodeH = (rcv_buf[2]&0xFF);
    //take cmd 
    g_bootloader.cmd.nCmdCodeL = (rcv_buf[3] & 0xFF);
    //clear error code
    g_bootloader.cmd.nCmdCodeL &= 0x1F; //TODO add error code
    
    num_of_par = 2;
	COMMAND_UTILITY_ANSWER_FIELD(0,&g_bootloader.cmd.nCmdCodeH,1);
	COMMAND_UTILITY_ANSWER_FIELD(1,&g_bootloader.cmd.nCmdCodeL,1);
	trm_ena = 1;
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
	command_ans_common1();
}

/******************************************************************************/