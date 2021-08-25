#include "core/sip.h"
#include "core/global.h"
#include "core/const.h"
#include "core/math_dsp.h"

#include "hardware/qei.h"

#define	INT32MAX_DIV2   (1073741823)	//e. 0.5*MAX_QEI_CNT
#define INT32MIN_DIV2   (-1073741823)	//e. -0.5*MAX_QEI_CNT
#define SHIFT_TO_FRACT  (18) //e. shift for converting integer to float (14.18) format  

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
} // interpolation
/******************************************************************************/
void clc_Pulses()
{
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
 
    //e. selecting display mode in the Rate mode
    switch (g_gld.RgConB.word) {
   		case RATE_VIBRO_1:
			if (Latch_Rdy) {	
                dif_Curr_32_Ext = interpolation(Dif_Curr_32, LatchPhase ); 
                
                //#NDA no cnt
				//Output.Str.Tmp_Out[2] = (int)((g_gld.Curr_Cnt_Vib>>16)& 0xffff); 
				//Output.Str.Tmp_Out[3] = (int)(g_gld.Curr_Cnt_Vib & 0xffff); 
                
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
         if (data_Rdy & HALF_PERIOD) {
            g_gld.pulses.Cnt_curr = g_gld.pulses.Curr_Cnt_Vib;
            RefMeand_Cnt_Dif = g_gld.pulses.Cnt_curr - Old_Cnt;
            Old_Cnt = g_gld.pulses.Cnt_curr;
            Cnt_Overload(RefMeand_Cnt_Dif, INT32MAX_DIV2, INT32MIN_DIV2);

            //e. "+" direction 
			if (qei_get_direction()==_x_plus) {
	  		 	//sumCnt_Mns += -RefMeand_Cnt_Dif;
				Cnt_Mns = RefMeand_Cnt_Dif;
			} else {										
                //e. "-" direction 
	  			//sumCnt_Pls += RefMeand_Cnt_Dif;
				Cnt_Pls = -RefMeand_Cnt_Dif;
			}

            //UART1_SendByte((dif_sum_32>>8) & 0xff);
            //UART1_SendByte((dif_sum_32) & 0xff);
		 
            //e. period of vibro elapsed
            if (data_Rdy & WHOLE_PERIOD) {
                // LPC_GPIO2->FIOSET = 0x10;
                last_Cnt_Plus = Cnt_Pls;	
                dif_sum_32 += Cnt_Pls - Cnt_Mns;
            }
            data_Rdy &= ~RESET_PERIOD;
        }
        //e it's time for output 
		if (Latch_Rdy) {
            LatchPhase = INT32_MAX;

            //#NDA TODO temporarly off
            //Output.Str.Tmp_Out[2] = (int)((g_gld.Curr_Cnt_Vib>>16)& 0xffff); 
            //Output.Str.Tmp_Out[3] = (int)(g_gld.Curr_Cnt_Vib & 0xffff); 

            cntPls_sum_32 += last_Cnt_Plus - preLast_Cnt_Plus;

            Output.Str.Cnt_Dif = dif_sum_32;            
            Output.Str.Cnt_Dif += cntPls_sum_32 >> 1;            

            dif_sum_32 = 0;				    //e. reset accumulators 
            ResetBitsOfWord(&cntPls_sum_32, 1); 
            preLast_Cnt_Plus = last_Cnt_Plus;	
            //e. current last sample became previous

            Output.Str.Cnt_Mns = Cnt_Mns;//sumCnt_Mns;	   
            //e. rewrite accumulated data to output
            Output.Str.Cnt_Pls = Cnt_Pls;//sumCnt_Pls;	   

            //sumCnt_Mns = 0;	   //e. prepare for new accumulation
            //sumCnt_Pls = 0;
		}		 
        break;
    }	 
    //e. WP_scope1, WP_scope2 - variables for control in the Rate3 mode 
 	Output.Str.WP_scope1 = g_gld.pulses.Dif_Curr_Vib;  
 	Output.Str.WP_scope2 = (Dif_Curr_32 >> (SHIFT_TO_FRACT-2)); 
} // clc_Pulses