#include "core/hfo.h"
#include "core/gld.h"
#include "core/math_dsp.h"
#include "core/global.h"

/******************************************************************************/
void currentReg_init()
{
    Output.Str.CURR_reg = Device_blk.Str.Curr_start;
}

void clc_AD()
{
    
	//e. filtration of an output of the amplitude detector
    Output.Str.AD_value = (dsp_MovAverFilt(g_input.word.ad_out)*9830)>>15;
    
    //signal amplitude
    // HF_dif	= HF_out - Device_blk.Str.HF_ref;
	//Output.Str.HF_dif = L_sub(Device_blk.Str.HF_ref, g_input.word.hf_out); 
    
	// cyclic built-in test
	if  ((g_input.word.ad_out < AD_MIN) || (g_input.word.ad_out > AD_MAX)){
		  Valid_Data |= AD_ERROR;
	}
}