

extern uint32_t L_Vibro;
extern uint32_t T_Vibro;
extern int 	F_ras;          //e. really output frequency, divided by 16  (for transmission) 
extern int 	T_VB_pll;       //e. PD output of the frequency regulator of the dither drive  //r. выход ФД регулятора частоты вибропривода


void init_Dither_reg(void); 

void clc_Dith_regulator(void);
void clc_OutFreq_regulator(void);
