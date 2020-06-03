#include "mathDSP.h"
#include "SIP.h"
#include "CyclesSync.h"
#include "CntrlGLD.h"
#include "ThermoCalc.h"
//#include "el_lin.h"

#include "hardware/hardware.h"
#include "core/global.h"

uint32_t	 Old_Cnt_Vib = 0;
uint32_t	 Old_Cnt = 0;
int32_t	 RefMeand_Cnt_Dif;
int32_t	 PSdif_sum_Vib_32 = 0;
__int64	 PSdif_sum_Vib_64 = 0;
int32_t	 dif_Curr_32_Ext; 		//e. difference(number) for the external latch mode //r. разность (число) для режима внешней защелки
int32_t	 dif_Curr_32_previous; 	//e. Previous (in comparison with Dif_Curr_32) number //r. предыдущее (по сравнению с Dif_Curr_32) число
int32_t	temp22=0;
//+++++++++++++++++++++++++++++++INPUT DATA++++++++++++++++++++++++++++++++++++++++++++++++
uint32_t	Curr_Cnt_Vib;
uint32_t 	Cnt_curr;

//+++++++++++++++++++++++++++++++variables for output++++++++++++++++++++++++++++++++++++++
int32_t	 	Dif_Curr_Vib;	    //e. current difference output for dithering control in LightUp mode and Dither regulator
int32_t	    Dif_Curr_32;		//e. current difference without dithering for dithering control

#if (defined GLOBALRATE)
//variables for rate mode
    int32_t	 cntPls_sum_32;
     int32_t	 last_Cnt_Plus;
     int32_t	 dif_sum_32;
     int32_t	 Cnt_Pls;
     int32_t	 Cnt_Mns;
     int32_t	 preLast_Cnt_Plus;
    // uint32_t   sumCnt_Mns = 0;	   
     //uint32_t   sumCnt_Pls = 0;
  extern int32_t dif_cur_test[30];
  extern   unsigned ii;
     uint32_t halfPeriod = 0;
#endif

void ResetBitsOfWord(int * x32, int truncate_bits)
{
	int hi_part;

	hi_part = *x32 >> truncate_bits;
	*x32 -= hi_part << truncate_bits; //e. //r. оставляем младшие 16 бит
} 
/******************************************************************************
** Function name:		interpolation
**
** Descriptions: Number interpolation for external latch moment appearing		
**
** Returned value:	Number in moment of external latch	appearing 	
** parameters:		y_curr - current number, y_prev - number at one cycle before time
**					x_interp - moment of external latch appearing,
					
** 	Precision of interpolation is 1/8 of impulse (3 digits after point in 14.18 format)
******************************************************************************/
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
/******************************************************************************
** Function name:		clc_Pulses
**
** Descriptions: 	Processing of information from SPOI		
**
** parameters:	None		
** Returned value:	None	
** 
******************************************************************************/
void clc_Pulses()
{
    static   int32_t	 cntPls_sum_32;
    static   int32_t	 last_Cnt_Plus;
    static   int32_t	 dif_sum_32;
    static   int32_t	 Cnt_Pls;
    static   int32_t	 Cnt_Mns;
    static   int32_t	 preLast_Cnt_Plus;

    //e. accumulated number of pulses
    Dif_Curr_Vib = Curr_Cnt_Vib - Old_Cnt_Vib; 			 

    Cnt_Overload(Dif_Curr_Vib, INT32MAX_DIV2, INT32MIN_DIV2);
    
    //e. save current number of pulses
    Old_Cnt_Vib = Curr_Cnt_Vib;	

    Dif_Curr_32 = VibroReduce(Dif_Curr_Vib << SHIFT_TO_FRACT); //e. Precision of filtration is 1/(2^18)					
 
    //e. selecting display mode in the Rate mode
    switch (RgConB) {
   		case RATE_VIBRO_1:
			if (Latch_Rdy) {	
                dif_Curr_32_Ext = interpolation(Dif_Curr_32, LatchPhase ); 
                //#NDA 20200304 temporarly comented
				//Output.Str.Tmp_Out[2] = (int)((Curr_Cnt_Vib>>16)& 0xffff); 
				//Output.Str.Tmp_Out[3] = (int)(Curr_Cnt_Vib & 0xffff); 
                
				//e. add to the accumulated sum the interpolated sample of an external latch  //r. добавляем к накопленной сумме интерполированный отсчет внешней защелки
				PSdif_sum_Vib_32 += dif_Curr_32_Ext; // PSdif_sum_Vib_32 += dif_Curr_32_Ext; 
				PSdif_sum_Vib_64 += dif_Curr_32_Ext; //e. receive last data
				//count--;
	
				//e. preparing number for output //r. подготовить число для выдачи
				Output.Str.BINS_dif = PSdif_sum_Vib_32 - TermoCompens_Sum;	 //e. substract the accumulated termocompensational part from the accumulated number //r. из накопленного числа вычитаем накопленную термокомпенсац. составляющую
				Output.Str.PS_dif = Output.Str.BINS_dif >> 16;	
				LatchPhase = INT32_MAX;	 //in Latch_Event it's indicator of latch appearing
				Output.Str.SF_dif = PSdif_sum_Vib_64; 
				TermoCompens_Sum = 0;  //e. nulling the accumulated termocompenstion for beginning of the new cycle of accumulation //r. обнуляем накопленную термокомпенсацию для начала нового цикла накопления
														 				
					if ((Device_Mode == DM_EXT_LATCH_DELTA_BINS_PULSE) || \
					   ((Device_Mode == DM_EXT_LATCH_DELTA_SF_PULSE) && Ext_Latch_ResetEnable))
						  {	 
							PSdif_sum_Vib_32 = 0;      //e. to initialize a new external latch cycle //r. инициализировать новый цикл измерения по защелке 
							PSdif_sum_Vib_64 = 0;					
						  }
					else
							ResetBitsOfWord(&PSdif_sum_Vib_32, 16);	  
							
				 dif_Curr_32_Ext = Dif_Curr_32 - temp22;//dif_Curr_32_Ext;

				PSdif_sum_Vib_32 +=  dif_Curr_32_Ext; // preserve rest of counters difference for next measure cycle: PSdif_sum_Vib_32 += Dif_Curr_32 - dif_Curr_32_Ext;
				PSdif_sum_Vib_64 +=  dif_Curr_32_Ext;
			// LPC_GPIO2->FIOCLR |= 0x00000004;		// turn off the LED 										 
			}
			else //e. the latch at this moment is abscent//r. защелки на настоящий момент не было 
			{	
				//e. continue accumulating the sum from internal samples //r. продолжаем накапливать сумму из внутренних отсчетов
				PSdif_sum_Vib_32 += Dif_Curr_32; // PSdif_sum_Vib_32 += Dif_Curr_32 ;			
				PSdif_sum_Vib_64 += Dif_Curr_32;	 //e. sum for scale factor measurement mode
			} 

			dif_Curr_32_previous = Dif_Curr_32; //e. save previous number //r. запоминаем предыдущее число 
            break;

  	 case RATE_REPER_OR_REFMEANDR:
         //e. calculate Cnt_Mns or Cnt_Pls
         if (data_Rdy & HALF_PERIOD) {
             Cnt_curr = Curr_Cnt_Vib;
            RefMeand_Cnt_Dif = Cnt_curr - Old_Cnt;
            Old_Cnt = Cnt_curr;
            //  LPC_GPIO2->FIOCLR = 0x10;
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

            Output.Str.Tmp_Out[2] = (int)((Curr_Cnt_Vib>>16)& 0xffff); 
            Output.Str.Tmp_Out[3] = (int)(Curr_Cnt_Vib & 0xffff); 

            cntPls_sum_32 += last_Cnt_Plus - preLast_Cnt_Plus;

            Output.Str.Cnt_Dif = dif_sum_32;            
            Output.Str.Cnt_Dif += cntPls_sum_32 >> 1;            

            dif_sum_32 = 0;				    //e. reset accumulators //r. и сбросить регистры накопления
            ResetBitsOfWord(&cntPls_sum_32, 1); 			//r. оставляем младший разряд, чтобы не терялась точность при суммировании
            preLast_Cnt_Plus = last_Cnt_Plus;	//e. current last sample became previous //r. текущий последний отсчет стал предыдущим

            Output.Str.Cnt_Mns = Cnt_Mns;//sumCnt_Mns;	   //e. rewrite accumulated data to output
            Output.Str.Cnt_Pls = Cnt_Pls;//sumCnt_Pls;	   

            //sumCnt_Mns = 0;	   //e. prepare for new accumulation
            //sumCnt_Pls = 0;
		}		 
        break;
    }	 
    //e. WP_scope1, WP_scope2 - variables for control in the Rate3 mode //r. WP_scope1, WP_scope2 - переменные для контроля в режиме rate3
 	Output.Str.WP_scope1 = Dif_Curr_Vib;  
 	Output.Str.WP_scope2 = (Dif_Curr_32 >> (SHIFT_TO_FRACT-2)); //r. 2 дробных разряда оставляем для большей наглядности при анализе сигнала rate3
} // clc_Pulses



 /******************************************************************************
**                            End Of File
******************************************************************************/
