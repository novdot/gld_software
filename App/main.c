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
//#include "Dither_Reg.h"
//#include "commandset.h"
//#include "CntrlGLD.h"

#include "hardware/hardware.h"
#include "core/command.h"
#include "core/global.h"
#include "core/config.h"
#include "core/gld.h"
#include "core/ring_buffer.h"
#include "core/sip.h"

#include <math.h>
#include <stdio.h>
/******************************************************************************
**   Main Function  main()
#POSSIBLE DEFINES
UART1REC UART2REC
UART0DBG 
__CONFIG_COMMANDS_DEFAULT 
HOST4 HOST5
NO_BOOTLOADER - from memory coef sector start

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
    int i=0;
    char dbg[64];
    //program variables
    x_ring_init(&g_gld.cmd.dbg.ring_in,g_gld.cmd.dbg.buf_in,GLD_RINGBUFFER_SIZE);
    x_ring_init(&g_gld.cmd.dbg.ring_out,g_gld.cmd.dbg.buf_out,GLD_RINGBUFFER_SIZE);
    
    g_gld.version.bit.Lo = FIRMWARE_VER;
    g_gld.version.bit.Hi = FIRMWARE_VER>>4;
    
    DBG4(&g_gld.cmd.dbg.ring_out,dbg,64,"_Build in:%s %s ver:%d.%d\n\r"
        ,__DATE__
        , __TIME__
        ,g_gld.version.bit.Hi
        ,g_gld.version.bit.Lo
    );
    
    //e. clocking control initialization
    SystemInit();
    
    //WDTFeed();
    
    hardware_configure_lightup();
    hardware_configure_backlight();
    
    ignit_init();
    
    DMA_Init();
    //e. start loading of parameters from flash
    //params_FlashDMA_Init(); 
    
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
    params_load(_params_load_fash);//_params_load_fash _params_load_default
    
    //e. initialize DMA channel for UART
    uart_dma_init(trm_buf);
    
    //e. initialize channel for setting of photodetector gain
    hardware_photo_init();
    hardware_photo_set(Device_blk.Str.Gain_Ph_A, Device_blk.Str.Gain_Ph_B);
    
    thermo_init();
    
    open_all_loops();
    
    currentReg_init();
    cplc_init();
    dither_init();
    g_gld.RgConB.word = RATE_VIBRO_1;
    
    g_gld.dbg_buffers.iteration = 100;
    __enable_irq();
    
    //Device_blk.Str.My_Addres = 2;
}
/******************************************************************************/
void loop_echo()
{
    //command_echo();
}
void loop()
{
    static int nSwitch = 0;
    x_uint8_t dbg[64];
    int i;
    
    uart_recieve_unblocked(0,&g_gld.cmd.dbg.ring_in);
    UART_DBG_SEND(&g_gld.cmd.dbg.ring_out);
    
    command_recieve(_command_recieve_flag_gld);
    command_decode();
    
    if (! (LPC_PWM1->IR & 0x0001) ) return;
	//	hardware_lightup_on();
    //delay();
    //WDTFeed();
    /**
    nSwitch++;
    if(nSwitch>10000){
        nSwitch = 0;
        //UART_SendString("123",3);
        //DBG1(&g_gld.cmd.dbg.ring_out,dbg,64,"nADCData:%u \n\r",g_gld.nADCData[5]);
        //DBG1(&g_gld.cmd.dbg.ring_out,dbg,64,"WP_reg:%u \n\r",Output.Str.WP_reg);
        //DBG1(&g_gld.cmd.dbg.ring_out,dbg,64,"cnt:%u \n\r",g_gld.dbg_buffers.counters_latch);
        //g_gld.dbg_buffers.counters_latch = 0;
    }
    /*if(LPC_GPIO0->FIOPIN&(1<<1)){
        g_gld.dbg_buffers.counters_latch++;
    }*/
    
    //prepare ADC for sampling
    hardware_reset_adc(); 
    //state DAC voltage
    hardware_set_dac(); 
    				
    Latch_Event();	
    clc_Pulses();
    
    ServiceTime();
    
    //start ADC sampling
    hardware_set_adc();		
    //start DAC prepearing for writing     
    hardware_reset_dac();
    exchange_regul_data_read();
    exchange_regul_data_write();
    
    thermo_clc_ThermoSensors();	 
    clc_AD();
    cplc_regulator();
    clc_Dith_regulator(); 
    clc_OutFreq_regulator();
    Output.Str.WP_sin = cplc_calc_modulator();
  	
    //contrl_GLD();
    gld_control();
    
    hardware_photo_exchange(&Output.Str.Cnt_Dif);
    
    command_transm();
    //command_echo();
    
    dbg_recieve();
    
    // data_Rdy &= ~RESET_PERIOD;
    LPC_PWM1->IR = 0x0001; //e. clear interrupt flag 	
}

/******************************************************************************/
int main(void)
{
    init();
  
    do {

        loop();
			//hardware_lightup_off();
        //loop();
    } while(1);	    // main infinie loop            
}

/******************************************************************************/
