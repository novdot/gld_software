#include  "commandset.h"
#include  "CntrlGLD.h"
#include  "Cyclessync.h"
#include  "dspfns.h"
#include  "SIP.h"
#include  "el_lin.h"


#include "core/command.h"
#include "core/ignit.h"
#include "core/global.h"

#include "hardware/hardware.h"

#define COMMAND_DEBUG  


void Sts_Pulse(void);
void Sts_Pause(void);
void Sts_Polling(void);

	//e. table of states of the procedure of starting 
void 	(* Tab_Gld_Start[3])(void) = {Sts_Pulse, Sts_Pause, Sts_Polling};
	//e. table of states of the procedure of light-up 
void	(* Tab_Gld_Pulse[1])(void) = {Sts_Pulse};

	//e. stack of states of the procedure of starting 
uint32_t		Stk_Gld_Start[3] = {LIGHT_UP_PULSE_WDTH, LIGHT_UP_PAUSE, LIGHT_UP_POLLING};

uint32_t		gyro_Sts = 0;       //e. device status register //r. регистр состояния прибора

uint32_t		start_Go = 0;       //e. procedure of starting is active //r. процедура запуска активна
//uint32_t		pulse_Rq = 0;       //e. request for light-up of laser //r. запрос на поджиг лазера
uint32_t		pulse_Go = 0;       //e. light-up process is active //r. процесс поджига активен

uint32_t     	state_index;    //e. index of status of procedure of starting: //r. индекс состояния процедуры (старта)
uint32_t     	sts_work;       //e. 1 - current status - in active state, 0 - initialization //r. 1 - текущее состояние - в рабочей фазе, 0 - инициализации
uint32_t     	sts_time;       //e. counter of time of the current status //r. счетчик времени текущего состояния
uint32_t     	attempt_Str;    //e. counter of attempts of device starting //r. счетчик попыток запуска прибора

/******************************************************************************/
void GLD_Stop(void)	//e.---------- procedure of GLD stopping 
{
	if (!stop_Rq)	//e. check the request for device switch off 
	{
		return;		//e. if no, return 
	}
	
    //e. flags reset:
	//start_Rq = 0;      //e. request for switch off, 
	start_Go = 0;      //e. activity of device starting process,
	stop_Rq  = 0;      //e. request for stopping 
	open_all_loops();  //e. switch off all regulators

	gyro_Sts = 0;      //e. reset the GLD hardware status register 

	//e. set the disable values of output parameters of regulators
#if !defined COMMAND_DEBUG
	disable_DACs();
#endif
} // GLD_Stop

/******************************************************************************/
void GLD_Status(void)     //e.----------  prestart control of GLD parameters
{
  static uint32_t Device_Mode_Old = DM_INT_10KHZ_LATCH/*DM_EXT_LATCH_DELTA_SF_PULSE*/;

	if ( loop_is_closed(GLD_ON) == 0 )
	{
		open_all_loops();
		GLD_Stop();
	}
	if (Device_Mode_Old != Device_Mode)		   //deviсe mode has been changed
	 {
	  if (SwitchMode())						   //e. mode is changed successful
	  {
	   Device_Mode_Old = Device_Mode;		   //e. save current mode name
	   if (Device_Mode_Old == DM_INT_10KHZ_LATCH)
	   trm_ena = 1;							   //e. enable answer
	   }	
	 }	 
} // GLD_Status

/******************************************************************************/
//e. procedure of the Pause state
void Sts_Pause(void)      
{
	if (sts_work)      //e. is state in active phase?
    {
		sts_time--;      //e. decrement of the counter of time of state
	    if (sts_time > 0)
	    {
	    	return; //e. continue operation, while counter is positive
	    }
	    //e. time of state is over //r. 
	    sts_work = 0;       //e. reset the flag of activity
	    state_index++;     //e. move the pointer to the next state
	}
    else
    {
		sts_time = Stk_Gld_Start[state_index];	//e. time of the current state //r. время данного состояния      
		sts_work = 1;        //e. set the flag of active phase //r. установить флаг активной фазы
	}       
} // Sts_Pause

/******************************************************************************/
//e. procedure of states of light-up pulse
void Sts_Pulse()       
{
    if (sts_work)      //e. is state in active phase? //r. состояние в активной фазе?
    {
		sts_time--;      //e. decrement of the counter of time of state //r. декремент счетчика времени состояния
	    if (sts_time > 0)
	    {
	    	return; //e. continue operation, while counter is positive //r. продолжать работу, пока счетчик положителен
	    }
        //e. time of state is over //r. время состояния кончилось
	    sts_work = 0;       //e. reset the flag of activity //r. сбросить флаг активности
	    state_index++;     //e. move the pointer to the next state //r. передвинуть указатель на следующее состояние
	    
        hardware_lightup_off();		//e. switch off the signal of laser light-up //r. снять сигнал поджига лазера        
        
	} else {
		sts_time = Stk_Gld_Start[state_index];	//e. time of the current state //r. время данного состояния      
		sts_work = 1;        //e. set the flag of active phase //r. установить флаг активной фазы

        hardware_lightup_on(); //e. activate the signal of laser light-up //r. активировать сигнал поджига лазера
        
	}
} // Sts_Pulse

/******************************************************************************/
//e. state machine procedure and device start-up
void Sts_Polling(void)
{
	static int32_t Sum_F_ras = 0; //e. integral sum of output frequency for procedure of starting 

    if (sts_work)      //e. is state in active phase?
    {
			if 	(Dif_Curr_Vib >0)
        		Sum_F_ras = L_add(Sum_F_ras, Dif_Curr_Vib);
			else
				Sum_F_ras = L_sub(Sum_F_ras, Dif_Curr_Vib);

        //e. no, skip accumulating of output frequency sum 
		sts_time--;      //e. decrement of the counter of time of state
	    if (sts_time > 0)
	    {
	    	return; //e. continue operation, while counter is positive
	    }
	        //e. time of state is over
	    sts_work = 0;       //e. reset the flag of activity
	    state_index++;     //e. move the pointer to the next state
        //e. check the availability of generation
        if ( Sum_F_ras >= F_RAS_MIN) //e. //r. F_RAS_MIN = 10000 ???
        {
	        gyro_Sts |= LASER_ON; //e. otherwise - start took place, set the state register of ... 
    	    return;                    //e. the device and end the process
        }
        //e. if it is less, it is not generation 
        attempt_Str--;          //e. decrement of counter of attempts of laser starting
        if (attempt_Str <= 0)
        {
        	return;        //e. if attempts are over, end the process 
        }
        //e. otherwise - set the state pointer on
        state_index = 0;      //e. the beginning of starting process for repeated attempt
	}
    else
    {
		sts_time = Stk_Gld_Start[state_index];	//e. time of the current state 
		sts_work = 1;        //e. set the flag of active phase
        Sum_F_ras = 0;                 //e. reset of accumulated sum of output frequency
	}
	
} // Sts_Polling


/******************************************************************************
//e.---------- device starting process ------------------------------------- //r.---------- процесс запуска прибора -------------------------------------
void GLD_Start(void)
{
	if (!start_Rq)	//e. check the request for device starting //r. проверить состояние запроса на запуск прибора
	{
		return;		//e. there is not request, return //r. запроса нет, возврат
	}

	if (!start_Go)	 //e. starting process was not already active? //r. процесс запуска еще не был активен?
	{	
        hardware_backlight_on();
        
		//e. it is begin of start //r. это начало запуска
        attempt_Str = N_START_MAX;  //e. set the maximal amount of attempts of starting //r. установить максимальное число попыток запуска
		//e. open loops of regulators //r. разомкнуть контура регуляторов
		//open_loop(VB_FREQ_ON | VB_TAU_ON | WP_REG_ON | HF_REG_ON); // 0xff95
		//close_all_loops();
	
        sts_work = 0;      //e. reset flag of active phase of state //r. сбросить флаг активной фазы состояния
        state_index= 0;      //e. set pointer of state to the initial position //r. установить указатель состояний в начальное положение
        start_Go = 1;        //e. starting process begins //r. процесс старта начался
	}

	( *Tab_Gld_Start[state_index] )();		//e. call state procedure by index //r. вызов процедуры состояния по номеру в массиве
    	
    if (sts_work)      	//e. current state is ended? //r. текущее состояние окончено?
    {
    	return;			//e. continue waiting //r. продолжать ожидание
    }
    //e. state is ended, check the state pointer //r. состояние закончилось проверить указатель состояний
    if ( state_index >= 3 )
    {
		//e. stop of the process //r. окончание процесса
	    //e. flags reset: //r. сбросить флаги
	    start_Rq = 0;      //e. request, //r. запроса
	    start_Go = 0;      //e. activity of process //r. и активности процесса
#if !defined COMMAND_DEBUG
	    Out_G_photo(Device_blk.Str.Gain_Ph_A, Device_blk.Str.Gain_Ph_B); 		//e. re-initialise gain factors of photodetector channels //r. повторно инициализировать коэф-ты передачи фотоприемников
        hardware_photo_out(Device_blk.Str.Gain_Ph_A, Device_blk.Str.Gain_Ph_B);
#endif
        //close_all_loops();
        
        hardware_backlight_off();
        hardware_lightup_off();
	}
        
} // GLD_Start

/******************************************************************************
//e laser light-up process
void GLD_Pulse(void)       
{
    //e. check the request for laser light-up 
	if (!pulse_Rq) {
    	return; 		//e. there is not request, return
    }
                
    //e. light-up process was not already active?
    if (!pulse_Go) {
        sts_work = 0;      //e. reset flag of active phase of state
        state_index = 0;      //e. set pointer of state to the initial position
        pulse_Go = 1;       //e. starting process begins
        hardware_backlight_on();
        //hardware_lightup_on();
    }
    
    ( *Tab_Gld_Pulse[state_index] )();		//e. call state procedure by index
	
    //e. current state is ended?
	if (sts_work) {
        //e. continue waiting
    	return;
    }
    
	if ( state_index >= 1 ) {
	    pulse_Rq = 0;
	    pulse_Go = 0;
        hardware_backlight_off();
        hardware_lightup_off();
    }
        
} // GLD_Pulse

/******************************************************************************/
void contrl_GLD(void)     //e.========== procedure of device control =============================== //r.========== процедура управления прибором ===============================
{
	//GLD_Start();  //e. start the device, if request is exists //r. запустить прибор, если есть запрос
	GLD_Status(); //e. checking the device status //r. проверка состояния прибора
	GLD_Stop();   //e. stop the device, if necessary //r. остановить прибор, если необходимо
	//GLD_Pulse();  //e. generation of the light-up pulse if request is exists //r. генерация импульса поджига при наличии запроса
	ignit_start();
    gld_output(); //e. gyro output modes //r. режимы выдачи данных из гироскопа
} // contrl_GLD

/******************************************************************************/
void gld_output(void)
{
    //latch appeared
    if (Latch_Rdy) {
        //enable packet generation
        if (trm_cycl) trm_ena = 1;   

        switch (CMD_Mode) {
            case 1: //e. Delta _PS mode
                command_cmd_DELTA_PS_EXEC();
                break;

            case 5:
                //e. reset bits of current command code settings of periodicity and transfer rate
                //r. СЃР±СЂРѕСЃРёС‚СЊ РІ С‚РµРєСѓС‰РµРј РєРѕРґРµ РєРѕРјР°РЅРґС‹ Р±РёС‚С‹ СѓСЃС‚Р°РЅРѕРІРєРё РїРµСЂРёРѕРґРёС‡РЅРѕСЃС‚Рё Рё СЃРєРѕСЂРѕСЃС‚Рё РїРµСЂРµРґР°С‡Рё
                CMD_Code &= 0xff1f;
                //e. is it the Rate2 mode?
                //r. СЌС‚Рѕ СЂРµР¶РёРј Rate2?
                if (CMD_Code == SUBCMD_M_RATE2)	 {
                    if (data_Rdy & WHOLE_PERIOD)  {
                        trm_ena = 1;
                    } else trm_ena = 0;
                }   	
                break;	
        }
    }
}
