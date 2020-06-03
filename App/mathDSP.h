#include "dspfns.h"


typedef enum BAND_PASS_TYPE_{PLC, DUP} BAND_PASS_TYPE;

extern 	BAND_PASS_TYPE BandPassType;
extern unsigned int Vibro_Filter_Aperture;

#define  	PI 	3.14159265

	//e. constants for the filter in the CPLC regulator //r. константы для фильтра в контуре СРП
#define  NUMB_OF_BIQUADS_PLC	2
#define  TAPS_PLC   		 	5*NUMB_OF_BIQUADS_PLC
#define	 DELAY_LEN_PLC		 	(2*NUMB_OF_BIQUADS_PLC+2)

	//e. constants for the filter in the APS regulator //r. константы для фильтра в контуре ДУП
#define	 NUM_OF_BIQUADS_DUP		2
#define  TAPS_DUP   		 	5*NUM_OF_BIQUADS_DUP
#define	 DELAY_LEN_DUP		 	(2*NUM_OF_BIQUADS_DUP+2)

#define		X				0.98 //0.96 //0.86
#define		A0_HP			((1 + X)/2) * 0x40000000
#define		A1_HP			(-(1 + X)/2) * 0x40000000
#define		B1_HP			(X * 0x80000000)/2

#define 	MULT_7680_12500			26667

#define	Saturation(Uin, UpSat, DownSat)	if (Uin > UpSat) Uin = UpSat; \
										if (Uin < DownSat) Uin = DownSat;

#define	Cnt_Overload(Uin, UpSat, DwnSat)	if (Uin > UpSat) Uin -= INT32_MAX; \
											if (Uin < DwnSat) Uin += INT32_MAX;

#define	CPL_reset_calc(U0, Kgrad, Ti, T0)	U0 + L_mult( Kgrad, (Ti - T0) )	

int HFO_MovAverFilt (int );

int VibroReduce (int input);
void init_VibroReduce(void);
int PLC_PhaseDetFilt (int input);
int DUP_Filt (int input);
void init_BandPass(double CenterFreq, double BandWidth, BAND_PASS_TYPE );

