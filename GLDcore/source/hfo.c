#include "core/hfo.h"
#include "core/gld.h"
#include "core/math_dsp.h"
#include "core/global.h"

#define		HFO_NEG_MIN			8738	// -4.5 V
#define		HFO_NEG_MAX			25122	// -10.5 V
#define		HFO_POZ_MIN			-32221	// +10.5 V
#define		HFO_POZ_MAX			-15837 	// +4.5 V


/******************************************************************************/
void clc_HFO()
{
	static int  hf_reg = 0; //e. the value of the integrator in the HFO regulator 

	//e. filtration of an output of the amplitude detector before transfer to the HFO regulator
	//Input.StrIn.HF_out = HFO_MovAverFilt(Input.StrIn.HF_out<<HFO_SHIFT);
	//Output.Str.HF_dif = L_sub(Device_blk.Str.HF_ref, Input.StrIn.HF_out); // HF_dif	= HF_out - Device_blk.Str.HF_ref;
    g_input.word.hf_out = HFO_MovAverFilt(g_input.word.hf_out<<HFO_SHIFT);
	Output.Str.HF_dif = L_sub(Device_blk.Str.HF_ref, g_input.word.hf_out); // HF_dif	= HF_out - Device_blk.Str.HF_ref;
 
    //e. the regulator loop is closed 
	if ( loop_is_closed(HF_REG_ON) ){
		/*hf_reg = L_mac( hf_reg, Output.Str.HF_dif, Device_blk.Str.HF_scl ); // hf_reg32 += HFO_error * Device_blk.Str.HF_scl;
		Saturation(hf_reg, Device_blk.Str.HF_max << HFO_SHIFT, Device_blk.Str.HF_min << HFO_SHIFT); //e. checking range 
		Output.Str.HF_reg = (hf_reg >> HFO_SHIFT);*/
	} else {
		/*hf_reg = Output.Str.HF_reg << HFO_SHIFT;	*///e. assign to the integrator the previous value of the HF_reg
	}

	// cyclic built-in test
	if ( ! ( ((Output.Str.HF_reg < HFO_NEG_MAX) && (Output.Str.HF_reg > HFO_NEG_MIN)) 
		|| ((Output.Str.HF_reg < HFO_POZ_MAX) && (Output.Str.HF_reg > HFO_POZ_MIN))) )
	{
		Valid_Data |= HFO_VOLT_ERROR;
	}
}