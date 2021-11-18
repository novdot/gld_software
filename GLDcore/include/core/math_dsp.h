/**

*/
#ifndef __CORE_MATH_DSP_H_INCLUDED__
#define __CORE_MATH_DSP_H_INCLUDED__

//#include  <math.h>
#include "dspfns.h"

//e. shift for converting integer to float (14.18) format 
#define SHIFT_TO_FRACT  (18) 
//e. conversion to the 14.18 format //r. преобразование в формат 14.18
#define	LONG_2_FRACT_14_18(long_var)	(long_var) * ((long int)1 << SHIFT_TO_FRACT);

typedef enum BAND_PASS_TYPE_{
    PLC
    , DUP
} BAND_PASS_TYPE;

#define	Saturation(Uin, UpSat, DownSat)	if (Uin > UpSat) Uin = UpSat; \
										if (Uin < DownSat) Uin = DownSat;

#define	Cnt_Overload(Uin, UpSat, DwnSat)	if (Uin > UpSat) Uin -= INT32_MAX; \
											if (Uin < DwnSat) Uin += INT32_MAX;

#define	Cnt_Overload_Unsigned(Uin, UpSat)	if (Uin > UpSat) Uin -= UpSat;

#define	CPL_reset_calc(U0, Kgrad, Ti, T0)	U0 + L_mult( Kgrad, (Ti - T0) )	

/**
* @brief HFO_MovAverFilt 
    Moving average filter for ammplitude signal filtration 
*/
int dsp_MovAverFilt (int );

/**
* @brief VibroReduce
    Routine for reduce of vibro
   @return Filtered magnitude
*/
int VibroReduce (int input);

/**
* @brief init_VibroReduce
    Prepare	coefficiennts and delay 
    line for vibro reduce filter
*/
void init_VibroReduce(void);

/**
* @brief PLC_PhaseDetFilt
    PLC phase detector
*/
int PLC_PhaseDetFilt (int input);

/**
* @brief DUP_Filt
    Filter for dither frequency regulator
   @return Filtered magnitude
*/
int DUP_Filt (int input);

/**
* @brief init_BandPass
    Initialization of IIR filters for PLC and DUP signals 
*/
void init_BandPass(double CenterFreq, double BandWidth, BAND_PASS_TYPE );

#endif //__CORE_MATH_DSP_H_INCLUDED__