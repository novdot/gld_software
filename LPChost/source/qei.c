
#include "hardware/qei.h"


/******************************************************************************/
void SOI_Init(void)
{
    // Turn On MCPWM PCLK
    LPC_SC->PCONP |= (1<<18);//0x00040000;		

    LPC_SC->PCLKSEL1 &= ~(3<<0);    //e. timer 3 input frequency equal to CLCK/4    
    LPC_SC->PCLKSEL1 |= (0<<0);	   //00 CLK/4;	1 CLK; 2 CLK/2; 3 CLK/8
    
    LPC_PINCON->PINSEL3 &= ~0x4100;
    LPC_PINCON->PINSEL3 |= 0x4100;	  //P1.20, p1.23 are inputs for QEI
    LPC_PINCON->PINMODE3 |= 0x3C300;	  //P1.20, p1.23, p1.24 are pulled-down

    LPC_QEI->MAXPOS = MAX_QEI_CNT;	
    LPC_QEI->FILTER = 20;	 //for 00 CLK/4 	  
    LPC_QEI->CON = 0xF;				//reset all counter registers
    LPC_QEI->CONF = (0<<2) |(0<<1);	//Quadrature inputs, no inverting,only A pulses are counted 

    LPC_QEI->CLR = 0x1fff; 			//e. reset all interrupts 
    LPC_QEI->IEC = 0x1fff;			//e. disable direction changing interrupt 
    LPC_QEI->CMPOS0 = MAX_QEI_CNT + 1;
    LPC_QEI->CMPOS1 = MAX_QEI_CNT + 1;
    LPC_QEI->CMPOS2 = MAX_QEI_CNT + 1;
    //NVIC_SetPriority(QEI_IRQn, 0);
    NVIC_EnableIRQ(QEI_IRQn);
    return;
}

/******************************************************************************/
x_uint32_t qei_get_position()
{
    return LPC_QEI->POS;
}

/******************************************************************************/
x_direction_t qei_get_direction()
{
    if (LPC_QEI->STAT) return _x_plus;
    
    return _x_minus;
}