/**
Обработка и подсчет импульсов
*/
#ifndef _CORE_SIP_H_INCLUDED__
#define _CORE_SIP_H_INCLUDED__

#define	INT32MAX_DIV2   (1073741823)	//e. 0.5*MAX_QEI_CNT
#define INT32MIN_DIV2   (-1073741823)	//e. -0.5*MAX_QEI_CNT
#define SHIFT_TO_FRACT  (18) //e. shift for converting integer to float (14.18) format 

void ResetBitsOfWord(int * x32, int truncate_bits);

/**
    @brief interpolation
    Number interpolation for external latch moment appearing
    Precision of interpolation is 1/8 of impulse (3 digits after point in 14.18 format)
    @return 
    Number in moment of external latch	appearing 	
    y_curr - current number, y_prev - number at one cycle before time
    x_interp - moment of external latch appearing,
*/
int interpolation(int y_curr, int x_interp);

/**
    @brief clc_Pulses
    Processing of information from SPOI
    @return 
*/
void clc_Pulses();

#endif //_CORE_SIP_H_INCLUDED__