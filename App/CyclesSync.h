/**--------------File Info---------------------------------------------------------------------------------
** File name:           CycleSync.h
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
#include "lpc17xx.h"
#include "CntrlGLD.h"

#define DELAY_UART_ENBL				4000			//e. delay = DELAY_UART_ENBL*8/CLCK
#define DELAY_UART_DISBL			2500			//e. delay = DELAY_UART_ENBL*8/CLCK

#define HALF_PERIOD 0x00000004
#define WHOLE_PERIOD 0x00000008
#define RESET_PERIOD 0x0000000C
//-----------------------------PWM Registers----------------------------------------

#define TCR_CNT_EN		0x00000001
#define TCR_RESET		0x00000002
#define TCR_PWM_EN		0x00000008

#define PWMMR0I			(1 << 0)
#define PWMMR0R			(1 << 1)
#define PWMMR0S			(1 << 2)
#define PWMENA1			(1 << 9)
#define LER0_EN			(1 << 0)

//-----------------------Drive cycle registers------------------------------------
#define MR0_RESET		0x00000002
#define MR1_RESET		0x00000010
#define MR1_STOP		0x00000020
#define MR0_STOP		0x00000004
#define MR0_NO_STOP		0x00000000
#define MR0_INT_EN		0x00000001
#define SYNC_CLCK4		0xffff3fff
#define SYNC_CLCK		0xfffffff

//------------------------WDT registers--------------------------------------------
#define WDEN		(0x1<<0)
#define WDRESET		(0x1<<1)
#define WDTOF		(0x1<<2)
#define WDINT		(0x1<<3)
#define WDT_FEED_VALUE	0x003FFFFF

extern uint32_t		WDTInit( void );
extern void 		WDTFeed( void );

extern void CounterIquiryCycle_Init(uint32_t);
extern void ExtLatch_Init(void);
extern void IntLatch_Init(void);
extern void Latch_Event(void);
extern void SetIntLatch(uint32_t);
extern void SwitchRefMeandInt(uint32_t);
extern int SwitchMode(void);

#if defined PERFOMANCE
	void IntLatch(void);
#endif

void ServiceTime(void);

/*****************************************************************************
**                            End Of File
******************************************************************************/
