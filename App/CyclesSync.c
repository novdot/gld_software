
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

uint32_t	num;


uint32_t	Delay_UART_Enbl = DELAY_UART_ENBL;
//uint32_t	Delay_UART_Disbl = DELAY_UART_ENBL;



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
  static unsigned PreLatch = 0;
//  static int cc = 0;
	  if (LatchPhase < INT32_MAX)  //e. latch is present
	    {	
		  Latch_Rdy = 1;		   //e. set the flag for processing below
		   if (g_gld.RgConB.word)			   //e. work whith vibro counters
			{
		  	  if (PreLatch)		   //e. we have had delayed latch
		     	  PreLatch = 0;		 	  				
		  	  else if ((LatchPhase < LPC_PWM1->TC) && (num == Sys_Clock)) //e. latch have appeared in current cycle
		  		{			
		  			Latch_Rdy = 0;	//e. bring it to the next cycle	
					PreLatch = 1;
		  		}						  			
			} 
		}
	  else
		Latch_Rdy = 0;				//e. latch is absent
 //---------------------------temp-------------------------------
 /*	Latch_Rdy = 0;				//e. latch is absent  
	if (cc++ == 19)
	{
	  cc = 0;
	  Latch_Rdy = 1; 
	  LatchPhase = 2500; 
	} 	*/
	//----------------------temp--------------------------------
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
 __irq void QEI_IRQHandler (void) 
 {
   static uint32_t halfQEIPeriod = 0;

	 g_gld.Cnt_curr =  LPC_QEI->POS;		//e. read accumulated value of counter
	 if (LPC_QEI->INTSTAT & 0x0008)
	 {
//-----------debug-------------------------
/*if (LPC_QEI->STAT & 1)//(halfPeriod2 & 0x0001)//	if (LPC_GPIO0->FIOPIN & (1<<4))
{	LPC_GPIO2->FIOSET = 0x10;

	 // 
//	LPC_GPIO0->FIOSET = (1<<4);	//reset light up signal
	}
	 else
{	 LPC_GPIO2->FIOCLR = 0x10;	 

	 //
	//	LPC_GPIO0->FIOCLR = (1<<4);	//set light up signal
		}						 */
//-----------debug-------------------------

	   data_Rdy = 0x0004;			//e. data for Cnt_Pls or Cnt_Mns calculation are ready

	  if (++halfQEIPeriod & 0x0001)	//e. period elapsed, we can calculate Cnt_Dif
	  {
	  // Set_LightUp;
	   data_Rdy = 0x000C;	
	   }
	//   else
	   //Reset_LightUp;
	}
	  LPC_QEI->CLR = 0x1fff;			//e. reset interrupt request //r. сбросить запрос прерывания
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
 LPC_TIM3->TCR = 0x2;							//switch off and reset timer3
 if (cycle != 0)
 {
 	LPC_TIM3->MR0 = (cycle<<2);						//load new value
 	LPC_TIM3->TCR = 1;							//switch on timer3
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
  LPC_QEI->CLR = 0x1fff; 			//e. reset all interrupts //r. сбросить все прерывания
 if (s)
  LPC_QEI->IEC = 0x1fff;			//e.  disable direction changing interrupt //r. запретить прерывание при изменении направления
 else
  LPC_QEI->IES = 0x0008;			//e.  enable direction changing interrupt //r. разрешить прерывание при изменении направления
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
 __irq void EINT3_IRQHandler (void) 
 {
 //LPC_GPIO2->FIOSET = 0x00000020;		//e. turn on the LED 
 	LatchPhase = LPC_PWM1->TC;			//e. read moment of latch
	LPC_TIM0->TCR = 1;					//e. start Mltdrop delay timer
	LPC_GPIOINT->IO0IntClr |= 0x0000800;//e. clean interrupt request
 //LPC_GPIO2->FIOCLR = 0x00000020;		//e. turn off the LED 
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
    num = Sys_Clock;
    //	count++;
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
    //disable latch sources
    SetIntLatch(0); 	   					//e. disable internal latch
    LPC_TIM3->IR = 0x0001;				//e. clear internal latch interrupt request

    LPC_GPIOINT->IO0IntEnR &= ~0x0000800;	//e. disable external latch
    LPC_GPIOINT->IO0IntClr |=  0x0000800;	//e. clean external latch interrupt request

    LPC_TIM0->TCR = 2;						//e. stop and reset the multidrop delay timer
    LPC_TIM0->IR = 0x03F;				//e. clear internal latch interrupt request
    //wait while UART and DMA are active									 	
	if ( LPC_GPDMACH1->CConfig & DMA_BUSY)				//e. if DMA channel is busy, wait //r. если канал передачи занят, ждать
	  return 0;	
	LPC_GPDMACH1->CConfig &=  ~DMAChannelEn;			//e. disable DMA for UART transmition
	LPC_GPDMACH2->CConfig &=  ~DMAChannelEn;

	if (!(LPC_UART1->LSR & TRANS_SHIFT_BUF_EMPTY))      //e. transmit buffer is not empty //r. передающий буфер не пуст
	 return 0;
	///LPC_UART1->FCR |= 0x4;								//e. reset TX FIFO

	LPC_TIM0->IR = 0x3F;				 		//e. clear all interrupt flags 
//---------------------configure a new exchanging parameters------------
   if (Device_Mode > 3)		   					//e. external latch mode enabled
   {
    LPC_TIM0->MR0 = 10;	
	LPC_TIM0->MR1 = 50;						//e. /10 = delay before enable signal (us)
 //   LPC_UART1->FCR &= ~0x08;  					//e. TX FIFO is not source for DMA request

// 	LPC_SC->DMAREQSEL = 0xC;						//e. external latch delay timer is source for DMA request
//	LPC_GPIOINT->IO0IntEnR |= 0x0000800;	//e. enable rising edge interrupt
   }
   else						   				//e. internal latch mode enabled
   {
   	LPC_TIM0->MR0 = 10;	
	LPC_TIM0->MR1 = 5000;						//e. /10 = delay before enable signal (us)
//	LPC_SC->DMAREQSEL = 0x3;   				//e. FIFO generate DMA request

//	LPC_SC->EXTINT = 0x8;					//e. clean interrupt request
   }
 
   UART_SwitchSpeed(SRgR & 0x0030);

   if (Device_Mode == DM_INT_LATCH_DELTA_PS)
   	 SetIntLatch(50000);
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
    static x_uint8_t isStarted = 0;
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
	if ( (g_gld.time_Seconds==3) && (isStarted==0)) { 
        isStarted = 1; 
        close_all_loops();
    }	
	Sys_Clock++; //e. increment of the system clock register 
   
    PrevPeriod = LPC_PWM1->MR0;
#if defined PERFOMANCE
	PrevPeriod = DEVICE_SAMPLE_RATE_HZ;
#endif
} // ServiceTime

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
  int val = LPC_TIM0->IR;
  LPC_TIM0->IR |= 3;

  if (val & 1)	//MAT 1.0 interrupt
  {
    LPC_GPIO2->FIOSET |= 1<<6;		// turn on the LED 	
	LPC_TIM0->IR |= 1;
	 return;
  }
  if (val & 2)	 //MAT 1.1 interrupt
  {
	LPC_GPIO2->FIOCLR |= 1<<6;		// turn on the LED 	
	LPC_TIM0->IR |= 2;
	 return;
  }
  //MAT 0.2 interrupt
  if (val & 4) {
    //LPC_GPIO0->FIOSET2 |= 0xFF;				//set P0.23
    //LPC_GPIO2->FIOSET |= 0x00000040;		// turn on the LED 	
    //LPC_TIM0->IR |= 4; 
	return;
  }			 
 return;
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
	LPC_PINCON->PINSEL4 &= ~0xC000000;		//e. select P2.13 as general purpose (not EINT3)
    LPC_PINCON->PINSEL0 &= ~0x0C00000;		//e. select P0.11 as general purpose
	LPC_GPIO0->FIODIR   &= ~0x0000800;		//e. select P0.11 as input	
	LPC_GPIOINT->IO0IntEnR &= ~0x0000800;	//e. disable external latch
	LPC_GPIOINT->IO0IntClr |=  0x0000800;	//e. clean external latch interrupt request

	NVIC_EnableIRQ(EINT3_IRQn);	

//+++++++ initialization of timer for multidrop delay generation+++++++++++++++++++++++
										 //e.  TIMER0 enabled by default   
	LPC_SC->PCLKSEL0 &= ~(3<<2);		 //e. reset timer 0 input frequency 
    LPC_SC->PCLKSEL0 |= (3<<2);		 	 //e. timer 0 input frequency equal to CLCK/8
	LPC_TIM0->PR = 0;					 //e. set timer 0 prescaler to 0
	LPC_TIM0->IR = 0x3F;				 //e. clear all interrupt flags 
	LPC_TIM0->MCR = 1 |(1<<3)|MR1_RESET |MR1_STOP; //e. reset and stop timer after MR1 matches TC
	LPC_TIM0->CCR = 0; 			 		 //e. content of TC0 is loaded when rising edge of ext. latch appear
	LPC_TIM0->CTCR = 0; 				 //e. timer1 is in timer mode
	
	LPC_TIM0->MR0 = /*Device_blk.Address**/10;	 		//e. delay before UART transmitter loading
	LPC_TIM0->MR1 = /*Device_blk.Address**/5000;		//e. delay before UART transmitter start
									//e. set first empty) event of timer
	NVIC_DisableIRQ(TIMER0_IRQn);		 
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
