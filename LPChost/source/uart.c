#include "hardware/uart.h"

#include "lpc17xx.h"
#include "system_LPC17xx.h"

#define FOSC 12000000 
#define FCCLK (FOSC  * 8) 
#define FCCO (FCCLK * 3)
#define FPCLK (FCCLK / 4)

extern unsigned int SystemCoreClock; 

uint32_t EnablLength = 12;
uint32_t LLI0_TypeDef[4];
uint32_t LLI1_TypeDef[4];
uint32_t EnablTx = 0x80;
uint32_t EnablDMA = 0;
/*
Выбрать UART0REC или UART1REC
в качестве периферии для реализации обмена
*/
//#define UART0REC
#define UART1REC

#if defined (UART1REC)
    #define CDESTADDR (UART1_DMA_TX_DST)
    #define GPDMACH1_CCONFIG (DstDMA_UART1_TX)
    #define GPDMACH2_CCONFIG (SrcDMA_UART1_RX|DstDMA_UART1_RX)
#elif defined (UART0REC)
    #define CDESTADDR (UART0_DMA_TX_DST)
    #define GPDMACH1_CCONFIG (DstDMA_UART0_TX)
    #define GPDMACH2_CCONFIG (SrcDMA_UART0_RX|DstDMA_UART0_RX)
#else
#error "Please define UART1REC or UART0REC for recive and trans ASK_GLD!"
#endif

/******************************************************************************/
void UART0_Init(void)
{
    uint32_t Fdiv = 0;
    uint32_t pclk = 0;
    
    uint32_t baudrate = 38400;
    
    //switch on UART0
    LPC_SC->PCONP |= (1<<3);	
   
    LPC_PINCON->PINSEL0 |=  0x00000050;            
            
	pclk = SystemCoreClock/4;

    LPC_UART0->LCR  = word_length_8 |one_stop_bit |no_parity |back_trans_dis |DLAB_access;                     
    Fdiv = (pclk / 16) / baudrate;           
    LPC_UART0->DLM  = Fdiv / 256;
    LPC_UART0->DLL  = Fdiv % 256; 
    LPC_UART0->LCR  &= ~DLAB_access;	                      
    LPC_UART0->FCR  = TX_FIFO_Reset |RX_FIFO_Reset |FIFOs_En |RX_TrigLvl_14;
	LPC_UART0->IER = 0;//RBR_IntEnabl;

    //e. DMA mode select 
	LPC_UART0->FCR |= 0x08;  	
    
	// enable signal initialization
    /*LPC_PINCON->PINSEL1 &= ~0x0000C000;	//e. select P0.23 as general purpose
	LPC_GPIO0->FIODIR |= 0x00800000;	//e. P0.23 is output 
    //LPC_GPIO0->FIOMASK |= 0x007F0000;	//e. P0.16..P0.22 is not changed by FIOSET writing 
	LPC_GPIO0->FIOCLR |= 0x00800000;	// e. clear P0.23
    */
    return; 
}

/******************************************************************************/
void UART1_Init(void)
{
    uint32_t Fdiv = 0;
    uint32_t pclk = 0;

    //uint32_t baudrate = 256000;
    uint32_t baudrate = 38400;

    //switch on UART1
    LPC_SC->PCONP |= (1<<4);
   
    //P2.0, P2.1, P2.5, P2.7 
    LPC_PINCON->PINSEL4 |=  (2<<0)|(2<<2)|(2<<10)|(2<<14);          
            
    pclk = SystemCoreClock/4;

    LPC_UART1->LCR  = word_length_8 |one_stop_bit |no_parity |back_trans_dis |DLAB_access;                     
    Fdiv = (pclk / 16) / baudrate;           
    LPC_UART1->DLM  = Fdiv / 256;
    LPC_UART1->DLL  = Fdiv % 256; 
    LPC_UART1->LCR  &= ~DLAB_access;	                      
    LPC_UART1->FCR  = TX_FIFO_Reset |RX_FIFO_Reset |FIFOs_En |RX_TrigLvl_14;
	LPC_UART1->IER = 0;//RBR_IntEnabl;

	//LPC_UART1->RS485CTRL = (1<<5);

    //e. DMA mode select 
	LPC_UART1->FCR |= 0x08;  				
    return; 
}

/******************************************************************************/
void UART2_Init (void)
{
	uint16_t usFdiv;
    /* UART2 */
    LPC_PINCON->PINSEL0 |= (1 << 20); /* Pin P0.10 used as TXD2 (Com2) */
    LPC_PINCON->PINSEL0 |= (1 << 22); /* Pin P0.11 used as RXD2 (Com2) */

   	LPC_SC->PCONP = LPC_SC->PCONP|(1<<24);	      

    LPC_UART2->LCR  = 0x83;                      
    usFdiv = (FPCLK / 16) / 115200;            
    LPC_UART2->DLM  = usFdiv / 256;
    LPC_UART2->DLL  = usFdiv % 256; 
    LPC_UART2->LCR  = 0x03;                      
    LPC_UART2->FCR  = 0x06;
}

/******************************************************************************/
int UART0_SendByte(int ucData)
{
	while (!(LPC_UART0->LSR & 0x20));
    return (LPC_UART0->THR = ucData);
}
/******************************************************************************/
int UART1_SendByte(int ucData)
{
	while (!(LPC_UART1->LSR & 0x20));
    return (LPC_UART1->THR = ucData);
}

/******************************************************************************/
int UART2_SendByte (int ucData)
{
	while (!(LPC_UART2->LSR & 0x20));
    return (LPC_UART2->THR = ucData);
}
/******************************************************************************/
void DMA_Init( void )
{
    /* Enable CLOCK into GPDMA controller */
    LPC_SC->PCONP |= GPDMA_POWER_ON;

    LPC_GPDMA->Config = DMA_ControllerEn | DMA_AHB_Little;
    while ( !(LPC_GPDMA->Config & DMA_ControllerEn) ); //wait until DMA_Controller  switched on

    NVIC_DisableIRQ(DMA_IRQn);
    return;
}
/******************************************************************************/
void uart_dma_init(x_uint8_t*a_pBufferTransm)
{
    //config channel for UART0
	LPC_GPDMACH1->CConfig &=  ~DMAChannelEn;  

	LPC_GPDMA->IntTCClear = DMA1_IntTCClear;
	LPC_GPDMA->IntErrClr = DMA1_IntErrClear; 
	  
    LPC_GPDMACH1->CSrcAddr = (uint32_t)a_pBufferTransm;
    LPC_GPDMACH1->CDestAddr = CDESTADDR;
    LPC_GPDMACH1->CControl = SrcBSize_1|DstBSize_1|SrcWidth_8b |DstWidth_8b|SrcInc |DstFixed |TCIntDisabl;
    LPC_GPDMACH1->CConfig |= MaskTCInt |MaskErrInt|DMA_MEMORY |GPDMACH1_CCONFIG |(M2P << 11);

    //save register content for DMA starting in multidrop mode
	EnablDMA = (LPC_GPDMACH1->CConfig)|DMAChannelEn; 

    //for Rate mode output
    LLI1_TypeDef[0] = (uint32_t)&a_pBufferTransm[32];
    LLI1_TypeDef[1] = CDESTADDR;
    LLI1_TypeDef[2] = 0;
    LLI1_TypeDef[3] = (12 & 0x0FFF)|SrcBSize_1 |DstBSize_1 |SrcWidth_8b |DstWidth_8b|SrcInc |DstFixed |TCIntDisabl;

    LLI0_TypeDef[0] = (uint32_t)&a_pBufferTransm[16];
    LLI0_TypeDef[1] = CDESTADDR;
    LLI0_TypeDef[2] = (uint32_t)&LLI1_TypeDef;
    LLI0_TypeDef[3] = (16 & 0x0FFF)|SrcBSize_1 |DstBSize_1 |SrcWidth_8b |DstWidth_8b|SrcInc |DstFixed |TCIntDisabl;

    //config channel for transmit enable signal
    LPC_GPDMACH2->CConfig &= ~DMAChannelEn;  

  	LPC_GPDMA->IntTCClear = DMA2_IntTCClear;
	LPC_GPDMA->IntErrClr = DMA2_IntErrClear; 

    //e. content of TX UART1 enable register
    //e. address of TX UART1 enable register (U1TER)
	LPC_GPDMACH2->CSrcAddr = (uint32_t)&EnablTx;
	LPC_GPDMACH2->CDestAddr = 0x40010030;					
	LPC_GPDMACH2->CControl = SrcBSize_4 |DstBSize_4|SrcWidth_8b |DstWidth_8b|SrcFixed |DstFixed |TCIntEnabl;
	LPC_GPDMACH2->CConfig |= MaskTCInt |MaskErrInt |GPDMACH2_CCONFIG|(M2P << 11);
    //e. linked list is empty
	LPC_GPDMACH2->CLLI = 0;
}
/******************************************************************************/
void uart_recieve(x_uint8_t*a_pBuffer,x_uint32_t*a_uCount)                  
{ 
    //e. reciever contain some information
#if defined UART1REC
    while ((LPC_UART1->LSR & RecievBufEmpty) != 0) { 
        a_pBuffer[*a_uCount] = LPC_UART1->RBR;
        (*a_uCount) += 1;
    }
#else
    while ((LPC_UART0->LSR & RecievBufEmpty) != 0){ 
        a_pBuffer[*a_uCount] = LPC_UART0->RBR;
        (*a_uCount) += 1;
    }
#endif	 
}

/******************************************************************************/
void uart_recieve_reset(void)
{
#if defined UART1REC
    LPC_UART1->FCR |= RX_FIFO_Reset; 
#else
    LPC_UART0->FCR |= RX_FIFO_Reset; 
#endif
}

/******************************************************************************/
x_bool_t uart_is_ready_transm(void)
{
    //e. transmit buffer is empty   
    /*if ((LPC_UART1->LSR & TRANS_SHIFT_BUF_EMPTY))
        if (!( LPC_GPDMACH1->CConfig & (1<<17)))
            LPC_GPIO2->FIOCLR |= 8; //switch off UART1 driver
    */
    
	//e. transmit buffer is empty
    /*if (!(LPC_UART0->LSR & TRANS_SHIFT_BUF_EMPTY))  
        return; //e. if the previous trasmission is active, suspend new 
    else
        if (!(LPC_UART1->LSR & TRANS_SHIFT_BUF_EMPTY))      //e. transmit buffer is empty 
            return;
    */
    
    //e. DMA_Busy if DMA channel is busy, wait
    if ( LPC_GPDMACH1->CConfig & (1<<17)) 
        return _x_false;
    
    return _x_true;
}

/******************************************************************************/
void uart_transm(x_uint32_t trm_num_byt, int Device_Mode, x_uint8_t*a_pBufferTransm)
{
    LPC_GPDMACH1->CSrcAddr = (uint32_t)a_pBufferTransm;

    LPC_GPDMACH1->CControl &= ~0xFFF; //e. reset of numer bytes for transmitting
    LPC_GPDMACH2->CControl &= ~0xFFF; //e. reset of numer bytes for transmitting

    LPC_GPDMACH1->CLLI = 0; //e. linked list is empty

    if (trm_num_byt > 16) //e. a packet is too long for FIFO 
    {		
        LPC_GPDMACH1->CControl |= 16; //e. set length of first packet part
        LPC_GPDMACH1->CLLI = (uint32_t)&LLI0_TypeDef; //e. initialize chain for other parts transmitting
    } else	 
        LPC_GPDMACH1->CControl |= trm_num_byt; 

    LPC_GPDMACH2->CControl |= 1; //e. set 1 transfert for enable signal   

    if (Device_Mode < 4) //e. work with internal latch
    {						 	
        LPC_TIM0->TCR = 1; //e. start timer		

        //LPC_GPIO2->FIOSET |= 8; //turn on RS-422 driver 

        LPC_GPDMACH1->CConfig |=  DMAChannelEn;				    //e. DMA for UART transmition
    }   

    LPC_GPDMACH2->CConfig |=  DMAChannelEn;                   //e. DMA for enable signal 	  	                          	   
    return;	
}	  

/******************************************************************************/
void UART_SwitchSpeed(unsigned Speed)
{
  uint32_t Fdiv;
  uint32_t pclk;

  pclk = SystemCoreClock/4;
#if defined  UART1REC
	LPC_UART1->LCR |= DLAB_access;
#else
	LPC_UART0->LCR |= DLAB_access;
#endif
	switch (Speed) {
		case Sp38400:
            Fdiv = (pclk / 16) / 38400; 
            EnablLength = 3240;
            break;

		case Sp115200:
            Fdiv = (pclk / 16) /115200; 
            EnablLength = 1090;
            break;

		case Sp460800:
            Fdiv = (pclk / 16) / 460800; 		 
            break;

		case Sp921600:
            Fdiv = (pclk / 16) / 921600; 
            EnablLength = 140;
            break;

	}
#if defined UART1REC	           
    LPC_UART1->DLM  = Fdiv / 256;
    LPC_UART1->DLL  = Fdiv % 256; 
    LPC_UART1->LCR  &= ~DLAB_access;
#else
	LPC_UART0->DLM  = Fdiv / 256;
    LPC_UART0->DLL  = Fdiv % 256; 
    LPC_UART0->LCR  &= ~DLAB_access;
#endif
}
