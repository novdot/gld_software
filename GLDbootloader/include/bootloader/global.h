#ifndef __BOOTLOADER_GLOBAL_H_INCLUDED__
#define __BOOTLOADER_GLOBAL_H_INCLUDED__

#include "xlib/types.h"

#define BOOTLOADER_PTR_CODE_JUMP 0 //< код указателя перехода (указатель передачи управления)
#define BOOTLOADER_PTR_CODE_BUF 1 //< код указателя буфера (указатель RAM-буфера пересылок в операциях с ним)
#define BOOTLOADER_PTR_CODE_EXE 2 //< код указателя памяти программ
#define BOOTLOADER_PTR_CODE_DATA 3 //< код указателя памяти данных       
#define BOOTLOADER_PTR_CODE_IO 4 //< код указателя ввода/вывода       
#define BOOTLOADER_PTR_CODE_FLASH 5 //< код указателя байтовой памяти (флэш)        
#define BOOTLOADER_PTR_CODE_DEV1 6 //< код указателя периферийного устройства 1
#define BOOTLOADER_PTR_CODE_DEV2 7 //< код указателя периферийного устройства 2 

#define GET_OFFSET_FROM_PTR(ptr)    ((ptr&0x3)<<14)+(((ptr&0x00FF00)>>8)<<0)+(((ptr&0x3F0000)>>16)<<8)
#define GET_SECTOR_FROM_PTR(ptr)    ((ptr&0xFF)>>2)

#define BOOTLOADER_BUF_SIZE (64) 

typedef union bootloader_paramsFieldDef{
    struct{
        unsigned code: 4;
        unsigned all_ptr: 1;
        unsigned reserved: 3;
    }bit;
    x_uint8_t word;
}bootloader_paramsField;

typedef struct bootloader_ptrDef{
    x_uint32_t nPtrJump;
    x_uint32_t nPtrBuf;
    x_uint32_t nPtrExe;
    x_uint32_t nPtrData;
    x_uint32_t nPtrIo;
    x_uint32_t nPtrFlash;
    x_uint32_t nPtrDev1;
    x_uint32_t nPtrDev2;
}bootloader_ptr;

typedef struct bootloader_cmdDef{
    x_uint32_t send_data_ptr[3]; //< указатель на отправку
    x_uint32_t nCmdCodeH; //< код комманды
    x_uint32_t nCmdCodeL; //< 2ая часть кода команды
}bootloader_cmd;

/// стуктура предыдущей команды
typedef struct bootloader_prevCmdBufDef{
    x_uint8_t buf[134]; //< 
    x_uint8_t size; //< 
}bootloader_prevCmdBuf;

typedef struct bootloader_globalDef{
    bootloader_ptr ptr;
    bootloader_cmd cmd;
    x_uint8_t buf128[BOOTLOADER_BUF_SIZE];
    x_bool_t bMonitorMode;
    x_uint32_t nTimerCnt; //<счетчик времени автоматической загрузки основной программы
    bootloader_prevCmdBuf prevCmd; //< предыдущая команда буффер
    x_uint8_t buf_sector[1024]; //< буфер сектора
}bootloader_global;

extern bootloader_global g_bootloader;

/*!
    \brief initialize global value
*/
void global_bootloader_init();

#endif // __BOOTLOADER_GLOBAL_H_INCLUDED__