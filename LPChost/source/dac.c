
#include "hardware/dac.h"

#include "lpc17xx.h"

/******************************************************************************/
void hardware_dac_init()
{
    // setup the related pin to DAC output
    // set p0.26 to DAC output
    LPC_PINCON->PINSEL1 |= 0x00200000;	
    // pull up/down on p0.26 disabled
    LPC_PINCON->PINMODE1 |= 0x00200000;	 
    
    LPC_DAC->CNTVAL = 0;
    LPC_DAC->CTRL = 0;
    
    //PCLKSEL0
    //LPC_SC->PCLKSEL0 = 1<<24; //pclk=cclk
}
/******************************************************************************/
void hardware_dac_send(x_uint32_t a_data)
{
    //data 15..6
    LPC_DAC->CR = (a_data<<6);
}
/******************************************************************************/
