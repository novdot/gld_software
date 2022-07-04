#include "bootloader/command_bootloader.h"
#include "bootloader/global.h"
#include "hardware/hardware.h"
#include "core/global.h"
#include "core/command.h"
#include "xlib/ring_buffer.h"

void command_ans_common0();
void command_ans_common1();
void command_ans_m_status();

/******************************************************************************/
void command_handle()
{
	x_uint32_t uCmdCode = 0;
    x_uint32_t uCmdCodeLong = 0;
    char dbg[64];
    char dbg1[64];
    int i = 0;
    int iCRC_calc = 0;

    uCmdCode = (g_gld.cmd.recieve_cmd[2] & 0xFF) << 8;
    CMD_Code = uCmdCode | (g_gld.cmd.recieve_cmd[3] & 0xFF);
    
    //e. initialization of the flag of copying of receiving buffer
	g_gld.cmd.flags.bit.rx_cpy = 1;
    
    switch(uCmdCode){
	case  CMD_M_PTR_R    : 
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_PTR_R\n\r");  
        command_cmd_M_PTR_R();  
        return;
    
	case  CMD_M_PTR_W    : 
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_PTR_W\n\r");  
        command_cmd_M_PTR_W();  
        return;
    
	case  CMD_M_DAT_R    :  
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_DAT_R\n\r"); 
        command_cmd_M_DAT_R();  
        return;
    
	case  CMD_M_DAT_W    :   
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_DAT_W\n\r");
        command_cmd_M_DAT_W();  
        return;
    
    case  CMD_M_BUF_R    :   
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_BUF_R\n\r");
        command_cmd_M_BUF_R();  
        return;
    
    case  CMD_M_BUF_W    :   
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_BUF_W\n\r");
        command_cmd_M_BUF_W();  
        return;      
    
    case  CMD_M_CTL_R    :  
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_CTL_R\n\r"); 
        command_cmd_M_CTL_R();  
        return;
    
    case  CMD_M_CTL_M    :   
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_CTL_M\n\r");
        command_cmd_M_CTL_M();  
        return;
    
    case  CMD_M_FME_E    :   
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_FME_E\n\r");
        command_cmd_M_FME_E();  
        return;
    
    case  0x0f00    :   
    case  0x9900    :  
    case  CMD_M_SUBCMD1    :  
    case  CMD_M_SUBCMD2    :  
        uCmdCodeLong = uCmdCode | (g_gld.cmd.recieve_cmd[3] & 0xFF);
        break;
    
    default:
        //DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"ERR CODE\n\r");
        line_sts = line_sts | MODE_ERR;
        return;
	}
    
	switch(uCmdCodeLong){
    case  CMD_WRK_PC     :   
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_WRK_PC\n\r");
        command_cmd_WRK_PC();   
        return;
    
    case  CMD_MAINT      :   
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_MAINT\n\r");
        command_cmd_MAINT();    
        return;
    
    case  CMD_M_JUMP     :   
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_JUMP\n\r");
        command_cmd_M_JUMP();   
        return;
    
    case  CMD_M_LOAD     :   
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_LOAD\n\r");
        command_cmd_M_LOAD();   
        return;
    
    case  CMD_M_CONF     :   
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_CONF\n\r");
        command_cmd_M_CONF();   
        return;
    
    case  CMD_M_DCNF     :   
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_DCNF\n\r");
        command_cmd_M_DCNF();   
        return;
    
    case  CMD_M_CLEAR    :   
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_CLEAR\n\r");
        command_cmd_M_CLEAR();  
        return;
    
    case  CMD_M_MIRR     :   
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_MIRR\n\r");
        command_cmd_M_MIRR();   
        return;
    
    case  CMD_M_TSIV1    :   
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_TSIV1\n\r");
        command_cmd_M_TSIV1();  
        return;
    
    case  CMD_M_TSOV2    :	 
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"CMD_M_TSOV2\n\r");
        command_cmd_M_TSOV2();	 
        return;
    
    default:
        //DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"ERR CODE\n\r");
        line_sts = line_sts | MODE_ERR;
        return;
	}
    /***
    for (iCRC_calc = 0; iCRC_calc < (g_gld.cmd.recieve_cmd_size); iCRC_calc++){
            sprintf(dbg1,"%s%x",dbg1,g_gld.cmd.recieve_cmd[iCRC_calc]);
    }
    DBG1(&g_gld.cmd.dbg.ring_out,dbg,64,"%s'\n\r",dbg1);  
    /***/
    
    return;
}

void command_save_prevCmd()
{
    memcpy(g_bootloader.prevCmd.buf, g_gld.cmd.recieve_cmd, g_gld.cmd.recieve_cmd_size);
    g_bootloader.prevCmd.size = g_gld.cmd.recieve_cmd_size;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void command_cmd_WRK_PC()
{
    command_save_prevCmd();
	//nothing
}

/******************************************************************************/
void command_cmd_MAINT()
{
    command_save_prevCmd();
	//Включает режим монитора ГЛД и откладывает старт основной программы.
	g_bootloader.bMonitorMode = 1;
	//составляем ответ
	command_ans_m_status();
}

/******************************************************************************/
void command_cmd_M_JUMP()
{
    command_save_prevCmd();
	//run app
    hardware_tim_stop();
    hardware_flash_load_main();
}

/******************************************************************************/
void command_cmd_M_LOAD()
{
    command_save_prevCmd();
	//load from mem TODO
    //run app
    hardware_tim_stop();
    hardware_flash_load_main();
}

/******************************************************************************/
void command_cmd_M_CONF()
{
    command_save_prevCmd();
	//config pld
    command_ans_common1();
}

/******************************************************************************/
void command_cmd_M_DCNF()
{
    command_save_prevCmd();
	//reset pld
    command_ans_common1();
}

/******************************************************************************/
void command_cmd_M_CLEAR()
{
    command_save_prevCmd();
    //очистка регистра ошибок
    line_err = 0;
	//составляем ответ
	command_ans_m_status();
}

/******************************************************************************/
void command_cmd_M_MIRR()
{
	//возврат предыдущего содержимого буфера приемника
    //подкоманда возврата предыдущей команды
    command_ans_M_MIRR();
}

/******************************************************************************/
void command_cmd_M_TSIV1()
{
    command_save_prevCmd();
    //Проверка ОЗУ памяти данных
    //TODO
    
	command_ans_common0();
}

/******************************************************************************/
void command_cmd_M_TSOV2()
{
    command_save_prevCmd();
    //Проверка ОЗУ памяти данных
    //TODO
    
	command_ans_common0();
}

/******************************************************************************/
void command_cmd_M_PTR_R()
{
    bootloader_paramsField params;
    x_uint32_t data = 0;
    
    command_save_prevCmd();
    
    //set answer speed
	/*command_utility_SetSpeedPeriod();         		  
	UART_SwitchSpeed(trm_rate);*/
    command_utility_read_param();
    command_SwitchSpeed();
    
    //читаем что за указатель к нам пришел
    params.word = g_gld.cmd.recieve_cmd[3]&0x1F;
    
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
    
    command_save_prevCmd();
    
    //set answer speed
	/*command_utility_SetSpeedPeriod();         		  
	UART_SwitchSpeed(trm_rate);*/
    command_utility_read_param();
    command_SwitchSpeed();
    
    //читаем что за указатель к нам пришел
    params.word = g_gld.cmd.recieve_cmd[3]&0x1F;
    
    //read param value
    data = (g_gld.cmd.recieve_cmd[4]<<16) + (g_gld.cmd.recieve_cmd[5]<<8) + (g_gld.cmd.recieve_cmd[6]<<0);
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
    //формируем ответ
    command_ans_M_PTR_W();
}

/******************************************************************************/
void command_cmd_M_DAT_R()
{
    command_save_prevCmd();
    
	/*command_utility_SetSpeedPeriod();         		  
	UART_SwitchSpeed(trm_rate);*/
    
    command_utility_read_param();
    command_SwitchSpeed();
    
    //чтение блока данных из буфера у-ва. 128byte
    //формируем ответ
    command_ans_M_DAT_R();
}

/******************************************************************************/
void command_cmd_M_DAT_W()
{
    command_save_prevCmd();
    
	/*command_utility_SetSpeedPeriod();         		  
	UART_SwitchSpeed(trm_rate);*/
    
    command_utility_read_param();
    command_SwitchSpeed();
    
    //запись блока данных в буфер у-ва
    
    //answer
    command_ans_common1();
}

/******************************************************************************/
void command_cmd_M_BUF_R()
{
    command_save_prevCmd();
	//TODO from flash
    command_ans_M_BUF_R();
}

/******************************************************************************/
void command_cmd_M_BUF_W()
{
    command_save_prevCmd();
	//TODO to flash
    command_ans_M_BUF_W();
}

/******************************************************************************/
void command_cmd_M_CTL_R()
{
    int regType = 0;
    x_uint32_t reg = 0;
    
    command_save_prevCmd();
    
    g_bootloader.cmd.nCmdCodeH = (g_gld.cmd.recieve_cmd[2]&0xFF);
    g_bootloader.cmd.nCmdCodeL = (g_gld.cmd.recieve_cmd[3]&0xFF);
    
    //бит выбора регистра
    regType = (g_bootloader.cmd.nCmdCodeL>>4) & 0x1 ;
    switch(regType){
    case 0:
        reg = g_gld.RgConA.word;
        break;
    case 1:
        reg = g_gld.RgConB.word;
        break;
    }
	command_ans_M_CTL_R(&reg);
}

/******************************************************************************/
void command_cmd_M_CTL_M()
{
    int regType = 0;
    int regBitInd = 0;
    int regBitVal = 0;
    x_uint32_t reg = 0;
    
    command_save_prevCmd();
    
    g_bootloader.cmd.nCmdCodeH = (g_gld.cmd.recieve_cmd[2]&0xFF);
    g_bootloader.cmd.nCmdCodeL = (g_gld.cmd.recieve_cmd[3]&0xFF);
    
    //бит выбора регистра
    regType = (g_bootloader.cmd.nCmdCodeL>>4) & 0x1 ;
    switch(regType){
    case 0:
        reg = g_gld.RgConA.word;
        break;
    case 1:
        reg = g_gld.RgConB.word;
        break;
    }
    //перепишем бит
    regBitInd = g_bootloader.cmd.nCmdCodeL & 0xF;
    regBitVal = g_bootloader.cmd.nCmdCodeL>>7;
    
	command_ans_M_CTL_M(&reg);
}

/******************************************************************************/
void command_cmd_M_FME_E()
{
    command_save_prevCmd();
	//erase flash TODO
    
    //answer
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
	COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint16_t*)&CMD_Code,2);
	trm_ena = 1;              	//e. allow operation of the transmitter of the device
}
/******************************************************************************/
void command_ans_common0()
{
	num_of_par = 2;
	COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint16_t*)&num_of_par,2);//TODO адрес первого параметра
	COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint16_t*)&line_err,2);
	trm_ena = 1;
}

/******************************************************************************/
void command_ans_common1()
{
    char dbg[64];
    //take cmd 
    g_bootloader.cmd.nCmdCodeH = (g_gld.cmd.recieve_cmd[2]&0xFF);
    g_bootloader.cmd.nCmdCodeL = (g_gld.cmd.recieve_cmd[3]&0xFF);
    //clear error code
    g_bootloader.cmd.nCmdCodeL &= 0x1F;//TODO add error code
    
	num_of_par = 2;
	COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint16_t*)&(g_bootloader.cmd.nCmdCodeH),1);
	COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint16_t*)&(g_bootloader.cmd.nCmdCodeL),1);
	trm_ena = 1;
}

/******************************************************************************/
void command_ans_m_status()
{
	num_of_par = 2;
	COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint16_t*)&blt_in_test,2);
    COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint16_t*)&line_err,2);
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
    int ipar = 0;
	num_of_par = (g_bootloader.prevCmd.size)*2;
    for(ipar=0;ipar<g_bootloader.prevCmd.size;ipar++){
        COMMAND_UTILITY_ANSWER_FIELD(ipar*2,0,1);
        COMMAND_UTILITY_ANSWER_FIELD(ipar*2+1,(x_uint16_t*)&(g_bootloader.prevCmd.buf[ipar]),1);
    }
	trm_ena = 1;
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
void command_ans_M_PTR_R(x_uint32_t data)
{
    g_bootloader.cmd.nCmdCodeH = (g_gld.cmd.recieve_cmd[2]&0xFF);
    g_bootloader.cmd.send_data_ptr[0] = (data>>0)&0xff;
    g_bootloader.cmd.send_data_ptr[1] = (data>>8)&0xff;
    g_bootloader.cmd.send_data_ptr[2] = (data>>16)&0xff;
    
    num_of_par = 4;
	COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint16_t*)&(g_bootloader.cmd.nCmdCodeH),1);
	COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint16_t*)&(g_bootloader.cmd.send_data_ptr[2]),1);
	COMMAND_UTILITY_ANSWER_FIELD(2,(x_uint16_t*)&(g_bootloader.cmd.send_data_ptr[1]),1);
	COMMAND_UTILITY_ANSWER_FIELD(3,(x_uint16_t*)&(g_bootloader.cmd.send_data_ptr[0]),1);
	trm_ena = 1;
}

/******************************************************************************/
void command_ans_M_PTR_W()
{
    command_ans_common1();
}

/******************************************************************************/
void command_ans_M_DAT_R()
{
    int i = 0;
    g_bootloader.cmd.nCmdCodeH = (g_gld.cmd.recieve_cmd[2]&0xFF);
    
    for(i=0;i<BOOTLOADER_BUF_SIZE;i++){
        g_bootloader.buf128[i] = i;
    }
    
    num_of_par = 2;
	COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint16_t*)&(g_bootloader.cmd.nCmdCodeH),1);
	COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint16_t*)&(g_bootloader.buf128),BOOTLOADER_BUF_SIZE);
	trm_ena = 1;
}

/******************************************************************************/
void command_ans_M_DAT_W()
{
	
}

/******************************************************************************/
void command_ans_M_BUF_R()
{
	command_ans_common1();
}

/******************************************************************************/
void command_ans_M_BUF_W()
{
	
}

/******************************************************************************/
void command_ans_M_CTL_R(x_uint32_t*preg)
{
    //сбросить в нем поля ошибок и номера бита
    g_bootloader.cmd.nCmdCodeL &= (0x10); 
    
    num_of_par = 3;
	COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint16_t*)&(g_bootloader.cmd.nCmdCodeH),1);
	COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint16_t*)&(g_bootloader.cmd.nCmdCodeL),1);
	COMMAND_UTILITY_ANSWER_FIELD(2,(x_uint16_t*)preg,2);
	trm_ena = 1;
}

/******************************************************************************/
void command_ans_M_CTL_M(x_uint32_t*preg)
{
    //сбросить в нем поля ошибок и номера бита
    g_bootloader.cmd.nCmdCodeL &= (0x10); 
    
    num_of_par = 3;
	COMMAND_UTILITY_ANSWER_FIELD(0,(x_uint16_t*)&(g_bootloader.cmd.nCmdCodeH),1);
	COMMAND_UTILITY_ANSWER_FIELD(1,(x_uint16_t*)&(g_bootloader.cmd.nCmdCodeL),1);
	COMMAND_UTILITY_ANSWER_FIELD(2,(x_uint16_t*)(preg),2);
	trm_ena = 1;
}

/******************************************************************************/
void command_ans_M_FME_E()
{
	command_ans_common1();
}

/******************************************************************************/