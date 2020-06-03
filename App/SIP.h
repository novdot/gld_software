
#include "hardware/hardware.h"

#define	 INT32MAX_DIV2				1073741823	//e. 0.5*MAX_QEI_CNT
#define  INT32MIN_DIV2			   -1073741823	//e. -0.5*MAX_QEI_CNT

#define SHIFT_TO_FRACT				(18) //e. shift for converting integer to float (14.18) format  //r. сдвиг для преобразования целого числа к дробному формату 14.18

//#define OUTFREQ
//#define RATESIM
//#define GLOBALRATE
#define CONSTCYCLE
//#define DITHERSIM

extern  unsigned Cnt_curr;
extern  unsigned Curr_Cnt_Vib;
extern  int Dif_Curr_Vib;
extern 	int Dif_RG_vib_32;
extern 	int Dif_Curr_32;
extern 	__int64	 Dif_RG_vib_64;

extern	int  		WP_scope3, WP_scope4;
/*extern	int			Dif_Curr_Array[8];
extern	int			Dif_Filt_Array[8];	 */

