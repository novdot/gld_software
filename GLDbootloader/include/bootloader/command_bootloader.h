#ifndef __COMMAND_BOOTLOADER_H_INCLUDE__
#define __COMMAND_BOOTLOADER_H_INCLUDE__

#include "core/command.h"

#define  CMD_WRK_PC      0x0f0f //e. the code of the Wrk_PC comman 
#define  CMD_MAINT       0x9999 //e. the code of the Maintenance mode command 
#define  CMD_M_Jump      0xdfa5 //e. the code of the command, that starts the code in PM 
#define  CMD_M_Load      0xdf5a //e. the code of the command, that loads microprogramme from the BM and starts it 
#define  CMD_M_Conf      0xdf12 //e. the code of the command, that configurates PLD of uHost card
#define  CMD_M_Dcnf      0xdf21 //e. the code of the command, that resets PLD flex configuration
                             
#define  CMD_M_Clear     0xda00 //e. the code of the command, that resets the register of errors of line 485
#define  CMD_M_Mirr      0xda01 //e. the code of the command, that returns previous contents of the receiver buffer
#define  CMD_M_TsOv1     0xda11 //e. the code of the command, that tests data memory DMOVLAY = 1
#define  CMD_M_TsOv2     0xda12 //e. the code of the command, that tests data memory DMOVLAY = 2
                             
#define  CMD_M_Ptr_R     0xd300 //e. the code of the command, that reads a set of pointers to arrays
#define  CMD_M_Ptr_W     0xd200 //e. the code of the command, that writes a set of pointers to arrays
#define  CMD_M_Dat_R     0xd500 //e. the code of the command, that reads block of data
#define  CMD_M_Dat_W     0xd400 //e. the code of the command, that writes block of data
#define  CMD_M_Buf_R     0xd700 //e. the code of the command, that reads data from memory bank to buffer
#define  CMD_M_Buf_W     0xd600 //e. the code of the command, that writes data from buffer to memory bank                                                                       
#define  CMD_M_Ctl_R     0xd900 //e. the code of the command, that reads the control register of the device
#define  CMD_M_Ctl_M     0xd800 //e. the code of the command, that modificates of bit of the control register
#define  CMD_M_FMe_E     0xde00 //e. the code of the command, that erases flash-memory sector



#endif