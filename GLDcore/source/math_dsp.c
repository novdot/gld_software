#include "core/math_dsp.h"
#include "core/global.h"
#include "core/const.h"
#include "core/types.h"
#include "hardware/pwm.h"

#define	X	    0.98 //0.96 //0.86
#define	A0_HP   ((1 + X)/2) * 0x40000000
#define	A1_HP   (-(1 + X)/2) * 0x40000000
#define	B1_HP   (X * 0x80000000)/2

#define L_PLC   (3)
#define L_DUP   (3)
#define DIV_CONST	(768)
#define DIV_CONST2	(384)

#define	HALFINT	(16384) //???

int highPls = 0;
int lowPls = 0;
int BufInMovAverPls [67];
int BufInMovAverMns [67];
int BufInMovAverPls_2 [67];
int BufInMovAverMns_2 [67];
int hMovAver [67];

int aPLC[L_PLC], bPLC[L_PLC], aDUP[L_DUP], bDUP[L_DUP]; 
int aDUP_2[L_DUP] = {A0_HP, A1_HP, 0}, bDUP_2[L_DUP] = {0, B1_HP, 0};
/******************************************************************************/
void init_VibroReduce()
{
    unsigned int i = 0; 
    __int64 coeff = 0;

    //e. real expression is DEVICE_SAMPLE_RATE_HZ*Device_blk.Str.VB_N/7680000
    g_gld.Vibro_Filter_Aperture = Device_blk.Str.VB_N/DIV_CONST;		
    //e. add rounding to nearest integer	
    i = L_mult(g_gld.Vibro_Filter_Aperture,DIV_CONST2);	
    if ((Device_blk.Str.VB_N - i)>DIV_CONST2) g_gld.Vibro_Filter_Aperture++;  

    coeff = 0x7FFFFFFF/g_gld.Vibro_Filter_Aperture;

    for ( i=0; i < g_gld.Vibro_Filter_Aperture; i++){
        BufInMovAverPls[i] = 0;	 
        BufInMovAverMns[i] = 0;
        hMovAver[i]= coeff;
    }
    Vibro_2_CountIn = MULT_7680_12500/g_gld.Vibro_Filter_Aperture;
    Vibro_2_CountIn++;
}

/******************************************************************************/
int VibroReduce (int input)
{
    static unsigned  kIn = 0;
    unsigned  s;          
    __int64  outMns = 0;
    __int64  outPls = 0;
    BufInMovAverPls[kIn] = input;
    BufInMovAverMns[kIn] = -input;
    for (s=0; s<g_gld.Vibro_Filter_Aperture; s++){
        outPls += (__int64)hMovAver[s]*(__int64)BufInMovAverPls[s];
        outMns += (__int64)hMovAver[s]*(__int64)BufInMovAverMns[s];
    }
    highPls = (int)(outPls>>32);
    lowPls = (int)outPls;

    BufInMovAverPls_2[kIn] = (int)(outPls-outMns>>32);
    BufInMovAverMns_2[kIn] = -BufInMovAverPls_2[kIn];
    outPls = 0;
    outMns = 0;
    for (s=0; s<g_gld.Vibro_Filter_Aperture; s++) {
        outPls += (__int64)hMovAver[s]*(__int64)BufInMovAverPls_2[s];
        outMns += (__int64)hMovAver[s]*(__int64)BufInMovAverMns_2[s];
    }	 										 
    kIn++;
    if (kIn>(g_gld.Vibro_Filter_Aperture-1)) kIn = 0;

    return  (int)(outPls-outMns>>32);	 
}

/******************************************************************************/
int DUP_Filt (int input)
{
    static unsigned int kIn = 0;
    int ind;
    __int64 temp = 0;
    unsigned int i;
    static int BufInDUP_1 [L_DUP] = {0,0,0};
    static int BufInDUP_2 [L_DUP] = {0,0,0};
    //static __int64 BufOutDUP[L_DUP] = {0,0,0};

    if (kIn>(L_DUP-1)) kIn = 0;

    BufInDUP_1[kIn] = input;
    ind = kIn;
    BufInDUP_2[kIn] = 0;
    for (i=0; i<L_DUP; i++){
        temp += aDUP[i]*BufInDUP_1[ind];
        temp += bDUP[i]*BufInDUP_2[ind];
        if ((--ind) < 0) ind = L_DUP-1;
    }
    //take into account that filter coefficients are divided on 2
    BufInDUP_2[kIn] =(int)(temp>>14);	


    return (BufInDUP_2[kIn++]);
}
//-------------------------PLC phase detector----------------------------------
int PLC_PhaseDetFilt (int input)
{
    static unsigned kIn = 0;
    int ind;
    __int64	temp = 0;
    unsigned i;
    static int BufInPLC_1 [L_PLC] = {0,0,0};
    static int BufInPLC_2 [L_PLC] = {0,0,0};
    static int BufOutPLC [L_PLC] = {0,0,0};

    if (kIn>(L_PLC-1)) kIn = 0;

    BufInPLC_1[kIn] = input;
    ind = kIn;
    // BufInPLC_2[kIn] = 0;

    for (i=0; i<L_PLC; i++){
        temp += aPLC[i]*BufInPLC_1[ind];
        temp += bPLC[i]*BufInPLC_2[ind];
        if ((--ind) < 0) ind = L_PLC-1;
    }
    BufInPLC_2[kIn] =(int)(temp>>14);
    //2 section
    //  BufOutPLC[kIn] = 0;
    temp = 0;
    for (i=0; i<L_PLC; i++){
        temp += aPLC[i]*BufInPLC_2[ind];
        temp += bPLC[i]*BufOutPLC[ind];
        if ((--ind) < 0) ind = L_PLC-1;
    }
    BufOutPLC[kIn] =(int)(temp>>14);

    return (BufOutPLC[kIn++]);
}

/******************************************************************************/
void init_BandPass(double CenterFreq, double BandWidth, BAND_PASS_TYPE FiltType)
{
    double K, R, Cos_x_2, R_x_R; 

    R = 1.0 - 3.0 * BandWidth;
    R_x_R = R * R;
    Cos_x_2 = cos(2.0 * PI * CenterFreq) * 2.0;
    K = (1.0 - R * Cos_x_2 + R_x_R)/(2.0 - Cos_x_2);
    switch (FiltType){
    case PLC:
        aPLC[0] = (int)((1.0 - K)*HALFINT);
        aPLC[1] = (int)(((K - R) * Cos_x_2)*HALFINT);
        aPLC[2] = (int)((R_x_R - K)*HALFINT);
        bPLC[0] = 0;
        bPLC[1] = (int)((R * Cos_x_2)*HALFINT);
        bPLC[2] = (int)((- R_x_R)*HALFINT);
        break
            ;
    case DUP:
        aDUP[0] = (int)((1.0 - K)*HALFINT);
        aDUP[1] = (int)(((K - R) * Cos_x_2)*HALFINT);
        aDUP[2] = (int)((R_x_R - K)*HALFINT);
        bDUP[0] = 0;
        bDUP[1] = (int)((R * Cos_x_2)*HALFINT);
        bDUP[2] = (int)((- R_x_R)*HALFINT);   
        break;
    }
}

/******************************************************************************/
#define BUF_SIZE (64)
int HFO_MovAverFilt(int Input)
{   
    static __int64 smooth_HF = 0;
    static  int buffer_HF[BUF_SIZE] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    static unsigned i_HF = 0;

    smooth_HF -= buffer_HF[i_HF];
    buffer_HF[i_HF] = Input;
    smooth_HF += Input;

    i_HF++;
    i_HF &= (BUF_SIZE-1);

    //shift on additional 6 bits for smoothing 2^6 = 64
    return smooth_HF>>6;
}


