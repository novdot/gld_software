#ifndef __UART_H_INCLUDED 
#define __UART_H_INCLUDED

#include "xlib/types.h"
#include "xlib/ring_buffer.h"

#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

#define BUFSIZE		0x40
#define FIFOs_En			0x00000001
#define RX_FIFO_Reset		0x00000002
#define TX_FIFO_Reset		0x00000004
#define DMA_Mode_UART		0x00000008
#define RX_TrigLvl_1		0x00000000
#define RX_TrigLvl_4		0x00000040
#define RX_TrigLvl_8		0x00000080
#define RX_TrigLvl_14		0x000000C0

#define	word_length_8		0x00000003

#define one_stop_bit	    0x00000000

#define no_parity			0x00000000

#define back_trans_dis	    0x00000000

#define DLAB_access			0x00000080

#define TRANS_SHIFT_BUF_EMPTY		0x00000040
#define RecievBufNOTEmpty		0x00000001
#define DIS_ALL_INT			0x00000000
#define RBR_IntEnabl		0x00000001


#define FIFOs_En			0x00000001
#define RX_FIFO_Reset		0x00000002
#define TX_FIFO_Reset		0x00000004
#define DMA_Mode_UART		0x00000008
#define RX_TrigLvl_1		0x00000000
#define RX_TrigLvl_4		0x00000040
#define RX_TrigLvl_8		0x00000080
#define RX_TrigLvl_14		0x000000C0

#define	word_length_8		0x00000003

#define one_stop_bit	    0x00000000

#define no_parity			0x00000000

#define back_trans_dis	    0x00000000

#define DLAB_access			0x00000080

#define TRANS_SHIFT_BUF_EMPTY		0x00000040
#define DMA_BUSY					0x00020000
#define RecievBufEmpty		0x00000001
#define DIS_ALL_INT			0x00000000
#define RBR_IntEnabl		0x00000001
/* Second half of the second RAM is used for GPDMA operation. */

#define DMA_UART0_TX		8
#define DMA_UART0_RX		9
#define DMA_UART1_TX		10
#define DMA_UART1_RX		11
#define DMA_UART2_TX		12
#define DMA_UART2_RX		13
#define DMA_UART3_TX		14
#define DMA_UART3_RX		15

#define DMA_MEMORY			0
#define SrcDMA_UART0_RX		DMA_UART0_RX << 1
#define SrcDMA_UART0_TX		DMA_UART0_TX << 1
#define SrcDMA_UART1_TX		DMA_UART1_TX << 1
#define DstDMA_UART0_TX		DMA_UART0_TX << 6
#define DstDMA_UART1_TX		DMA_UART1_TX << 6
#define DstDMA_UART0_RX		DMA_UART0_RX << 6
#define SrcDMA_UART1_RX		DMA_UART1_RX << 1
#define DstDMA_UART1_RX		DMA_UART1_RX << 6

/* UART0 TX and RX */
#define UART0_DMA_TX_SRC	0x2007C800		/* starting addr of DATA register in UART0 */
#define UART0_DMA_TX_DST	LPC_UART0_BASE
#define UART0_DMA_RX_SRC	LPC_UART0_BASE
#define UART0_DMA_RX_DST	0x2007C900

#define UART2_DMA_TX_DST	LPC_UART2_BASE
#define UART1_DMA_TX_DST	LPC_UART1_BASE

#define GPDMA_POWER_ON		0x20000000

#define UART_REQ			0x00000000

//To clear particular DMA TC-interrupts
#define DMA0_IntTCClear		0x00000001
#define DMA1_IntTCClear		0x00000002
#define DMA2_IntTCClear		0x00000004
#define DMA3_IntTCClear		0x00000008
#define DMA4_IntTCClear		0x00000010
#define DMA5_IntTCClear		0x00000020
#define DMA6_IntTCClear		0x00000040
#define DMA7_IntTCClear		0x00000080

//To clear particular DMA Error-interrupts
#define DMA0_IntErrClear	0x00000001
#define DMA1_IntErrClear	0x00000002
#define DMA2_IntErrClear	0x00000004
#define DMA3_IntErrClear	0x00000008
#define DMA4_IntErrClear	0x00000010
#define DMA5_IntErrClear	0x00000020
#define DMA6_IntErrClear	0x00000040
#define DMA7_IntErrClear	0x00000080
#define DMACH1_IntTCPend	0x00000002

#define DMA_ControllerEn	0x00000001

#define DMA_AHB_Little		0x00000000
#define DMA_AHB_Big			0x00000002

#define SrcBSize_1			0x00000000
#define SrcBSize_4			0x00001000
#define SrcBSize_8			0x00002000
#define SrcBSize_16			0x00003000
#define SrcBSize_32			0x00004000
#define SrcBSize_64			0x00005000
#define SrcBSize_128		0x00006000
#define SrcBSize_256		0x00007000

#define DstBSize_1			0x00000000
#define DstBSize_4			0x00008000
#define DstBSize_8			0x00010000
#define DstBSize_16			0x00018000
#define DstBSize_32			0x00020000
#define DstBSize_64			0x00028000
#define DstBSize_128		0x00030000
#define DstBSize_256		0x00038000

#define SrcWidth_8b			0x00000000
#define SrcWidth_16b		0x00020000
#define SrcWidth_32b		0x00040000

#define DstWidth_8b			0x00000000
#define DstWidth_16b		0x00200000
#define DstWidth_32b		0x00400000

#define SrcInc				0x04000000
#define SrcFixed			0x00000000

#define	DstInc				0x08000000
#define	DstFixed			0x00000000

#define TCIntEnabl			0x80000000
#define TCIntDisabl			0x00000000	

#define DMAChannelEn		0x00000001
#define DMAChannelDis		0x00000000

#define CH2_ENABLED			0x00000004

#define DONtMaskTCInt		0x00008000
#define MaskTCInt			0x00000000
#define	DONtMaskErrInt		0x00004000
#define MaskErrInt			0x00000000

#define INT_DMA_Disabl		0x04000000
/* DMA mode */
#define M2M				0x00
#define M2P				0x01
#define P2M				0x02
#define P2P				0x03

#define Sp38400				0x00000
#define Sp115200			0x00010
#define Sp460800			0x00020
#define Sp921600			0x00030

typedef enum uart_baudrate_speedDef{
    _uart_baudrate_38400 = 0
    , _uart_baudrate_115200 = 1
    , _uart_baudrate_460800 = 2
    , _uart_baudrate_921600 = 3
}uart_baudrate_speed;

/******************************************************************************
** Function name:		UARTInit
**
** Descriptions:		Initialisation of UART on 38400 baud
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void UART_Init(x_uint32_t baudrate);

#define DBG_PREPARE(buf,size) memset(buf,' ',size);

#if defined(UART0DBG)
#define UART_DBG_SEND(buf) ; if(x_ring_get_count(buf)>0) uart_send_unblocked(0,buf);
#else 
#define UART_DBG_SEND(buf) ;
#endif
/**/
//buff to ring
#define DBG_SEND(ring,buf,size) \
    for(i=0;i<size;i++){x_ring_put(buf[i],ring);}\
    UART_DBG_SEND(ring);

#define DBG0(ring,buf,size,text)\
    DBG_PREPARE(buf,size)\
    sprintf(buf,text);\
    DBG_SEND(ring,buf,strlen(buf));
    
#define DBG1(ring,buf,size,text,par1)\
    DBG_PREPARE(buf,size)\
    sprintf(buf,text,par1);\
    DBG_SEND(ring,buf,strlen(buf));
    
#define DBG2(ring,buf,size,text,par1,par2)\
    DBG_PREPARE(buf,size)\
    sprintf(buf,text,par1,par2);\
    DBG_SEND(ring,buf,strlen(buf));
    
#define DBG3(ring,buf,size,text,par1,par2,par3)\
    DBG_PREPARE(buf,size)\
    sprintf(buf,text,par1,par2,par3);\
    DBG_SEND(ring,buf,strlen(buf));
    
#define DBG4(ring,buf,size,text,par1,par2,par3,par4)\
    DBG_PREPARE(buf,size)\
    sprintf(buf,text,par1,par2,par3,par4);\
    DBG_SEND(ring,buf,strlen(buf));
    
#define DBG5(ring,buf,size,text,par1,par2,par3,par4,par5)\
    DBG_PREPARE(buf,size)\
    sprintf(buf,text,par1,par2,par3,par4,par5);\
    DBG_SEND(ring,buf,strlen(buf));
    
#define DBG6(ring,buf,size,text,par1,par2,par3,par4,par5,par6)\
    DBG_PREPARE(buf,size)\
    sprintf(buf,text,par1,par2,par3,par4,par5,par6);\
    DBG_SEND(ring,buf,strlen(buf));
/**/
void UART_DBG_SendString(char* ucData,int size);
void UART_SendString(char* ucData,int size);
int UART_SendByte(char ucData);

void uart_send_unblocked(int ch, x_ring_buffer_t*a_pbuf);
/******************************************************************************
** Function name:		uart_recieve
**
** Descriptions:		receive process preparation
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void uart_recieve(x_uint8_t*a_pBuffer,x_uint32_t*a_uCount);
void uart_recieve_n(x_uint8_t a_ind, x_uint8_t*a_pBuffer, x_uint32_t*a_uCount);
/******************************************************************************
** Function name:		uart_recieve_reset
**
** Descriptions:		
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void uart_recieve_reset(void);

/******************************************************************************
** Function name:		uart_transm
**
** Descriptions:		transmit process preparation
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void uart_transm(x_uint32_t trm_num_byt, int Device_Mode, x_uint8_t*a_pBufferTransm);

/******************************************************************************
** Function name:		DMA_Init
**
** Descriptions:		
**
** parameters:			
** Returned value:		
** 
******************************************************************************/
void DMA_Init(void);

/******************************************************************************
** Function name:		UART_DMA_Init
**
** Descriptions:		Initialisation of DMA channel  for UART transmitter
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void uart_dma_init(x_uint8_t*a_pBufferTransm);

/******************************************************************************
** Function name:		UART_SwitchSpeed
**
** Descriptions:		Change UART speed 
**
** parameters:			Demanded speed
** Returned value:		None
** 
******************************************************************************/
void UART_SwitchSpeed(unsigned Speed);
void uart_disable_transm(void);
void uart_enable_transm(void);

#endif