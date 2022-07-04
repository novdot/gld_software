/**
* @brief Логика формирования поджига лазера
**/

#include "core/ignit.h"
#include "core/gld.h"
#include "core/global.h"
#include "hardware/hardware.h"

gld_error_t handle_pulse(void);
gld_error_t handle_pause(void);
gld_error_t handle_polling(void);
gld_error_t ignit_single(void);
void ignit_set_started(x_bool_t flag);
void ignit_set_busy(x_bool_t flag);

x_bool_t g_bIsStarted = _x_false; //on start procedure
x_bool_t g_bIsRequested = _x_false; //single ignit
x_bool_t g_bIsBusy = _x_false; //on single ingnit procedure

extern OUTPUT Output;
extern TDEVICE_BLK Device_blk; 

#define	IGNIT_POLLING_TIME_SHIFT	(13) //e. time of waiting laser generation = 0.8192 sec (1<<IGNIT_POLLING_TIME_SHIFT)
#define CONVERT_CURR(x) ((((x*15305)>>15)-713))
int g_nStatusTime = 0;

/******************************************************************************/
x_bool_t ignit_is_started()
{
    return g_bIsStarted;
}
/******************************************************************************/
void ignit_set_started(x_bool_t flag)
{
    g_bIsStarted = flag;
}
/******************************************************************************/
x_bool_t ignit_is_request(void)
{
    return g_bIsRequested;
}
/******************************************************************************/
void ignit_set_request(x_bool_t flag)
{
    g_bIsRequested = flag;
}

/******************************************************************************/
x_bool_t ignit_is_busy()
{
    return g_bIsBusy;
}
/******************************************************************************/
void ignit_set_busy(x_bool_t flag)
{
    g_bIsBusy = flag;
}
/******************************************************************************/
gld_error_t handle_pulse(void)
{
    //first time
    if(ignit_is_busy()==_x_false) {
        g_nStatusTime = LIGHT_UP_PULSE_WDTH;
        hardware_lightup_on();
    }
    
    g_nStatusTime--;
    if(g_nStatusTime>0) {
        ignit_set_busy(_x_true);
        return _gld_wrg_wait;
    }
    
    //end
    ignit_set_busy(_x_false);
    hardware_lightup_off();
    return _gld_error_ok;
}

/******************************************************************************/
gld_error_t handle_pause(void)
{
    if(ignit_is_busy()==_x_false) {
        g_nStatusTime = LIGHT_UP_PAUSE;
    }
    
    g_nStatusTime--;
    if(g_nStatusTime>0) {
        ignit_set_busy(_x_true);
        return _gld_wrg_wait;
    }
    
    //end
    ignit_set_busy(_x_false);
    return _gld_error_ok;
}

/******************************************************************************/
gld_error_t handle_polling(void)
{
	static int Curr_average = 0;
    if(ignit_is_busy()==_x_false) {
			  Curr_average = 0;
        g_nStatusTime = LIGHT_UP_POLLING;
    }
    
    g_nStatusTime--;
    if(g_nStatusTime>0) {
			  Curr_average += g_input.word.in2;
        ignit_set_busy(_x_true);
        return _gld_wrg_wait;
    }
		
		ignit_set_busy(_x_false);
    if((Curr_average>>IGNIT_POLLING_TIME_SHIFT) > CONVERT_CURR(Device_blk.Str.Curr_work)){
			Curr_average = 0;
			  return	_gld_error_ok;
		}
			Curr_average = 0;
    return _gld_error_no_ignit;
}

/******************************************************************************/
gld_error_t ignit_start(void)
{
    static x_uint8_t nIteration = 0;
	  static x_bool_t first_ignition = _x_true;
	
    gld_error_t nErrorIteration = _gld_error_no_ignit;
    
    //if no request to light-up go away
    if(ignit_is_request()==_x_false) return _gld_wrg_wait;
    
    if(ignit_is_started()==_x_false) {
        hardware_backlight_on();
        ignit_set_started(_x_true);
        nIteration = 0;
        ignit_set_busy(_x_false);
    }
    //if(ignit_is_busy()==_x_true) return _gld_wrg_wait;
    
    nErrorIteration = ignit_single();
    switch(nErrorIteration) {
        case _gld_error_ok: 
            nErrorIteration = _gld_error_ok;
            goto finishing;
        case _gld_wrg_wait: 
            nErrorIteration = _gld_wrg_wait;
            goto skip;
        case _gld_error_no_ignit:
        default :
					if (first_ignition){					
            nIteration++;
            if(nIteration>(N_START_MAX-1)) {
                nErrorIteration = _gld_error_no_ignit;
                goto failing;
            } else {
                nErrorIteration = _gld_wrg_wait;
                goto skip;
            }
					}else
						    nErrorIteration = _gld_error_no_ignit;
    }
failing:   
finishing:
    ignit_set_request(_x_false);
    ignit_set_started(_x_false);
    ignit_set_busy(_x_false);
		if (first_ignition == _x_true){
					Output.Str.CURR_reg = Device_blk.Str.Curr_work;
					close_all_loops();
					open_loop(CURR_REG_ON);
		}
		first_ignition = _x_false;
    hardware_backlight_off();
skip:
    return nErrorIteration;
}

/******************************************************************************/
typedef enum ignit_statusDef{
    _ignit_status_free = 0
    , _ignit_status_pulse
    , _ignit_status_pause
    , _ignit_status_polling
}ignit_status;

gld_error_t ignit_single(void)
{
    static ignit_status status = _ignit_status_free;
    gld_error_t retval = _gld_error_no_ignit;
    
    switch(status){
        case _ignit_status_free: 
            status = _ignit_status_pulse;
        case _ignit_status_pulse:
            retval = handle_pulse();
            if(retval==_gld_wrg_wait) goto skip;
            else status = _ignit_status_pause;
            break;
        
        case _ignit_status_pause:
            retval = handle_pause();
            if(retval==_gld_wrg_wait) goto skip;
            else status = _ignit_status_polling;
            break;
        
        case _ignit_status_polling: 
            retval = handle_polling();
            if(retval==_gld_wrg_wait) goto skip;
            else goto fail;
            break;
        
        default: break;
    }
skip:
    return _gld_wrg_wait;
fail:
    status = _ignit_status_free;
    return retval;
}

/******************************************************************************/
void ignit_init(void)
{
    ignit_set_request(_x_true);
    ignit_set_started(_x_false);
    ignit_set_busy(_x_false);
}