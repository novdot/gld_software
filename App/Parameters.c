//#include "CntrlGLD.h"
//#include "lpc17xx.h"
#include "Parameters.h"
//#include "el_lin.h"


#include "hardware/hardware.h"
#include "core/global.h"
#include "core/const.h"
#include "core/gld.h"

#define INT_ERR_TC		0x00000008
/******************************************************************************/
void params_load(unsigned source) 
{
    char dbg[64];
    int i = 0;
    switch(source){
        case _params_load_fash:
            //e. wait while DMA channel3 is busy
            /*while (LPC_GPDMACH3->CConfig & (1<<17));      	   

            //e. an errors found, reset interrupt
            if (LPC_GPDMA->RawIntErrStat & INT_ERR_TC){
                LPC_GPDMA->IntErrClr |= INT_ERR_TC;	 	   
            }else if (LPC_GPDMA->RawIntTCStat & INT_ERR_TC){
                //e. successful loading complete
                LPC_GPDMA->IntTCClear |= INT_ERR_TC;
                //e. flash contain valid data 
                if (Device_blk.Str.Header_Word == HEADER_WORD_CONST)  	       
                    return;
            } else {
                //e. transfer was not started, start it immediatly
                params_FlashDMA_Init();
                return;
            }*/
            params_load_flash();
            if (Device_blk.Str.Header_Word == HEADER_WORD_CONST) {	       
                return;
            }else{
                DBG1(&g_gld.cmd.dbg.ring_out,dbg,64,"Header_Word error %u",Device_blk.Str.Header_Word);
                params_load_default();
            }
            break;
            
        default:
            params_load_default();	
            break;
    }
    blt_in_test = ((uint32_t)FIRMWARE_VER << 8) | (Device_blk.Str.Device_SerialNumber & 0x00FF);
}
/******************************************************************************/
void params_load_default(void)      
{
	int i;

    Device_blk.Str.My_Addres = MY_ADDR_CONST; 

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
    Device_blk.Str.WP_start = 0;

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
    Device_blk.Str.Gain_Ph_A = G_PHOTO_STRA_CONST;
    Device_blk.Str.Gain_Ph_B = G_PHOTO_STRB_CONST;

    Device_blk.Str.Device_SerialNumber = DEVICE_SN_CONST;

} // init_DefaultParam
/******************************************************************************/
void params_FlashDMA_Init()
{
    LPC_GPDMACH3->CConfig &= ~DMAChannelEn; 

    LPC_GPDMA->IntTCClear = DMA3_IntTCClear;
    LPC_GPDMA->IntErrClr = DMA3_IntTCClear;

    /* Ch3 set for M2M transfer from Flash to RAM. */
    //0x40000;//e. address of device parameter block in flash memory (22 sec)
    LPC_GPDMACH3->CSrcAddr = MEMORY_COEF_MEM_START;
    LPC_GPDMACH3->CDestAddr = (uint32_t)&(Device_blk.Array);//e. address of device parameter block in RAM

    LPC_GPDMACH3->CControl = ((sizeof(Device_blk.Array))>>2)|SrcBSize_1 |DstBSize_1 
                            |SrcWidth_32b |DstWidth_32b |SrcInc |DstInc |TCIntEnabl;

    LPC_GPDMACH3->CConfig = MaskTCInt |MaskErrInt|DMA_MEMORY |DMA_MEMORY |(M2M << 11)| DMAChannelEn;	

    return;
}
/******************************************************************************/
void params_save2flash()
{
    //stop GLD
    gld_stop();
    NVIC_DisableIRQ(EINT3_IRQn);
    NVIC_DisableIRQ(TIMER0_IRQn);
    NVIC_DisableIRQ(TIMER3_IRQn);
    NVIC_DisableIRQ(PWM1_IRQn);
    LPC_PWM1->TCR = 0;
    Device_blk.Str.Header_Word = HEADER_WORD_CONST;
    //write to flash
    memory_write(
        MEMORY_COEF_SEC_START
        ,MEMORY_COEF_SEC_END
        ,0
        ,Device_blk.Array
        ,VARIABLE_COUNT
        );
}
/******************************************************************************/
void params_load_flash()
{
    memory_read(MEMORY_COEF_MEM_START,Device_blk.Array,VARIABLE_COUNT);
}