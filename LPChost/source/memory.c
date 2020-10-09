#include "hardware/memory.h"
#include "lpc17xx.h"
#include "hardware/IAP.h"

/******************************************************************************/
void memory_read(x_uint32_t a_addr, void* a_pdata)
{
    a_pdata = (void*)(a_addr);
    return;
}
/******************************************************************************/
void memory_write(x_uint32_t a_sec_begin,x_uint32_t a_sec_end,x_uint32_t a_addr, void* a_pdata, x_uint16_t a_size)
{
    u32IAP_PrepareSectors(a_sec_begin,a_sec_end);
    u32IAP_EraseSectors(a_sec_begin,a_sec_end);
    u32IAP_PrepareSectors(a_sec_begin,a_sec_end);
    u32IAP_CopyRAMToFlash(a_addr,  (uint32_t)(&a_pdata) , a_size);
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