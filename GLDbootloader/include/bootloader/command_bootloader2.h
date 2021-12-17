#ifndef __COMMAND_BOOTLOADER2_H_INCLUDE__
#define __COMMAND_BOOTLOADER2_H_INCLUDE__

#include "core/command.h"
#include "bootloader/global.h"
#include "xlib/types.h"
#include "xlib/ring_buffer.h"
/**
войти в загрузчик 
    CC01[LCC 16b] 
    DD01[status 16b][LCC 16b]
    
выйти из загрузчика 
    CC02[LCC 16b] 
    DD02[status 16b][LCC 16b]
    
параметры сектора ГЛД 
    CC03[sector 8b][LCC 16b] 
    CВВ03[sector 8b][status 16b][shift 16b][size 16b][LCC 16b] 
    
чтение flash (shift,size) 
    CC04[shift 16b][size 16b][LCC 16b]
    DD04[sector 8b][status 16b][data size bytes][LCC 16b]
    
очистка flash (shift,size) 
    CC05[shift 16b][size 16b][LCC 16b]
    DD05[sector 8b][status 16b][LCC 16b]
    
запись flash (shift,size) 
    CC06[shift 16b][size 16b][data size bytes][LCC 16b]
    DD06[sector 8b][status 16b][LCC 16b]
*/

#define  COMMAND_BOOTLOADER_CMD_SIGN      0xCC
#define  COMMAND_BOOTLOADER_ANS_SIGN      0xDD

#define  COMMAND_BOOTLOADER_ENTER_LOADER 0x01
#define  COMMAND_BOOTLOADER_EXIT_LOADER 0x02
#define  COMMAND_BOOTLOADER_GET_SECTOR_GLD 0x03
#define  COMMAND_BOOTLOADER_READ_FLASH 0x04
#define  COMMAND_BOOTLOADER_ERISE_FLASH 0x05
#define  COMMAND_BOOTLOADER_WRITE_FLASH 0x06

#define COMMAND_BOOTLOADER_SIGN_LEN (1)
#define COMMAND_BOOTLOADER_SIGN_LEN (4)
#define COMMAND_BOOTLOADER_SIGN_LEN (4)
#define COMMAND_BOOTLOADER_SIGN_LEN (4)
//2byte LCC + 1byte sign + 1 byte CMD code
#define MINIMUM_PACKET_LEN (4)
/*
    Get data from ringbuffer and parse it
*/
x_bool_t bootloader_read_cmd(x_ring_buffer_t *a_pbuf);

#endif //__COMMAND_BOOTLOADER2_H_INCLUDE__