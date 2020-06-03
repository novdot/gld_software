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
#include "CntrlGLD.h"

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
			do
					rcv_buf[--rcv_num_byt] = 0;
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
void command_handle(void)
{
    x_uint32_t uCmdCode = 0;
    x_uint32_t uCmdCodeLong = 0;

    uCmdCode = (rcv_buf[2] & 0xFF) << 8;
    CMD_Code = uCmdCode | (rcv_buf[3] & 0xFF);
    
    //e. initialization of the flag of copying of receiving buffer
	rx_buf_copy = 1;
    
    switch(uCmdCode){
        case CMD_DELTA_PS    : command_cmd_DELTA_PS();	return;
        case CMD_DELTA_BINS  : command_cmd_DELTA_BINS();	return;
        case CMD_DELTA_SF	 : command_cmd_DELTA_SF(); return;
        case CMD_DEV_MODE    : command_cmd_DEV_MODE(); return;
        case CMD_BIT_MODE    : command_cmd_BIT_MODE();	return;
        case CMD_RATE        : command_cmd_RATE();	return;
        case CMD_DELTA       : command_cmd_DELTA();	return;
        case CMD_D_PERIOD_W  : command_cmd_D_PERIOD_W();	return;

        case SUBCMD_M_STIMUL : command_subcmd_M_STIMUL();  return;
        case SUBCMD_M_RESET  : command_subcmd_M_RESET();  return;

        case SUBCMD_M_CTL_R  : command_subcmd_M_CTL_R();  return;
        case SUBCMD_M_CTL_M  : command_subcmd_M_CTL_M();  return;

        case SUBCMD_M_TMP_W  : command_subcmd_M_TMP_W();  return;
        case SUBCMD_M_TMP_R  : command_subcmd_M_TMP_R();  return;
        case SUBCMD_M_E5R_W  : command_subcmd_M_E5R_W();  return;
        case SUBCMD_M_ADC_R  : command_subcmd_M_ADC_R();  return;
        case SUBCMD_M_VIB_W  : command_subcmd_M_VIB_W();  return;
        case SUBCMD_M_CNT_R  : command_subcmd_M_CNT_R();  return;
        case SUBCMD_M_GPH_W  : command_subcmd_M_GPH_W();  return;
        case SUBCMD_M_FLG_R  : command_subcmd_M_FLG_R();  return;
        case SUBCMD_M_PARAM_W: command_subcmd_M_PARAM_W();  return;
        case SUBCMD_M_PARAM_R: command_subcmd_M_PARAM_R();  return;
        case SUBCMD_M_E5RA_W : command_subcmd_M_E5RA_W();  return;

        case SUBCMD_M_RATE  : 
            uCmdCodeLong = uCmdCode | (rcv_buf[3] & 0x1F);
            break;

        default:
            uCmdCodeLong = uCmdCode | (rcv_buf[3] & 0xFF);
            break;
    }
    
    switch(uCmdCodeLong){
        case CMD_MAINT       :  command_cmd_MAINT(); return;

        case SUBCMD_M_CLEAR  :  command_subcmd_M_CLEAR(); return;
        case SUBCMD_M_MIRR   :  command_subcmd_M_MIRR(); return;
        case SUBCMD_M_LDPAR_F:  command_subcmd_M_LDPAR_F(); return;
        case SUBCMD_M_LDPAR_D:  command_subcmd_M_LDPAR_D(); return;
        case SUBCMD_M_START  :  command_subcmd_M_START(); return;
        case SUBCMD_M_STOP   :  command_subcmd_M_STOP(); return;
        case SUBCMD_M_PULSE  :  command_subcmd_M_PULSE(); return;

        case SUBCMD_M_RATE1  :  command_subcmd_M_RATE1(); return;
        case SUBCMD_M_RATE2  :  command_subcmd_M_RATE2(); return;
        case SUBCMD_M_RATE3  :  command_subcmd_M_RATE3(); return;
        case SUBCMD_M_RATE7  :  command_subcmd_M_RATE7(); return;
        case SUBCMD_M_RATE5K :  command_subcmd_M_RATE5K(); return;
        
        default: break;
    }
}
/******************************************************************************/