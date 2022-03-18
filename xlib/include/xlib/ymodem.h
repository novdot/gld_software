/**
  ******************************************************************************
  * @file    IAP/inc/ymodem.h 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    10/15/2010
  * @brief   This file provides all the software function headers of the ymodem.c 
  *          file.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _YMODEM_H_
#define _YMODEM_H_

#include "xlib/types.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)
#define PACKET_TRAILER          (2)
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)

#define FILE_NAME_LENGTH        (256)
#define FILE_SIZE_LENGTH        (16)

#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define EOT                     (0x04)  /* end of transmission */
#define ACK                     (0x06)  /* acknowledge */
#define NAK                     (0x15)  /* negative acknowledge */
#define CA                      (0x18)  /* two of these in succession aborts transfer */
#define CRC16                   (0x43)  /* 'C' == 0x43, request 16-bit CRC */

#define ABORT1                  (0x41)  /* 'A' == 0x41, abort by user */
#define ABORT2                  (0x61)  /* 'a' == 0x61, abort by user */

/****
#define NAK_TIMEOUT             (0x100000)
#define MAX_ERRORS              (5)
/****/

#define NAK_TIMEOUT             (0x300000)
#define MAX_ERRORS              (45)
/****/
#define YM_MAX_FILESIZE         (10*1024*1024)

typedef x_uint32_t (*recieve_byte_t)(x_uint8_t *key);
typedef void (*send_byte_t)(x_uint8_t c);
typedef x_bool_t (*mem_write_t)(x_uint8_t* file,x_uint16_t size,x_uint8_t *buf);
typedef x_bool_t (*mem_erase_t)(x_uint8_t* file,x_uint16_t size);

typedef struct{
    recieve_byte_t recieve_byte;
    send_byte_t send_byte;
    mem_write_t mem_write;
    mem_erase_t mem_erase;
}x_ymodem_setups;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
//Receive a file using the ymodem protocol
x_int32_t x_Ymodem_Receive(x_ymodem_setups, x_uint8_t *);

//Transmit a file using the ymodem protocol
x_uint8_t x_Ymodem_Transmit(x_ymodem_setups ,x_uint8_t *,const  x_uint8_t* , x_uint32_t );

#endif  /* _YMODEM_H_ */

/*******************(C)COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
