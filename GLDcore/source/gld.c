#include "core/gld.h"
#include "core/global.h"
#include "core/ignit.h"
#include "core/command_handlers.h"
#include "hardware/hardware.h"

//TODO
#include  "Cyclessync.h"

x_bool_t g_bIsStartRequested = _x_false; //start
x_bool_t g_bIsStopRequested = _x_false;
extern uint32_t delay_output;

/******************************************************************************/
x_bool_t ignit_is_start_request(void)
{
    return g_bIsStartRequested;
}
/******************************************************************************/
void ignit_set_start_request(x_bool_t flag)
{
    g_bIsStartRequested = flag;
}
/******************************************************************************/
void gld_start(void)
{
    gld_error_t nErrorIteration = _gld_error_ok;
    if(ignit_is_start_request()==_x_false) return;
    
    //gld_loop_all_regs_set(_x_open);
    
    nErrorIteration = ignit_start();
    switch(nErrorIteration) {
        case _gld_wrg_wait: return;
        default:
            break;
    }
fail:
end:
    //gld_loop_all_regs_set(_x_close);
    ignit_set_start_request(_x_false);
    //e. re-initialise gain factors of photodetector channels
    hardware_photo_set(Device_blk.Str.Gain_Ph_A, Device_blk.Str.Gain_Ph_B);
}
/******************************************************************************/
x_bool_t gld_is_stop_request(void)
{
    return g_bIsStopRequested;
}
/******************************************************************************/
void gld_set_stop_request(x_bool_t flag)
{
    g_bIsStopRequested = flag;
}
/******************************************************************************/
void gld_stop(void)
{
    if(gld_is_stop_request()==_x_false) return;
    
    gld_set_stop_request(_x_false);
    ignit_set_start_request(_x_false);
    //gld_loop_all_regs_set(_x_open);
}
/******************************************************************************/
void gld_status(void)
{
    /*DM_EXT_LATCH_DELTA_SF_PULSE*/
    static uint32_t Device_Mode_Old = DM_INT_10KHZ_LATCH;

    //if ( gld_loop_is_gld_reg() == _x_open ) {
        //gld_loop_all_regs_set(_x_open);
     //   gld_stop();
    //}
    //devmode has been changed
    if (Device_Mode_Old != Device_Mode)	 {
        //e. mode is changed successful
        if (SwitchMode()) {
            Device_Mode_Old = Device_Mode;
            //e. enable answer
        if (Device_Mode_Old == DM_INT_10KHZ_LATCH)
            trm_ena = 1;							   
        }	
    }	 
}
/******************************************************************************/
void gld_output(void)
{
    x_uint8_t dbg[64];
    //latch appeared
    if (Latch_Rdy) {
        //enable packet generation
        if (g_gld.cmd.trm_cycl) trm_ena = 1;   

        switch (CMD_Mode) {
            case 1: //e. Delta _PS mode
                command_cmd_DELTA_PS_EXEC();
            break;

            case 5: //rate mode
                
                //e. reset bits of current command code settings of periodicity and transfer rate
                //r. сбросить в текущем коде команды биты установки периодичности и скорости передачи
                CMD_Code &= 0xff1f;
            
                /*
                //e. is it the Rate2 mode?
                //r. это режим Rate2?
                if (CMD_Code == SUBCMD_M_RATE2){
                    trm_ena = 1;  
                }	*/								
									               	
            break;	
        }
    }
}

/******************************************************************************/
void gld_control(void)
{
    gld_status(); //e. checking the device status
    gld_stop();   //e. stop the device, if necessary 
	ignit_start();
    gld_output();
}
/******************************************************************************
void gld_loop_laser_set(x_loop_t flag) { g_gld.RgConA.bit.Las = flag;}
void gld_loop_hf_reg_set(x_loop_t flag) { g_gld.RgConA.bit.HFG = flag;  }
void gld_loop_ri_reg_set(x_loop_t flag) { g_gld.RgConA.bit.Rlr = flag;}
void gld_loop_wp_reg_set(x_loop_t flag) { g_gld.RgConA.bit.WPr = flag;}
void gld_loop_wp_sin_set(x_loop_t flag) { g_gld.RgConA.bit.WPs = flag;}
void gld_loop_vb_tau_set(x_loop_t flag) { g_gld.RgConA.bit.VBT = flag;}
void gld_loop_vb_freq_set(x_loop_t flag) { g_gld.RgConA.bit.VBF = flag;}
void gld_loop_gld_reg_set(x_loop_t flag) { g_gld.RgConA.bit.GLD = flag; }
/******************************************************************************
void gld_loop_all_regs_set(x_loop_t flag)
{
    gld_loop_laser_set(flag);
    gld_loop_hf_reg_set(flag);
    gld_loop_ri_reg_set(flag);
    gld_loop_wp_reg_set(flag);
    gld_loop_wp_sin_set(flag);
    gld_loop_vb_tau_set(flag);
    gld_loop_vb_freq_set(flag);
    gld_loop_gld_reg_set(flag);
}

/******************************************************************************
x_loop_t gld_loop_is_laser() {return g_gld.RgConA.bit.Las;}
x_loop_t gld_loop_is_hf_reg() {return g_gld.RgConA.bit.HFG;}
x_loop_t gld_loop_is_ri_reg() {return g_gld.RgConA.bit.Rlr;}
x_loop_t gld_loop_is_wp_reg() {return g_gld.RgConA.bit.WPr;}
x_loop_t gld_loop_is_wp_sin() {return g_gld.RgConA.bit.WPs;}
x_loop_t gld_loop_is_vb_tau() {return g_gld.RgConA.bit.VBT;}
x_loop_t gld_loop_is_vb_freq() {return g_gld.RgConA.bit.VBF;}
x_loop_t gld_loop_is_gld_reg() {return g_gld.RgConA.bit.GLD;}
/******************************************************************************/
