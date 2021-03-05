//#include "CntrlGLD.h"
#include "core/gld.h"
#include "lpc17xx.h"
#include "Parameters.h"
#include "el_lin.h"


#include "hardware/hardware.h"
#include "core/global.h"
#include "core/const.h"

/******************************************************************************
** Function name:		LoadFlashParam
**
** Descriptions:		Load of the parameters from the flash memory 
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void LoadFlashParam(unsigned source) 
{
  if (source == FromFLASH) 
  {  
   	 while (LPC_GPDMACH3->CConfig & (1<<17));      	   //e. wait while DMA channel3 is busy

     if (LPC_GPDMA->RawIntErrStat & INT_ERR_TC) 
	 	 	LPC_GPDMA->IntErrClr |= INT_ERR_TC;	 	   //e. an errors found, reset interrupt

     else if (LPC_GPDMA->RawIntTCStat & INT_ERR_TC) 
	 {  //e. successful loading complete
	    LPC_GPDMA->IntTCClear |= INT_ERR_TC;
       if (Device_blk.Str.Header_Word == HEADER_WORD_CONST) //e. flash contain valid data  	       
	    return;
	 }
	 else 
	 {  					//e. transfer was not started, start it immediatly
	   FlashDMA_Init();
	  return;
	 }
   init_DefaultParam();		//e. load parameters by default 								 									 
  }								
   else  //e. source == ByDefault
	   init_DefaultParam();		//e. load parameters by default 

  blt_in_test = ((uint32_t)FIRMWARE_VER << 8) | (Device_blk.Str.Device_SerialNumber & 0x00FF);
}
/******************************************************************************
** Function name:		init_DefaultParam
**
** Descriptions:		Initialization of variables of computing procedures 
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void init_DefaultParam(void)      
{
	int i;

    Device_blk.Str.My_Addres = My_Addres_const; 

    Device_blk.Str.HF_ref = HF_REF_CONST;
    Device_blk.Str.HF_scl = HF_SCL_CONST; 
    Device_blk.Str.HF_scl_2 = Device_blk.Str.HF_scl;
    Device_blk.Str.HF_max = HF_MAX_CONST; 

    Device_blk.Str.HF_min = HF_MIN_CONST; 
    Device_blk.Str.RI_ref= RI_REF_CONST; 
    Device_blk.Str.RI_scl= RI_SCL_CONST; 

    Device_blk.Str.WP_ref = WP_REF_CONST; 
    Device_blk.Str.WP_scl = WP_SCL_CONST; 
    Device_blk.Str.WP_rup = WP_RUP_CONST; 
    Device_blk.Str.WP_rdw = WP_RDW_CONST; 
    Device_blk.Str.WP_mdy = WP_MDY_CONST; 

    Device_blk.Str.VB_phs = VB_PHS_CONST;
    Device_blk.Str.VB_scl = VB_SCL_CONST;
    Device_blk.Str.VB_Fdf_Lo = VB_FDF_LO_CONST;
    Device_blk.Str.VB_Fdf_Hi = VB_FDF_HI_CONST;
    Device_blk.Str.VB_Fsc = VB_FSC_CONST;
    Device_blk.Str.VB_Tmin = VB_TMIN_CONST;
    Device_blk.Str.VB_Tmax = VB_TMAX_CONST;

    Device_blk.Str.VB_N = T_VIB_START;
    Device_blk.Str.VB_Nmin = T_VIB_START - T_VIB_DELTA;
    Device_blk.Str.VB_Nmax = T_VIB_START + T_VIB_DELTA;
    Device_blk.Str.VB_tau = L_VIB_START;

    Device_blk.Str.VBN_Tzd = VBN_TZD_CONST;
    Device_blk.Str.VBN_Ran = VBN_RAN_CONST;
    Device_blk.Str.VBN_k = VBN_K_CONST;

    Device_blk.Str.PI_a0 = PI_A0_CONST;
    Device_blk.Str.PI_a1 = PI_A1_CONST;
    Device_blk.Str.PI_a2 = PI_A2_CONST;
    Device_blk.Str.PI_a3 = PI_A3_CONST;
    Device_blk.Str.PI_a4 = PI_A4_CONST;
    Device_blk.Str.PI_a5 = PI_A5_CONST;
    Device_blk.Str.PI_a6 = PI_A6_CONST;
                                       
    Device_blk.Str.PI_b1 = PI_B1_CONST;
    Device_blk.Str.PI_b2 = PI_B2_CONST;
    Device_blk.Str.PI_b3 = PI_B3_CONST;
    Device_blk.Str.PI_b4 = PI_B4_CONST;
    Device_blk.Str.PI_b5 = PI_B5_CONST;
    Device_blk.Str.PI_b6 = PI_B6_CONST;

    Device_blk.Str.PI_Fsc = PI_FSC_CONST;
    Device_blk.Str.PI_Fb0 = PI_FB0_CONST;
    Device_blk.Str.PI_scl = PI_SCL_CONST;
    Device_blk.Str.PI_bia = PI_BIA_CONST;

    for (i = 0; i <6 ; i++) {
       Device_blk.Str.Tmp_scal[i] = TMP_SCALE;
       Device_blk.Str.Tmp_bias[i] = TMP_BIAS;
    }
    Device_blk.Str.WP_transition_step = WP_TRANS_STEP;
    Device_blk.Str.WP_reset = WP_RESET_CONST;
    Device_blk.Str.WP_reset2 = WP_RESET2_CONST;
    Device_blk.Str.Gain_Ph_A = G_PHOTO_STRA;
    Device_blk.Str.Gain_Ph_B = G_PHOTO_STRB;

    Device_blk.Str.Device_SerialNumber = DEVICE_SN;

} // init_DefaultParam
/******************************************************************************
** Function name:		FlashDMA_Init
**
** Descriptions:		Initialisation of DMA channel for flash reading 
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void FlashDMA_Init()
{
    LPC_GPDMACH3->CConfig &= ~DMAChannelEn; 

    LPC_GPDMA->IntTCClear = DMA3_IntTCClear;
    LPC_GPDMA->IntErrClr = DMA3_IntTCClear;

    /* Ch3 set for M2M transfer from Flash to RAM. */
    LPC_GPDMACH3->CSrcAddr = 0x40000;	   //e. address of device parameter block in flash memory (22 sec)
    LPC_GPDMACH3->CDestAddr = (uint32_t)&(Device_blk.Array);//e. address of device parameter block in RAM

    LPC_GPDMACH3->CControl = ((sizeof(Device_blk.Array))>>2)|SrcBSize_1 |DstBSize_1 
                            |SrcWidth_32b |DstWidth_32b |SrcInc |DstInc |TCIntEnabl;

    LPC_GPDMACH3->CConfig = MaskTCInt |MaskErrInt|DMA_MEMORY |DMA_MEMORY |(M2M << 11)| DMAChannelEn;	

    return;
}
/******************************************************************************/
