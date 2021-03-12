/**
  ******************************************************************************
  * @file    const.h
  *
  * @brief   core gld consts
  *
  * @author  Дмитрий Новиков novikov@elektrooptika.ru
  *
  ******************************************************************************
  * @attention
  *
  * в разработке
  *
  * <h2><center>&copy; 2020 Электрооптика</center></h2>
  ******************************************************************************
  */
#ifndef GLD_CONST_H_INCLUDED
#define GLD_CONST_H_INCLUDED

//e. constants for the piecewise-linear thermocompensation 
#define TERMO_FUNC_SIZE    14 //e. amount of the points of the table function of thermocompensation 
#define MAX_ORDER           9 //e. maximal value of order for the thermocompensation coefficients 
#define TSENS_NUMB          4 //e. number of the temperature sensor used for the thermocompensation

#define	DEVICE_SAMPLE_RATE_HZ		10000		//e. sampling frequency 10 kHz 
#define	DEVICE_SAMPLE_RATE_uks		100000000		//e. sampling frequency 10 kHz

	//e. *** Default device parameters
#define	DEVICE_SN			1		//e. device serial number = 01
#define	My_Addres_const		0	      //e.    - device own address
/******************************************************************************/
#define PARAMS_TEST
#if defined (PARAMS_TEST)
    #define	HF_REF_CONST	15000	
    #define	HF_SCL_CONST	0	    
    #define	HF_MIN_CONST	( (0x8365a0)>>4 )//-4896  
    #define	HF_MAX_CONST    -1327
    #define	HFO_SHIFT		16		
    
    #define	WP_REF_CONST	10       
    #define	WP_SCL_CONST	10	    
    #define	WP_MDY_CONST	0		
    #define	WP_RUP_CONST	0	
    #define	WP_RDW_CONST	0	
    
    #define	VB_PHS_CONST	-9     	
    #define	VB_SCL_CONST	256	
    #define	T_VIB_START     18960	
    								
    #define	T_VIB_DELTA	    1000	
    
    #define	VB_NMIN_CONST   12080	
    #define	VB_NMAX_CONST   64000	
    #define	VB_FDF_HI_CONST	3	    
    #define	VB_FDF_LO_CONST	3392  		
    #define	VB_FSC_CONST	-256	
    #define	VB_TMIN_CONST	100	
    #define	VB_TMAX_CONST	12772	
    #define	L_VIB_START 3324	

    #define	VBN_TZD_CONST 400     
    #define	VBN_RAN_CONST 350     
    #define	VBN_K_CONST 6000	
    
    #define	RI_REF_CONST	0	
    #define	RI_SCL_CONST	0	    
    
    #define	PI_FSC_CONST 128	    
    #define	PI_FB0_CONST 0	    
    #define	PI_SCL_CONST 256	    
    #define	PI_BIA_CONST 0	    
    
    #define	PI_A0_CONST	0
    #define	PI_A1_CONST	0
    #define	PI_A2_CONST	0
    #define	PI_A3_CONST	0
    #define	PI_A4_CONST	0
    #define	PI_A5_CONST	0
    #define	PI_A6_CONST	0
    #define	PI_B1_CONST	0
    #define	PI_B2_CONST	0
    #define	PI_B3_CONST	0
    #define	PI_B4_CONST	0
    #define	PI_B5_CONST	0
    #define	PI_B6_CONST	0
    
    #define TMP_SCALE 0x4000 
    #define TMP_BIAS 0
    #define	WP_RESET_CONST 7360
    #define WP_RESET2_CONST 29216
    #define WP_TRANS_STEP 32767	

    #define	G_PHOTO_STRA 200
    #define	G_PHOTO_STRB 200
    
    #define HEADER_WORD_CONST 0x55aa
#else
	//e. =============== parameters of HF regulator     
	#define	HF_REF_CONST	15080	//e. 1 - value of the reference 
	#define	HF_SCL_CONST	1	    // 0x64 //e. 2 - the gain factor (1.15)         
	#define	HF_MIN_CONST	-32668  //e. 3 - minimum of the output value on the regulator DAC (appropriate to maximal voltage on the HFO) 
	#define	HF_MAX_CONST    -17379	//e. 4 - maximum of the output value on the regulator DAC (appropriate to minimal voltage on the HFO) 
	#define	HFO_SHIFT		16		//e. number of digits of fractional part in 32-bit variable of the hf_reg32 varaible 

	//e. =============== parameters of the CPLC regulator
	#define	WP_REF_CONST	5       //e. 7 -  value of the reference                                              
	#define	WP_SCL_CONST	5	    //e. 8 - the gain factor (1.15) 
	#define	WP_MDY_CONST	30		//e. 9 - value of the reset delay                                      
	#define	WP_RUP_CONST	31936	//e. 10 - lower value of DAC adjustment (appropriate to minimal voltage on the heater)                                     
	#define	WP_RDW_CONST	11801	//e. 11 - upper value of the DAC adjustment (appropriate to maximal voltage on the heater) 
     	
 //e. =============== parameters of the dither drive regulator of the GLD
	#define	VB_PHS_CONST	4     	//e. 12 - the phase delay parameter of the dither drive PLL 
	#define	VB_SCL_CONST	1024	//e. 13 - the gain factor (1.15) of the dither drive PLL
	#define	T_VIB_START     16600	//e. 14 - divider for dither drive period (defines dither period)
									//e. _VB_N - the initial value of the oscillation period of the dither drive (406Hz-18916, 17067 - 450Hz)
	#define	T_VIB_DELTA	    1000	    //e. the range of the oscillation period of the dither drive (~ +/- 10 Hz)

	#define	VB_NMIN_CONST   12080	    //e. 15 - minimum of the output value of the oscillation period regulator 
	#define	VB_NMAX_CONST   64000	    //e. 16 - maximum of the output value of the oscillation period regulator 
	#define	VB_FDF_HI_CONST	3	    //e. 17 - adjusted output frequency (H)                     
	#define	VB_FDF_LO_CONST	0  		//e. 18 - (L) (double precision)                    
	#define	VB_FSC_CONST	-2000	//e. 19 - the gain factor of the output frequency regulator 
	#define	VB_TMIN_CONST	100	//e. 20 - minimum of the output value of the Tau regulator 
	#define	VB_TMAX_CONST	10022	//e. 21 - maximum of the output value of the Tau regulator 
	#define	L_VIB_START      5120	//e. 22 - pulse width of the dither drive (before noiseness)
								 //e. _VB_tau - initial pulse width of the dither drive   

	#define	VBN_TZD_CONST	500     //e. 23 - adjusted noise[??] period (user`s constant) 
	#define	VBN_RAN_CONST	400     //e. 24 - range of the random component of noiseness 
	#define	VBN_K_CONST  	7000	//e. 25 - adjusted noise constant (user`s constant) 

   //e. =============== parameters of DS power regulator
	#define	RI_REF_CONST	25600	//e. 5 - value of the reference                                             
	#define	RI_SCL_CONST	0	    //e. 6 - the gain factor (1.15) 
	
               
                //e. =============== parameters of processing of accurate data
	#define	PI_FSC_CONST	128	    //e.  26 - the S_ds scale parameter 
    #define	PI_FB0_CONST	0	    //e.  27 - the B_ds zero shift of the DS 
	#define	PI_SCL_CONST	256	    //e.  28 - the Scale scale coefficient of the gyro 
	#define	PI_BIA_CONST	0	    //e.  29 - the Bias zero shift of the gyro 

                //e. ===============  coefficients of the temperature correction ================= //r. ===============  믽𴨶馭򻠲歯池򳰭褐믰𥪶騠=================
	#define	PI_A0_CONST		0	//  30
	#define	PI_A1_CONST		0	//  31
	#define	PI_A2_CONST		0	//  32
	#define	PI_A3_CONST		0	//  33 
	#define	PI_A4_CONST		0	//  34
	#define	PI_A5_CONST		0	//  35
	#define	PI_A6_CONST		0	//  36
	#define	PI_B1_CONST		0	//  37
	#define	PI_B2_CONST		0	//  38
	#define	PI_B3_CONST		0	//  39
	#define	PI_B4_CONST		0	//  40
	#define	PI_B5_CONST		0	//  41
	#define	PI_B6_CONST		0	//  42

	#define TMP_SCALE		0x4000 // 43 - 54  Tmp_bias[6]; Tmp_scal[6]
	#define TMP_BIAS		0
	#define	WP_RESET_CONST 7360	// 55
	#define WP_RESET2_CONST 29216
	#define WP_TRANS_STEP	32767		
    
	//e. ================ initial gain factor of photodetector channels
	#define	G_PHOTO_STRA	60     // 56 
	#define	G_PHOTO_STRB  	60		// 57

	//e. =============== switch of the source of loading GLD variables block === 
	#define HEADER_WORD_CONST	0x55aa	//e. 58 - magic number flash validity 
#endif

#define  DITHER_REG_PERIOD		40	//e. period of dither regulator (in periods of dither frequency) 
#define  PLC_RESET_THRESHOLD 	(-3276) //e. correspond to the voltage +1.2 Volts 

	//e. minimal allowable output frequency, at which data are considered authentic  
    #define		F_RAS_MIN	    10000 	//e.  minimal output frequency at start
    #define		F_OUT_MIN		(5000>>4)  	//   5000 Ķ / 16
#define		F_OUT_NORM		(70000>>4)  	//  90000 Ķ / 16
#define		F_OUT_MAX		(300000>>4) 	// 300000 Ķ / 16

	//e. constants for the piecewise-linear thermocompensation 
#define 	TERMO_FUNC_SIZE		14 	//e. amount of the points of the table function of thermocompensation 
#define 	MAX_ORDER			9   //e. maximal value of order for the thermocompensation coefficients 
#define 	TSENS_NUMB			4   //e. number of the temperature sensor used for the thermocompensation
#define	    VALID_START_4SEC		3 //e. time after start, when temperature came authentic 

#define		N_START_MAX     	1 // 4       //e. count of attempts of device starting 
#define		LIGHT_UP_PULSE_WDTH 5000 //1000 //e. width of light-up pulse = 100 msec
#define		LIGHT_UP_PAUSE		1000 //e. pause after light-up = 100 msec
#define		LIGHT_UP_POLLING	10000 //e. time of waiting laser generation = 1 sec

#define		TS_MIN				-2500 	// minimal temperature for our thermal sensors
#define		TS_MAX				+13000  // maximal temperature for our thermal sensors
#define		TS_DIFF_MAX			1300	// maximal difference between 2 thermal sensors

#define		HFO_NEG_MIN			8738	// -4.5 V
#define		HFO_NEG_MAX			25122	// -10.5 V
#define		HFO_POZ_MIN			-32221	// +10.5 V
#define		HFO_POZ_MAX			-15837 	// +4.5 V

/////////////////////////////////////////////
#define		DITH_VBN_SHIFT			2 

////////////////////////////////////////////

	// Status word errors bits
#define	OUT_FREQ_ERROR		(0x8000)
#define	DITH_FREQ_ERROR		(0x4000)
#define	HFO_VOLT_ERROR		(0x2000)
#define	THERMO_RANGE_ERROR	(0x1000)
#define	THERMO_DIFF_ERROR	(0x0800)

////////////////////////////////////////////
//Loops bits
#define  LASER_ON        (0x0001)  		//< switch on/off laser-up
#define  HF_REG_ON       (0x0002)  		//< switch on/off the HFO regulator
#define  RI_REG_ON       (0x0004)  		//< switch on/off the DS power regulator
#define  WP_REG_ON       (0x0008)  		//< a mask of switch on/off of the CPLC regulator
#define  WP_SIN_ON       (0x0010)  		//< switch on/off search signal of the CPLC
#define  VB_TAU_ON       (0x0020)  		//< switch on/off amplitude regulator of the dither drive
#define  VB_FREQ_ON      (0x0040)  		//< switch on/off frequency regulator of the dither drive
#define  GLD_ON          (0x0080)  		//< switch on/off all GLD
/******************************************************************************/
//e.   device operation modes
#define  DM_INT_10KHZ_LATCH				1		//e. mode of internal latch 10 kHz 		
#define  DM_INT_LATCH_DELTA_PS			2
#define	 DM_INT_LATCH_DELTA_BINS		3
#define  DM_EXT_LATCH_DELTA_PS_PULSE	4		//e. mode of external latch with output of the Delta_PS command by pulse 
#define  DM_EXT_LATCH_DELTA_BINS_PULSE	6		//e. mode of external latch with output of the Delta_BINS command by pulse
#define  DM_EXT_LATCH_DELTA_SF_PULSE	7		//e. mode of Delta Scale factor 

//sync
#define HALF_PERIOD 0x00000004
#define WHOLE_PERIOD 0x00000008
#define RESET_PERIOD 0x0000000C
//////////////////////////////////////////////////

#define  	PI 	3.14159265


#endif //GLD_CONST_H_INCLUDED