/**
  ******************************************************************************
  * @file    command.с
  *
  * @brief   Чтение,расшифровка,выдача команд
  *
  * @author  Дмитрий Новиков novikov@elektrooptika.ru
  *
  ******************************************************************************
  * @attention
  *
  * в разработке
  *
  * <h2><center>&copy; 2020 Электрооптика</center></h2>
  ******************************************************************************
  */
#include "core/command.h"
#include "core/global.h"
#include "bootloader/global.h"
#include "hardware/hardware.h"
#include "hardware/uart.h"
#include "xlib/ring_buffer.h"

//TODO
#include "CyclesSync.h"
#include "Parameters.h"

#include <stdio.h>

void command_recieve_gld();
void command_recieve_bootloader();
/******************************************************************************/
x_bool_t command_check_lcc(x_uint8_t* a_pBuffer,x_uint32_t a_uCount)
{
	int iCRC_calc = 0;
	int CRC_calc = 0;
	int CRC_real = 0;
    char dbg[64];
    char dbg_word[64];
    int i = 0;
    
    sprintf(dbg_word,"");

	for (iCRC_calc = 1; iCRC_calc < (a_uCount-2); iCRC_calc++)
		CRC_calc += a_pBuffer[iCRC_calc];  

	CRC_real = (a_pBuffer[a_uCount-2] << 8) | a_pBuffer[a_uCount-1];

	if( (CRC_real - CRC_calc) == 0) {
        return _x_true;
    } else {
        //for (iCRC_calc = 0; iCRC_calc < (a_uCount); iCRC_calc++)
        //    sprintf(dbg_word,"%s%x",dbg_word,a_pBuffer[iCRC_calc]);
        //DBG2(&g_gld.cmd.dbg.ring_out,dbg,64,"command_check_lcc FAILED! word:%s, cnt:%u\n\r",dbg_word,(unsigned int)a_uCount);
        return _x_false;
    }
}
/******************************************************************************/
void command_recieve_gld()
{
    /*****************/
    static int ToWaitEnd, ErrReg ;
    int iCRC_calc = 0;
    int ibyte = 0;
    int ibyte_head = 0;
    char dbg[64];
    char dbg_word[64];
    int i = 0;
    
    //e. end part of packet is absent
	if (( ToWaitEnd > 25000)) {
        sprintf(dbg_word,"");
        for (iCRC_calc = 0; iCRC_calc < (rcv_num_byt); iCRC_calc++)
            sprintf(dbg_word,"%s%x",dbg_word,rcv_buf[iCRC_calc]);
        DBG2(&g_gld.cmd.dbg.ring_out,dbg,64,"end part of packet is absent %s, cnt:%u\n\r",dbg_word,(unsigned int)rcv_num_byt);
        
        do rcv_buf[--rcv_num_byt] = 0;
        while(rcv_num_byt);
        
        rcv_num_byt_old = rcv_num_byt;
        uart_recieve_reset();
        ToWaitEnd = 0;
        return;
	}
	//e. we have not received any new bytes
	if (rcv_num_byt_old == rcv_num_byt) { 
        if (ToWaitEnd) ToWaitEnd++;
        return;
	}
    //try to move buffer for header sign
    /************/
    if (rcv_buf[0] != 0xCC) {
        //try to find header
        for(ibyte=0;ibyte<rcv_num_byt;ibyte++){
            if (rcv_buf[ibyte] == 0xCC){
                ibyte_head = ibyte;
                break;
            }
        }
        //cant find header - clear all buff
        if(ibyte_head==0){
            do rcv_buf[--rcv_num_byt] = 0;
            while(rcv_num_byt);
            //ToWaitEnd++;
            return;
        }
        //header finded - move buffer
        for(ibyte=0;ibyte<(rcv_num_byt-ibyte_head);ibyte++){
            rcv_buf[ibyte] = rcv_buf[ibyte_head+ibyte];
        }
        rcv_num_byt -= ibyte_head;
    }
    /************/
	rcv_num_byt_old = rcv_num_byt;
    
	//e. we received less than 6 bytes or no parity bytes
    if ((rcv_num_byt < 6) || ((rcv_num_byt & 0x0001) == 1)) {
        ToWaitEnd++;
        return;
    }
    
    //e. the header of packet has not recieved
    /*********/
    if ((!ToWaitEnd) && (rcv_num_byt > 1)){
        if (
                (rcv_buf[0] != 0xCC) 
                || (( rcv_buf[1] > 2) && ( rcv_buf[1] != 0x1F))
        ) {   
            ErrReg |= 5; //L1_Rc_err (HEADER_ERR); NoCMD_Err
            ToWaitEnd++;
            return;
        }
    }
    /********/
        
    //check packet lenght
    if (rcv_num_byt == 6) {	 
        if (
            (rcv_buf[2] == 0x0A) 
            || (rcv_buf[2] == 0xE0) 
            || (rcv_buf[2] == 0xE4) 
            || (rcv_buf[2] == 0xE6) 
            || (rcv_buf[2] == 0xE8)
        ) {
            //e. packet length is not valid, so we have the error 
            ToWaitEnd++; 	
            return;
        }
    } else  if (rcv_num_byt == 8) {
        if ((rcv_buf[2] == 0xE0) || (rcv_buf[2] == 0xE4)) {
            ToWaitEnd++;
            return;
        }
    }
    
    //e. checksum is bad 
	if (command_check_lcc(rcv_buf,rcv_num_byt) == _x_false){
        ToWaitEnd++;
        return;
	} else {
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"rcv_Rdy\n\r");
		rcv_Rdy = 1;
	}
	ToWaitEnd = 0;
    /**********************/
}
/******************************************************************************/
void command_recieve_reset()
{
    g_gld.cmd.recieve_cmd_size = 0;
}
void command_recieve_bootloader()
{

#if 0
    char dbg[256];
    int i =0;
    int rcv_num_byt = 0;
    static int nToWaitEnd = 0;
    x_uint8_t data = 0;
    x_uint8_t idata = 0;
    
    /*uart_recieve_n(0,_rcv_buf,&_rcv_num_byt);
    if (_rcv_num_byt == 0)
        return;
    
    for(idata=0;idata<_rcv_num_byt;idata++){
    //while(_rcv_num_byt>0){
        //_rcv_num_byt--;
        x_ring_put(_rcv_buf[idata],&g_gld.cmd.dbg.ring_in);
    }
    
    //if (x_ring_get_count(&g_gld.cmd.ring_in) < 2)
    //    return;
    */
    
    if (( nToWaitEnd > 2500000)) {
        nToWaitEnd = 0;
        command_recieve_reset();
        x_ring_clear(&g_gld.cmd.ask.ring_in);
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"end part of packet is absent\n\r");
        return;
    }
    
    //check count
    rcv_num_byt = x_ring_get_count(&g_gld.cmd.ask.ring_in);
    if(
        //(rcv_num_byt > 6)
        (rcv_num_byt == 6)
        ||(rcv_num_byt == 8)
        ||(rcv_num_byt == 9)
        ||(rcv_num_byt == 22)
        ||(rcv_num_byt == 30)
        ||(rcv_num_byt == 134)
    ){
        //save cmd
    } else {
        //ждем
        nToWaitEnd++;
        return;
    }
    if(rcv_num_byt > 134 ){
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"rx overflow\n\r");
        nToWaitEnd = 0;
        command_recieve_reset();
        x_ring_clear(&g_gld.cmd.ask.ring_in);
        return;
    }
    
    if(g_gld.cmd.recieve_cmd_size>=GLD_BUF_SIZE){
        g_gld.cmd.recieve_cmd_size = 0;
    }
    
    //pop from ring
    g_gld.cmd.recieve_cmd_size = rcv_num_byt;
    for(i=0;i<(g_gld.cmd.recieve_cmd_size);i++){
        g_gld.cmd.recieve_cmd[i] = x_ring_get_symbol(i,&g_gld.cmd.ask.ring_in);
    }
    
    //check LCC
    if (command_check_lcc(g_gld.cmd.recieve_cmd,g_gld.cmd.recieve_cmd_size) == 
        _x_false){
        //LCC_Err 0x0002
        nToWaitEnd++;
        return;
	} else {
        //DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"get packet bootloader\n\r");
        nToWaitEnd = 0;
        for(i=0;i<(g_gld.cmd.recieve_cmd_size);i++){
            g_gld.cmd.recieve_cmd[i] = x_ring_pop(&g_gld.cmd.ask.ring_in);
        }        
        return;      
	}
    
    
    /*
    static int ToWaitEnd, ErrReg ;
    char dbg[64];
    
    //e. end part of packet is absent
	if (( ToWaitEnd > 25000)) {
        do rcv_buf[--rcv_num_byt] = 0;
        while(rcv_num_byt);
        
        rcv_num_byt_old = rcv_num_byt;
        uart_recieve_reset();
        ToWaitEnd = 0;
        return;
	}
    //e. we have not received any new bytes
	if (rcv_num_byt_old == rcv_num_byt) { 
        if (ToWaitEnd) ToWaitEnd++;
        return;
	}
	rcv_num_byt_old = rcv_num_byt;
    
    
    //проверим размеры команд. если не подходит - ждем корректную команду
    if(
        (rcv_num_byt == 6)
        ||(rcv_num_byt == 8)
        ||(rcv_num_byt == 9)
        ||(rcv_num_byt == 22)
        ||(rcv_num_byt == 30)
        ||(rcv_num_byt == 134)
    ){
    } else if(rcv_num_byt>134) {
        //TODO ошибка - нестандартный размер
        ToWaitEnd++;
        return;
    } else {
        //ждем
        ToWaitEnd++;
        return;
    }
    //проверим LCC
    if (command_check_lcc(rcv_buf,rcv_num_byt) == _x_false){
        //LCC_Err 0x0002
        ToWaitEnd++;
        return;
	} else {
        //DBG0(dbg,64,"get packet bootloader");
		rcv_Rdy = 1;	  	
	}
	ToWaitEnd = 0;
    */
#endif
}
/******************************************************************************/
void command_recieve(command_recieve_flag flag)
{ 
    x_uint32_t num;
#ifdef UART_RING
#else
	uart_recieve(rcv_buf,&rcv_num_byt);
#endif
    
    switch(flag){
    case _command_recieve_flag_gld:
        command_recieve_gld();
        break;
        
    case _command_recieve_flag_bootloader:
        //command_recieve_bootloader();
        break;
    
    default:
        break;
    }
	
	return;
} 
/******************************************************************************/
void command_transm(void)
{
    x_uint32_t param, param_byte, CRC; 
    x_int16_t *trans_param;	
    //char out[512];
    x_uint8_t trm_buf[GLD_BUF_SIZE];
    int i = 0;
    char dbg[64];
    char dbg1[512];		
    int iCRC_calc = 0;
    int trm_num_byt = 0;
    
    //e. is transfer needed?  
    if (trm_ena == 0)										
        return;
    
    if (uart_is_ready_transm() == _x_false)										
        return;
    
    trm_ena = 0; //e. reset the flag of transmission allowing 

    trm_num_byt = 2;

    trm_buf[0] = 0x00dd; //e. header of answering packet 
    trm_buf[1] = Device_blk.Str.My_Addres; //e. own device address 
    CRC = trm_buf[1]; //e. initialization of CRC counter   
    
    //out[0] =  0xdd;
    //out[1] =  Device_blk.Str.My_Addres;
    //CRC = out[1];
    
    //e. data block creation cycle 
    for ( param = 0; param < num_of_par; param++) {		  		  	
        //e. reading of current output parameter's address 	    
        trans_param = (x_int16_t *)addr_param[param]; 
        
        for (param_byte = 0; param_byte < size_param[param]; param_byte++) {   
            if ( (param_byte & 0x0001) == 0 ) {
                //e.if we are reading MSB
                //e. allocating of the current parameter in the packet 
                if(size_param[param]>1) trm_buf[trm_num_byt] = ( (*trans_param) >> (8)) & 0x00ff;
                else if(size_param[param]==1) trm_buf[trm_num_byt] = ( (*trans_param) ) & 0x00ff;
            }else {
                trm_buf[trm_num_byt] = (*trans_param) & 0x00ff;
                trans_param ++; //e. go to next memory cell         
            }
            CRC += trm_buf[trm_num_byt]; //e. current CRC calculation              
            trm_num_byt++; //e. number of bytes which have transmitted into packet    
        } 
    }
    trm_buf[trm_num_byt] = CRC >> 8; //e. writing of final CRC into the packet 
    trm_buf[trm_num_byt+1] = CRC & 0x00ff;

    trm_num_byt += 2;
    
#ifdef UART_RING
    for(i=0;i<trm_num_byt;i++){
        x_ring_put(trm_buf[i],&g_gld.cmd.ask.ring_out);
    }
#else
    uart_transm( trm_num_byt, Device_Mode, trm_buf);
#endif
    
    /******
    for (iCRC_calc = 0; iCRC_calc < (trm_num_byt); iCRC_calc++){
            sprintf(dbg1,"%s%x",dbg1,trm_buf[iCRC_calc]);
    }
    DBG1(&g_gld.cmd.dbg.ring_out,dbg,512,"%s\n\r",dbg1);  
    /******/
}
/******************************************************************************/
void command_decode(void)
{
    int size;
    int i = 0;
    char dbg[64];	
    
    //e. is data in receive buffer?
    if (g_gld.cmd.recieve_cmd_size==0) return;
    
    //e. Whether there were errors of receiving of start-bit?
    if (line_sts==0){  
        //e. there were not errors of receiving of bytes, check the device address
        if ( (g_gld.cmd.recieve_cmd[1] != Device_blk.Str.My_Addres)
            && (g_gld.cmd.recieve_cmd[1] != BROADCAST_ADDRESS)
            ) {
            DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"Wrong address\n\r");
			goto end;
		}//My_Addres
            
        //e. there is new command in the receiver buffer, stop the transfer 
        if (g_gld.cmd.recieve_cmd[0] == COMMAND_PREFIX) {
            //e. reset the flag of transmission allowing 
            trm_ena = 0;
            DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"command_handle\n\r");
            command_handle();  
            //e. check up presence of errors in operation of this procedure 						
            if ( ((line_sts & CODE_ERR) == CODE_ERR) || ((line_sts & PARAM_ERR) == PARAM_ERR) ) {
                //e. set error bits in the error register of the line 
                line_err = line_sts;
            }            
        }else{
            DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"Wrong PREFIX\n\r");
        }//COMMAND_PREFIX
    }else{
        DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"Wrong line_err\n\r");
		line_err = line_sts;
    }//line_sts
    //e. is copying of present received packet needed?
	if (g_gld.cmd.flags.bit.rx_cpy) {
        memcpy(
            g_gld.cmd.recieve_cmd_cpy
            , g_gld.cmd.recieve_cmd
            , g_gld.cmd.recieve_cmd_size);
    }
end:   
    //clear recieved
    memset(
            g_gld.cmd.recieve_cmd
            , 0x0
            , g_gld.cmd.recieve_cmd_size);
    g_gld.cmd.recieve_cmd_size = 0;
    
    /*************
    //e. is data in receive buffer?
	if (!rcv_Rdy) return;
	
    //e. Whether there were errors of receiving of start-bit?
	if (!line_sts){  
		//e. there were not errors of receiving of bytes, check the device address 
		if (rcv_buf[1] != Device_blk.Str.My_Addres) {
			if (rcv_buf[1] != BROADCAST_ADDRESS) {
				goto end;
			}
		}
		
        //e. there is new command in the receiver buffer, stop the transfer 
        if (rcv_buf[0] == COMMAND_PREFIX) {
            //e. reset the flag of transmission allowing 
            trm_ena = 0;
            command_handle();  
            //e. check up presence of errors in operation of this procedure 						
            if ( ((line_sts & CODE_ERR) == CODE_ERR) || ((line_sts & PARAM_ERR) == PARAM_ERR) ) {
                //e. set error bits in the error register of the line 
                line_err = line_sts;
            }
        } else {
            line_err = line_sts | NO_CMD_ERR;
        }
	} else {
		line_err = line_sts;
	}
end:
    //e. is copying of present received packet needed?
	if (rx_buf_copy) {
		size = rcv_num_byt; 
        //e. save the size of copy 
		rcv_byt_copy = rcv_num_byt; 
        //e. compare the size of receiving buffer with amount of received bytes 
		if (rcv_num_byt > sizeof(rcv_buf)) {
			size = sizeof(rcv_buf);
		}
        //e. copy of received amount of bytes, but no more than buffer size
		memcpy(rcv_copy, rcv_buf, size);
	}
    //e. reset the flag of necessity of copying of receiving buffer  
    rx_buf_copy = 0;
   	
	do
	    rcv_buf[--rcv_num_byt] = 0;
	while(rcv_num_byt);

    rcv_num_byt_old = rcv_num_byt;
    //e.  reset the line status register
    line_sts = 0;
    //e. allow further data reception
    rcv_Rdy = 0;
    /*************/
}

/******************************************************************************/
//e procedure of set of rate and periodicity of answer 
void command_utility_read_param(void)
{
	//e. is periodic data transmission needed? 
	if ((g_gld.cmd.recieve_cmd[3] & 0x0080) != 0)  {
        //e. yes, set present flag 
		g_gld.cmd.trm_cycl = 1;
	} else {
        //e. no, reset present flag
		g_gld.cmd.trm_cycl = 0; 
	}
	//e. clear the bit of transfer rate
	SRgR &= 0xffcf;
	g_gld.cmd.trm_rate = (g_gld.cmd.recieve_cmd[3] >> 1) & 0x0030;
    //e. set present transfer rate
	SRgR |= g_gld.cmd.trm_rate;
    //уберем 4 разряда чтобы получить полноценный код
    g_gld.cmd.trm_rate = g_gld.cmd.trm_rate>>4;
} // SetSpeedPeriod

/******************************************************************************/

/******************************************************************************/
void command_SwitchSpeed(void)
{
    int i = 0;
    
    //check if trm_rate changed
    if(g_gld.cmd.trm_rate == g_gld.cmd.trm_rate_prev) {
        return;
    }else{
        g_gld.cmd.trm_rate_prev = g_gld.cmd.trm_rate;
    }
    //if changed upd registers
    
    //disable DMA
    //whait until current tranfer ends
    uart_disable_transm();
    
    //switch uart speed
    UART_SwitchSpeed(g_gld.cmd.trm_rate);
    
    //enable DMA
    uart_enable_transm();
}
/******************************************************************************/