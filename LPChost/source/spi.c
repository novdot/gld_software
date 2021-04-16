#include "hardware/spi.h"

#include "lpc17xx.h"

#define SSPCR1_SSE		 0x00000002

#define TX_SSP_EMPT		 0x00000001
#define RX_SSP_notEMPT	 0x00000004
#define TX_SSP_notFULL	 0x00000002
#define RX_SSP_FULL		 0x00000008
#define SSP_BUSY		 0x00000010

#define ADC_ERR_MSK		 0x00000001
#define DAC_ERR_MSK		 0x00000002

/******************************************************************************/
void spi_init()
{
    int Dummy = 0;

    /* Enable AHB clock to the SSP0, SSP1 */
    LPC_SC->PCONP |= (0x1<<21);
    // LPC_SC->PCONP |= (0x1<<10);

    /* Further divider is needed on SSP0,SSP1 clock. Using default divided by 4 */
    LPC_SC->PCLKSEL1 &= ~(0x3<<10);	//00 CLK/4;	1 CLK; 2 CLK/2; 3 CLK/8
    LPC_SC->PCLKSEL1 |= (0x0<<10);	//00 CLK/4;	1 CLK; 2 CLK/2; 3 CLK/8
    // LPC_SC->PCLKSEL0 &= ~(0x3<<20);

    // P0.15~0.18 as SSP0 
    LPC_PINCON->PINSEL0 &= ~(0x3UL<<30);
    LPC_PINCON->PINSEL0 |=  (0x2UL<<30);
    LPC_PINCON->PINSEL1 &= ~((0x3<<0)|(0x3<<2)|(0x3<<4));
    LPC_PINCON->PINSEL1 |=  ((0x2<<2)|(0x2<<4));	 

    LPC_PINCON->PINMODE0 &= ~(0x3UL<<30);
    LPC_PINCON->PINMODE0 |=  (0x3UL<<30);
    LPC_PINCON->PINMODE1 &= ~((0x3<<2)|(0x3<<4));
    LPC_PINCON->PINMODE1 |=  ((0x3<<2)|(0x3<<4));

    /* P0.6~0.9 as SSP1 */
    /*  LPC_PINCON->PINSEL0 &= ~((0x3<<12)|(0x3<<14)|(0x3<<16)|(0x3<<18));
    LPC_PINCON->PINSEL0 |=  ((0x2<<12)|(0x2<<14)|(0x2<<16)|(0x2<<18));
    LPC_PINCON->PINMODE0 &= ~((0x3<<16)|(0x3<<12)|(0x3<<14)|(0x3<<18));
    LPC_PINCON->PINMODE0 |= ((0x3<<16)|(0x3<<14)|(0x3<<18));	 */

    /* Set DSS data to 16-bit, Frame format TI, SCR is 2*/
    LPC_SSP0->CR0 = ((3<<8)|(0<<7)|(0<<4) |0xF);
    //LPC_SSP1->CR0 = 0x0207;

    /* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
    LPC_SSP0->CPSR = 0x2;	  // freq = CLK/(cpsdvr*(scr+1)) = 1.6 MHz
    //LPC_SSP1->CPSR = 0x2;	

    /*SSP enable, master mode */
    LPC_SSP0->CR1 = SSPCR1_SSE;	
    //LPC_SSP1->CR1 = SSPCR1_SSE;

    while(LPC_SSP0->SR & SSP_BUSY) ;
    //while(LPC_SSP1->SR & SSP_BUSY);

    /* clear the RxFIFO */
    while (LPC_SSP0->SR & RX_SSP_notEMPT)
        Dummy = LPC_SSP0->DR;		
    
    //all pins after reset is in GPIO mode, so CS pins needn't to configure
    LPC_GPIO0->FIODIR |= PIN_ADC_CS;		// P0.16 defined as CS for ADC
    LPC_GPIO0->FIOSET |= PIN_ADC_CS;		// set CS for ADC

    LPC_GPIO0->FIODIR |= PIN_DAC_CS;		// P defined as CS for DAC
    LPC_GPIO0->FIOCLR |= PIN_DAC_CS;		// set CS for DAC 
    /*
    while (LPC_SSP1->SR & RX_SSP_notEMPT)
        Dummy = LPC_SSP1->DR;
    */
}

/******************************************************************************/
void spi_write(int* a_nDataOut,int a_nCount,int*a_pExchangeErr)
{
    int iterator = 0;
    //e. reset ADC bit of error register;
    (*a_pExchangeErr) &= ~ADC_ERR_MSK;		   

    //e. if exchanging is activing, error
    if (LPC_SSP0->SR & SSP_BUSY) {
        //e. reset DAC bit of error register;
        (*a_pExchangeErr) |= ADC_ERR_MSK;
    } else {
        for(iterator=0;iterator<a_nCount;iterator++) {
            LPC_SSP0->DR=a_nDataOut[iterator];
        }
    }
}

/******************************************************************************/
void spi_read(int*a_pArrayIn,int a_nCount,int*a_pExchangeErr)
{
    uint32_t Dummy = 0;
    int iterator = 0;
    
    Dummy = Dummy;
    (*a_pExchangeErr) &= ~ADC_ERR_MSK;

    //exchanging is active, error
    if (LPC_SSP0->SR & SSP_BUSY) {
        (*a_pExchangeErr) |= ADC_ERR_MSK;
        //buffer is not empty
        if (!(LPC_SSP0->SR & TX_SSP_EMPT))
            (*a_pExchangeErr) |= ADC_ERR_MSK;
    } else {
        //exchanging has finished, read buffer
        for(iterator=0;iterator<a_nCount;iterator++) {
            a_pArrayIn[iterator] = LPC_SSP0->DR;
            //Dummy = LPC_SSP0->DR;	
            //Dummy = LPC_SSP0->DR;	
        }
        
        // clear the RxFIFO
        while (LPC_SSP0->SR & RX_SSP_notEMPT)
            Dummy = LPC_SSP0->DR;	
    }
}

/******************************************************************************/
void spi_set_cs(x_uint32_t data)
{
    LPC_GPIO0->FIOSET = data;
}
/******************************************************************************/
void spi_reset_cs(x_uint32_t data)
{
    LPC_GPIO0->FIOCLR = data;
}
