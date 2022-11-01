#include "core/exchange.h"

#include "core/exchange.h"
#include "core/global.h"
#include "hardware/hardware.h"

#include "stdlib.h"
#include "math.h"

int ExchangeErr = 0;

/******************************************************************************/
void exchange_regul_data_write()
{
	static int g_ChInd = 0; //< индекс канала ЦАП
	
	//g_gld.nDACData[0] = ((Output.Str.WP_reg*19661) >> 19)+0x08F0;
#ifdef HOST4
    g_gld.nDACData[0] = (((Output.Str.WP_reg+61013)*19661) >> 19);
    g_gld.nDACData[1] = (((Output.Str.CURR_reg+61013)*19661) >> 19);
#else    //HOST5	
    /**
    2022 10 31 
    ASK GLD formula for input value:
    code -> Volt
    0 -> 0 V
    +26833 -> +175 V
    -26833 -> -175 V
    154 -> 1 V
    
    for HOST5
    DAC ltc16xx 12bit data.
    code DAC -> Volt
    2303 -> 0 V
    7.5 -> 1 V
    */
    g_gld.nDACData[0] = (Output.Str.WP_reg/154*75/10 + 2303);
    if(g_gld.nDACData[0]>0xFFF) g_gld.nDACData[0] = 0xfff;
    
    g_gld.nDACData[1] = (((Output.Str.CURR_reg+61013)*19661) >> 19);
    if(g_gld.nDACData[1]>0xFFF) g_gld.nDACData[1] = 0xfff;
#endif     
    g_ChInd = (g_ChInd == 0) ? 1:0;
    hardware_regul_data_write(
            g_ChInd
            , &ExchangeErr
            , g_gld.nDACData[g_ChInd]
        );
}
/******************************************************************************/
void exchange_regul_data_read()
{ 
    static const int hf_out_max = 32767; //16bit adc
    
    hardware_regul_data_read((int*)g_gld.nADCData,6,&ExchangeErr);
/*
    if(g_gld.nADCData[0] > hf_out_max){
        g_input.word.ad_out = 0;
    }else{
        g_input.word.ad_out = (hf_out_max - g_gld.nADCData[0])*6/5;
    }
    if(g_input.word.ad_out>0x7FFF) g_input.word.ad_out = 0x7FFF;
 */
	  g_input.word.ad_out = (g_gld.nADCData[0]*30000)>>15;
	
    g_input.word.wp_sel = g_gld.nADCData[1];
    //g_input.word.in2 = -16495 + g_gld.nADCData[4]*423/1000;
    //g_input.word.in1 = -16495 + g_gld.nADCData[5]*423/1000;
#ifdef HOST4    
    g_input.word.in2 = -15955 + ((g_gld.nADCData[4]*13098)>>15);
    g_input.word.in1 = -15955 + ((g_gld.nADCData[5]*13098)>>15);
#else    //HOST5
    /**
    2022 10 28 
    ASK GLD formula for input value:
    abs(int16+16340)*0.0306
    if int16=0 result will be 500mkA
    
    for HOST5
    (ADC-16471)/41.4=mkA
    */
    g_input.word.in2 = -16340+( (g_gld.nADCData[4]-16471) )*1000/1267;
    g_input.word.in1 = -16340+( (g_gld.nADCData[5]-16471) )*1000/1267;  
#endif    
    g_input.word.temp1 = g_gld.nADCData[2] + ((g_gld.nADCData[2]*7232)>>15) - 21832;
    g_input.word.delta_t = g_gld.nADCData[3] ;
    
    //for ask gld
    Output.Str.Tmp_Out[0] = 0;
    Output.Str.Tmp_Out[1] = 0;
    Output.Str.Tmp_Out[2] = g_input.word.in1;
    Output.Str.Tmp_Out[3] = g_input.word.in2;
    
    /**/
}