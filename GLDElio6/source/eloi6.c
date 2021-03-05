
#include "hardware/hardware.h"

#define CS_0 (1<<16)

#define BUFFER_CNT 6
/******************************************************************************/
int main(void)
{
    int i=0;
    int nArrayIn=0;
    int nCount=0;
    int nExchangeErr=0;
    int buffer[BUFFER_CNT];
    
    int* pExchangeErr = &nExchangeErr;
    
    //e. clocking control initialization
    SystemInit();
    
    //e. to calculate SystemCoreClock  for UART particularly
    SystemCoreClockUpdate();
    
    spi_init();
    
    while(1){
        memset(buffer,0,BUFFER_CNT*sizeof(int));
        
        hardware_set_adc();	
        spi_write(buffer,6,pExchangeErr);
        hardware_reset_adc(); 
        return;
        
        
        
        //spi_read(buffer,BUFFER_CNT,pExchangeErr);
        //hardware_reset_adc(); 
        
        //for(i=0;i<1000;i++);
    }
    
    return 0;
}
/******************************************************************************/