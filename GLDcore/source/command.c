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
#include "hardware/hardware.h"

//TODO
#include "CyclesSync.h"

/******************************************************************************/
x_bool_t command_check_lcc(x_uint8_t* a_pBuffer,x_uint32_t a_uCount)
{
	int iCRC_calc = 0;
	int CRC_calc = 0;
	int CRC_real = 0;

	for (iCRC_calc = 1; iCRC_calc < (a_uCount-2); iCRC_calc++)
		CRC_calc += a_pBuffer[iCRC_calc];  

	CRC_real = (a_pBuffer[a_uCount-2] << 8) | a_pBuffer[a_uCount-1];

	if( (CRC_real - CRC_calc) == 0) return _x_true;
	else return _x_false;
}
/******************************************************************************/
void command_recieve(void)
{ 
	static int ToWaitEnd, ErrReg ;

	uart_recieve(rcv_buf,&rcv_num_byt);
    
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

	if ((rcv_num_byt < 6) || ((rcv_num_byt & 0x0001) == 1)) {
			ToWaitEnd++;
			return;
	}

	//e. the header of packet has not recieved
	if ((!ToWaitEnd) && (rcv_num_byt > 1))
			if (
					(rcv_buf[0] != 0xCC) 
					|| (( rcv_buf[1] > 2) && ( rcv_buf[1] != 0x1F))
			) {   
					//	L1_Rc_err (HEADER_ERR);
					ErrReg |= 5;
					ToWaitEnd++;
					return;
			}
	//  if (ErrReg != 0)	//e. trying of recovering of packet 
	//	  PacketSafing();


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
        return;
	} else {
		rcv_Rdy = 1;	  	
	}
	ToWaitEnd = 0;

	return;
} 
/******************************************************************************/
void command_transm(void)
{
    x_uint32_t param, param_byte, CRC; 
    x_int32_t *trans_param;		
    
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
    
    for ( param = 0; param < num_of_par; param++) //e. data block creation cycle 
    {		  		  	
        trans_param = (int32_t *)addr_param[param]; //e. reading of current output parameter's address 	    

        for (param_byte = 0; param_byte < size_param[param]; param_byte++) {   
            if ( (param_byte & 0x0001) == 0 ) //e.if we are reading MSB 
                trm_buf[trm_num_byt] = (*trans_param >> (8)) & 0x00ff;	    	 //e. allocating of the current parameter in the packet 
            else {
                trm_buf[trm_num_byt] = *trans_param & 0x00ff;
                trans_param ++; //e. go to next memory cell         
            }
            CRC += trm_buf[trm_num_byt]; //e. current CRC calculation              
            trm_num_byt++; //e. number of bytes which have transmitted into packet    
        } 
    }
    trm_buf[trm_num_byt] = CRC >> 8; //e. writing of final CRC into the packet 
    trm_buf[trm_num_byt+1] = CRC & 0x00ff;

    trm_num_byt += 2;
    
    uart_transm( trm_num_byt, Device_Mode, trm_buf);
}
/******************************************************************************/
void command_decode(void)
{
    int size;
  
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
}

/******************************************************************************/
//e procedure of set of rate and periodicity of answer 
void command_utility_SetSpeedPeriod(void)
{
	//e. is periodic data transmission needed? 
	if ((rcv_buf[3] & 0x0080) != 0)  {
        //e. yes, set present flag 
		trm_cycl = 1;
	} else {  
        //e. no, reset present flag
		trm_cycl = 0; 
	}
	//e. clear the bit of transfer rate
	SRgR &= 0xffcf;
	trm_rate = (rcv_buf[3] >> 1) & 0x0030;
    //e. set present transfer rate
	SRgR |= trm_rate;
} // SetSpeedPeriod

/******************************************************************************/
