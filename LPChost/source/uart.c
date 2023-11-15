#include "hardware/uart.h"

#include "lpc17xx.h"
#include "system_LPC17xx.h"

/* Work on these values of system clock
* FOSC 12000000 
* FCCLK (FOSC  * 8) 
* FCCO (FCCLK * 3)
* FPCLK (FCCLK / 4)
* FPCLK (103200000) 
*/
extern unsigned int SystemCoreClock; 

uint32_t EnablLength = 12;
uint32_t LLI0_TypeDef[4];
uint32_t LLI1_TypeDef[4];
uint32_t EnablTx = 0x80;
//uint32_t EnablDMA = 0;

#define FRACTIONAL_BAUD

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

// PCUART0
#define PCUART0_POWERON (1 << 3)
#define PCLK_UART0 6
#define PCLK_UART0_MASK (3 << PCLK_UART0)

// PCUART1
#define PCUART1_POWERON (1 << 4)
#define PCLK_UART1 8
#define PCLK_UART1_MASK (3 << PCLK_UART1)

// PCUART2
#define PCUART2_POWERON (1 << 24)
#define PCLK_UART2 16
#define PCLK_UART2_MASK (3 << PCLK_UART2)

// PCUART3
#define PCUART3_POWERON (1 << 25)
#define PCLK_UART3 18
#define PCLK_UART3_MASK (3 << PCLK_UART3)


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

#define BR_LOOKUP_SIZE 72

//Tables for looking up fractional baud rate values.
float FRList[BR_LOOKUP_SIZE] = {1.000,1.067,1.071,1.077,1.083,1.091,1.100,1.111,1.125,1.133,1.143,1.154,1.167,1.182,1.200,1.214,1.222,1.231,1.250,
1.267,1.273,1.286,1.300,1.308,1.333,1.357,1.364,1.375,1.385,1.400,1.417,1.429,1.444,1.455,1.462,1.467,1.500,1.533,1.538,1.545,1.556,
1.571,1.583,1.600,1.615,1.625,1.636,1.643,1.667,1.692,1.700,1.714,1.727,1.733,1.750,1.769,1.778,1.786,1.800,1.818,1.833,1.846,1.857,
1.867,1.875,1.889,1.900,1.909,1.917,1.923,1.929,1.933};
float DIVADDVALList[BR_LOOKUP_SIZE] = {0.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,2.0,1.0,2.0,1.0,2.0,1.0,3.0,2.0,3.0,1.0,4.0,3.0,2.0,3.0,4.0,1.0,5.0,4.0,3.0,
5.0,2.0,5.0,3.0,4.0,5.0,6.0,7.0,1.0,8.0,7.0,6.0,5.0,4.0,7.0,3.0,8.0,5.0,7.0,9.0,2.0,9.0,7.0,5.0,8.0,11.0,3.0,10.0,7.0,11.0,4.0,9.0,5.0,
11.0,6.0,13.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0};
float MULVALList[BR_LOOKUP_SIZE] = {1.0,15.0,14.0,13.0,12.0,11.0,10.0,9.0,8.0,15.0,7.0,13.0,6.0,11.0,5.0,14.0,9.0,13.0,4.0,15.0,11.0,7.0,10.0,13.0,3.0,
14.0,11.0,8.0,13.0,5.0,12.0,7.0,9.0,11.0,13.0,15.0,2.0,15.0,13.0,11.0,9.0,7.0,12.0,5.0,13.0,8.0,11.0,14.0,3.0,13.0,10.0,7.0,11.0,15.0,
4.0,13.0,9.0,14.0,5.0,11.0,6.0,13.0,7.0,15.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0};


void UART0_Init(x_uint32_t baudrate);
void UART1_Init(x_uint32_t baudrate);
void UART2_Init(x_uint32_t baudrate);


void UART0_SendString(char* ucData,int size);
void UART1_SendString(char* ucData,int size);
void UART2_SendString(char* ucData,int size);
/******************************************************************************/
int getPclk()
{
    switch( (LPC_SC->PCLKSEL0 >> 6) & 0x03 ) {
        case 0x00:
            return SystemCoreClock/4;
        
        case 0x01:
            return SystemCoreClock;
        
        case 0x02:
            return SystemCoreClock/2;
        
        case 0x03:
            return SystemCoreClock/8;

    }
    return 0;    
}
/******************************************************************************/
/**
 * Return 1 if the double is an int value, 0 if not
 */
int isIntValue(double value) 
{
	int intValue = (int)value;
	if( value == intValue ) {
		return 1;
	}
	return 0;
}

/******************************************************************************/
/*
 * Get the fraction values for the given FRest value.
 */
int getFRValues(double FRest, float *divAddVal, float *mulVal) 
{
	float lastDiff = -1;
	float thisDiff;
	int index;
	//Look through the lookup table and find the index of the value
	//that provides the smallest difference between the FRest value
	//and the lookup table value.
	for( index=0 ; index<BR_LOOKUP_SIZE ; index++ ) {
		if( FRest > FRList[index] ) {
			thisDiff = FRest-FRList[index];
		}
		else {
			thisDiff = FRList[index]-FRest;
		}
        if( lastDiff != -1 && thisDiff > lastDiff ) {
          //Set the fractional values required
          *divAddVal=DIVADDVALList[index-1];
          *mulVal=MULVALList[index-1];
          return 0;
        }
        lastDiff=thisDiff;
	}
	return -1;
}

/******************************************************************************/
/*
 * Get the fraction values required to set an accurate BR
 *
 * Return -1 on error, 0 on success.
 */
int getFractionValues(int pclk, int baudRate, int *dlEst, float *divAddVal, float *mulVal) 
{
	double  dlEstFloat = pclk/(16.0*baudRate);
	double 	FRestSeed = 1.5;
	double  FRest;
	int 	DLest;

	//If this pclk and baud rate give and integer division
	//we don't need the fractional calculation
    if( isIntValue(dlEstFloat) ) {
    	*dlEst = (int)dlEstFloat;
    	*divAddVal=0.0;
    	*mulVal=1.0;
    	return 0;
    }

	while(1) {
		DLest = (int)(pclk/(16.0*baudRate*FRestSeed));
		FRest = pclk/(16.0*baudRate*DLest);
		//If we have the required accuracy
		if( FRest >= 1.1 && FRest < 1.9) {
			break;
		}

		if( FRestSeed <= 1.5 ) {
			FRestSeed-=0.001;
			if( FRestSeed < 1.1 ) {
				FRestSeed=1.5001;
			}
		}
		else {
			FRestSeed=FRestSeed+0.001;
        	if( FRestSeed >= 1.9 ) {
        		return -1;
        	}
		}
	}
	*dlEst=(int)DLest;
	return getFRValues(FRest, divAddVal, mulVal);
}


/******************************************************************************/
void UART_Init(x_uint32_t baudrate)
{
#if defined (UART0REC)
    UART0_Init(baudrate);
#elif defined (UART1REC)
    UART1_Init(baudrate);
#elif defined (UART2REC)
    UART2_Init(baudrate);
#endif
    
#if defined(UART0DBG)
    //UART0_Init(38400);
    UART0_Init(921600);
#endif
}
/******************************************************************************/
void UART0_Init(x_uint32_t baudrate)
{
    //uint32_t Fdiv = 0;
    uint32_t pclk = 0;
	int 	dlEest;
	float 	divAddVal, mulVal;
    
    //switch on UART0
    LPC_SC->PCONP |= (1<<3);	
   
    LPC_PINCON->PINSEL0 |=  0x00000050;            
            
	pclk = getPclk();

    LPC_UART0->LCR  = word_length_8 |one_stop_bit |no_parity |back_trans_dis |DLAB_access;                     
    
#if defined(FRACTIONAL_BAUD) 
    if( getFractionValues(pclk, baudrate, &dlEest, &divAddVal, &mulVal) == -1 ) {
        return;
    }    
    LPC_UART0->DLM = dlEest / 256;
    LPC_UART0->DLL = dlEest % 256;
    //Setup the fractional divider register
    LPC_UART0->FDR = (((int)mulVal)<<4)|(int)divAddVal;
#else
    Fdiv = (pclk / 16) / baudrate;           
    LPC_UART0->DLM  = Fdiv / 256;
    LPC_UART0->DLL  = Fdiv % 256;
#endif //FRACTIONAL_BAUD
     
    LPC_UART0->LCR  &= ~DLAB_access;	                      
    LPC_UART0->FCR  = TX_FIFO_Reset |RX_FIFO_Reset |FIFOs_En |RX_TrigLvl_14;
	LPC_UART0->IER = 0;//RBR_IntEnabl;

#if defined(UART0DBG)
#elif defined(UART0REC)
    //e. DMA mode select 
	LPC_UART0->FCR |= 0x08; 
#endif 	
    
	// enable signal initialization
    /*LPC_PINCON->PINSEL1 &= ~0x0000C000;	//e. select P0.23 as general purpose
	LPC_GPIO0->FIODIR |= 0x00800000;	//e. P0.23 is output 
    //LPC_GPIO0->FIOMASK |= 0x007F0000;	//e. P0.16..P0.22 is not changed by FIOSET writing 
	LPC_GPIO0->FIOCLR |= 0x00800000;	// e. clear P0.23
    */
    return; 
}

/******************************************************************************/
void UART1_Init(x_uint32_t baudrate)
{
    //uint32_t Fdiv = 0;
    //uint32_t usFdiv = 0;
    uint32_t pclk = 0;
    
    int 	dlEest;
	float 	divAddVal, mulVal;
    
    //switch on UART1
    LPC_SC->PCONP |= (1<<4);
   
    //P2.0, P2.1, P2.5, P2.7 
    LPC_PINCON->PINSEL4 |= (2 << 0); /* Pin P2.0 used as TXD0 (Com0) */
    LPC_PINCON->PINSEL4 |= (2 << 2); /* Pin P2.1 used as RXD0 (Com0) */
    //LPC_PINCON->PINSEL4 |= (2 << 10);
    LPC_PINCON->PINSEL4 |= (2 << 14);

    pclk = getPclk();

    LPC_UART1->LCR  = word_length_8 |one_stop_bit |no_parity |back_trans_dis |DLAB_access;                         
    //LPC_UART1->LCR  = 0x83; 
    
#if defined(FRACTIONAL_BAUD) 
    if( getFractionValues(pclk, baudrate, &dlEest, &divAddVal, &mulVal) == -1 ) {
        return;
    }    
    LPC_UART1->DLM = dlEest / 256;
    LPC_UART1->DLL = dlEest % 256;
    //Setup the fractional divider register
    LPC_UART1->FDR = (((int)mulVal)<<4)|(int)divAddVal;
#else
    Fdiv = (pclk / 16) / baudrate;           
    LPC_UART1->DLM  = Fdiv / 256;
    LPC_UART1->DLL  = Fdiv % 256;
#endif //FRACTIONAL_BAUD
    
    LPC_UART1->LCR  &= ~DLAB_access;
    //LPC_UART1->LCR  = 0x03;    
    LPC_UART1->FCR  = TX_FIFO_Reset |RX_FIFO_Reset |FIFOs_En |RX_TrigLvl_14;
	//LPC_UART1->FCR  = 0x07; 
    LPC_UART1->IER = 0;//RBR_IntEnabl;

    //так как на этой линии используется MAX3294 доп пин TXE нужно установить в 3.3В
    // Enable Auto Direction Control.
    //The direction control pin will be driven to logic ‘1’ when the transmitter has data to
    //be sent. It will be driven to logic ‘0’ after the last bit of data has been transmitted
	LPC_UART1->RS485CTRL = (1<<5)|(1<<4);
		
    //e. DMA mode select 
#if defined(UART1DBG)
#elif defined(UART1REC)
	LPC_UART1->FCR |= 0x08; 
#endif 					
    return; 
}

/******************************************************************************/
void UART2_Init(x_uint32_t baudrate)
{
    //uint32_t Fdiv = 0;
    //uint32_t usFdiv = 0;
    uint32_t pclk = 0;
   
    int dlEest = 0;
	float divAddVal, mulVal = 0.0;
    
    /* UART2 */
    LPC_PINCON->PINSEL0 |= (1 << 20); /* Pin P0.10 used as TXD2 (Com2) */
    LPC_PINCON->PINSEL0 |= (1 << 22); /* Pin P0.11 used as RXD2 (Com2) */

   	LPC_SC->PCONP = LPC_SC->PCONP|(1<<24);	      

    LPC_UART2->LCR  = 0x83;              
        
    pclk = getPclk();
    
#if defined(FRACTIONAL_BAUD) 
    if( getFractionValues(pclk, baudrate, &dlEest, &divAddVal, &mulVal) == -1 ) {
        return;
    }    
    LPC_UART2->DLM = dlEest / 256;
    LPC_UART2->DLL = dlEest % 256;
    //Setup the fractional divider register
    LPC_UART2->FDR = (((int)mulVal)<<4)|(int)divAddVal;
#else
    Fdiv = (pclk / 16) / baudrate;           
    LPC_UART2->DLM  = Fdiv / 256;
    LPC_UART2->DLL  = Fdiv % 256;
#endif //FRACTIONAL_BAUD 
    
    
    LPC_UART2->LCR  = 0x03;                      
    LPC_UART2->FCR  = 0x06;
}


/******************************************************************************/
void UART_DBG_SendString(char* ucData,int size)
{
#if defined (UART0DBG)
    UART0_SendString(ucData,size);
#elif defined (UART1DBG)
    UART1_SendString(ucData,size);
#elif defined (UART2DBG)
    UART2_SendString(ucData,size);
#endif 
}
/******************************************************************************/
void UART_SendString(char* ucData,int size)
{
#if defined (UART0REC)
    UART0_SendString(ucData,size);
#elif defined (UART1REC)
    UART1_SendString(ucData,size);
#elif defined (UART2REC)
    UART2_SendString(ucData,size);
#endif 
}
/******************************************************************************/
int UART_SendByte(char ucData)
{
#if defined (UART0REC)
    return UART0_SendByte(ucData);
#elif defined (UART1REC)
    return UART1_SendByte(ucData);
#elif defined (UART2REC)
    return UART2_SendByte(ucData);
#endif 
}

/******************************************************************************/
int UART0_SendByte(int ucData)
{
	while (!(LPC_UART0->LSR & 0x20));
    return (LPC_UART0->THR = ucData);
}
void UART0_SendString(char* ucData,int size)
{
    int iitem = 0;
    for(iitem=0;iitem<size;iitem++) {
        while (!(LPC_UART0->LSR & 0x20));
        LPC_UART0->THR = ucData[iitem];
    }
}
/******************************************************************************/
int UART1_SendByte(int ucData)
{
	while (!(LPC_UART1->LSR & 0x20));
    return (LPC_UART1->THR = ucData);
}
void UART1_SendString(char* ucData,int size){}
/******************************************************************************/
int UART2_SendByte(int ucData)
{
	while (!(LPC_UART2->LSR & 0x20));
    return (LPC_UART2->THR = ucData);
}
void UART2_SendString(char* ucData,int size){}
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
	//EnablDMA = (LPC_GPDMACH1->CConfig)|DMAChannelEn; 

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

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
x_uint32_t uart_recieve_byte(x_uint8_t *key)
{
 #if defined UART1REC
    if ((LPC_UART1->LSR & RecievBufEmpty) != 0) { 
        *key = LPC_UART1->RBR;
        //TODO temp for tests
        UART0_SendByte(*key);
        return 1;
    }else
#elif defined UART0REC
    if ((LPC_UART0->LSR & RecievBufEmpty) != 0){ 
        *key = LPC_UART0->RBR;
        return 1;
    }else
#endif
    {
        *key = 0;
        return 0;
    }
}

/******************************************************************************/
void uart_send_byte(x_uint8_t c)
{
    UART_SendByte(c);
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void uart_send_unblocked(int a_ch, x_ring_buffer_t*a_pbuf)
{
    switch(a_ch){
        case 0:
            if(!(LPC_UART0->LSR & 0x20)) return;
            if(x_ring_get_count(a_pbuf)<=0) return;
            LPC_UART0->THR = x_ring_pop(a_pbuf);
            break;
        case 1:
            if(!(LPC_UART1->LSR & 0x20)) return;
            if(x_ring_get_count(a_pbuf)<=0) return;
            LPC_UART1->THR = x_ring_pop(a_pbuf);
            break;
    }
}
/******************************************************************************/
void uart_send_blocked(int a_ch, x_ring_buffer_t*a_pbuf)
{
    switch(a_ch){
        case 0:
            //if(!(LPC_UART0->LSR & 0x20)) return;
            while(x_ring_get_count(a_pbuf)>0){
                while (!(LPC_UART0->LSR & 0x20));
                LPC_UART0->THR = x_ring_pop(a_pbuf);
            }
            break;
        case 1:
            //if(!(LPC_UART1->LSR & 0x20)) return;
            while(x_ring_get_count(a_pbuf)>0){
                while (!(LPC_UART1->LSR & 0x20));
                LPC_UART1->THR = x_ring_pop(a_pbuf);
            }
            break;
    }
}
/******************************************************************************/
#include <core/global.h>
void uart_recieve(x_uint8_t*a_pBuffer,x_uint32_t*a_uCount)                  
{ 
    static uint8_t max_packet = 0;
    //e. reciever contain some information
#if defined UART1REC
    //The U1IER is used to enable the four UART1 interrupt sources
    //LPC_UART1->IER = 0; 
    
    //The U1MSR is a read-only register that provides status information 
    //on the modem input signals. 
    //LPC_UART1->MSR;
    
    //The U1LCR determines the format of the data character that is to 
    //be transmitted or received.
    //DLAB set 0
    //LPC_UART1->LCR &= 0x7F;
    
    //The U1LSR is a read-only register that provides status information 
    //on the UART1 TX and RX blocks.
    if( (LPC_UART1->LSR&0x1)==1 ){
        if( (*a_uCount)>=(RCV_BUF_SIZE-1) ) return;
        a_pBuffer[(*a_uCount)] = LPC_UART1->RBR;
        (*a_uCount) += 1;
        if( (*a_uCount)>=RCV_BUF_SIZE ) *a_uCount=(RCV_BUF_SIZE-1);
    }
#elif defined UART0REC
    while ((LPC_UART0->LSR & RecievBufEmpty) != 0){ 
        a_pBuffer[(*a_uCount)] = LPC_UART0->RBR;
        (*a_uCount) += 1;
    }
#endif	 
}
/******************************************************************************/
void uart_recieve_n(x_uint8_t a_ind, x_uint8_t*a_pBuffer, x_uint32_t*a_uCount)
{
    switch(a_ind){
        case 0:
            while ((LPC_UART0->LSR & RecievBufEmpty) != 0) { 
                a_pBuffer[(*a_uCount)] = LPC_UART0->RBR;
                (*a_uCount) += 1;
            }
            break;
            
        case 1:
            while ((LPC_UART1->LSR & RecievBufEmpty) != 0) { 
                a_pBuffer[(*a_uCount)] = LPC_UART1->RBR;
                (*a_uCount) += 1;
            }
            break;
    }
}
/******************************************************************************/
void uart_recieve_unblocked(int a_ch, x_ring_buffer_t*a_pbuf)
{
    x_uint8_t temp = 0;
    switch(a_ch){
        case 0:
            if((LPC_UART0->LSR & RecievBufEmpty) == 0) return;
            temp = LPC_UART0->RBR;
            x_ring_put(temp,a_pbuf);
            break;
        case 1:
            if((LPC_UART1->LSR & RecievBufEmpty) == 0) return;
            temp = LPC_UART1->RBR;
            x_ring_put(temp,a_pbuf);
            break;
    }
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
void uart_disable_transm(void)
{
    LPC_GPDMACH1->CConfig &= ~(DMAChannelEn&(1<<18));
    LPC_GPDMACH2->CConfig &= ~(DMAChannelEn&(1<<18));
    //while( LPC_GPDMACH1->CConfig & (1<<17)) ;
    //whait until current tranfer ends - TX bit check
    while( !(LPC_UART1->LSR & (1<<5)) );
}
/******************************************************************************/
void uart_enable_transm(void)
{
    LPC_GPDMACH1->CConfig |= DMAChannelEn&(1<<18);
    LPC_GPDMACH2->CConfig |= DMAChannelEn&(1<<18);
}
/******************************************************************************/
void uart_transm(x_uint32_t trm_num_byt, int ext_latch, x_uint8_t*a_pBufferTransm)
{
    LPC_GPDMACH1->CSrcAddr = (uint32_t)a_pBufferTransm;
    
    /** Config
    0 E Channel enable. 
    5:1 SrcPeripheral Source peripheral. 
    10:6 DestPeripheral Destination peripheral. 
    13:11 TransferType 
    14 IE Interrupt error mask.
    15 ITC Terminal count interrupt mask.
    16 L Lock. 
    17 A Active:
    18 H Halt
    
    Control
    11:0 TransferSize
    14:12 SBSize
    17:15 DBSize
    20:18 SWidth
    23:21 DWidth
    25:24 - 0
    26 SI
    27 DI
    28 Prot1
    29 Prot2
    30 Prot3
    31 I
    */

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

    //if (Device_Mode < 4) //e. work with internal latch
    if(ext_latch==0)
    {						 	
        //LPC_TIM0->TCR = 1; //e. start timer		

        //LPC_GPIO2->FIOSET |= 8; //turn on RS-422 driver 

        LPC_GPDMACH1->CConfig |=  DMAChannelEn;//e. DMA for UART transmition
        LPC_GPDMACH2->CConfig |=  DMAChannelEn;//e. DMA for enable signal 
    }//else
    {
        //LPC_TIM0->TCR = 1; //e. start timer		//latch_start_meas        
        //LPC_GPIOINT->IO0IntEnR |= (1<<1);
        //LPC_GPIOINT->IO0IntClr |= (1<<1); //e. clean external latch interrupt request
        //LPC_GPDMACH1->CConfig |=  DMAChannelEn; //e. DMA for UART transmition
    }
    /*if (Device_Mode == 4) //e. work with ext latch
    {					
        //LPC_TIM0->TCR = 1; //e. start timer		//latch_start_meas        
        //LPC_GPIOINT->IO0IntEnR |= (1<<1);
        //LPC_GPIOINT->IO0IntClr |= (1<<1); //e. clean external latch interrupt request
        //LPC_GPDMACH1->CConfig |=  DMAChannelEn;				    //e. DMA for UART transmition
    }*/
	  	                          	   
    return;	
}	  

/******************************************************************************/
void UART_SwitchSpeed(unsigned Speed)
{
    //uint32_t Fdiv = 0;
    uint32_t pclk = 0;
    int baudrate = 0;
	int dlEest = 0;
	float divAddVal, mulVal = 0;
    
//#include "core/global.h"
    //int i=0;
    //char dbg[64];
    
    /*
    Переключение скорости только при настройке
    модуля по умолчанию
    */
#ifndef __CONFIG_COMMANDS_DEFAULT
    return;
#endif //__CONFIG_COMMANDS_DEFAULT
    
    
#if defined  UART1REC
	LPC_UART1->LCR |= DLAB_access;
    
#elif defined  UART2REC
	LPC_UART2->LCR |= DLAB_access;
    
#elif defined  UART0REC
	LPC_UART0->LCR |= DLAB_access;
    
#endif //UART1REC
    
	switch (Speed) {
		case _uart_baudrate_38400:
            baudrate = 38400;
            break;

		case _uart_baudrate_115200:
            baudrate = 115200;
            break;

		case _uart_baudrate_460800:
            baudrate = 460800;	
            break;

		case _uart_baudrate_921600:
            baudrate = 921600;
            break;
        default:
            break;
	}
    
    //DBG1(&g_gld.cmd.dbg.ring_out,dbg,64,"Switch to BR:%d",baudrate );

    pclk = getPclk();
    
#if defined(FRACTIONAL_BAUD)   
    if( getFractionValues(pclk, baudrate, &dlEest, &divAddVal, &mulVal) == -1 ) {
        return;
    }
#else
    Fdiv = (pclk / 16) / baudrate; 
#endif //FRACTIONAL_BAUD
    
#if defined UART1REC
    #if defined(FRACTIONAL_BAUD)
    LPC_UART1->DLM = dlEest / 256;
    LPC_UART1->DLL = dlEest % 256;
    //Setup the fractional divider register
    LPC_UART1->FDR = (((int)mulVal)<<4)|(int)divAddVal;
    #else
    LPC_UART1->DLM  = Fdiv / 256;
    LPC_UART1->DLL  = Fdiv % 256;
    #endif //FRACTIONAL_BAUD
    LPC_UART1->LCR  &= ~DLAB_access;
    
#elif defined UART2REC  
    #if defined(FRACTIONAL_BAUD)
    LPC_UART2->DLM = dlEest / 256;
    LPC_UART2->DLL = dlEest % 256;
    //Setup the fractional divider register
    LPC_UART2->FDR = (((int)mulVal)<<4)|(int)divAddVal;
    #else
    LPC_UART2->DLM  = Fdiv / 256;
    LPC_UART2->DLL  = Fdiv % 256;
    #endif //FRACTIONAL_BAUD
    LPC_UART2->LCR  &= ~DLAB_access;
    
#elif defined UART0REC
#if defined(FRACTIONAL_BAUD)
    LPC_UART0->DLM = dlEest / 256;
    LPC_UART0->DLL = dlEest % 256;
    //Setup the fractional divider register
    LPC_UART0->FDR = (((int)mulVal)<<4)|(int)divAddVal;
    #else
    LPC_UART0->DLM  = Fdiv / 256;
    LPC_UART0->DLL  = Fdiv % 256;
    #endif //FRACTIONAL_BAUD
    LPC_UART0->LCR  &= ~DLAB_access;
    
#endif //UART1REC
    
}
