#include "core/exchange.h"

#include "core/exchange.h"
#include "core/global.h"
#include "hardware/hardware.h"

int ExchangeErr = 0;
/******************************************************************************/
void exchange_regul_data_write()
{
    hardware_regul_data_write(
        Sys_Clock
        , &ExchangeErr
        , Output.Str.HF_reg
        , Output.Str.WP_reg
    );
}
/******************************************************************************/
void exchange_regul_data_read()
{ 
    //int buffer[6];
    hardware_regul_data_read((int*)g_gld.nADCData,6,&ExchangeErr);
    g_input.word.wp_sel = g_gld.nADCData[5];
    g_input.word.hf_out = g_gld.nADCData[4];
    g_input.word.delta_t = g_gld.nADCData[3];
    g_input.word.in2 = g_gld.nADCData[2];
    g_input.word.in1 = g_gld.nADCData[1];
    g_input.word.temp1 = g_gld.nADCData[0];
    
    /*g_gld.nADCData[5] = buffer[5];
    g_gld.nADCData[4] = buffer[4];
    g_gld.nADCData[3] = buffer[3];
    g_gld.nADCData[2] = buffer[2];
    g_gld.nADCData[1] = buffer[1];
    g_gld.nADCData[0] = buffer[0];*/
}