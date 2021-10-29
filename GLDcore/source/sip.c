#include "core/sip.h"
#include "core/global.h"
#include "core/const.h"
#include "core/math_dsp.h"

#include "hardware/qei.h"
#include "math.h" 

int32_t	temp22=0;
/******************************************************************************/
void ResetBitsOfWord(int * x32, int truncate_bits)
{
	int hi_part;

	hi_part = *x32 >> truncate_bits;
	*x32 -= hi_part << truncate_bits;
} 
/******************************************************************************/
int interpolation(int y_curr, int x_interp)
{
	__int64 temp,temp3; 

	temp = (__int64)y_curr *(__int64)x_interp;
	temp /= PrevPeriod;

	temp3 = (__int64)y_curr *(__int64)(x_interp+1);
	temp3 /= PrevPeriod;
	temp22 = (int)temp3;

	return ((int)temp);
}
/******************************************************************************/
void sip_clc_vibro1_mode()
{
    
}
/******************************************************************************/
void sip_clc_reperOfMeandr_mode()
{
    
}
#include "hardware/uart.h"
#include "hardware/qei.h"
#include <math.h>
#include <stdlib.h>
/******************************************************************************/
void clc_Pulses()
{
    x_uint8_t dbg[64];
    int i;
    
    static int32_t cntPls_sum_32 = 0;
    static int32_t last_Cnt_Plus = 0;
    static int32_t dif_sum_32 = 0;
    static int32_t Cnt_Pls = 0;
    static int32_t Cnt_Mns = 0;
    static int32_t preLast_Cnt_Plus = 0;
    
    static uint32_t	Old_Cnt_Vib = 0;
    static uint32_t Old_Cnt = 0;
    static int32_t RefMeand_Cnt_Dif;
    static int32_t PSdif_sum_Vib_32 = 0;
    static __int64 PSdif_sum_Vib_64 = 0;
    static int32_t dif_Curr_32_Ext = 0; //e. difference(number) for the external latch mode 
    static int32_t dif_Curr_32_previous = 0; //e. Previous (in comparison with Dif_Curr_32) number 
    
    static int32_t	Dif_Curr_32 = 0;//e. current difference without dithering for dithering control
    
    //read pulses
    g_gld.pulses.Curr_Cnt_Vib = qei_get_position();
    
    //e. accumulated number of pulses
    g_gld.pulses.Dif_Curr_Vib = g_gld.pulses.Curr_Cnt_Vib - Old_Cnt_Vib; 			 
    Cnt_Overload(g_gld.pulses.Dif_Curr_Vib, INT32MAX_DIV2, INT32MIN_DIV2);
    
    //e. save current number of pulses
    Old_Cnt_Vib = g_gld.pulses.Curr_Cnt_Vib;	

    //e. Precision of filtration is 1/(2^18)	
    Dif_Curr_32 = VibroReduce(g_gld.pulses.Dif_Curr_Vib << SHIFT_TO_FRACT); 
    //Dif_Curr_32 = g_gld.pulses.Dif_Curr_Vib;
	
	//e. selecting therStrmocompensation mode
	switch (Device_blk.Str.TermoMode&0x0F) {
	case TERMO_ON:
	case TERMO_ON_NUMB_OFF:
		//e. accumulation of the value of thermocompensation from request to request 
		g_gld.thermo.TermoCompens_Sum += g_gld.thermo.StartTermoCompens + thermo_DynamicTermoCompens(); 
		break;

	case TERMO_ON_STATIC_ONLY:
	case TERMO_ON_STATIC_ONLY_NUMB_OFF:
		//e. accumulation of the value of thermocompensation from request to request 
		g_gld.thermo.TermoCompens_Sum += g_gld.thermo.StartTermoCompens; 
		break;

	case	TERMO_ON_DYNAMIC_ONLY:
	case	TERMO_ON_DYNAMIC_ONLY_NUMB_OFF:
		//e. accumulation of the value of thermocompensation from request to request 
		g_gld.thermo.TermoCompens_Sum += thermo_DynamicTermoCompens(); 
		break;

	case	TERMO_OFF:
	default:
		//e. thermocompensation is disable, therefore its part is equal to zero 
		g_gld.thermo.TermoCompens_Sum = 0;
		break;
	}
 
    //e. selecting display mode in the Rate mode
    switch (g_gld.RgConB.word) {
   		case RATE_VIBRO_1:
			if (Latch_Rdy) {	
                dif_Curr_32_Ext = interpolation(Dif_Curr_32, LatchPhase ); 
                
				//e. add to the accumulated sum the interpolated sample of an external latch 		
                PSdif_sum_Vib_32 += dif_Curr_32_Ext; // PSdif_sum_Vib_32 += dif_Curr_32_Ext; 
				PSdif_sum_Vib_64 += dif_Curr_32_Ext; //e. receive last data
				//count--;
	
				//e. preparing number for output
				//e. substract the accumulated termocompensational part from the accumulated number 			
                Output.Str.BINS_dif = PSdif_sum_Vib_32 - g_gld.thermo.TermoCompens_Sum;	 
                Output.Str.PS_dif = Output.Str.BINS_dif >> 16;	
				LatchPhase = INT32_MAX;	 //in Latch_Event it's indicator of latch appearing
				Output.Str.SF_dif = PSdif_sum_Vib_64; 
				//e. nulling the accumulated termocompenstion for beginning of the new cycle of accumulation 
				g_gld.thermo.TermoCompens_Sum = 0;  
                										 				
                if ((Device_Mode == DM_EXT_LATCH_DELTA_BINS_PULSE) || \
                   ((Device_Mode == DM_EXT_LATCH_DELTA_SF_PULSE) && Ext_Latch_ResetEnable)){	 
                          //e. to initialize a new external latch cycle 
                        PSdif_sum_Vib_32 = 0;      
                        PSdif_sum_Vib_64 = 0;					
                }else{
                        ResetBitsOfWord(&PSdif_sum_Vib_32, 16);	 
                }
							
				 dif_Curr_32_Ext = Dif_Curr_32 - temp22;//dif_Curr_32_Ext;

				PSdif_sum_Vib_32 +=  dif_Curr_32_Ext; 
                // preserve rest of counters difference for next measure cycle: PSdif_sum_Vib_32 += Dif_Curr_32 - dif_Curr_32_Ext;
				PSdif_sum_Vib_64 +=  dif_Curr_32_Ext;
			// LPC_GPIO2->FIOCLR |= 0x00000004;		// turn off the LED 										 
			}else {	
                //e. the latch at this moment is abscent
				//e. continue accumulating the sum from internal samples
                PSdif_sum_Vib_32 += Dif_Curr_32; // PSdif_sum_Vib_32 += Dif_Curr_32 ;			
				PSdif_sum_Vib_64 += Dif_Curr_32; //e. sum for scale factor measurement mode
			} 

            //e. save previous number
			dif_Curr_32_previous = Dif_Curr_32; 
            break;

  	case RATE_REPER_OR_REFMEANDR:
        //e. calculate Cnt_Mns or Cnt_Pls
    
        g_gld.pulses.reper_meandr.cnt_iter++;
    
        /*if( 
            (LPC_MCPWM->TC0>g_gld.pulses.reper_meandr.cnt_tc_prev) 
            &&(g_gld.pulses.reper_meandr.flags.bit.get_zero==0)
        ){
            g_gld.pulses.reper_meandr.flags.bit.get_zero = 1;
            g_gld.pulses.reper_meandr.flags.bit.get_peak = 0;
            
            g_gld.pulses.reper_meandr.cnt_curr = qei_get_position();
            g_gld.pulses.reper_meandr.cnt_dif = 
                (g_gld.pulses.reper_meandr.cnt_curr - g_gld.pulses.reper_meandr.cnt_prev); 
            g_gld.pulses.reper_meandr.cnt_prev = g_gld.pulses.reper_meandr.cnt_curr;
            Cnt_Overload(g_gld.pulses.reper_meandr.cnt_dif, INT32MAX_DIV2, INT32MIN_DIV2);
            g_gld.pulses.reper_meandr.cnt_pls = abs(g_gld.pulses.reper_meandr.cnt_dif);
        }
        
        //check MAT2 interrupt
        if( 
            (LPC_MCPWM->TC0<g_gld.pulses.reper_meandr.cnt_tc_prev) 
            &&(g_gld.pulses.reper_meandr.flags.bit.get_peak==0)
        ){
            g_gld.pulses.reper_meandr.flags.bit.get_zero = 0;
            g_gld.pulses.reper_meandr.flags.bit.get_peak = 1;
            
            g_gld.pulses.reper_meandr.cnt_curr = qei_get_position();
            g_gld.pulses.reper_meandr.cnt_dif = 
                (g_gld.pulses.reper_meandr.cnt_curr - g_gld.pulses.reper_meandr.cnt_prev); 
            g_gld.pulses.reper_meandr.cnt_prev = g_gld.pulses.reper_meandr.cnt_curr;
            Cnt_Overload(g_gld.pulses.reper_meandr.cnt_dif, INT32MAX_DIV2, INT32MIN_DIV2);
            g_gld.pulses.reper_meandr.cnt_mns = abs(g_gld.pulses.reper_meandr.cnt_dif);
            g_gld.pulses.reper_meandr.cnt_delta +=
                    ( g_gld.pulses.reper_meandr.cnt_pls - g_gld.pulses.reper_meandr.cnt_mns);
        }
        g_gld.pulses.reper_meandr.cnt_tc_prev = LPC_MCPWM->TC0;*/
        
        /////////////////////////////////////////////////
        if (data_Rdy & HALF_PERIOD) {
            //g_gld.pulses.Cnt_curr = g_gld.pulses.Curr_Cnt_Vib;
            /*
            RefMeand_Cnt_Dif = L_abs(g_gld.pulses.Cnt_curr - Old_Cnt);
            Old_Cnt = g_gld.pulses.Cnt_curr;
            Cnt_Overload(RefMeand_Cnt_Dif, INT32MAX_DIV2, INT32MIN_DIV2);*/
            
            g_gld.pulses.reper_meandr.cnt_dif = 
                (g_gld.pulses.reper_meandr.cnt_curr - g_gld.pulses.reper_meandr.cnt_prev); 
            g_gld.pulses.reper_meandr.cnt_prev = g_gld.pulses.reper_meandr.cnt_curr;
            Cnt_Overload(g_gld.pulses.reper_meandr.cnt_dif, INT32MAX_DIV2, INT32MIN_DIV2);
            
            
            //e. "+" direction 
            if (qei_get_direction()==_x_plus) {
                //sumCnt_Mns += -RefMeand_Cnt_Dif;
                //Cnt_Mns = RefMeand_Cnt_Dif;
                //Cnt_Pls = (RefMeand_Cnt_Dif);
                g_gld.pulses.reper_meandr.cnt_pls =L_abs(g_gld.pulses.reper_meandr.cnt_dif);
            } else {										
                //e. "-" direction 
                //sumCnt_Pls += RefMeand_Cnt_Dif;
                //Cnt_Pls = -RefMeand_Cnt_Dif;
                //Cnt_Mns = (RefMeand_Cnt_Dif);
                g_gld.pulses.reper_meandr.cnt_mns = L_abs(g_gld.pulses.reper_meandr.cnt_dif);
            }

            //e. period of vibro elapsed
            if (data_Rdy & WHOLE_PERIOD) {
                // LPC_GPIO2->FIOSET = 0x10;
                //last_Cnt_Plus = Cnt_Pls;	
                //dif_sum_32 += (Cnt_Pls - Cnt_Mns);
                
                g_gld.pulses.reper_meandr.cnt_delta +=
                    ( g_gld.pulses.reper_meandr.cnt_pls - g_gld.pulses.reper_meandr.cnt_mns);
                
                /*if( (g_gld.dbg_buffers.iteration<50)&&(g_gld.pulses.reper_meandr.cnt_delta>100) ){
                    //DBG1(&g_gld.cmd.dbg.ring_out,dbg,64,"%d\n\r",Output.Str.WP_Phase_Det_Array[0]);
                    //DBG1(&g_gld.cmd.dbg.ring_out,dbg,64,"%d\n\r",g_input.word.wp_sel);
                    DBG3(&g_gld.cmd.dbg.ring_out,dbg,64,"%d %d = %d\n\r"
                        ,g_gld.pulses.reper_meandr.cnt_pls
                        ,g_gld.pulses.reper_meandr.cnt_mns
                        ,g_gld.pulses.reper_meandr.cnt_delta
                    );
                    g_gld.dbg_buffers.iteration++;
                }*/
            }
            data_Rdy &= ~RESET_PERIOD;//reset bits
        }
        //e it's time for output 
		if (Latch_Rdy) {
            LatchPhase = INT32_MAX;
            
            //cntPls_sum_32 += last_Cnt_Plus - preLast_Cnt_Plus;
            //preLast_Cnt_Plus = last_Cnt_Plus;	 //e. current last sample became previous

            //Output.Str.Cnt_Dif = dif_sum_32; 
            //g_gld.pulses.Cnt_curr = 0;            
            //Output.Str.Cnt_Dif += cntPls_sum_32 >> 1;
            //dif_sum_32 = 0;				    //e. reset accumulators 
            Output.Str.Cnt_Dif = g_gld.pulses.reper_meandr.cnt_delta;
            g_gld.pulses.reper_meandr.cnt_delta = 0;            

            //ResetBitsOfWord(&cntPls_sum_32, 1); 

            /*Output.Str.Cnt_Mns = Cnt_Mns;//sumCnt_Mns;	   
            //e. rewrite accumulated data to output
            Output.Str.Cnt_Pls = Cnt_Pls;//sumCnt_Pls;	 */ 
            Output.Str.Cnt_Mns = g_gld.pulses.reper_meandr.cnt_mns;            
            Output.Str.Cnt_Pls = g_gld.pulses.reper_meandr.cnt_pls;

            g_gld.pulses.reper_meandr.cnt_int_sum = g_gld.pulses.reper_meandr.cnt_int;
            g_gld.pulses.reper_meandr.cnt_int = 0;
            
            /*if( (g_gld.dbg_buffers.iteration<500) ){
                DBG1(&g_gld.cmd.dbg.ring_out,dbg,64,"%d\n\r"
                    ,g_gld.pulses.reper_meandr.cnt_int_sum
                );
                g_gld.dbg_buffers.iteration++;
            }*/
            
            //sumCnt_Mns = 0;	   //e. prepare for new accumulation
            //sumCnt_Pls = 0;
		}		 
        break;
    }	 
    //e. WP_scope1, WP_scope2 - variables for control in the Rate3 mode 
 	Output.Str.WP_scope1 = g_gld.pulses.Dif_Curr_Vib;  
 	Output.Str.WP_scope2 = (Dif_Curr_32 >> (SHIFT_TO_FRACT-2)); 
} // clc_Pulses