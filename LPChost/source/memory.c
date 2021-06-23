#include "hardware/memory.h"
#include "lpc17xx.h"
#include "hardware/IAP.h"
#include "hardware/uart.h"

/******************************************************************************/
void memory_read(x_uint32_t a_addr, x_uint32_t* a_pdata, x_uint32_t a_u32_cnt)
{
    int i = 0;
    int *p = (int *)MEMORY_COEF_MEM_START;
    
    for(i=0;i<a_u32_cnt;i++){
        a_pdata[i] = p[i];
    }
    return;
}
/******************************************************************************/
void memory_write(x_uint32_t a_sec_begin,x_uint32_t a_sec_end, void* a_pdata, x_uint16_t a_u32_cnt)
{
    uint32_t error = 0;
    char dbg[64];
    int ipage = 0;
    x_uint32_t a_addr = 0;
    
    error = u32IAP_PrepareSectors(a_sec_begin,a_sec_end);
    if(error!= IAP_STA_CMD_SUCCESS) {
        DBG1(dbg,64,"PrepareSectors error %u",error);
        goto fail;
    }
    
    error = u32IAP_EraseSectors(a_sec_begin,a_sec_end);
    if(error!= IAP_STA_CMD_SUCCESS) {
        DBG1(dbg,64,"EraseSectors error %u",error);
        goto fail;
    }
    
    error = u32IAP_PrepareSectors(a_sec_begin,a_sec_end);
    if(error!= IAP_STA_CMD_SUCCESS) {
        DBG1(dbg,64,"PrepareSectors error %u",error);
        goto fail;
    }
    
    //get address by page
    switch(a_sec_begin){
        case 0:
            a_addr = FLASH_SECTOR_0;
            break;
        
        case 1:
            a_addr = FLASH_SECTOR_1;
            break;
        
        case 2:
            a_addr = FLASH_SECTOR_2;
            break;
        
        case 3:
            a_addr = FLASH_SECTOR_3;
            break;
        
        case 4:
            a_addr = FLASH_SECTOR_4;
            break;
        
        case 5:
            a_addr = FLASH_SECTOR_5;
            break;
        
        case 6:
            a_addr = FLASH_SECTOR_6;
            break;
        
        case 7:
            a_addr = FLASH_SECTOR_7;
            break;
        
        case 8:
            a_addr = FLASH_SECTOR_8;
            break;
        
        case 9:
            a_addr = FLASH_SECTOR_9;
            break;
        
        case 10:
            a_addr = FLASH_SECTOR_10;
            break;
        
        case 11:
            a_addr = FLASH_SECTOR_11;
            break;
        
        case 12:
            a_addr = FLASH_SECTOR_12;
            break;
        
        case 13:
            a_addr = FLASH_SECTOR_13;
            break;
        
        case 14:
            a_addr = FLASH_SECTOR_14;
            break;
        
        case 15:
            a_addr = FLASH_SECTOR_15;
            break;
        
        case 16:
            a_addr = FLASH_SECTOR_16;
            break;
        
        case 17:
            a_addr = FLASH_SECTOR_17;
            break;
        
        case 18:
            a_addr = FLASH_SECTOR_18;
            break;
        
        case 19:
            a_addr = FLASH_SECTOR_19;
            break;
        
        case 20:
            a_addr = FLASH_SECTOR_20;
            break;
        
        case 21:
            a_addr = FLASH_SECTOR_21;
            break;
        
        case 22:
            a_addr = FLASH_SECTOR_22;
            break;
        
        case 23:
            a_addr = FLASH_SECTOR_23;
            break;
        
        case 24:
            a_addr = FLASH_SECTOR_24;
            break;
        
        case 25:
            a_addr = FLASH_SECTOR_25;
            break;
        
        case 26:
            a_addr = FLASH_SECTOR_26;
            break;
        
        case 27:
            a_addr = FLASH_SECTOR_27;
            break;
        
        case 28:
            a_addr = FLASH_SECTOR_28;
            break;
        
        case 29:
            a_addr = FLASH_SECTOR_29;
            break;
        
        default:
            goto fail;
    }
    
    //check how much page we need
    ipage = (a_u32_cnt*4) / IAP_FLASH_PAGE_SIZE_BYTES;
    if(((a_u32_cnt*4) % IAP_FLASH_PAGE_SIZE_BYTES)>0 ) ipage++;
    while(ipage%2 != 0) ipage++;
    
    error = u32IAP_CopyRAMToFlash(a_addr,  (uint32_t)(a_pdata) , IAP_FLASH_PAGE_SIZE_BYTES*(ipage));
    if(error!= IAP_STA_CMD_SUCCESS)  {
        DBG1(dbg,64,"CopyRAMToFlash error %u",error);
        goto fail;
    }
    
    return;
fail:
    return;
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