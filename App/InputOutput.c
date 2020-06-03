#include "InputOutput.h"
//TODO
#include "CyclesSync.h"
#include "lpc17xx.h"
#include "el_lin.h"

#include "hardware/hardware.h"


int ExchangeErr = 0;  
//----------------------temp---------------------
extern char test;
/******************************************************************************
** Function name:		G_Photo_Exchange
**
** Descriptions:		Driver for I2C exchange 
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/

void G_Photo_Exchange(void) 
{
    i2c_read(&Output.Str.Cnt_Dif);
}
/******************************************************************************
** Function name:		DAC_ADC_Exchange
**
** Descriptions:		Loading data to DACs and initialization of ADC reading 
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void DAC_ADC_Exchange()
{
    hardware_regul_data_write(
        Sys_Clock
        , &ExchangeErr
        , Output.Str.HF_reg
        , Output.Str.WP_reg
    );
}
/******************************************************************************
** Function name:		ADC_Input
**
** Descriptions:		Reading data from ADC 
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void ADC_Input()
{ 
    int buffer[6];
    spi_read(buffer,6,&ExchangeErr);
    g_input.word.wp_sel = buffer[5];
    g_input.word.hf_out = buffer[4];
    g_input.word.delta_t = buffer[3];
    g_input.word.in2 = buffer[2];
    g_input.word.in1 = buffer[1];
    g_input.word.temp1 = buffer[0];
    
    /**
    uint32_t Dummy;
    //---------------------read data from ADC buffer---------------------------------------------
    Dummy = Dummy;
    ExchangeErr &= ~ADC_ERR_MSK;

    if (LPC_SSP0->SR & SSP_BUSY)	  					//exchanging is active, error
    {
        ExchangeErr |= ADC_ERR_MSK;
        if (!(LPC_SSP0->SR & TX_SSP_EMPT))				//buffer is not empty
            ExchangeErr |= ADC_ERR_MSK;
    } else {	 
        //exchanging has finished, read buffer
        Input.ArrayIn[0] = LPC_SSP0->DR; //Thermo2
        Input.ArrayIn[1] = LPC_SSP0->DR; //Thermo1
        Input.ArrayIn[2] = LPC_SSP0->DR; //HF_out

        // clear the RxFIFO 
        while (LPC_SSP0->SR & RX_SSP_notEMPT)
            Dummy = LPC_SSP0->DR;	
    }
    /**/
}

/******************************************************************************
** Function name:		G_Photo_Init
**
** Descriptions:		Initialization of exchange with digital potentiometers
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void G_Photo_Init(void ) 
{
    i2c_init();
}
/******************************************************************************
** Function name:		DAC_ADC_Exchange_Init
**
** Descriptions:		Initialization of data exchange with DACs and ADCs
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void DAC_ADC_Exchange_Init()
{
    spi_init();
}

/******************************************************************************
** Function name:		Out_G_photo
**
** Descriptions:		Start of potentiometer data writing process
**
** parameters:			pointer to gain factors
** Returned value:		None
** 
******************************************************************************/
void  Out_G_photo(uint32_t Ph_A, uint32_t Ph_B)
{
    i2c_write(Ph_A,Ph_B);
}
