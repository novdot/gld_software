/**************************************************************************//**
 * @file     system_LPC17xx.c
 * @brief    CMSIS Cortex-M3 Device Peripheral Access Layer Source File
 *           for the NXP LPC17xx Device Series
 * @version  V1.03
 * @date     07. October 2009
 *
 * @note
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#include <stdint.h>
#include "LPC17xx.h"

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
*/

/*--------------------- Clock Configuration ----------------------------------
//
// <e> Clock Configuration
//   <h> System Controls and Status Register (SCS)
//     <o1.4>    OSCRANGE: Main Oscillator Range Select
//                     <0=>  1 MHz to 20 MHz
//                     <1=> 15 MHz to 24 MHz
//     <e1.5>       OSCEN: Main Oscillator Enable
//     </e>
//   </h>
//
//   <h> Clock Source Select Register (CLKSRCSEL)
//     <o2.0..1>   CLKSRC: PLL Clock Source Selection
//                     <0=> Internal RC oscillator
//                     <1=> Main oscillator
//                     <2=> RTC oscillator
//   </h>
//
//   <e3> PLL0 Configuration (Main PLL)
//     <h> PLL0 Configuration Register (PLL0CFG)
//                     <i> F_cco0 = (2 * M * F_in) / N
//                     <i> F_in must be in the range of 32 kHz to 50 MHz
//                     <i> F_cco0 must be in the range of 275 MHz to 550 MHz
//       <o4.0..14>  MSEL: PLL Multiplier Selection
//                     <6-32768><#-1>
//                     <i> M Value
//       <o4.16..23> NSEL: PLL Divider Selection
//                     <1-256><#-1>
//                     <i> N Value
//     </h>
//   </e>
//
//   <e5> PLL1 Configuration (USB PLL)
//     <h> PLL1 Configuration Register (PLL1CFG)
//                     <i> F_usb = M * F_osc or F_usb = F_cco1 / (2 * P)
//                     <i> F_cco1 = F_osc * M * 2 * P
//                     <i> F_cco1 must be in the range of 156 MHz to 320 MHz
//       <o6.0..4>   MSEL: PLL Multiplier Selection
//                     <1-32><#-1>
//                     <i> M Value (for USB maximum value is 4)
//       <o6.5..6>   PSEL: PLL Divider Selection
//                     <0=> 1
//                     <1=> 2
//                     <2=> 4
//                     <3=> 8
//                     <i> P Value
//     </h>
//   </e>
//
//   <h> CPU Clock Configuration Register (CCLKCFG)
//     <o7.0..7>  CCLKSEL: Divide Value for CPU Clock from PLL0
//                     <3-256><#-1>
//   </h>
//
//   <h> USB Clock Configuration Register (USBCLKCFG)
//     <o8.0..3>   USBSEL: Divide Value for USB Clock from PLL0
//                     <0-15>
//                     <i> Divide is USBSEL + 1
//   </h>
//
//   <h> Peripheral Clock Selection Register 0 (PCLKSEL0)
//     <o9.0..1>    PCLK_WDT: Peripheral Clock Selection for WDT
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o9.2..3>    PCLK_TIMER0: Peripheral Clock Selection for TIMER0
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o9.4..5>    PCLK_TIMER1: Peripheral Clock Selection for TIMER1
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o9.6..7>    PCLK_UART0: Peripheral Clock Selection for UART0
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o9.8..9>    PCLK_UART1: Peripheral Clock Selection for UART1
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o9.12..13>  PCLK_PWM1: Peripheral Clock Selection for PWM1
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o9.14..15>  PCLK_I2C0: Peripheral Clock Selection for I2C0
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o9.16..17>  PCLK_SPI: Peripheral Clock Selection for SPI
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o9.20..21>  PCLK_SSP1: Peripheral Clock Selection for SSP1
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o9.22..23>  PCLK_DAC: Peripheral Clock Selection for DAC
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o9.24..25>  PCLK_ADC: Peripheral Clock Selection for ADC
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o9.26..27>  PCLK_CAN1: Peripheral Clock Selection for CAN1
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 6
//     <o9.28..29>  PCLK_CAN2: Peripheral Clock Selection for CAN2
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 6
//     <o9.30..31>  PCLK_ACF: Peripheral Clock Selection for ACF
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 6
//   </h>
//
//   <h> Peripheral Clock Selection Register 1 (PCLKSEL1)
//     <o10.0..1>   PCLK_QEI: Peripheral Clock Selection for the Quadrature Encoder Interface
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o10.2..3>   PCLK_GPIO: Peripheral Clock Selection for GPIOs
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o10.4..5>   PCLK_PCB: Peripheral Clock Selection for the Pin Connect Block
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o10.6..7>   PCLK_I2C1: Peripheral Clock Selection for I2C1
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o10.10..11> PCLK_SSP0: Peripheral Clock Selection for SSP0
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o10.12..13> PCLK_TIMER2: Peripheral Clock Selection for TIMER2
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o10.14..15> PCLK_TIMER3: Peripheral Clock Selection for TIMER3
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o10.16..17> PCLK_UART2: Peripheral Clock Selection for UART2
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o10.18..19> PCLK_UART3: Peripheral Clock Selection for UART3
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o10.20..21> PCLK_I2C2: Peripheral Clock Selection for I2C2
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o10.22..23> PCLK_I2S: Peripheral Clock Selection for I2S
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o10.26..27> PCLK_RIT: Peripheral Clock Selection for the Repetitive Interrupt Timer
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o10.28..29> PCLK_SYSCON: Peripheral Clock Selection for the System Control Block
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//     <o10.30..31> PCLK_MC: Peripheral Clock Selection for the Motor Control PWM
//                     <0=> Pclk = Cclk / 4
//                     <1=> Pclk = Cclk
//                     <2=> Pclk = Cclk / 2
//                     <3=> Pclk = Hclk / 8
//   </h>
//
//   <h> Power Control for Peripherals Register (PCONP)
//     <o11.1>      PCTIM0: Timer/Counter 0 power/clock enable
//     <o11.2>      PCTIM1: Timer/Counter 1 power/clock enable
//     <o11.3>      PCUART0: UART 0 power/clock enable
//     <o11.4>      PCUART1: UART 1 power/clock enable
//     <o11.6>      PCPWM1: PWM 1 power/clock enable
//     <o11.7>      PCI2C0: I2C interface 0 power/clock enable
//     <o11.8>      PCSPI: SPI interface power/clock enable
//     <o11.9>      PCRTC: RTC power/clock enable
//     <o11.10>     PCSSP1: SSP interface 1 power/clock enable
//     <o11.12>     PCAD: A/D converter power/clock enable
//     <o11.13>     PCCAN1: CAN controller 1 power/clock enable
//     <o11.14>     PCCAN2: CAN controller 2 power/clock enable
//     <o11.15>     PCGPIO: GPIOs power/clock enable
//     <o11.16>     PCRIT: Repetitive interrupt timer power/clock enable
//     <o11.17>     PCMC: Motor control PWM power/clock enable
//     <o11.18>     PCQEI: Quadrature encoder interface power/clock enable
//     <o11.19>     PCI2C1: I2C interface 1 power/clock enable
//     <o11.21>     PCSSP0: SSP interface 0 power/clock enable
//     <o11.22>     PCTIM2: Timer 2 power/clock enable
//     <o11.23>     PCTIM3: Timer 3 power/clock enable
//     <o11.24>     PCUART2: UART 2 power/clock enable
//     <o11.25>     PCUART3: UART 3 power/clock enable
//     <o11.26>     PCI2C2: I2C interface 2 power/clock enable
//     <o11.27>     PCI2S: I2S interface power/clock enable
//     <o11.29>     PCGPDMA: GP DMA function power/clock enable
//     <o11.30>     PCENET: Ethernet block power/clock enable
//     <o11.31>     PCUSB: USB interface power/clock enable
//   </h>
//
//   <h> Clock Output Configuration Register (CLKOUTCFG)
//     <o12.0..3>   CLKOUTSEL: Selects clock source for CLKOUT
//                     <0=> CPU clock
//                     <1=> Main oscillator
//                     <2=> Internal RC oscillator
//                     <3=> USB clock
//                     <4=> RTC oscillator
//     <o12.4..7>   CLKOUTDIV: Selects clock divider for CLKOUT
//                     <1-16><#-1>
//     <o12.8>      CLKOUT_EN: CLKOUT enable control
//   </h>
//
// </e>
*/
#define CLOCK_SETUP           1
//System control - system control and status register:
//  bit 4 - main oscillator range:
//    0 - 1...20MHz
//    1 - 15...25MHz
//  bit 5 - main oscillator enable
//    0 - disabled
//    1 - enabled
//  bit 6 - main oscillator status
//    0 - not ready
//    1 - ready
#define SCS_Val               0x00000020
//Clock Source Select register
//  bits 0-1:
//    00 - Selects the Internal RC oscillator as the PLL0 clock source (default)
//    01 - Selects the main oscillator as the PLL0 clock source
//    10 - Selects the RTC oscillator as the PLL0 clock source
//    11 - Reserved, do not use this setting
#define CLKSRCSEL_Val         0x00000001
#define PLL0_SETUP            1
//PLL0 Configuration register
//  bits 0...14 - PLL0 multiplier value minus 1. Supported multiplier M range 6...512
//  bits 16...23 - PLL0 Pre-Divider value minus 1. Supported divider N range 1...32
//  Fcc0 = (2 * M * Fin) / N
//#define PLL0CFG_Val           0x00050063 
#define PLL0CFG_Val           0x00040055 //M - 86, N - 5, output = 2 * 86 * 12MHz / 5 = 400MHz
//#define PLL0CFG_Val           0x0003003d //M - 86, N - 5, output = 2 * 86 * 12MHz / 5 = 400MHz
#define PLL1_SETUP            1
#define PLL1CFG_Val           0x00000023 //M - 36, N - 1, output = 2 * 36 * 12MHz / 1 = 864MHz?
//CPU Clock Configure Register
#define CCLKCFG_Val           0x00000003 //Divide by 4

//USB Clock Configuration register
//  bits 0...3
//    5 - PLL0 output is divided by 6. PLL0 output must be 288 MHz
//    7 - PLL0 output is divided by 8. PLL0 output must be 384 MHz
//    9 - PLL0 output is divided by 10. PLL0 output must be 480 MHz
#define USBCLKCFG_Val         0x00000000//default
//Peripheral Clock Selection register 0
//  1:0 PCLK_WDT Peripheral clock selection for WDT. 00
//  3:2 PCLK_TIMER0 Peripheral clock selection for TIMER0. 00
//  5:4 PCLK_TIMER1 Peripheral clock selection for TIMER1. 00
//  7:6 PCLK_UART0 Peripheral clock selection for UART0. 00
//  9:8 PCLK_UART1 Peripheral clock selection for UART1. 00
//  11:10 - Reserved. NA
//  13:12 PCLK_PWM1 Peripheral clock selection for PWM1. 00
//  15:14 PCLK_I2C0 Peripheral clock selection for I2C0. 00
//  17:16 PCLK_SPI Peripheral clock selection for SPI. 00
//  19:18 - Reserved. NA
//  21:20 PCLK_SSP1 Peripheral clock selection for SSP1. 00
//  23:22 PCLK_DAC Peripheral clock selection for DAC. 00
//  25:24 PCLK_ADC Peripheral clock selection for ADC. 00
//  27:26 PCLK_CAN1 Peripheral clock selection for CAN1.[1] 00
//  29:28 PCLK_CAN2 Peripheral clock selection for CAN2.[1] 00
//  31:30 PCLK_ACF Peripheral clock selection for CAN acceptance filtering
//  bits values:
//  00 PCLK_peripheral = CCLK/4
//  01 PCLK_peripheral = CCLK
//  10 PCLK_peripheral = CCLK/2
//  11 PCLK_peripheral = CCLK/8, except for CAN1, CAN2, and CAN filtering when ӱ1Ԡselects = CCLK/6.
//#define PCLKSEL0_Val          0x00000010//Peripheral clock selection for TIMER1 - CCLK, other peripherals - CCLK/4
  #define PCLKSEL0_Val          0x40000150//Peripheral clock selection for TIMER1 - CCLK, other peripherals - CCLK/4
//#define PCLKSEL0_Val          0x000003d0//Peripheral clock selection for TIMER1 - CCLK, other peripherals - CCLK/4
//Peripheral Clock Selection register 1
//  1:0 PCLK_QEI Peripheral clock selection for the Quadrature Encoder Interface.00
//  3:2 PCLK_GPIOINT Peripheral clock selection for GPIO interrupts. 00
//  5:4 PCLK_PCB Peripheral clock selection for the Pin Connect block. 00
//  7:6 PCLK_I2C1 Peripheral clock selection for I2C1. 00
//  9:8 - Reserved. NA
//  11:10 PCLK_SSP0 Peripheral clock selection for SSP0. 00
//  13:12 PCLK_TIMER2 Peripheral clock selection for TIMER2. 00
//  15:14 PCLK_TIMER3 Peripheral clock selection for TIMER3. 00
//  17:16 PCLK_UART2 Peripheral clock selection for UART2. 00
//  19:18 PCLK_UART3 Peripheral clock selection for UART3. 00
//  21:20 PCLK_I2C2 Peripheral clock selection for I2C2. 00
//  23:22 PCLK_I2S Peripheral clock selection for I2S. 00
//  25:24 - Reserved. NA
//  27:26 PCLK_RIT Peripheral clock selection for Repetitive Interrupt Timer. 00
//  29:28 PCLK_SYSCON Peripheral clock selection for the System Control block. 00
//  31:30 PCLK_MC Peripheral clock selection for the Motor Control PWM
#define PCLKSEL1_Val          0x00000000//CCLK/4
//Power Control for Peripherals register
//0 - Reserved. NA
//1 PCTIM0 Timer/Counter 0 power/clock control bit. 1
//2 PCTIM1 Timer/Counter 1 power/clock control bit. 1
//3 PCUART0 UART0 power/clock control bit. 1
//4 PCUART1 UART1 power/clock control bit. 1
//5 - Reserved. NA
//6 PCPWM1 PWM1 power/clock control bit. 1
//7 PCI2C0 The I2C0 interface power/clock control bit. 1
//8 PCSPI The SPI interface power/clock control bit. 1
//9 PCRTC The RTC power/clock control bit. 1
//10 PCSSP1 The SSP 1 interface power/clock control bit. 1
//11 - Reserved. NA
//12 PCADC A/D converter (ADC) power/clock control bit. Note: Clear the PDN bit in the AD0CR before clearing this bit, and set this bit before setting PDN. 0
//13 PCCAN1 CAN Controller 1 power/clock control bit. 0
//14 PCCAN2 CAN Controller 2 power/clock control bit. 0
//15 PCGPIO Power/clock control bit for IOCON, GPIO, and GPIO interrupts. 1
//16 PCRIT Repetitive Interrupt Timer power/clock control bit. 0
//17 PCMCPWM Motor Control PWM 0
//18 PCQEI Quadrature Encoder Interface power/clock control bit. 0
//19 PCI2C1 The I2C1 interface power/clock control bit. 1
//20 - Reserved. NA
//21 PCSSP0 The SSP0 interface power/clock control bit. 1
//22 PCTIM2 Timer 2 power/clock control bit. 0
//23 PCTIM3 Timer 3 power/clock control bit. 0
//24 PCUART2 UART 2 power/clock control bit. 0
//25 PCUART3 UART 3 power/clock control bit. 0
//26 PCI2C2 I2C interface 2 power/clock control bit. 1
#define PCONP_Val         0x046887DE//ADC,CAN1/2,RIT,Timer3,UART2,UART3 disabled
//Clock Output Configuration register
//  3:0 CLKOUTSEL Selects the clock source for the CLKOUT function. 0
//    0000 Selects the CPU clock as the CLKOUT source.
//    0001 Selects the main oscillator as the CLKOUT source.
//    0010 Selects the Internal RC oscillator as the CLKOUT source.
//    0011 Selects the USB clock as the CLKOUT source.
//    0100 Selects the RTC oscillator as the CLKOUT source.
//    Others Reserved, do not use these settings.
//  7:4 CLKOUTDIV Integer value to divide the output clock by, minus one. 0
//    0000 Clock is divided by 1.
//    0001 Clock is divided by 2.
//    0010 Clock is divided by 3.
//    ... ...
//    1111 Clock is divided by 16.
//  8 CLKOUT_EN CLKOUT enable control, allows switching the CLKOUT source without glitches. Clear to stop CLKOUT on the next falling edge. Set to enable CLKOUT. 0
//  9 CLKOUT_ACT CLKOUT activity indication. Reads as 1 when CLKOUT is enabled. Read as 0 when CLKOUT has been disabled via the CLKOUT_EN bit and the clock has completed being stopped.
#define CLKOUTCFG_Val         0x00000000//Host4: CLKOUT pin not used

/*--------------------- Flash Accelerator Configuration ----------------------
//
// <e> Flash Accelerator Configuration
//   <o1.0..1>   FETCHCFG: Fetch Configuration
//               <0=> Instruction fetches from flash are not buffered
//               <1=> One buffer is used for all instruction fetch buffering
//               <2=> All buffers may be used for instruction fetch buffering
//               <3=> Reserved (do not use this setting)
//   <o1.2..3>   DATACFG: Data Configuration
//               <0=> Data accesses from flash are not buffered
//               <1=> One buffer is used for all data access buffering
//               <2=> All buffers may be used for data access buffering
//               <3=> Reserved (do not use this setting)
//   <o1.4>      ACCEL: Acceleration Enable
//   <o1.5>      PREFEN: Prefetch Enable
//   <o1.6>      PREFOVR: Prefetch Override
//   <o1.12..15> FLASHTIM: Flash Access Time
//               <0=> 1 CPU clock (for CPU clock up to 20 MHz)
//               <1=> 2 CPU clocks (for CPU clock up to 40 MHz)
//               <2=> 3 CPU clocks (for CPU clock up to 60 MHz)
//               <3=> 4 CPU clocks (for CPU clock up to 80 MHz)
//               <4=> 5 CPU clocks (for CPU clock up to 100 MHz)
//               <5=> 6 CPU clocks (for any CPU clock)
// </e>
*/
#define FLASH_SETUP           1
#define FLASHCFG_Val          0x0000303A

/*
//-------- <<< end of configuration section >>> ------------------------------
*/

/*----------------------------------------------------------------------------
  Check the register settings
 *----------------------------------------------------------------------------*/
#define CHECK_RANGE(val, min, max)                ((val < min) || (val > max))
#define CHECK_RSVD(val, mask)                     (val & mask)

/* Clock Configuration -------------------------------------------------------*/
#if (CHECK_RSVD((SCS_Val),       ~0x00000030))
   #error "SCS: Invalid values of reserved bits!"
#endif

#if (CHECK_RANGE((CLKSRCSEL_Val), 0, 2))
   #error "CLKSRCSEL: Value out of range!"
#endif

#if (CHECK_RSVD((PLL0CFG_Val),   ~0x00FF7FFF))
   #error "PLL0CFG: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((PLL1CFG_Val),   ~0x0000007F))
   #error "PLL1CFG: Invalid values of reserved bits!"
#endif

#if ((CCLKCFG_Val != 0) && (((CCLKCFG_Val - 1) % 2)))
   #error "CCLKCFG: CCLKSEL field does not contain only odd values or 0!"
#endif

#if (CHECK_RSVD((USBCLKCFG_Val), ~0x0000000F))
   #error "USBCLKCFG: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((PCLKSEL0_Val),   0x000C0C00))
   #error "PCLKSEL0: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((PCLKSEL1_Val),   0x03000300))
   #error "PCLKSEL1: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((PCONP_Val),      0x10100821))
   #error "PCONP: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((CLKOUTCFG_Val), ~0x000001FF))
   #error "CLKOUTCFG: Invalid values of reserved bits!"
#endif

/* Flash Accelerator Configuration -------------------------------------------*/
#if (CHECK_RSVD((FLASHCFG_Val), ~0x0000F07F))
   #error "FLASHCFG: Invalid values of reserved bits!"
#endif


/*----------------------------------------------------------------------------
  DEFINES
 *----------------------------------------------------------------------------*/
    
/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define XTAL        (12000000UL)        /* Oscillator frequency               */
#define OSC_CLK     (      XTAL)        /* Main oscillator frequency          */
#define RTC_CLK     (   32000UL)        /* RTC oscillator frequency           */
#define IRC_OSC     ( 4000000UL)        /* Internal RC oscillator frequency   */


/* F_cco0 = (2 * M * F_in) / N  */
#define __M               (((PLL0CFG_Val      ) & 0x7FFF) + 1)
#define __N               (((PLL0CFG_Val >> 16) & 0x00FF) + 1)
#define __FCCO(__F_IN)    ((2 * __M * __F_IN) / __N) 
#define __CCLK_DIV        (((CCLKCFG_Val      ) & 0x00FF) + 1)

/* Determine core clock frequency according to settings */
 #if (PLL0_SETUP)
    #if   ((CLKSRCSEL_Val & 0x03) == 1)
        #define __CORE_CLK (__FCCO(OSC_CLK) / __CCLK_DIV)
    #elif ((CLKSRCSEL_Val & 0x03) == 2)
        #define __CORE_CLK (__FCCO(RTC_CLK) / __CCLK_DIV)
    #else 
        #define __CORE_CLK (__FCCO(IRC_OSC) / __CCLK_DIV)
    #endif
 #else
    #if   ((CLKSRCSEL_Val & 0x03) == 1)
        #define __CORE_CLK (OSC_CLK         / __CCLK_DIV)
    #elif ((CLKSRCSEL_Val & 0x03) == 2)
        #define __CORE_CLK (RTC_CLK         / __CCLK_DIV)
    #else
        #define __CORE_CLK (IRC_OSC         / __CCLK_DIV)
    #endif
 #endif


/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = __CORE_CLK;/*!< System Clock Frequency (Core Clock)*/


/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{
  /* Determine clock frequency according to clock register values             */
  if (((LPC_SC->PLL0STAT >> 24) & 3) == 3) { /* If PLL0 enabled and connected */
    switch (LPC_SC->CLKSRCSEL & 0x03) {
      case 0:                                /* Int. RC oscillator => PLL0    */
      case 3:                                /* Reserved, default to Int. RC  */
        SystemCoreClock = (IRC_OSC * 
                          ((2 * ((LPC_SC->PLL0STAT & 0x7FFF) + 1)))  /
                          (((LPC_SC->PLL0STAT >> 16) & 0xFF) + 1)    /
                          ((LPC_SC->CCLKCFG & 0xFF)+ 1));
        break;
      case 1:                                /* Main oscillator => PLL0       */
        SystemCoreClock = (OSC_CLK *         //it is our case osc_clk = 12 MHz
                          ((2 * ((LPC_SC->PLL0STAT & 0x7FFF) + 1)))  /	//PLL0 multiplier value
                          (((LPC_SC->PLL0STAT >> 16) & 0xFF) + 1)    /	//PLL0 pre-divider
                          ((LPC_SC->CCLKCFG & 0xFF)+ 1));				//divider for CCLK (SystemCoreClock)
        break;
      case 2:                                /* RTC oscillator => PLL0        */
        SystemCoreClock = (RTC_CLK * 
                          ((2 * ((LPC_SC->PLL0STAT & 0x7FFF) + 1)))  /
                          (((LPC_SC->PLL0STAT >> 16) & 0xFF) + 1)    /
                          ((LPC_SC->CCLKCFG & 0xFF)+ 1));
        break;
    }
  } else {
    switch (LPC_SC->CLKSRCSEL & 0x03) {
      case 0:                                /* Int. RC oscillator => PLL0    */
      case 3:                                /* Reserved, default to Int. RC  */
        SystemCoreClock = IRC_OSC / ((LPC_SC->CCLKCFG & 0xFF)+ 1);
        break;
      case 1:                                /* Main oscillator => PLL0       */
        SystemCoreClock = OSC_CLK / ((LPC_SC->CCLKCFG & 0xFF)+ 1);
        break;
      case 2:                                /* RTC oscillator => PLL0        */
        SystemCoreClock = RTC_CLK / ((LPC_SC->CCLKCFG & 0xFF)+ 1);
        break;
    }
  }

}

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit (void)
{
#if (CLOCK_SETUP)                       /* Clock Setup                        */
  LPC_SC->SCS       = SCS_Val;
  if (SCS_Val & (1 << 5)) {             /* If Main Oscillator is enabled      */
    while ((LPC_SC->SCS & (1<<6)) == 0);/* Wait for Oscillator to be ready    */
  }

  LPC_SC->CCLKCFG   = CCLKCFG_Val;      /* Setup Clock Divider                */

#if (PLL0_SETUP)
  LPC_SC->CLKSRCSEL = CLKSRCSEL_Val;    /* Select Clock Source for PLL0       */

  LPC_SC->PLL0CFG   = PLL0CFG_Val;      /* configure PLL0                     */
  LPC_SC->PLL0FEED  = 0xAA;
  LPC_SC->PLL0FEED  = 0x55;

  LPC_SC->PLL0CON   = 0x01;             /* PLL0 Enable                        */
  LPC_SC->PLL0FEED  = 0xAA;
  LPC_SC->PLL0FEED  = 0x55;
  while (!(LPC_SC->PLL0STAT & (1<<26)));/* Wait for PLOCK0                    */

  LPC_SC->PLL0CON   = 0x03;             /* PLL0 Enable & Connect              */
  LPC_SC->PLL0FEED  = 0xAA;
  LPC_SC->PLL0FEED  = 0x55;
  while (!(LPC_SC->PLL0STAT & ((1<<25) | (1<<24))));/* Wait for PLLC0_STAT & PLLE0_STAT */
#endif

#if (PLL1_SETUP)
  LPC_SC->PLL1CFG   = PLL1CFG_Val;
  LPC_SC->PLL1FEED  = 0xAA;
  LPC_SC->PLL1FEED  = 0x55;

  LPC_SC->PLL1CON   = 0x01;             /* PLL1 Enable                        */
  LPC_SC->PLL1FEED  = 0xAA;
  LPC_SC->PLL1FEED  = 0x55;
  while (!(LPC_SC->PLL1STAT & (1<<10)));/* Wait for PLOCK1                    */

  LPC_SC->PLL1CON   = 0x03;             /* PLL1 Enable & Connect              */
  LPC_SC->PLL1FEED  = 0xAA;
  LPC_SC->PLL1FEED  = 0x55;
  while (!(LPC_SC->PLL1STAT & ((1<< 9) | (1<< 8))));/* Wait for PLLC1_STAT & PLLE1_STAT */
#else
  LPC_SC->USBCLKCFG = USBCLKCFG_Val;    /* Setup USB Clock Divider            */
#endif

  LPC_SC->PCLKSEL0  = PCLKSEL0_Val;     /* Peripheral Clock Selection         */
  LPC_SC->PCLKSEL1  = PCLKSEL1_Val;

  LPC_SC->PCONP     = PCONP_Val;        /* Power Control for Peripherals      */

  LPC_SC->CLKOUTCFG = CLKOUTCFG_Val;    /* Clock Output Configuration         */
#endif

#if (FLASH_SETUP == 1)                  /* Flash Accelerator Setup            */
  LPC_SC->FLASHCFG  = FLASHCFG_Val;
#endif
}
