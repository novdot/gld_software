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
//#include "SIP.h"
//#include "el_lin.h"
#include "CyclesSync.h"
#include "Parameters.h"							  
#include "Dither_Reg.h"
//#include "commandset.h"
//#include "CntrlGLD.h"

#include <math.h>

#include "hardware/hardware.h"
#include "core/command.h"
#include "core/global.h"
#include "core/config.h"
#include "core/gld.h"
#include "core/ring_buffer.h"
#include "core/sip.h"

/******************************************************************************
**   Main Function  main()
*******************************************************************************/

/******************************************************************************
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
}*/
/******************************************************************************/
void init()
{
    char dbg[64];
    
    int i;
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
    cplc_init();
    init_Dither_reg();
    g_gld.RgConB.word = RATE_VIBRO_1;
    
    //program variables
    //ringbuffer_init(&g_gld.ringBuf,32);
    
    DBG2(dbg,64,"Build in %s %s",__DATE__,__TIME__);
    DBG0(dbg,64,"Init done!");
}
/******************************************************************************/
void loop_echo()
{
    command_echo();
}
void loop()
{
    static int nSwitch = 0;
    static char dbg[64];
    int i;
    
    if (! (LPC_PWM1->IR & 0x0001) ) return;
    //delay();
    //WDTFeed();
    
    //prepare ADC for sampling
    hardware_reset_adc(); 
    //state DAC voltage
    hardware_set_dac(); 
    
    g_gld.Curr_Cnt_Vib = qei_get_position();
    				
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
    cplc_regulator();
    clc_Dith_regulator(); 
    clc_OutFreq_regulator();
    Output.Str.WP_sin = cplc_calc_modulator();
  	
    //contrl_GLD();
    gld_control();
    
    hardware_photo_exchange(&Output.Str.Cnt_Dif);
    
    //command_echo();
    command_recieve(_command_recieve_flag_gld);
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
        //loop();
    } while(1);	    // main infinie loop            
}

/******************************************************************************/
