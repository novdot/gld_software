#include "core/sip.h"
#include "core/global.h"
#include "core/const.h"
#include "core/math_dsp.h"
#include "core/thermo.h"
#include "core/command_handlers.h"      

#include "hardware/qei.h"
#include "hardware/uart.h"
#include <math.h>
#include <stdlib.h>

int32_t	temp22=0;
int32_t	Dif_Curr_32 = 0;


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
void clc_Pulses()
{   
    static int32_t cntPls_sum_32 = 0;
    static int32_t last_Cnt_Plus = 0;
    static int32_t dif_sum_32 = 0;
    static int32_t preLast_Cnt_Plus = 0;
    //static int32_t sign_accum = 0;
    static uint32_t	Old_Cnt_Vib = 0;
    static uint32_t Old_Cnt = 0;
    static int32_t RefMeand_Cnt_Dif;
    static int32_t PSdif_sum_Vib_32 = 0;
    static __int64 PSdif_sum_Vib_64 = 0;
    static int32_t dif_Curr_32_Ext = 0; //e. difference(number) for the external latch mode 
    static int32_t dif_Curr_32_previous = 0; //e. Previous (in comparison with Dif_Curr_32) number 
    static int32_t TermoCompens_Sum = 0;
    float percent = 0.0;// соотношение разницы для переворота импульсов в команде mrate2
    x_direction_t dir;
    
    int i = 0;
    char dbg[256];
    static int nSwitch = 0;
    
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
	switch (Device_blk.Str.TermoMode&0x0001) {
	case TERMO_ON:
	case TERMO_ON_NUMB_OFF:
	case TERMO_ON_DYNAMIC_ONLY:
	case TERMO_ON_DYNAMIC_ONLY_NUMB_OFF:
	case TERMO_ON_STATIC_ONLY:
	case TERMO_ON_STATIC_ONLY_NUMB_OFF:
		//e. accumulation of the value of thermocompensation from request to request 
		TermoCompens_Sum += thermo_CalcCompens(); 
	break;

	case TERMO_OFF:
	default:
		//e. thermocompensation is disable, therefore its part is equal to zero 
		TermoCompens_Sum = 0;
	break;
	}
    nSwitch++;
    
    //e. selecting display mode
    switch (g_gld.RgConB.word) {
    case RATE_VIBRO_1:
        
        // Доработка для команды M_Delay
        if(g_gld.pulses.vibro1.idelay_max!=0){
            //save to buf Dif_Curr_32
            g_gld.pulses.vibro1.delay_buff[g_gld.pulses.vibro1.idelay_curr] = Dif_Curr_32;
            g_gld.pulses.vibro1.idelay_curr++;
            if(g_gld.pulses.vibro1.idelay_curr>=g_gld.pulses.vibro1.idelay_max){
                g_gld.pulses.vibro1.idelay_curr = 0;
            }
            Dif_Curr_32 = g_gld.pulses.vibro1.delay_buff[g_gld.pulses.vibro1.idelay_curr];
        }
    
        if (Latch_Rdy) {
            dif_Curr_32_Ext = interpolation(Dif_Curr_32, LatchPhase );
            //in Latch_Event it's indicator of latch appearing 
            LatchPhase = INT32_MAX;	  

            if(nSwitch>10000){
                nSwitch = 0;
                DBG2(&g_gld.cmd.dbg.ring_out,dbg,64
                        ,"dif=%u TermoComp=%u\n\r"
                        ,dif_Curr_32_Ext
                        ,TermoCompens_Sum
                    );
            }
            
            //e. add to the accumulated sum the interpolated sample of an external latch 	
            //e. substract the accumulated termocompensational part from the accumulated number
            // PSdif_sum_Vib_32 += dif_Curr_32_Ext;             
            PSdif_sum_Vib_32 += dif_Curr_32_Ext - TermoCompens_Sum;
            //e. receive last data
            PSdif_sum_Vib_64 += dif_Curr_32_Ext - TermoCompens_Sum; 
            //e. nulling the accumulated termocompenstion for beginning of the 
            //new cycle of accumulation
            TermoCompens_Sum = 0; 
            
            //e. preparing number for output
            Output.Str.BINS_dif = PSdif_sum_Vib_32;	  
            Output.Str.PS_dif = Output.Str.BINS_dif >> 16;	
            Output.Str.SF_dif = PSdif_sum_Vib_64; 
                                                                    
            if ((Device_Mode == DM_EXT_LATCH_DELTA_BINS_PULSE) || \
               ((Device_Mode == DM_EXT_LATCH_DELTA_SF_PULSE) && Ext_Latch_ResetEnable)){	 
                //e. to initialize a new external latch cycle 
                PSdif_sum_Vib_32 = 0;      
                PSdif_sum_Vib_64 = 0;					
            }else{
                ResetBitsOfWord(&PSdif_sum_Vib_32, 16);	 
            }
                        
            //dif_Curr_32_Ext;
            dif_Curr_32_Ext = Dif_Curr_32 - temp22;
            //e. preserve rest of counters difference for next measure cycle: 
            //PSdif_sum_Vib_32 += Dif_Curr_32 - dif_Curr_32_Ext;
            PSdif_sum_Vib_32 +=  dif_Curr_32_Ext;                
            PSdif_sum_Vib_64 +=  dif_Curr_32_Ext;									 
        } else {	
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
        g_gld.pulses.reper_meandr.cnt_iter ++;
		
        dir = qei_get_direction();
        /*
        //if first direction _x_minus - skip this iteration
        if( (dir==_x_plus)
            &&( (data_Rdy & WHOLE_PERIOD)==0 )
            &&(g_gld.pulses.reper_meandr.cnt_mns==REPER_MEANDR_CNT_PLS_MIN_INIT) 
            &&(g_gld.pulses.reper_meandr.cnt_pls==REPER_MEANDR_CNT_PLS_MIN_INIT) 
        ){
            g_gld.pulses.reper_meandr.halfQEIPeriod = 0;
            Latch_Rdy = 0;	
			data_Rdy &= ~RESET_PERIOD;//reset bits  	 
            break;
        }*/
    
        /*if (dir==_x_plus){
            --sign_accum;
        }else{
            ++sign_accum;
        }
				
        Output.Str.WP_scope2 = sign_accum;*/
        
        /////////////////////////////////////////////////
        if (data_Rdy & HALF_PERIOD) { 
					
            g_gld.pulses.reper_meandr.cnt_dif = 
                (g_gld.pulses.reper_meandr.cnt_curr - g_gld.pulses.reper_meandr.cnt_prev); 
            g_gld.pulses.reper_meandr.cnt_prev = g_gld.pulses.reper_meandr.cnt_curr;
            Cnt_Overload(g_gld.pulses.reper_meandr.cnt_dif, INT32MAX_DIV2, INT32MIN_DIV2);
            
            //e. "+" direction 
            if (dir==_x_plus) {
                g_gld.pulses.reper_meandr.cnt_pls = L_abs(g_gld.pulses.reper_meandr.cnt_dif);
            } else {										
                g_gld.pulses.reper_meandr.cnt_mns = L_abs(g_gld.pulses.reper_meandr.cnt_dif);
            }
			
            //sign_accum = 0;
            //e. period of vibro elapsed
            if (data_Rdy & WHOLE_PERIOD) {
                last_Cnt_Plus = g_gld.pulses.reper_meandr.cnt_pls;
                dif_sum_32 += g_gld.pulses.reper_meandr.cnt_pls - g_gld.pulses.reper_meandr.cnt_mns;                

                if (g_gld.internal_latch.work_period == 0){ 
                    //Rate2
                    Latch_Rdy = 1;										
                }	                            
            }
					
			data_Rdy &= ~RESET_PERIOD;//reset bits  
        }//HALF_PERIOD

        /////////////////////////////////////////////////
        //e it's time for output 
		if (Latch_Rdy) {
            LatchPhase = INT32_MAX;

            cntPls_sum_32 += last_Cnt_Plus - preLast_Cnt_Plus;             
            g_gld.pulses.reper_meandr.cnt_delta = dif_sum_32 + (cntPls_sum_32>>1);     			
            ResetBitsOfWord(&cntPls_sum_32, 1); 
            dif_sum_32 = 0;												
            preLast_Cnt_Plus = last_Cnt_Plus;	
			
            Output.Str.Cnt_Dif = g_gld.pulses.reper_meandr.cnt_delta;
            g_gld.pulses.reper_meandr.cnt_delta = 0;            

            /**
            Проверка на ассиметрию характеристики
            // Вывод текущего угла
            double s = -0.5 * m_counters[0].A;
            for ( int i=0; i<CountOfPoints; i++ )
            {
                dataIn[i].y = s;
                if ( i%2 == 0 )
                    s += double(m_counters[i/2].A);
                else
                    s -= double(m_counters[i/2].B);
            }
            *
            if(g_gld.pulses.reper_meandr.curr_angle.flags.bit.measure==0){
                g_gld.pulses.reper_meandr.curr_angle.s += g_gld.pulses.reper_meandr.cnt_pls;
                g_gld.pulses.reper_meandr.curr_angle.s_pls = g_gld.pulses.reper_meandr.curr_angle.s;
                g_gld.pulses.reper_meandr.curr_angle.s -= g_gld.pulses.reper_meandr.cnt_mns;
                g_gld.pulses.reper_meandr.curr_angle.s_mns = g_gld.pulses.reper_meandr.curr_angle.s;
                //находим дельту
                if(g_gld.pulses.reper_meandr.curr_angle.s_mns_prev!=0){
                    if( g_gld.pulses.reper_meandr.curr_angle.s_mns_delta <
                        abs(g_gld.pulses.reper_meandr.curr_angle.s_mns_prev - g_gld.pulses.reper_meandr.curr_angle.s_mns) )
                    g_gld.pulses.reper_meandr.curr_angle.s_mns_delta =
                        abs(g_gld.pulses.reper_meandr.curr_angle.s_mns_prev - g_gld.pulses.reper_meandr.curr_angle.s_mns);
                }
                if(g_gld.pulses.reper_meandr.curr_angle.s_pls_prev!=0){
                    if( g_gld.pulses.reper_meandr.curr_angle.s_pls_delta <
                        abs(g_gld.pulses.reper_meandr.curr_angle.s_pls_prev - g_gld.pulses.reper_meandr.curr_angle.s_pls) )
                    g_gld.pulses.reper_meandr.curr_angle.s_pls_delta =
                        abs(g_gld.pulses.reper_meandr.curr_angle.s_pls_prev - g_gld.pulses.reper_meandr.curr_angle.s_pls);
                }
                //обновляем прошлые значения
                g_gld.pulses.reper_meandr.curr_angle.s_mns_prev = g_gld.pulses.reper_meandr.curr_angle.s_mns;
                g_gld.pulses.reper_meandr.curr_angle.s_pls_prev = g_gld.pulses.reper_meandr.curr_angle.s_pls;
                
                //если дельты просчитаны определим разницу
                if( (g_gld.pulses.reper_meandr.curr_angle.s_pls_delta!=0)
                    && (g_gld.pulses.reper_meandr.curr_angle.s_mns_delta!=0)
                ){
                    percent = (float)g_gld.pulses.reper_meandr.curr_angle.s_pls_delta/
                        (float)g_gld.pulses.reper_meandr.curr_angle.s_mns_delta ;
                    
                    if( (percent>2.0) ||(percent<0.5) ){
                        g_gld.pulses.reper_meandr.curr_angle.flags.bit.inverse=1;
                    }else {
                        //g_gld.pulses.reper_meandr.curr_angle.flags.bit.inverse=0;
                    }
                    //g_gld.pulses.reper_meandr.curr_angle.flags.bit.measure = 1;
                }
            }*/
            /*
            Delta_pls_i = A_i - B_i-1
            Delta_mns_i = A_i - B_i
            Delta_pls_i/Delta_mns_i < 2.0
            *
            if(
                (g_gld.pulses.reper_meandr.curr_angle.cnt_mns_prev!=REPER_MEANDR_CNT_PLS_MIN_INIT) 
                &&(g_gld.pulses.reper_meandr.curr_angle.cnt_pls_prev!=REPER_MEANDR_CNT_PLS_MIN_INIT) ) {
                percent = (float)fabs(g_gld.pulses.reper_meandr.cnt_pls - g_gld.pulses.reper_meandr.curr_angle.cnt_mns_prev) /
                        (float)fabs(g_gld.pulses.reper_meandr.cnt_pls - g_gld.pulses.reper_meandr.cnt_mns) ;
                g_gld.pulses.reper_meandr.curr_angle.flags.bit.measure = 0;
                g_gld.pulses.reper_meandr.curr_angle.flags.bit.ready = 1;
            }else{
                percent = 0.0;
            }
            //data for Rate2
            if( (percent>=2.0)&&(g_gld.pulses.reper_meandr.curr_angle.flags.bit.ready==1) ){
                Output.Str.Cnt_Mns = g_gld.pulses.reper_meandr.curr_angle.cnt_mns_prev;            
                Output.Str.Cnt_Pls = g_gld.pulses.reper_meandr.cnt_pls;
            }else
            {
                Output.Str.Cnt_Mns = g_gld.pulses.reper_meandr.cnt_mns;            
                Output.Str.Cnt_Pls = g_gld.pulses.reper_meandr.cnt_pls;
            }
            /***/
            Output.Str.Cnt_Mns = g_gld.pulses.reper_meandr.cnt_mns;            
            Output.Str.Cnt_Pls = g_gld.pulses.reper_meandr.cnt_pls;
            /***/

            //update previous value
            g_gld.pulses.reper_meandr.curr_angle.cnt_mns_prev = g_gld.pulses.reper_meandr.cnt_mns;
            g_gld.pulses.reper_meandr.curr_angle.cnt_pls_prev = g_gld.pulses.reper_meandr.cnt_pls;
            
            //for dbg
            g_gld.pulses.reper_meandr.cnt_int_sum = g_gld.pulses.reper_meandr.cnt_int;
            g_gld.pulses.reper_meandr.cnt_int = 0;   
            
            /**
            DBG2(&g_gld.cmd.dbg.ring_out,dbg,256,"inverse:%d percent:%2.4f\n\r"
                ,g_gld.pulses.reper_meandr.curr_angle.flags.bit.inverse
                ,percent
                );
           /* */
            //enable cyclic transmittion flag for Rate2    
            if( g_gld.cmd.trm_cycl != g_gld.cmd.trm_cycl_sync ){
                g_gld.cmd.trm_cycl = g_gld.cmd.trm_cycl_sync; 
            }
		}//latch_rdy		 
        break;
    }	 
} // clc_Pulses