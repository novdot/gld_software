#include "core/exchange.h"

#include "core/exchange.h"
#include "core/global.h"
#include "hardware/hardware.h"

int ExchangeErr = 0;
int g_ChInd = 0; //< индекс канала ЦАП
/******************************************************************************/
void exchange_regul_data_write()
{
    static int reg, sign  = 0;
    if(g_ChInd==1) {
        g_ChInd = 0;
        //вернем сдвиг
        reg = (Output.Str.WP_reg>>4);//+0x800;
        if(reg>=0) sign = 0;
        else sign = 1;
        
        reg = abs(reg);
        
        //сожмем диапазон
        reg = (reg/26)*17;
        
        if(sign==0) reg = 0x08F0+reg; //ноль host4
        else reg = 0x08F0-reg;
        
        //reg++;
    }else{
        g_ChInd = 1;
        reg = (Output.Str.HF_reg>>4)+0x800;
    }
    hardware_regul_data_write(
            g_ChInd
            , &ExchangeErr
            , reg
        );
}
/******************************************************************************/
void exchange_regul_data_read()
{ 
    hardware_regul_data_read((int*)g_gld.nADCData,6,&ExchangeErr);
    g_input.word.wp_sel = g_gld.nADCData[5];
    g_input.word.hf_out = g_gld.nADCData[4];
    g_input.word.delta_t = g_gld.nADCData[3];
    g_input.word.in2 = g_gld.nADCData[2];
    g_input.word.in1 = g_gld.nADCData[1];
    g_input.word.temp1 = g_gld.nADCData[0];
}