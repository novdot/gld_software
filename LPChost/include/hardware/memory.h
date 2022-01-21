/**
  ******************************************************************************
  * @file    memory.h
  *
  * @brief  work with memory
  *
  * @author  Дмитрий Новиков
  * @email  novikov@elektrooptika.ru
  *
  ******************************************************************************
  * @attention
  *
  * в разработке
  *
  * <h2><center>&copy; 2020 Электрооптика</center></h2>
  ******************************************************************************
  */
#ifndef __HARDWARE_MEMORY_H_INCLUDED 
#define __HARDWARE_MEMORY_H_INCLUDED

#include "xlib/types.h"

/**    Table for start adress of sectors
 *
 *        LPC1768 internal flash memory sector numbers and addresses
 *
 *        LPC1768 flash memory are and sector number/size
 *        Table 568 "Sectors in a LPC17xx device", Section 5. "Sector numbers", usermanual
 *
 *        0x00000000 - 0x0007FFFF        flash (29 sectors)
 *
 *      Sector0:     0x00000000 - 0x00000FFF        4K
 *      Sector1:     0x00001000 - 0x00001FFF        4K
 *      Sector2:     0x00002000 - 0x00002FFF        4K
 *      Sector3:     0x00003000 - 0x00003FFF        4K
 *      Sector4:     0x00004000 - 0x00004FFF        4K
 *      Sector5:     0x00005000 - 0x00005FFF        4K
 *      Sector6:     0x00006000 - 0x00006FFF        4K
 *      Sector7:     0x00007000 - 0x00007FFF        4K
 *      Sector8:     0x00008000 - 0x00008FFF        4K
 *      Sector9:     0x00009000 - 0x00009FFF        4K
 *      Sector10:    0x0000A000 - 0x0000AFFF        4K
 *      Sector11:    0x0000B000 - 0x0000BFFF        4K
 *      Sector12:    0x0000C000 - 0x0000CFFF        4K
 *      Sector13:    0x0000D000 - 0x0000DFFF        4K
 *      Sector14:    0x0000E000 - 0x0000EFFF        4K
 *      Sector15:    0x0000F000 - 0x0000FFFF        4K
 *
 *      Sector16:    0x00010000 - 0x00017FFF        32K
 *      Sector17:    0x00018000 - 0x0001FFFF        32K
 *      Sector18:    0x00020000 - 0x00027FFF        32K
 *      Sector19:    0x00028000 - 0x0002FFFF        32K
 *      Sector20:    0x00030000 - 0x00037FFF        32K
 *      Sector21:    0x00038000 - 0x0003FFFF        32K
 *      Sector22:    0x00040000 - 0x00047FFF        32K
 *      Sector23:    0x00048000 - 0x0004FFFF        32K
 *      Sector24:    0x00050000 - 0x00057FFF        32K
 *      Sector25:    0x00058000 - 0x0005FFFF        32K
 *      Sector26:    0x00060000 - 0x00067FFF        32K
 *      Sector27:    0x00068000 - 0x0006FFFF        32K
 *      Sector28:    0x00070000 - 0x00077FFF        32K
 *      Sector29:    0x00078000 - 0x0007FFFF        32K
 */
#define     FLASH_SECTOR_0       0x00000000
#define     FLASH_SECTOR_1       0x00001000
#define     FLASH_SECTOR_2       0x00002000
#define     FLASH_SECTOR_3       0x00003000
#define     FLASH_SECTOR_4       0x00004000
#define     FLASH_SECTOR_5       0x00005000
#define     FLASH_SECTOR_6       0x00006000
#define     FLASH_SECTOR_7       0x00007000
#define     FLASH_SECTOR_8       0x00008000
#define     FLASH_SECTOR_9       0x00009000
#define     FLASH_SECTOR_10      0x0000A000
#define     FLASH_SECTOR_11      0x0000B000
#define     FLASH_SECTOR_12      0x0000C000
#define     FLASH_SECTOR_13      0x0000D000
#define     FLASH_SECTOR_14      0x0000E000
#define     FLASH_SECTOR_15      0x0000F000

#define     FLASH_SECTOR_16      0x00010000
#define     FLASH_SECTOR_17      0x00018000
#define     FLASH_SECTOR_18      0x00020000
#define     FLASH_SECTOR_19      0x00028000
#define     FLASH_SECTOR_20      0x00030000
#define     FLASH_SECTOR_21      0x00038000
#define     FLASH_SECTOR_22      0x00040000
#define     FLASH_SECTOR_23      0x00048000
#define     FLASH_SECTOR_24      0x00050000
#define     FLASH_SECTOR_25      0x00058000
#define     FLASH_SECTOR_26      0x00060000
#define     FLASH_SECTOR_27      0x00068000
#define     FLASH_SECTOR_28      0x00070000
#define     FLASH_SECTOR_29      0x00078000

#define MEMORY_VOID_SEC_NUM (0xFF)

#define MEMORY_BOOT_SEC_NUM (0)
#define MEMORY_BOOT_SEC_START (0)
#define MEMORY_BOOT_SEC_END (16)
#define MEMORY_BOOT_MEM_START (FLASH_SECTOR_0)
#define MEMORY_BOOT_MEM_SIZE (FLASH_SECTOR_16-FLASH_SECTOR_0)
                    
#define MEMORY_MAIN_SEC_NUM (1)
#define MEMORY_MAIN_SEC_START (16)
#define MEMORY_MAIN_SEC_END (22)
#define MEMORY_MAIN_MEM_START (FLASH_SECTOR_16)
#define MEMORY_MAIN_MEM_SIZE (FLASH_SECTOR_22-FLASH_SECTOR_16)
                    
#define MEMORY_FPGA_SEC_NUM (2)
#define MEMORY_FPGA_SEC_START (0)
#define MEMORY_FPGA_END_START (0)
#define MEMORY_FPGA_MEM_START (0)
#define MEMORY_FPGA_MEM_SIZE (0)
                    
#define MEMORY_COEF_SEC_NUM (4)
#define MEMORY_COEF_SEC_START (26)
#define MEMORY_COEF_SEC_END (29)
#define MEMORY_COEF_MEM_START (FLASH_SECTOR_26)
#define MEMORY_COEF_MEM_SIZE (FLASH_SECTOR_29-FLASH_SECTOR_26)

/**
    @brief чтение из памяти ПЗУ по адресу (копирпование из ПЗУ в ОЗУ)
    @param a_addr адрес в памяти ПЗУ
    @param a_pdata указатель на начало данных куда необходимо скопировать данные из ПЗУ
    @param a_u32_cnt кол-во 32х разрядных слов
*/
void memory_read(x_uint32_t a_addr, x_int32_t* a_pdata, x_uint32_t a_u32_cnt);

/**
    @brief запись в память ПЗУ из ОЗУ
    @param a_sec_begin начальный сектор в памяти ПЗУ
    @param a_sec_end конечный сектор в памяти ПЗУ
    @param a_pdata указатель на начало данных откуда необходимо скопировать данные в ПЗУ
    @param a_u32_cnt кол-во 32х разрядных слов
*/
void memory_write(
    x_uint32_t a_sec_begin
    , x_uint32_t a_sec_end
    , void* a_pdata
    , x_uint16_t a_u32_cnt);

/**
    @brief очистка памяти ПЗУ
    @param a_sec_begin начальный сектор в памяти ПЗУ
    @param a_sec_end конечный сектор в памяти ПЗУ
*/
void memory_erase(x_uint32_t a_sec_begin, x_uint16_t a_sec_end);

/**
    @brief загружаем из памяти ПЗУ программу
    @param a_addr адрес в памяти ПЗУ
*/
void memory_load(x_uint32_t a_addr);

#endif //__HARDWARE_MEM_H_INCLUDED