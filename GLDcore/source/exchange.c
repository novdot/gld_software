#include "core/exchange.h"

#include "core/exchange.h"
#include "core/global.h"
#include "hardware/hardware.h"

#include "stdlib.h"
#include "math.h"

int ExchangeErr = 0;
int g_ChInd = 0; //< индекс канала ЦАП
/******************************************************************************/
void exchange_regul_data_write()
{
    static int reg, sign  = 0;
    if(g_ChInd==1) {
        g_ChInd = 0;
        //вернем сдвиг
        reg = (Output.Str.WP_reg>>4);
        
    }else{
        g_ChInd = 1;
        reg = (Output.Str.HF_reg>>4);
    }
    //TODO maybe move into hardware module
    if(reg>=0) sign = 0;
    else sign = 1;
    
    reg = abs(reg);
    
    //сожмем диапазон
    reg = (reg/26)*17;
    
    if(sign==0) reg = 0x08F0+reg; //ноль host4
    else reg = 0x08F0-reg;
    
    g_gld.nDACData[g_ChInd] = reg;
    hardware_regul_data_write(
            g_ChInd
            , &ExchangeErr
            , reg
        );
}
/******************************************************************************/
void exchange_regul_data_read()
{ 
    static const int hf_out_max = 32767; //16bit adc
    
    hardware_regul_data_read((int*)g_gld.nADCData,5,&ExchangeErr);
    /**
    g_input.word.wp_sel = g_gld.nADCData[0];
    g_input.word.hf_out = g_gld.nADCData[1];
    g_input.word.delta_t = g_gld.nADCData[2];
    g_input.word.in2 = ((g_gld.nADCData[3]));//-40092)*38)/100;;
    g_input.word.in1 = ((g_gld.nADCData[4]));//-40092)*38)/100;;
    g_input.word.temp1 = g_gld.nADCData[5];
    **/
    if(g_gld.nADCData[0] > hf_out_max){
        g_input.word.hf_out = 0;
    }else{
        g_input.word.hf_out = (hf_out_max - g_gld.nADCData[0])*6/5;
    }
    if(g_input.word.hf_out>0x7FFF) g_input.word.hf_out = 0x7FFF;
    
    g_input.word.wp_sel = g_gld.nADCData[1];
    g_input.word.in2 = -16495 + g_gld.nADCData[2]*423/1000;
    g_input.word.in1 = -16495 + g_gld.nADCData[3]*423/1000;
    g_input.word.temp1 = g_gld.nADCData[4];
    g_input.word.delta_t = 0;
    
    //for ask gld
    Output.Str.Tmp_Out[0] = 0;
    Output.Str.Tmp_Out[1] = 0;
    Output.Str.Tmp_Out[2] = g_input.word.in1;
    Output.Str.Tmp_Out[3] = g_input.word.in2;
    
    Output.Str.Tmp_Out[5] = g_input.word.delta_t;
    /**/
}