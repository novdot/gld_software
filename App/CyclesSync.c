
/**--------------File Info---------------------------------------------------------------------------------
** File name:           CycleSync.c
** Last modified Date:  2011-09-06
** Last Version:        V1.00
** Descriptions:        
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Electrooptica Inc.
** Created date:        2011-09-06
** Version:             V1.00
** Descriptions:        There is the routines for device synchronization
**
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/
#include "CyclesSync.h"
//#include "CntrlGLD.h"
#include "core/gld.h"
//#include "SIP.h"
//#include "el_lin.h"

#include "hardware/hardware.h"
#include "core/global.h"
#include "core/sip.h"

uint32_t	latch_num;
uint32_t	Delay_UART_Enbl = DELAY_UART_ENBL;
//uint32_t	Delay_UART_Disbl = DELAY_UART_ENBL;

#define EXT_LATCH_UART_DELAY (Device_blk.Str.My_Addres*1000 + 1)

/******************************************************************************
** Function name:		Latch_Event
**
** Descriptions:		Routine for latch appearing
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void Latch_Event()
{
    x_uint8_t dbg[64];
    int i;
    static unsigned PreLatch = 0;
    
    //e. latch is present
    if (LatchPhase < INT32_MAX){	
        //DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"Latch_Event LatchPhase\n\r");
        Latch_Rdy = 1; //e. set the flag for processing below
        if (g_gld.RgConB.word) {
            //e. work whith vibro counters
            if (PreLatch){ //e. we have had delayed latch
                //DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"PreLatch\n\r");
                PreLatch = 0;		 	  				
            }else if ((LatchPhase < LPC_PWM1->TC) && (latch_num == Sys_Clock)){
                //DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"appeared in current cycle\n\r");
                //e. latch have appeared in current cycle
                Latch_Rdy = 0;	//e. bring it to the next cycle	
                PreLatch = 1;
            }	  			
        } 
    } else
        Latch_Rdy = 0;				//e. latch is absent
}

/******************************************************************************
** Function name:		QEI_IRQHandler
**
** Descriptions:		Latch counters by reference meander
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/

#include "hardware/qei.h"
#include "core/sip.h"
#include "core/math_dsp.h"
#include <math.h>
#include <stdlib.h>

 uint32_t halfQEIPeriod = 0;

 __irq void QEI_IRQHandler (void) 
{
    //static uint32_t halfQEIPeriod = 0;
    //static x_direction_t direction_pos = _x_unk;
    
    //check int flag position on
    if( (LPC_QEI->INTSTAT & 0x0008) == 0) goto end;
    if(g_gld.pulses.reper_meandr.cnt_iter<7) goto end;
    
    g_gld.pulses.reper_meandr.cnt_iter = 0;
    
    g_gld.pulses.reper_meandr.cnt_curr = qei_get_position();
  
    g_gld.pulses.reper_meandr.cnt_int++;
        
    data_Rdy |= HALF_PERIOD;
    //e. period elapsed, we can calculate Cnt_Dif
    if( (++halfQEIPeriod)&1 ){
        data_Rdy |= WHOLE_PERIOD;	
    }
	
end:;		 
    // reset interrupt request
    LPC_QEI->CLR = 1<<3;	
}
 

__irq void MCPWM_IRQHandler (void) 
{
    //check LIM0 interrupt
    if (LPC_MCPWM->INTF & 0x0001) {
        g_gld.dither.flags.bit.isLimInt = 1;
        LPC_MCPWM->INTF_CLR |= 0x0001;
	}
}
/******************************************************************************
** Function name:		SetIntLatch
**
** Descriptions:		Set timer for accumulation period
**
** parameters:			Period of accumulation
** Returned value:		None
** 
******************************************************************************/
void SetIntLatch(uint32_t cycle)
{
    LPC_TIM3->TCR = 0x2; //switch off and reset timer3
    if (cycle != 0) {
        LPC_TIM3->MR0 = (cycle<<2); //load new value
        LPC_TIM3->TCR = 1; //switch on timer3
    }
}
/******************************************************************************
** Function name:		SwitchRefMeandInt
**
** Descriptions:		Enable/disable interrupt from reference meander
**
** parameters:			switcher
** Returned value:		None
** 
******************************************************************************/
void SwitchRefMeandInt(uint32_t s)
{
    LPC_QEI->CLR = 0x1fff; //e. reset all interrupts
    
    switch(s){
    case RATE_VIBRO_1:
        LPC_QEI->IEC = 0x1fff; //e.  disable direction changing interrupt
        break;
    case RATE_REPER_OR_REFMEANDR:
			  g_gld.pulses.reper_meandr.cnt_prev = qei_get_position();
				halfQEIPeriod = 0;
				data_Rdy = 0;
        LPC_QEI->IES = 0x0008; //e.  enable direction changing interrupt
        break;
    }
}

 
 /******************************************************************************
** Function name:		SwitchMode
**
** Descriptions:		Switch mode of device functionality
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
int SwitchMode()
{
    x_uint8_t dbg[64];
    int i;
    //DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"SwitchMode\n\r");
    
    //disable latch sources
    SetIntLatch(0); 	   					//e. disable internal latch
    LPC_TIM3->IR = 0x0001;				//e. clear internal latch interrupt request
#ifdef HOST4    // GPIO P0.1 as input
    LPC_GPIOINT->IO0IntEnR &= ~(1<<1);	//e. disable external latch
	  LPC_GPIOINT->IO0IntClr |= (1<<1);	//e. clean external latch interrupt request
#else  				  // GPIO P0.7 as input
    LPC_GPIOINT->IO0IntEnR &= ~(1<<8);	//e. disable external latch
	  LPC_GPIOINT->IO0IntClr |= (1<<8);	//e. clean external latch interrupt request	
#endif
    //LPC_TIM0->TCR = 2;						//e. stop and reset the multidrop delay timer
    //LPC_TIM0->IR = 0x03F;				//e. clear internal latch interrupt request
    //wait while UART and DMA are active									 	
	if ( LPC_GPDMACH1->CConfig & DMA_BUSY)				//e. if DMA channel is busy, wait
	  return 0;	
	LPC_GPDMACH1->CConfig &=  ~DMAChannelEn;			//e. disable DMA for UART transmition
	LPC_GPDMACH2->CConfig &=  ~DMAChannelEn;
    //uart_enable_transm
	if (!(LPC_UART1->LSR & TRANS_SHIFT_BUF_EMPTY))      //e. transmit buffer is not empty
        return 0;
	///LPC_UART1->FCR |= 0x4;								//e. reset TX FIFO

	//LPC_TIM0->IR = 0x3F;				 		//e. clear all interrupt flags 
    //---------------------configure a new exchanging parameters------------
    if (Device_Mode > 3) //e. external latch mode enabled
    {
        LPC_TIM0->MR0 = EXT_LATCH_UART_DELAY;
        //LPC_TIM0->MR0 = Device_blk.Str.My_Addres*10;	
        //LPC_TIM0->MR1 = Device_blk.Str.My_Addres*5000; //e. /10 = delay before enable signal (us)
        
        //DBG0(&g_gld.cmd.dbg.ring_out,dbg,64,"external latch mode enabled\n\r");
        LPC_SC->DMAREQSEL = 0x3; //0xC //e. external latch delay timer is source for DMA request
        LPC_UART1->FCR &= ~0x08;  					//e. TX FIFO is not source for DMA request

        //LPC_GPIOINT->IO0IntEnR |= 0x0000800;	//e. enable rising edge interrupt
    }
    else //e. internal latch mode enabled
    {
        LPC_TIM0->MR0 = EXT_LATCH_UART_DELAY;
        //LPC_TIM0->MR0 = Device_blk.Str.My_Addres*10;	
        //LPC_TIM0->MR1 = Device_blk.Str.My_Addres*5000; //e. /10 = delay before enable signal (us)
        
        LPC_SC->DMAREQSEL = 0;//0x3; //e. FIFO generate DMA request
        LPC_UART1->FCR |= 0x08;
        //	LPC_SC->EXTINT = 0x8; //e. clean interrupt request
    }
    //UART_SwitchSpeed(SRgR & 0x0030);
    UART_SwitchSpeed(g_gld.cmd.trm_rate);
   
    switch(Device_Mode){
    case DM_INT_10KHZ_LATCH:
        //внутренняя Защелка
        if(g_gld.cmd.trm_cycl==1){
            SetIntLatch(g_gld.internal_latch.work_period);
        }
        break;
    case DM_INT_SIGN_MEANDER_LATCH:
        if(g_gld.cmd.trm_cycl==1){
            g_gld.internal_latch.work_period = 50000;
            SetIntLatch(g_gld.internal_latch.work_period);
        }
        break;
    
    case DM_EXT_LATCH_DELTA_PS_LINE:
    case DM_EXT_LATCH_DELTA_BINS_LINE:
        //по запросу UART
        break;
    
    case DM_EXT_LATCH_DELTA_PS_PULSE:
    case DM_EXT_LATCH_DELTA_BINS_PULSE:
        //от внешней Защелки
        //if(g_gld.cmd.trm_cycl==0){
#ifdef HOST4
        LPC_GPIOINT->IO0IntEnR |= (1<<1);
        LPC_GPIOINT->IO0IntClr |= (1<<1); //e. clean external latch interrupt request
#else
        LPC_GPIOINT->IO0IntEnR |= (1<<8);
        LPC_GPIOINT->IO0IntClr |= (1<<8); //e. clean external latch interrupt request		
#endif
        //}else{
        //SetIntLatch(50000);
        //}
        break;
    }
   //DBG2(&g_gld.cmd.dbg.ring_out,dbg,64,"SwitchMode:%d sp:%d\n\r",Device_Mode,g_gld.cmd.trm_rate);
   
   
   return 1;
}

 /******************************************************************************
** Function name:		ServiceTime
**
** Descriptions:		Routine for pseudoseconds counting
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
 void ServiceTime(void)
{
    if(g_gld.time_1_Sec<PrevPeriod){
        g_gld.time_1_Sec = 0;
    }else{
        g_gld.time_1_Sec -= PrevPeriod;
    }

    //1 sec elapsed with accurate 0.5 of main period
    if (g_gld.time_1_Sec < (PrevPeriod>>1)){
        g_gld.time_1_Sec = DEVICE_SAMPLE_RATE_uks;
        g_gld.time_Seconds++;
	}

	Sys_Clock++; //e. increment of the system clock register 
   
    PrevPeriod = LPC_PWM1->MR0;

} // ServiceTime
 void TimeFunctions(void){
	 
 }
/******************************************************************************
** Function name:		WDTFeed
**
** Descriptions:		Feed watchdog timer to prevent it from timeout
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void WDTFeed( void )
{
  LPC_WDT->FEED = 0xAA;		/* Feeding sequence */
  LPC_WDT->FEED = 0x55;
  return;
}

/******************************************************************************
** Function name:		CounterIquiryCycle_Init
**
** Descriptions:		CounterIquiryCycle_Init setup demanded GPIOs for PWM1,
**						reset counter, all latches are enabled, interrupt
**						on PWMMR0, install PWM interrupt to the VIC table.
**
** parameters:			
** Returned value:		
** 
******************************************************************************/
void CounterIquiryCycle_Init(uint32_t cycle)
{ 			
	LPC_SC->PCLKSEL0 &= ~(3<<12); //PWM1 enabled after reset by default	
 	LPC_SC->PCLKSEL0 |=  (1<<12);//PWM1 is synchronized by CCLK (100 MHz)
  
	LPC_PWM1->TCR = TCR_RESET;	//Counter Reset
	LPC_PWM1->MCR = PWMMR0I |PWMMR0R; //generate interrupt and reset timer counter
	
	LPC_PWM1->CCR = 0x1; //interrupt from match0 is enabled only	 					   
		
    PrevPeriod = DEVICE_SAMPLE_RATE_HZ;
    //PrevPeriod = cycle;

	LPC_PWM1->MR0 = PrevPeriod;	//set PWM cycle

	LPC_PWM1->PCR = PWMENA1;  			//single edge control mode, PWM1 out enabled only
	LPC_PWM1->LER = LER0_EN ;  			//enable updating of register
	LPC_PWM1->TCR = TCR_CNT_EN | TCR_PWM_EN;	//counter enable, PWM enable

  	NVIC_DisableIRQ(PWM1_IRQn);
  return ;
}

/******************************************************************************/
__irq void TIMER0_IRQHandler()
{
    static int toggle= 1;
    int val = LPC_TIM0->IR;
    
    LPC_TIM0->IR = 3;
    
    /**
    bit
    0 MR0 Interrupt Interrupt flag for match channel 0. 0
    1 MR1 Interrupt Interrupt flag for match channel 1. 0
    2 MR2 Interrupt Interrupt flag for match channel 2. 0
    3 MR3 Interrupt Interrupt flag for match channel 3. 0
    4 CR0 Interrupt Interrupt flag for capture channel 0 event. 0
    5 CR1 Interrupt Interrupt flag for capture channel 1 event. 0
    */
    g_gld.dbg_buffers.counters_latch++;
    
    //e. delay before UART transmitter loading
    if( (val>>0)&1 ){
        LPC_TIM0->IR = 1;
        LPC_GPDMACH1->CConfig |=  DMAChannelEn; //e. DMA for UART transmition
        LPC_GPDMACH2->CConfig |=  DMAChannelEn; 
        if(toggle){
            hardware_lightup_on();
            toggle = 0;
        }else{
            hardware_lightup_off();
            toggle = 1;
        }
        return;
    }
    
    //e. delay before UART transmitter start
    if( (val>>1)&1 ){
        LPC_TIM0->IR = 2;
        //hardware_lightup_off();
        return;
    }
    //LPC_GPIO2->FIOSET |= 1<<6;		// turn on the LED 	
	//LPC_GPIO2->FIOCLR |= 1<<6;		// turn on the LED 		 
    return;
}
/******************************************************************************
** Function name:		ExtLatch_IRQHandler
**
** Descriptions:		Routine for external latch appearence processing
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
 __irq void EINT3_IRQHandler(void) 
{
#ifdef HOST4
	LPC_GPIOINT->IO0IntClr =  (1<<1);	//e. clean external latch interrupt request
#else
	LPC_GPIOINT->IO0IntClr =  (1<<8);	//e. clean external latch interrupt request	
#endif	
    LatchPhase = (int)LPC_PWM1->TC; //e. read moment of latch
	LPC_TIM0->IR = 0x3F;				 //e. clear all interrupt flags 
    LPC_TIM0->TCR = 1; //e. start and reset the multidrop delay timer
    //LPC_GPIOINT->IO0IntClr |= 0x0000800; //e. clean interrupt request
    latch_num = Sys_Clock;
    
    //g_gld.dbg_buffers.counters_latch++;
}
/******************************************************************************
** Function name:		ExtLatch_Init
**
** Descriptions:		Initialization of external latch 
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void ExtLatch_Init() 
{
	//LPC_PINCON->PINSEL4 &= ~0xC000000;		//e. select P2.13 as general purpose (not EINT3)
    /*
    LPC_PINCON->PINSEL0 &= ~0x0C00000;		//e. select P0.11 as general purpose
	LPC_GPIO0->FIODIR   &= ~0x0000800;		//e. select P0.11 as input	
	LPC_GPIOINT->IO0IntEnR &= ~0x0000800;	//e. disable external latch
	LPC_GPIOINT->IO0IntClr |=  0x0000800;	//e. clean external latch interrupt request
    */
    NVIC_DisableIRQ(TIMER0_IRQn);	
    NVIC_DisableIRQ(EINT3_IRQn);
    /**/

#ifdef HOST4    // GPIO P0.1 as input.
    LPC_PINCON->PINSEL0 &= ~(0x3 << 2);		
    LPC_PINCON->PINMODE0 &= ~(0x3 << 2);
    LPC_PINCON->PINMODE0 |= (0x3 << 2);
	LPC_GPIO0->FIODIR   &= ~(1<<1);		//e. select as input	
	LPC_GPIOINT->IO0IntEnR &= ~(1<<1);	//e. disable external latch
	LPC_GPIOINT->IO0IntClr |= (1<<1);	//e. clean external latch interrupt request
#else
    LPC_PINCON->PINSEL0 &= ~(0x3 << 16);		
    LPC_PINCON->PINMODE0 &= ~(0x3 << 16);
    LPC_PINCON->PINMODE0 |= (0x3 << 16);
    LPC_GPIO0->FIODIR   &= ~(1<<8);		//e. select as input	
    LPC_GPIOINT->IO0IntEnR &= ~(1<<8); //7	//e. disable external latch
    LPC_GPIOINT->IO0IntClr |= (1<<8);	//e. clean external latch interrupt request
#endif	
    //LPC_SC->EXTINT = 1<<3;
    /**/
	NVIC_EnableIRQ(EINT3_IRQn);	

    // initialization of timer for multidrop delay generation
    LPC_SC->PCONP |= 1 << 1; // Power up Timer 0
    
	LPC_SC->PCLKSEL0 &= ~(3<<2);		 //e. reset timer 0 input frequency 
    LPC_SC->PCLKSEL0 |= (3<<2);		 	 //e. timer 0 input frequency equal to CLCK/8
	LPC_TIM0->PR = 1;					 //e. set timer 0 prescaler to 0
	LPC_TIM0->IR = 0x3F;				 //e. clear all interrupt flags 
    //Interrupt on MR0: an interrupt is generated when MR0 matches the value in the TC.
    //Interrupt on MR1: an interrupt is generated when MR1 matches the value in the TC
    //Reset on MR1: the TC will be reset if MR1 matches it
    //Stop on MR1: the TC and PC will be stopped and TCR[0] will be set to 0 if MR1 matches the TC.
	LPC_TIM0->MCR = (1<<0) | (1<<1) | (1<<2) ; //e. reset and stop timer after MR0 matches TC
	LPC_TIM0->CCR = 0; 			 		 //e. content of TC0 is loaded when rising edge of ext. latch appear
	LPC_TIM0->CTCR = 0; 				 //e. timer1 is in timer mode
    
	LPC_TIM0->MR0 = EXT_LATCH_UART_DELAY;	 		//e. delay before UART transmitter loading
	//LPC_TIM0->MR1 = Device_blk.Str.My_Addres*5000;		//e. delay before UART transmitter start set first empty) event of timer
    
    //NVIC_DisableIRQ(TIMER0_IRQn);	
	NVIC_EnableIRQ(TIMER0_IRQn);		 
}

 /******************************************************************************
** Function name:		IntLatch_IRQHandler
**
** Descriptions:		Routine for Internal latch appearence processing
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
__irq void IntLatch_IRQHandler (void) 
{
    LatchPhase =(int)LPC_PWM1->TC;			//e. read moment of latch
    LPC_TIM3->IR = 0x0001;				//e. clear interrupt flag 
    latch_num = Sys_Clock;
    //	count++;
    
    //g_gld.dbg_buffers.counters_latch++;
}
 
/******************************************************************************
** Function name:		IntLatch_Init
**
** Descriptions:		Initialization of iternal latch cycle generation
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void IntLatch_Init() 
{
    LPC_SC->PCONP |= (1<<23);  //enable TIMER3
	//e. by default timer3 mode is not counter (T3CTCR[1,0]=0) 
    LPC_SC->PCLKSEL1 &= SYNC_CLCK4;		 //e. timer 3 input frequency equal to CLCK/4
	LPC_SC->PCLKSEL1 |= (1<<14);		 //e. timer 3 input frequency equal to CLCK/4
//	LPC_PINCON->PINSEL1 |= (3<<16);	      //e. CAP 3.1 is connected to P0.24
	//e. by default CAP3.0 is connected to timer3
	LPC_TIM3->PR = 0;					 //e. set timer 3 prescaler to 0
	LPC_TIM3->IR = 0x0001;				 //e. clear interrupt flag 
	LPC_TIM3->MCR = MR0_RESET |MR0_INT_EN |MR0_NO_STOP;			 
	LPC_TIM3->CCR = 0x0001; 			//e. content of TC3 is loaded when rising edge of ext. latch appear 
	
	NVIC_EnableIRQ(TIMER3_IRQn);	
}
/*****************************************************************************
** Function name:		WDTInit
**
** Descriptions:		Initialize watchdog timer, install the
**						watchdog timer interrupt handler
**
** parameters:			None
** Returned value:		true or false, return false if the VIC table
**				is full and WDT interrupt handler can be
**				installed.
** 
*****************************************************************************/
uint32_t WDTInit( void )
{
  NVIC_DisableIRQ(WDT_IRQn);

  LPC_WDT->TC = WDT_FEED_VALUE;	// once WDEN is set, the WDT will start after feeding 
  LPC_WDT->MOD = WDEN;

  LPC_WDT->FEED = 0xAA;		// Feeding sequence
  LPC_WDT->FEED = 0x55;  
    
  return 1;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
