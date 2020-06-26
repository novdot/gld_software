/**
  ******************************************************************************
  * @file    command.h
  *
  * @brief   система команд прибора ГЛД
    Чтение,расшифровка,выдача команд
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
#ifndef __COMMAND_H_INCLUDED 
#define __COMMAND_H_INCLUDED

#include "xlib/types.h"

/******************************************************************************/
#define  STRT_ERR        0x0080 //e. error of the start bit
#define  STOP_ERR        0x0040 //e. error of the stop bit
#define  SIZE_ERR        0x0004 //e. size of the received data packet mismatches the agreement  
#define  LCC_ERR         0x0002 //e. checksum has not coincided 
#define  NO_CMD_ERR      0x0100 //e. in the received packet there is no attribute of command 
#define  CODE_ERR        0x0200 //e. unknown code of a command 
#define  MODE_ERR        0x0400 //e. code of a command mismatches a current mode
#define  PARAM_ERR       0x0800 //e. parameters of a command was set incorrectly 
#define  MAXSIZE_ERR     0x1000 //e. receiver buffer overflow 
#define  ADDR_ERR        0x2000 //e. incorrect address of the device 
#define  READ_ERR        0x4000 //e. stop of link because of untimely data read  
#define  WASQ_ERR        0x8000 //e. error of waiting of authentic answer 
                                
#define  Trm_En_Rd       0x0080 //e. a mask of permission/readiness of transmitter of the 2 line 
#define  Rcv_Rdy         0x0040 //e. a mask of the readiness bit of the receiver 
#define  Rcv_Ferr        0x0020 //e. a mask of the "format error" bit 
#define  Rcv_Tout        0x0010 //e. a mask of the "time-out" bit 
#define  Rcv_Rful        0x0008 //e. a mask of the "stack is full" bit 

/******************************************************************************/
//e. command codes, received from a line
#define  COMMAND_PREFIX	    0xCC			//e. attribute of command  
#define  BROADCAST_ADDRESS  0x1f  		//e. the broadcasting address ( shared for 3 GLD ) 

/******************************************************************************/
#define COMMAND_UTILITY_ANSWER_FIELD(index,ptr_addr,size) \
	addr_param[index] = ptr_addr; \
	size_param[index] = size;

/******************************************************************************/
/**
    @brief CRC checking
    @param a_pBuffer - data
    @param a_uCount - data size
    @return true - correct packet
*/
x_bool_t check_lcc(x_uint8_t* a_pBuffer,x_uint32_t a_uCount);

/**
    @brief procedure of set of rate and periodicity of answer 
    @return 
*/
void command_utility_SetSpeedPeriod(void);

/**
    @brief чтение пакета информации из периферии и сохранение в буффер
    @return 
*/
void command_recieve(void);

/**
    @brief выдача пакета информации из буффера в периферию
    @return
*/
void command_transm(void);

/**
    @brief обработка заголовка пакета
    @return
*/
void command_decode(void);

/**
    @brief обработка полей пакета
    @return
*/
void command_handle(void);


/**
    @brief 
    @return
*/
void command_echo(void);


#endif