#include "core/ignit.h"
#include "hardware/hardware.h"

gld_error_t handle_pulse(void);
gld_error_t handle_pause(void);
gld_error_t handle_polling(void);
gld_error_t ignit_single(void);
void ignit_set_started(x_bool_t flag);
void ignit_set_busy(x_bool_t flag);

x_bool_t g_bIsStartRequested = _x_false; //start
x_bool_t g_bIsStarted = _x_false; //on start procedure
x_bool_t g_bIsRequested = _x_false; //single ignit
x_bool_t g_bIsBusy = _x_false; //on single ingnit procedure

#define	IGNIT_PULSE_TIME    (1000) //e. width of light-up pulse = 100 msec
#define	IGNIT_PAUSE_TIME    (1000) //e. pause after light-up = 100 msec
#define	IGNIT_POLLING_TIME	(10000) //e. time of waiting laser generation = 1 sec
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
        g_nStatusTime = IGNIT_PULSE_TIME;
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
        g_nStatusTime = IGNIT_PAUSE_TIME;
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
    if(ignit_is_busy()==_x_false) {
        g_nStatusTime = IGNIT_POLLING_TIME;
    }
    
    g_nStatusTime--;
    if(g_nStatusTime>0) {
        ignit_set_busy(_x_true);
        return _gld_wrg_wait;
    }
    
    //end
    ignit_set_busy(_x_false);
    return _gld_error_no_ignit;
}

/******************************************************************************/
gld_error_t ignit_start(void)
{
    static x_uint8_t nIteration = 0;
    gld_error_t nErrorIteration = _gld_error_no_ignit;
    
    //if no request to light-up go away
    if(ignit_is_start_request()==_x_false) return _gld_wrg_wait;
    
    if(ignit_is_started()==_x_false) {
        hardware_backlight_on();
        //close_all_loops();
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
            nIteration++;
            if(nIteration>(IGNIT_MAX_ITERATIONS-1)) {
                nErrorIteration = _gld_error_no_ignit;
                goto failing;
            } else {
                nErrorIteration = _gld_wrg_wait;
                goto skip;
            }
    }
failing:   
finishing:
    ignit_set_start_request(_x_false);
    ignit_set_started(_x_false);
    ignit_set_busy(_x_false);
    nIteration = 0;
    hardware_backlight_off();
    //open_all_loops();
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
            status = _ignit_status_free;
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
            retval = handle_pause();
            if(retval==_gld_wrg_wait) goto skip;
            else goto fail;
            break;
        
        default: break;
    }
skip:
    return _gld_wrg_wait;
fail:
    status = _ignit_status_free;
    return _gld_error_no_ignit;
}