#include "hardware/memory.h"
#include "lpc17xx.h"
#include "hardware/IAP.h"
#include "hardware/uart.h"

#include "core/global.h"

#include <stdio.h>
#include <string.h>

/******************************************************************************/
void memory_read(x_uint32_t a_addr, x_int32_t* a_pdata, x_uint32_t a_u32_cnt)
{
    int i = 0;
    x_int32_t *p = (x_int32_t *)MEMORY_COEF_MEM_START;
    
    for(i=0;i<a_u32_cnt;i++){
        a_pdata[i] = (x_int32_t)p[i];
    }
    return;
}
/******************************************************************************/
x_bool_t memory_write(
    x_uint32_t a_sec_begin
    ,x_uint32_t a_sec_end
    ,x_uint32_t a_addr_shift
    , void* a_pdata
    , x_uint32_t a_u32_cnt)
{
    uint32_t error = 0;
    char dbg[64];
    int i = 0;
    x_uint32_t ipage = 0;
    x_uint32_t sec_size = 0;
    x_uint32_t sec_shift = 0;
    x_uint32_t addr = 0;
    x_uint32_t addr_shift = 0;
    
    //PAGE SIZE AFTER 16 sector = 32768 bytes!
    if(a_sec_begin>=16) 
        sec_size = MEMORY_SECTOR_16_29_SIZE;
    else 
        sec_size = MEMORY_SECTOR_0_15_SIZE;
    
    //calculate shift sector
    sec_shift = a_addr_shift/sec_size;
    a_sec_begin += sec_shift;
    
    //calculate shift addr
    addr_shift = a_addr_shift%sec_size;
    
    /*error = u32IAP_PrepareSectors(a_sec_begin,a_sec_end);
    if(error!= IAP_STA_CMD_SUCCESS) {
        DBG1(&g_gld.cmd.dbg.ring_out,dbg,64,"PrepareSectors error %u\n\r",error);
        goto fail;
    }
    
    error = u32IAP_EraseSectors(a_sec_begin,a_sec_end);
    if(error!= IAP_STA_CMD_SUCCESS) {
        DBG1(&g_gld.cmd.dbg.ring_out,dbg,64,"EraseSectors error %u\n\r",error);
        goto fail;
    }*/
    
    error = u32IAP_PrepareSectors(a_sec_begin,a_sec_end);
    if(error!= IAP_STA_CMD_SUCCESS) {
        DBG1(&g_gld.cmd.dbg.ring_out,dbg,64,"PrepareSectors error %u\n\r",error);
        goto fail;
    }
    
    //get address by page
    switch(a_sec_begin){
        case 0:
            addr = FLASH_SECTOR_0;
            break;
        
        case 1:
            addr = FLASH_SECTOR_1;
            break;
        
        case 2:
            addr = FLASH_SECTOR_2;
            break;
        
        case 3:
            addr = FLASH_SECTOR_3;
            break;
        
        case 4:
            addr = FLASH_SECTOR_4;
            break;
        
        case 5:
            addr = FLASH_SECTOR_5;
            break;
        
        case 6:
            addr = FLASH_SECTOR_6;
            break;
        
        case 7:
            addr = FLASH_SECTOR_7;
            break;
        
        case 8:
            addr = FLASH_SECTOR_8;
            break;
        
        case 9:
            addr = FLASH_SECTOR_9;
            break;
        
        case 10:
            addr = FLASH_SECTOR_10;
            break;
        
        case 11:
            addr = FLASH_SECTOR_11;
            break;
        
        case 12:
            addr = FLASH_SECTOR_12;
            break;
        
        case 13:
            addr = FLASH_SECTOR_13;
            break;
        
        case 14:
            addr = FLASH_SECTOR_14;
            break;
        
        case 15:
            addr = FLASH_SECTOR_15;
            break;
        
        case 16:
            addr = FLASH_SECTOR_16;
            break;
        
        case 17:
            addr = FLASH_SECTOR_17;
            break;
        
        case 18:
            addr = FLASH_SECTOR_18;
            break;
        
        case 19:
            addr = FLASH_SECTOR_19;
            break;
        
        case 20:
            addr = FLASH_SECTOR_20;
            break;
        
        case 21:
            addr = FLASH_SECTOR_21;
            break;
        
        case 22:
            addr = FLASH_SECTOR_22;
            break;
        
        case 23:
            addr = FLASH_SECTOR_23;
            break;
        
        case 24:
            addr = FLASH_SECTOR_24;
            break;
        
        case 25:
            addr = FLASH_SECTOR_25;
            break;
        
        case 26:
            addr = FLASH_SECTOR_26;
            break;
        
        case 27:
            addr = FLASH_SECTOR_27;
            break;
        
        case 28:
            addr = FLASH_SECTOR_28;
            break;
        
        case 29:
            addr = FLASH_SECTOR_29;
            break;
        
        default:
            goto fail;
    }
    
    //check how much page we need
    //ipage = (a_u32_cnt*4) / IAP_FLASH_PAGE_SIZE_BYTES;
    //if(((a_u32_cnt*4) % IAP_FLASH_PAGE_SIZE_BYTES)>0 ) ipage++;
    //while(ipage%2 != 0) ipage++;
    
    ipage = MEMORY_PAGE_SIZE ;
    
    error = u32IAP_CopyRAMToFlash(addr + addr_shift,  (uint32_t)(a_pdata) , ipage);
    if(error!= IAP_STA_CMD_SUCCESS)  {
        DBG1(&g_gld.cmd.dbg.ring_out,dbg,64
        ,"CopyRAMToFlash error %u"
            ,error
        );
        goto fail;
    }
    
    return _x_true;
fail:
    return _x_false;
}
/******************************************************************************/
void memory_erase(x_uint32_t a_sec_begin, x_uint16_t a_sec_end)
{
    u32IAP_PrepareSectors(a_sec_begin, a_sec_end);
    u32IAP_EraseSectors(a_sec_begin, a_sec_end);
    return;
}
/******************************************************************************/
__asm void boot_jump( uint32_t address )
{
    LDR SP, [R0]            ;Load new stack pointer address
    LDR PC, [R0, #4]        ;Load new program counter address
}
void memory_load(x_uint32_t a_addr)
{
    /* Change the Vector Table to the [address]
    in case the user application uses interrupts */
    SCB->VTOR = a_addr & 0x1FFFFF80;

    boot_jump(a_addr);
    return;
}