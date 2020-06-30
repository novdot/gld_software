#ifndef __COMMAND_BOOTLOADER_H_INCLUDE__
#define __COMMAND_BOOTLOADER_H_INCLUDE__

#include "core/command.h"
#include "bootloader/global.h"

/******************************************************************************/
//e.errors of command execution of the Host-computer
//r.ошибки выпо~нения команд Host-компьютера
#define  C_Line_Err     0x0020           //r. ошибка кана~а связи      
#define  C_PBfL_Err      0x0040          //r. указате~ь буфера меньше нача~ьного адреса буфера
#define  C_PBfH_Err      0x0060          //r. указате~ь буфера бо~ьше пос~еднего адреса буфера
#define  C_BDMA_Err      0x0080          //r. ошибка обмена с внешней памятью
#define  C_Flsh_Err      0x00a0          //r. ошибка операции ф~эш-памяти
#define  C_TimO_Err      0x00c0          //r. максима~ьное чис~о попыток обращения достигнуто
#define  C_NoCm_Err      0x00e0          //r. данная команда не поддерживается

//e.errors of the communication channel with Host-computer
//r.ошибки кана~а связи с Host-компьютером
#define  Strt_Err        0x0080          //r. ошибка старт-бита
#define  Stop_Err        0x0040          //r. ошибка стоп-бита
#define  Size_Err        0x0004          //r. размер принятого пакета не соответствует сог~ашению
#define  LCC_Err         0x0002          //r. не совпа~а контро~ьная сумма
#define  NoCMD_Err       0x0100          //r. в принятом пакете нет признака команды
#define  Code_Err        0x0200          //r. неизвестный код команды
#define  Mode_Err        0x0400          //r. код команды не соответствует текущему режиму
#define  Prmt_Err        0x0800          //r. неверно заданные параметры команды
#define  MxSz_Err        0x1000          //r. перепо~нение буфера приемника
#define  Addr_Err        0x2000          //r. неверный адрес устройства
#define  Read_Err        0x4000          //r. останов связи несвоевременным чтением данных
#define  WAsq_Err        0x8000          //r. ошибка ожидания достоверного ответа

/******************************************************************************/
#define  CMD_WRK_PC      0x0f0f //e. the code of the Wrk_PC comman 
#define  CMD_MAINT       0x9999 //e. the code of the Maintenance mode command 

#define  CMD_M_SUBCMD1 	 0xdf00 
#define  CMD_M_JUMP      0xdfa5 //e. the code of the command, that starts the code in PM 
#define  CMD_M_LOAD      0xdf5a //e. the code of the command, that loads microprogramme from the BM and starts it 
#define  CMD_M_CONF      0xdf12 //e. the code of the command, that configurates PLD of uHost card
#define  CMD_M_DCNF      0xdf21 //e. the code of the command, that resets PLD flex configuration
   
#define  CMD_M_SUBCMD2 	 0xda00  
#define  CMD_M_CLEAR     0xda00 //e. the code of the command, that resets the register of errors of line 485
#define  CMD_M_MIRR      0xda01 //e. the code of the command, that returns previous contents of the receiver buffer
#define  CMD_M_TSIV1     0xda11 //e. the code of the command, that tests data memory DMOVLAY = 1
#define  CMD_M_TSOV2     0xda12 //e. the code of the command, that tests data memory DMOVLAY = 2
                             
#define  CMD_M_PTR_R     0xd300 //e. the code of the command, that reads a set of pointers to arrays
#define  CMD_M_PTR_W     0xd200 //e. the code of the command, that writes a set of pointers to arrays
#define  CMD_M_DAT_R     0xd500 //e. the code of the command, that reads block of data
#define  CMD_M_DAT_W     0xd400 //e. the code of the command, that writes block of data
#define  CMD_M_BUF_R     0xd700 //e. the code of the command, that reads data from memory bank to buffer
#define  CMD_M_BUF_W     0xd600 //e. the code of the command, that writes data from buffer to memory bank                                                                       
#define  CMD_M_CTL_R     0xd900 //e. the code of the command, that reads the control register of the device
#define  CMD_M_CTL_M     0xd800 //e. the code of the command, that modificates of bit of the control register
#define  CMD_M_FME_E     0xde00 //e. the code of the command, that erases flash-memory sector

/******************************************************************************/
/**
    handlers
*/
void command_cmd_WRK_PC();
void command_cmd_MAINT();
void command_cmd_M_JUMP();
void command_cmd_M_LOAD();
void command_cmd_M_CONF();
void command_cmd_M_DCNF();
void command_cmd_M_CLEAR();
void command_cmd_M_MIRR();
void command_cmd_M_TSIV1();
void command_cmd_M_TSOV2();
void command_cmd_M_PTR_R();
void command_cmd_M_PTR_W();
void command_cmd_M_DAT_R();
void command_cmd_M_DAT_W();
void command_cmd_M_BUF_R();
void command_cmd_M_BUF_W();
void command_cmd_M_CTL_R();
void command_cmd_M_CTL_M();
void command_cmd_M_FME_E();

/**
    answer
*/
void command_ans_common(void);

void command_ans_WRK_PC();
void command_ans_MAINT();
void command_ans_M_JUMP();
void command_ans_M_LOAD();
void command_ans_M_CONF();
void command_ans_M_DCNF();
void command_ans_M_CLEAR();
void command_ans_M_MIRR();
void command_ans_M_TSIV1();
void command_ans_M_TSOV2();
void command_ans_M_PTR_R(x_uint32_t data);
void command_ans_M_PTR_W();
void command_ans_M_DAT_R();
void command_ans_M_DAT_W();
void command_ans_M_BUF_R();
void command_ans_M_BUF_W();
void command_ans_M_CTL_R(x_uint16_t*preg);
void command_ans_M_CTL_M(x_uint16_t*preg);
void command_ans_M_FME_E();

#endif//__COMMAND_BOOTLOADER_H_INCLUDE__