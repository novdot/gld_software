/****************************************Copyright (c)****************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           main.c
** Last modified Date:  2011-10-24
** Last Version:        V1.00
** Descriptions:        The main() function
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Electrooptica Incorp.
** Created date:        2011-08-22
** Version:             V1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/
#include "SIP.h"
//#include "el_lin.h"
#include "CyclesSync.h"
#include "Parameters.h"							  
#include "Dither_Reg.h"
//#include "commandset.h"
#include "CntrlGLD.h"
#include <math.h>

#include "hardware/hardware.h"
#include "core/command.h"
#include "core/global.h"
#include "core/config.h"

/******************************************************************************
**   Main Function  main()
******************************************************************************/

gld_global g_gld;

/******************************************/
#include	"LPC17xx.h"

void delay( ) {
    int i = 10000;
    while(i) {i--;};
}
void Vout(int v) {
    LPC_DAC->CR = v <<6;
}
void init1()
{
    int v = 0;
    LPC_PINCON->PINSEL1 |= 0x02<<20;
    LPC_SC->PCLKSEL0 |= 1 <<24;
    while(1){
         Vout(v++);
         v &= 0x3FF;
         delay();   //will allow signal to be observed on a multimeter
    }
}
/******************************************************************************/
void init()
{
    //e. clocking control initialization
    SystemInit();
    
    //WDTFeed();
    
    hardware_configure_lightup();
    hardware_configure_backlight();
    
    ignit_init();
    
    DMA_Init();
    //e. start loading of parameters from flash
    FlashDMA_Init(); 
    
    //e. to calculate SystemCoreClock  for UART particularly
    SystemCoreClockUpdate();
    //e. initialization of UART on 38400
    UART_Init(CONFIG_COMMANDS_BAUDRATE);
    
    //e. initialization of exchange with hardware
    hardware_regul_data_init();
    hardware_dac_init();
    //e. quadrature encoder initialization
    SOI_Init(); 		
    //e. initialization of internal latch
    IntLatch_Init();
    //e. initialization of device for external latch
    ExtLatch_Init();
    
    //e.to load device's constants
    LoadFlashParam(FromFLASH);
    
    //e. initialize DMA channel for UART
    uart_dma_init(trm_buf);
    
    //e. initialize channel for setting of photodetector gain
    //G_Photo_Init();
    hardware_photo_init();
    
    hardware_photo_out(Device_blk.Str.Gain_Ph_A, Device_blk.Str.Gain_Ph_B);
    
    open_all_loops();
    
    Output.Str.HF_reg = Device_blk.Str.HF_min;
    init_PLC();
    init_Dither_reg();
    RgConB = RATE_VIBRO_1;
}

/******************************************************************************/
void loop()
{
    static int nSwitch = 0;
    
    if (! (LPC_PWM1->IR & 0x0001) ) return;
    //delay();
    //WDTFeed();
    
    //prepare ADC for sampling
    hardware_reset_adc(); 
    //state DAC voltage
    hardware_set_dac(); 
    
    Curr_Cnt_Vib = qei_get_position();
    				
    Latch_Event();	
    clc_Pulses();
    
    ServiceTime();
    
    //start ADC sampling
    hardware_set_adc();		
    //start DAC prepearing for writing     
    hardware_reset_dac();
    exchange_regul_data_read();
    exchange_regul_data_write();
    
    clc_ThermoSensors();	 
    clc_HFO();
    clc_PLC();
    clc_Dith_regulator(); 
    clc_OutFreq_regulator();
    Output.Str.WP_sin = clc_WP_sin();
  	
    contrl_GLD();
    
    hardware_photo_exchange(&Output.Str.Cnt_Dif);
    
    command_recieve();
    command_decode();
    command_transm();
    
    // data_Rdy &= ~RESET_PERIOD;
    LPC_PWM1->IR = 0x0001; //e. clear interrupt flag 	
}

/******************************************************************************/
int main(void)
{
    init();
    
    do {
        loop();
    } while(1);	    // main infinie loop            
}

/******************************************************************************/
