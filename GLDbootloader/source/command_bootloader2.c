#include "bootloader/command_bootloader2.h"
#include "bootloader/global.h"
#include "hardware/hardware.h"
#include "core/global.h"
#include "core/command.h"

/******************************************************************************
x_bool_t bootloader_read_cmd(x_ring_buffer_t *a_pbuf)
{
    int iStart = 0;
    int i = 0;
    int rcv_num_byt = 0;
    x_bool_t isSign = _x_false;
    
    //check len
    rcv_num_byt = x_ring_get_count(a_pbuf);
    if(rcv_num_byt<MINIMUM_PACKET_LEN) {
        goto fail;
    }
    
    //check first byte cmd
    for(i=0;i<(rcv_num_byt);i++){
        if(x_ring_get_symbol(i,a_pbuf)!=COMMAND_BOOTLOADER_CMD_SIGN){ 
            continue;
        }else{
            isSign = _x_true;
            break;
        }
    }
    iStart = i;
    
    //clear trash from ring
    if(isSign==_x_true){
        for(i=0;i<(iStart);i++){
            x_ring_pop(&g_gld.cmd.ask.ring_in);
        }
    }else{
        x_ring_clear(a_pbuf);
    }
    
    //check code / compare with len
    rcv_num_byt = x_ring_get_count(a_pbuf);
    switch(x_ring_get_symbol(2,a_pbuf)){
    case COMMAND_BOOTLOADER_ENTER_LOADER :
        if(rcv_num_byt<(MINIMUM_PACKET_LEN)) goto fail;
        break;
    
    case COMMAND_BOOTLOADER_EXIT_LOADER :
        if(rcv_num_byt<(MINIMUM_PACKET_LEN)) goto fail;
        break;
    
    case COMMAND_BOOTLOADER_READ_SECTOR_GLD :
        if(rcv_num_byt<(MINIMUM_PACKET_LEN+1)) goto fail;
        break;
    
    case COMMAND_BOOTLOADER_ERISE_SECTOR_GLD :
        if(rcv_num_byt<(MINIMUM_PACKET_LEN+1)) goto fail;
        break;
    
    case COMMAND_BOOTLOADER_WRITE_SECTOR_GLD :
        if(rcv_num_byt<(MINIMUM_PACKET_LEN+3)) goto fail;
        break;
    
    case COMMAND_BOOTLOADER_READ_FLASH :
        if(rcv_num_byt<(MINIMUM_PACKET_LEN+4)) goto fail;
        break;
    
    case COMMAND_BOOTLOADER_ERISE_FLASH :
        if(rcv_num_byt<(MINIMUM_PACKET_LEN+4)) goto fail;
        break;
    
    case COMMAND_BOOTLOADER_WRITE_FLASH :
        if(rcv_num_byt<(MINIMUM_PACKET_LEN+4)) goto fail;
        break;
    
    default:
        if(rcv_num_byt<MINIMUM_PACKET_LEN) goto fail;
        break;
    }
    
    //check lcc
    
    //if have one cmd read from ring
    
    
    return _x_true;
fail:
    return _x_false;
}

/******************************************************************************/