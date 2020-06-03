#include "stdlib.h"

#define NUM_OF_THERMOSENS	6
#define		DITH_VBN_SHIFT			2 //e. //r. определяет сдвиг (деление на 4) коэффициента деления вибропривода, чтобы иметь запас на регулирование

#define DEBUG
extern  unsigned int 	IsHeating;
extern 		 int	Temp_Aver; //e. the mean temperature for 1 Sec for T4 sensor 
extern 		 int 	Tmp_Out[NUM_OF_THERMOSENS];
extern		 int	StartTermoCompens;
extern		 int	TermoCompens_Sum;

void DithFreqRangeCalc(void);
void ThermoAverage(void);
int DynamicDeltaCalc(void);
