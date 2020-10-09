
#include "hardware/hardware.h"

#define CS_0 (1<<16)
/******************************************************************************/
int main(void)
{
    int i=0;
    int nArrayIn=0;
    int nCount=0;
    int nExchangeErr=0;
    
    //инициализация GPIO
    hardware_configure_lightup();
    //инициализация SPI
    spi_init();
    
    //all pins after reset is in GPIO mode, so CS pins needn't to configure
    LPC_GPIO0->FIODIR |= (CS_0);		// P0.16 defined as CS for ADC
    LPC_GPIO0->FIOSET |= (CS_0);		// set CS for ADC
    
    //all pins after reset is in GPIO mode, so CS pins needn't to configure
    LPC_GPIO0->FIODIR |= (1<<18);		// P0.16 defined as CS for ADC
    LPC_GPIO0->FIOSET |= (1<<18);		// set CS for ADC
    
    //all pins after reset is in GPIO mode, so CS pins needn't to configure
    LPC_GPIO0->FIODIR |= (1<<19);		// P0.16 defined as CS for ADC
    LPC_GPIO0->FIOSET |= (1<<19);		// set CS for ADC
    
    //all pins after reset is in GPIO mode, so CS pins needn't to configure
    LPC_GPIO0->FIODIR |= (1<<20);		// P0.16 defined as CS for ADC
    LPC_GPIO0->FIOSET |= (1<<20);		// set CS for ADC
    
    hardware_lightup_off();
    while(1){
        hardware_lightup_off();
        
        //Прочтем данные с АЦП
        spi_set_cs(CS_0);
        spi_read(&nArrayIn,1,&nExchangeErr);
        spi_reset_cs(CS_0);
        
        for(i=0;i<10000;i++);
        hardware_lightup_on();
    }
    
    return 0;
}
/******************************************************************************/