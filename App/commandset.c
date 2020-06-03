#include <string.h>
#include "commandset.h"
#include "el_lin.h"
#include "CyclesSync.h"
#include "CntrlGLD.h"
#include "InputOutput.h"
#include "Parameters.h"

#include "core/global.h"
#include "core/command.h"

#define COMMAND_DEBUG



//e. variables 

 int32_t 	ScopeMode; //e. mode of display for a control point of an scope

/******************************************************************************/
void SetSpeedPeriod(void)     //e procedure of set of rate and periodicity of answer 
{
	if ((rcv_buf[3] & 0x0080) != 0) //e. is periodic data transmission needed? 
	{
		trm_cycl = 1;		//e. yes, set present flag 
	}
	else
	{  
		trm_cycl = 0;		//e. no, reset present flag 
	}
	
	SRgR &= 0xffcf;				//e. clear the bit of transfer rate
	trm_rate = (rcv_buf[3] >> 1) & 0x0030;
	SRgR |= trm_rate; 			//e. set present transfer rate
} // SetSpeedPeriod

/******************************************************************************/
void B_Delta_BINS(void)        //r.=== procedure of output of the accumulated differences of counters of pulses in INS in a format 14.18  
{
	Valid_Data = 0;						// reset all bits of status word

	num_of_par = 2;        			//e. 2 parameters output                   
	addr_param[0] = &Output.Str.BINS_dif;	//e. set the first parameter address
	addr_param[1] = &Valid_Data;    //e. set the second paremeter address
	size_param[0] = 4;        		//e. the length of the 1st parameter is 4 bytes 
	size_param[1] = 1;        		//e. the length of the 2nd parameter is 1 byte (!! - the high byte, instead of low is transmitted)
	trm_ena = 1;        			//e. allow operation of the transmitter 

} // B_Delta_BINS

/******************************************************************************/
void B_Delta_SF(void)        //r.=== procedure for Scale Factor measurement
{
	//e. time for data transfer has come, we work only with dither counters 
	RgConB = RATE_VIBRO_1;				//e. set in the additional register of device control the mode of work with dither counters  and the filter of moving average //r. устанавливаем в дополнительном регистре управления режим работы с вибросчетчиками и фильтром скользящего среднего

	Valid_Data = 0;						// reset all bits of status word

	num_of_par = 5;//8;        			    //e. 8 parameters output //r. выводить 8 параметра                       
	addr_param[0] = &Output.Str.SF_dif;	    //e. set the first parameter address //r. задать адрес первого параметра
	addr_param[1] = &Out_main_cycle_latch;    //e. set the second paremeter address //r. задать адрес второго параметра
	addr_param[2] = &Out_T_latch;//F_ras;
	addr_param[3] = &Output.Str.WP_reg;
	addr_param[4] = &Output.Str.Tmp_Out;
	size_param[0] = 8;        		//e. the length of the 1st parameter is 4 bytes //r. параметр 1  имеeт длину 4 байта
	size_param[1] = 4;        		//e. the length of the 2nd parameter is 1 byte (!! - the high byte, instead of low is transmitted) //r. параметр 2 имеет длину 1 байт (!!! передается старший байт, а не младший)
	size_param[2] = 2;
	size_param[3] = 2;
	size_param[4] = 12;
	trm_ena = 1;        			//e. allow operation of the transmitter //r. разрешить работу передатчика
} // B_Delta_SF

/******************************************************************************/
void B_Delta_PS_execution(void)
{
	static void * paramTable[11] = {
        &(Output.Str.F_ras) 
        , &(Output.Str.HF_reg)
        , &(Output.Str.T_Vibro)
        , &(Output.Str.L_Vibro)
        , &(Output.Str.WP_reg)
        , &(Output.Str.Tmp_Out[0])
        , &(Output.Str.Tmp_Out[1])
        , &(Output.Str.Tmp_Out[2])
        , &(Output.Str.Tmp_Out[3])
        , &(Output.Str.Tmp_Out[4])
        , &(Output.Str.Tmp_Out[5])
    };
	static uint32_t val, paramTmpWord;
	static uint32_t * ptr;
	static uint32_t index = 0;
		   
	if ((index & 1) == 0) 		// high byte
	{
		ptr = (uint32_t*)paramTable[index >> 1];
		val = *ptr;
		paramTmpWord = val >> 8; // move it to low byte of word
	}
	else
	{
		paramTmpWord = val;
	}
	paramTmpWord &= 0xFF;
	paramTmpWord |= index << 8;
	
	Valid_Data = 0;						// reset all bits of status word
 

	num_of_par = 2;        //e. 2 parameters output //r. выводить 2 параметра
	addr_param[0] = &Output.Str.PS_dif;           //e. set the first parameter address //r. задать адрес первого параметра
	addr_param[1] = &paramTmpWord;
	size_param[0] = 2;        		//e. the 1st parameter has 2 bytes length //r. параметр 1  имеeт длину 2 байт
	size_param[1] = 2;        		//e. the 2nd and 3rd parameters have 1 bytes length//r. параметры 2,3 имеют длину 1 байт
	trm_ena = 1;        			//e. allow operation of the transmitter //r. разрешить работу передатчика

	index++;
	if (index > 21)
	{
		index = 0;
	}	
} // B_Delta_PS_execution

/******************************************************************************/
void M_Mirror(void)       //e.---------------- prepare to transmission of the copy of receiving buffer ------------- //r.---------------- подготовка передачи копии приемного буфера -------------
{
	num_of_par = 1;     //e. 1 parameter output //r. выводить 1 параметр

	addr_param[0] =  &rcv_copy;         //e. set the address of the receiving buffer //r. установка адреса приемного буфера

	//e. multiply the number of copied words on 2, since each //r. умножить число скопированных слов на 2, т.к. каждое
	//e. will be trasferred in two steps: first zero, and then //r. будет передаватьсч в два приема: сначала нулевой, а затем
	size_param[0] = rcv_byt_copy << 1; // multiplay by 2
	if (size_param[0] >= 64)
	{
		size_param[0] = 64;		//e. maximal amount - no more than double length of the copy buffer //r. максимальное количество - не более двойной длины буфера-копии
	}
	trm_rate = 0;       //e. set the transfer rate to the 38400 bauds //r. установить скорость передачи 38400 бод
	trm_cycl = 0;       //e. forbid cyclic transmission of the parameter //r. запретить передачу параметра в цикле
	trm_ena = 1;        //e. allow operation of the transmitter //r. разрешить работу передатчика
} // M_Mirror
  
/******************************************************************************/      
void Mk_Ask1(void)        //r.----------------- prepare of the standart answer 1 ---------------------- //r.----------------- подготовка стандартного ответа 1 ----------------------
{
	CMD_Code &= 0xff00;			//e. clear bits of errors //r. очистить в нем поле ошибок
	num_of_par = 1;             //e. total amount parameters in aswer - 1  //r. всего параметров в ответе - 1
	addr_param[0] = &CMD_Code;  //e. and this parameter - returnable command code //r. и этот параметр - возвращаемый код команды
	size_param[0] = 2;          //e. and both its bytes //r. причем оба его байта
	trm_ena = 1;              	//e. allow operation of the transmitter of the device //r. разрешить работу передатчика устройства
} // Mk_Ask1

/******************************************************************************/
void Mk_AskDev(void)        //r.----------------- answer on a command for device mode set ------------ //r.----------------- Ответ на команду установки режима прибора ------------
{
	num_of_par = 2;     //e. 2 parameters output //r. выводить два параметра
	addr_param[0] = &Device_Mode;	//e. address of the counter mode register (intenal latch, external latch, etc.) //r. адрес регистра режима счетчиков (внутр., внешняя защелка и т.д.)
	addr_param[1] = &SRgR;   	//e. address of the mode register of the processor card //r. адрес регистра режима платы процессора
	size_param[0] = 2;  			//e. size of the counter mode register - 2 bytes //r. размер регистра режима счетчиков - 2 байта
	size_param[1] = 2;  			//e. size of the mode register of the processor card //r. размер регистра режима платы процессора
	trm_cycl = 0;       			//e. forbid cyclic transmission of the parameter //r. запретить передачу параметра в цикле
	trm_ena = 1;        			//e. allow operation of the transmitter //r. разрешить работу передатчика
} // Mk_AskDev

/******************************************************************************/
void GLD_Output(void)		//e. ----------- Output modes --------- //r. --------- Режимы вывода ---------
{
if (Latch_Rdy)		   //latch appeared
  {
  	if (trm_cycl)
		trm_ena = 1;   //enable packet generation

	switch (CMD_Mode)
	{
		case 1:	   							//e. Delta _PS mode
			B_Delta_PS_execution();
		break;

		case 5:
			CMD_Code &= 0xff1f; //e. reset bits of current command code settings of periodicity and transfer rate //r. сбросить в текущем коде команды биты установки периодичности и скорости передачи
			if (CMD_Code == 0xdd02)	 //e. is it the Rate2 mode? //r. это режим Rate2?
	 		{
				if (data_Rdy & WHOLE_PERIOD) 
	  			{
			 		trm_ena = 1;
	  			}
				else
			 		trm_ena = 0;
			}	
		break;	
	}
  }
} // GLD_Delta



/******************************************************************************/
///// -------- commands -------------------------------------------------------------
//e. set main device operation modes: //r. Установка основных режимов работы прибора:
//e. 1. acions with counters (internal latch, external latch with request over line, //r. 1. работа со счетчиками (внутренняя защелка, внешняя защелки с запросом по линии,
//e. external latch without request over line (instant answer after receiving of external latch pulse)) //r. внешняя защелка без запроса по линии (ответ сразу после прихода импульса внешн. защелки))
//e. 2. line receiving/transmission rate //r. 2. скорость приема/передачи по линии
void B_Dev_Mode(void)  
{
	Device_Mode = rcv_buf[3] & 0x00ff; //e. read the byte of command parameter from the receiver buffer //r. считать из буфера приемника байт параметра команды
										//e. and write it to the counter mode register  //r. и записываем в регистр режима счетчиков
	trm_cycl = 0;	//e. periodic data transmission is not needed //r. периодическая передача ответа не требуется
	Mk_AskDev();
} // B_Dev_Mode

/******************************************************************************/
void B_BIT_Mode(void)
{
	uint8_t temp;
	
	temp = rcv_buf[3] & 0x000f & (~((rcv_buf[3] & 0x00f0) >> 4));
	if (temp == 1) // 1 - const test
	{
		Is_BIT = 1;
		BIT_number = (long)(rcv_buf[4] & 0x00FF) << 24;
		BIT_number |= (long)(rcv_buf[5] & 0x00FF) << 16;
		BIT_number |= (long)(rcv_buf[6] & 0x00FF) << 8;
		BIT_number |= (long)(rcv_buf[7] & 0x00FF);
	}
	else
	{
		Is_BIT = 0;
	}
	trm_cycl = 0;	//e. periodic data transmission is not needed //r. периодическая передача ответа не требуется
	Mk_AskDev();	
} // B_BIT_Mode

/******************************************************************************/
void M_Stimul(void)       //e. === procedure of output of analog (DAC) and digital (flags) stimuluses //r. === процедура вывода аналоговых (ЦАП) и дискретных (флаги) стимулов
{
	uint32_t chan;
	
	ScopeMode = 4; //e. WP_PHASE_DETECTOR allocate a mode of display for a control point of an scope //r. WP_PHASE_DETECTOR назначить режим отображения для контрольной точки осциллографа

	chan = CMD_Code & 0x0007;           //e. extracting the number of stimulus //r. выделение номера стимула

    Output.ArrayOut[chan] = (((int)rcv_buf[4] << 8) | (int)rcv_buf[5])-0x8000;
		 	
	if ((CMD_Code & (1 << 7)) == 0) //e. to estimate: whether the answer is required //r. оценить: требуется ли ответ
	{
		return;			//e. if no - return //r. если нет - возврат
	}
	Mk_Ask1();           //e. otherwise - answer output //r. иначе - выдать ответ
} // M_Stymul
   
/******************************************************************************/     
void M_Status(void)        //e. === procedure of initialization of transmission of the device status //r. === процедура инициализации передачи статуса устройства
{
	SetSpeedPeriod();        //e. and set the answer transfer rate and its periodicity //r. и установить скорость передачи ответа и его периодичность

	num_of_par = 2;     //e. 2 parameters output //r. выводить два параметра
	addr_param[0] = &blt_in_test;      //e. the register address of the self-testing result //r. адрес регистра результата самотестирования
	addr_param[1] = &ser_num;   //e. address of the register of errors of line //r. адрес регистра ошибок линии
	size_param[0] = 2;     //e. size of the self-test register - 2 bytes //r. размер регистра самотестирования - 2 байта
	size_param[1] = 2;   //e. size of the register of errors of line //r. размер регистра ошибок линии

	trm_rate = 0;       //e. set the transfer rate to the 38400 bauds //r. установить скорость передачи 38400 бод
	trm_cycl = 0;       //e. forbid cyclic transmission of the parameter //r. запретить передачу параметра в цикле
	trm_ena = 1;        //e. allow operation of the transmitter //r. разрешить работу передатчика
} // M_Status

/******************************************************************************/
void M_Clear(void)        //e.---------------- cleaning of the register of errors of line -------------------------- //r.---------------- очистка регистра ошибок линии --------------------------
{
	line_err = 0;
} // M_Clear

/******************************************************************************/
void M_Tmp_W(void)        //e.----------------- write the command to the AD7714 IC ------------------------- //r.----------------- запись команды в схемы AD7714 -------------------------
{
	Mk_Ask1();
}

/******************************************************************************/
void M_Tmp_R(void)        //e.----------------- read the data from the AD7714 IC -------------------------- //r.----------------- чтение данных из схем AD7714 --------------------------
{

} // M_Tmp_R

/******************************************************************************/
void M_ADC_R(void)        //e.----------------- read the data from the ADCs ADS1250, ADS8321 --------- //r.----------------- чтение данных из каналов АЦП ADS1250, ADS8321 ---------
{
	SetSpeedPeriod();        		//e. set the answer transfer rate and its periodicity //r. установить скорость передачи ответа и его периодичность
	UART_SwitchSpeed(trm_rate);
	
	num_of_par = 4;     		//e. 4 parameters output //r. выводить четыре параметра
	addr_param[0] = 0; 			//e. //r. <!-- использовались для ДУП, теперь не используются
	addr_param[1] = 0;			//e. //r.  -->
	addr_param[2] = 0;
	//addr_param[3] = &(Input.StrIn.HF_out); //e. set the 4th parameter address  _HF_out //r. задать адрес четвертого параметра _HF_out
	addr_param[3] = &(g_input.word.hf_out);
    size_param[0] = 2;     		//e. size of the parameters - 2 bytes //r. размер параметров - 2 байта
	size_param[1] = 2;
	size_param[2] = 2;
	size_param[3] = 2;
	trm_ena = 1;        		//e. allow operation of the transmitter //r. разрешить работу передатчика
} // M_ADC_R

/******************************************************************************/
void M_Cnt_R(void)        //r.----------------- read the counters of the Elio5 card -------------------------- //r.----------------- чтение счетчиков платы Elio5 --------------------------
{
	SetSpeedPeriod();        		//e. set the answer transfer rate and its periodicity //r. установить скорость передачи ответа и его периодичность
	UART_SwitchSpeed(trm_rate);
	num_of_par = 2;     		//e. 2 parameters output //r. выводить два параметра

	addr_param[0] = &(Output.Str.Cnt_Pls);   //e. set the first parameter address //r. задать адрес первого параметра
	addr_param[1] = &(Output.Str.Cnt_Mns);   //e. set the second paremeter address //r. задать адрес второго параметра
	size_param[0] = 2;     		//e. size of the parameters - 2 bytes //r. размер параметров - 2 байта
	size_param[1] = 2;
	trm_ena = 1;        		//e. allow operation of the transmitter //r. разрешить работу передатчика
} // M_Cnt_R
    
/******************************************************************************  
/void M_e5r_W(void)        //r.----------------- write to the mode register of the Elio5 card ------------------ //r.----------------- запись в регистр режима платы Elio5  ------------------
{
	//e. read the byte of command parameter from the receiver buffer //r. считать из буфера приемника байт параметра команды
	//e. and write it to the card mode register and its copy //r. и записать его в регистр режима платы и в его копию
#if !defined COMMAND_DEBUG  
	Copy_e5_RgR = rcv_buf[3];  
	io_space_write(E5_RGR, Copy_e5_RgR);  
#endif
	trm_cycl = 0;      //e. periodic data transmission is not needed //r. периодическая передача ответа не требуется
	Mk_Ask1();
} // M_e5r_W  */

/******************************************************************************
void M_e5rA_W(void)       //e.----- write to the 1st additional mode register of the Elio5 card  ---------- //r.----- запись в 1-ый дополнительный регистр режима платы Elio5  ----------
{
	//e. this command is used for switching a signal on which data counters are latched: //r. эта команда используется для переключения сигнала, по которому защелкиваются
	//e. on the Reper signal or on Sign Meander //r. информационные счетчики: либо по Reper`у либо по RefMeandr`у
	
	//e. read the byte of command parameter from the receiver buffer //r. считать из буфера приемника байт параметра команды
	//e. and write it to the card mode register and its copy //r. и записать его в регистр режима платы и в его копию        
	Copy_e5_RgRA = rcv_buf[3];    
//???	Device_Mode = Copy_e5_RgRA; //e. and write it to the counter mode register  //r. и записываем в регистр режима счетчиков
#if !defined COMMAND_DEBUG 
	io_space_write(E5_RGRA, Copy_e5_RgRA);
#endif
	trm_cycl = 0;      			//e. periodic data transmission is not needed //r. периодическая передача ответа не требуется
	Mk_Ask1();
} // M_e5rA_W

/******************************************************************************/ 
void M_Ctl_R(void)        //r.----------------- reading the control register of the device ----------------- //r.----------------- чтение регистра управления устройства -----------------
{
	num_of_par = 2;                 //e. 2 parameters transfer //r. передавать 2 параметра
	addr_param[0] = &CMD_Code;         //e. the first parameter in answer - returned command code //r. первый параметр в ответе - возвращаемый код команды
	size_param[0] = 2;
	size_param[1] = 2;   //e. two bytes also have control registers //r. два байта имеют также и регистры управления
	if ((rcv_buf[3] & (1 << 4)) == 0) //e. is main control register needed? //r. требуется основной регистр управления?
	{
		addr_param[1] = &RgConA;	//e. yes //r. да
	}
	else
	{
		addr_param[1] = &RgConB;	//e. otherwise - load the address of the addititonal register //r. иначе - загрузить адрес дополнительного регистра
	}
	CMD_Code &= 0xff10;           //e. clear in it bit of errors and byte number //r. сбросить в нем поля ошибок и номера бита
	trm_ena = 1;                 //e. allow operation of the transmitter of line //r. разрешить работу передатчика линии
} // M_Ctl_R
       
/******************************************************************************/ 
void M_Ctl_M(void)        //r.----------------- modification of the control register of the device ------------ //r.----------------- модификация регистра управления устройства ------------
{
	uint32_t * ptr;
	uint32_t bit_numb;
	
	num_of_par = 2;                 //e. 2 parameters transfer //r. передавать 2 параметра
	addr_param[0] = &CMD_Code;         //e. the first parameter in answer - returned command code //r. первый параметр в ответе - возвращаемый код команды
	size_param[0] = 2;
	size_param[1] = 2;   //e. two bytes also have control registers //r. два байта имеют также и регистры управления
	if ((CMD_Code & (1 << 4)) == 0) //e. is main control register needed? //r. требуется основной регистр управления?
	{
		ptr = &RgConA;	//e. yes //r. да
	}
	else
	{
		ptr = &RgConB;	//e. otherwise - load the address of the addititonal register //r. иначе - загрузить адрес дополнительного регистра
	}
	addr_param[1] = ptr;         //e. the second parameter in answer - modified register //r. второй параметр в ответе - модифицированный регистр
	
	bit_numb = CMD_Code & 0x000f;    //e. extract the number of the changeable bit //r. выделить номер изменяемого бита 
	if ((CMD_Code & (1 << 7)) == 0) // Is clear bit
	{
		*ptr &= ~(1 << bit_numb); 	// yes, clear bit
	}
	else
	{
		*ptr |= 1 << bit_numb;		// no, set bit
	}
	
	CMD_Code &= 0xff10;           //e. clear in command bit of errors and byte number //r. сбросить в команде поля ошибок и номера бита
	trm_cycl = 0;       //e. forbid cyclic transmission of the parameter //r. запретить передачу параметра в цикле
	trm_ena = 1;                 //e. allow operation of the transmitter of line //r. разрешить работу передатчика линии
} // M_Ctl_M
       
/******************************************************************************/
void M_Flg_R(void)        //e.------------ reading the register of input flags --------------------------- //r.------------ чтение регистра входных флагов ---------------------------
{
	num_of_par = 1;     //e. 1 parameter output //r. выводить один параметр
	addr_param[0] = &In_Flag;	//e. the address of the flag register copy //r. адрес копии регистра флагов
	size_param[0] = 2;     		//e. size of the buffer -  2 bytes //r. размер буфера  - 2 байта
	trm_ena = 1;        		//e. allow operation of the transmitter //r. разрешить работу передатчика
} // M_Flg_R
      
/******************************************************************************/  
void M_Vib_W(void)        //r.------------ set the oscillation period of the dither drive ----------------- //r.------------ установка периода колебаний вибропривода -----------------
{
	ScopeMode = 1; //e. VB_PHASE_DETECTOR allocate a mode of display for a control point of an scope //r. VB_PHASE_DETECTOR назначить режим отображения для контрольной точки осциллографа

	Output.Str.T_Vibro = (rcv_buf[4] << 8) | (rcv_buf[5] & 0xFF); //e. new variable of the period //r. новая переменная периода
	Output.Str.L_Vibro= (rcv_buf[6] << 8) | (rcv_buf[7] & 0xFF); //e. new variable of the pulse width //r. новая переменная длительности импульсов
	VibroDither_Set();       //e. and output its value to period registers on card  //r. вывести ее значение в регистры периода на плате

	trm_cycl = 0;      //e. periodic data transmission is not needed //r. периодическая передача ответа не требуется
	Mk_Ask1();
} // M_Vib_W

/******************************************************************************/
void M_Gph_W(void)        //e.------------ set the gain factor of photodetector channels ------------------- //r.------------ установка усиления каналов фотоприемника -------------------
{
	Device_blk.Str.Gain_Ph_A = rcv_buf[4];     //e. read from the receiver buffer the value of the gain factor of the A channel //r. считать из буфера приемника значение усиления канала А
	Device_blk.Str.Gain_Ph_B = rcv_buf[5];     //e. read from the receiver buffer the value of the gain factor of the B channel //r. считать из буфера приемника значение усиления канала В

	Out_G_photo(Device_blk.Str.Gain_Ph_A, Device_blk.Str.Gain_Ph_B);       //e. display these values to digital potentiometers //r. вывести эти значения в цифровые потенциометры

	trm_cycl = 0;      //e. periodic data transmission is not needed //r. периодическая передача ответа не требуется
	Mk_Ask1();
} // M_Gph_W

/******************************************************************************/
void M_Rate(void)         //e.------------ start of transfer of the M_Rate parameters stack ------------------- //r.------------ запуск передачи набора параметров M_Rate -------------------
{
	uint8_t mode;

	SetSpeedPeriod();        //e. set the answer transfer rate and its periodicity //r. установить скорость передачи ответа и его периодичность
	UART_SwitchSpeed(trm_rate);

	mode = rcv_buf[3] & 0x001f;	//e. extract number of the parameters stack in the command //r. выделить номер набора параметров в команде
	
    switch (mode) {
		case 1:											//e. it is the Rate stack //r. это набор Rate
        	RgConB =  RATE_REPER_OR_REFMEANDR;			//e. counter latch every vibro period 
			SwitchRefMeandInt(RATE_REPER_OR_REFMEANDR); //e. enable reference meander interrupt
					
			wrk_period = 25000000; //e. load needed length of working period 1 с.//r. загрузить требуемую длительность рабочего периода	 1 с.
			num_of_par = 14;     //e. 9 parameters or groups of parameters output //r. выводить 9 параметров или групп параметров
	
			Valid_Data = 0;						// reset all bits of status word

	        addr_param[0] = &(Output.Str.Cnt_Pls);   	//e. set the address of the 1st parameter in the 1st group //r. задать адрес первого параметра  в 1-ой группе
	        addr_param[1] = &(Output.Str.Cnt_Mns);   	//e. set the address of the 1st parameter in the 2nd group //r. задать адрес первого параметра  в 2-ой группе
	        addr_param[2] = &(Output.Str.Cnt_Dif);   	//e. set the address of the 1st parameter in the 3rd group //r. задать адрес первого параметра  в 3-ой группе
	        addr_param[3] = &(Output.Str.F_ras);     	//e. set the address of the _F_ras parameter in the 4th group //r. задать адрес параметра  _F_ras во 4-ой группе
	        addr_param[4] = &(Output.Str.HF_reg);    	//e. set the address of the _HF_reg parameter in the 5th group //r. задать адрес параметра  _HF_reg в 5-ей группе
	        addr_param[5] = &(Output.Str.HF_dif);    	//e. set the address of the _HF_dif parameter in the 6th group //r. задать адрес параметра  _HF_dif в 6-ой группе
	        addr_param[6] = &(Output.Str.T_Vibro);   	//e. parameter _T_Vibro //r. параметр _T_Vibro
	        addr_param[7] = &(Output.Str.T_VB_pll);  	//e. parameter _T_VB_pll //r. параметрв _T_VB_pll
	        addr_param[8] = &(Output.Str.L_Vibro);   	//e. parameter _L_Vibro //r. параметр _L_Vibro     
	        //addr_param[9] = &(Input.StrIn.HF_out); 		//e. set the address of the _RI_reg parameter in the 8th group //r. задать адрес параметра  _RI_reg в 8-ой группе      
	        addr_param[9] = &(g_input.word.hf_out);
            addr_param[10] = &(Output.Str.WP_reg);   	//e. parameters: _WP_reg, _WP_pll //r. параметров _WP_reg, _WP_pll
	        addr_param[11] = &(Output.Str.WP_pll);   	//e. parameters: _WP_reg, _WP_pll //r. параметров _WP_reg, _WP_pll
	        addr_param[12] = &(Output.Str.Tmp_Out);      //e. set the address of the temperature sensors array //r. задать адрес массива датчиков температуры
	        addr_param[13] = &(Output.Str.WP_scope1);    //e. reserved //r. зарезервировано для дальнейших применений
	
	        size_param[0] = 2;            	//e. groups 1-6, 8-10 consists of one parameter ... //r. группы 1-6, 8-10 содержат по одному параметру
	        size_param[1] = 2;   			//e. and have 2 bytes length each //r. и имеют длину 2 байта каждая
	        size_param[2] = 2;
	        size_param[3] = 2;
	        size_param[4] = 2;
	        size_param[5] = 2;
	        size_param[6] = 2;
	        size_param[7] = 2;
	        size_param[8] = 2;
			size_param[9] = 2;
			size_param[10] = 2;
			size_param[11] = 2;
	
            size_param[12] = 12;   //e. format o the GLD array of temperatures - 12 bytes 
    
	        size_param[13] = 4;	        //e. the 11th group parameters has length of 4 bytes 
	
	        
	        trm_ena = 1;        //e. allow operation of the transmitter //r. разрешить работу передатчика
			break;
			
		case 2:			//e. it is the Rate2 stack //r. это набор Rate2
		    RgConB = RATE_REPER_OR_REFMEANDR;
			SwitchRefMeandInt(RATE_REPER_OR_REFMEANDR);   //e. enable interrupt from ref. meander
			wrk_period = 0;     //e. frequency of output = fvibro //r. частота вывода равна частоте ВП

			num_of_par = 2;     //e. 2 parameters output //r. выводить 2 параметра
	
	        addr_param[0] = &(Output.Str.Cnt_Pls);          //e. set the first parameter address //r. задать адрес первого параметра
	        addr_param[1] = &(Output.Str.Cnt_Mns);          //e. and the 2nd //r. и второго
	        size_param[0] = 2;     //e. parameters has 2 bytes length //r. параметры имеет длину 2 байта
	        size_param[1] = 2;
	        trm_ena = 1;        //e. allow operation of the transmitter //r. разрешить работу передатчика
			break;
			
		case 3:			//e. it is the Rate3 stack //r. это набор Rate3

			wrk_period = 2500;     //e. frequency of output = 10000 Hz //r. n=1, частота вывода = 10000 Hz
			num_of_par = 2;     //e. 2 parameters output //r. выводить 2 параметра
			
			addr_param[0] = &(Output.Str.WP_scope1);         //e. set the addresses of output parameters //r. задать адреса выводимых параметров
			addr_param[1] = &(Output.Str.WP_scope2);
			size_param[0] = 2;     //e. all parameters has 2 bytes length //r. все параметры имеют длину 2 байта
			size_param[1] = 2;
			trm_ena = 1;        //e. allow operation of the transmitter //r. разрешить работу передатчика
			break;
			
		case 4:			//e. it is the Rate4 stack //r. это набор Rate4
        	wrk_period = 20000;     //e. correspond to output frequency 1.25 kHz //r. соотвествует частоте вывода 1.25 kHz
			num_of_par = 4;     //e. 4 parameters output //r. выводить 4 параметра

			//e. set the addresses of output parameters //r. задать адреса выводимых параметров
        	// --- raw data array of numbers ---
		//	addr_param[0] = &Dif_Curr_Array;
        	// --- filtered array of numbers ---
	     //   addr_param[1] = &Dif_Filt_Array;
        	// ---------------------------------
	        addr_param[2] = &(Output.Str.HF_reg); 			//e. HFO regulator //r. напряжение ГВЧ
	        addr_param[3] = &(Output.Str.WP_reg); 			//e. CPLC heater regulator //r. напряжение нагревателя
	        size_param[0] = 16;     
	        size_param[1] = 32;     
	        size_param[2] = 2;
	        size_param[3] = 2;      
			trm_ena = 1;        //e. allow operation of the transmitter //r. разрешить работу передатчика
			break;
		case 7:
			wrk_period = 20000;     //e. correspond to output frequency 1250 Hz //r. соотвествует частоте вывода 1.25 kHz
			num_of_par = 5;     //e. 4 parameters output //r. выводить 4 параметра

			//e. set the addresses of output parameters //r. задать адреса выводимых параметров
        	// --- raw data array of PLC phase ---
			addr_param[0] = &(Output.Str.WP_Phase_Det_Array);
        	// --- filtered array of reference sin---
	        addr_param[1] = &(Output.Str.WP_sin_Array);
        	// ---------------------------------
	        addr_param[2] = &(Output.Str.WP_reg); 			//e. CPLC heater regulator //r. напряжение нагревателя
	        addr_param[3] = &(Output.Str.WP_pll); 			//e. CPLC phase  //r. фаза
	        addr_param[4] = &(Output.Str.HF_reg); //e. set the address of the _HF_reg parameter in the 5th group //r. задать адрес параметра  _HF_reg в 5-ей группе
	        size_param[0] = 16;     
	        size_param[1] = 16;     
	        size_param[2] = 2;
	        size_param[3] = 2;     
	        size_param[4] = 2;  
			trm_ena = 1;        //e. allow operation of the transmitter //r. разрешить работу передатчика
			break;
	}
   SetIntLatch(wrk_period);
} // M_Rate

/******************************************************************************/
void M_Reset(void)
{
  while(1);
} // will not achieve
	
/******************************************************************************/	        
void M_Param_R(void)      //e. ------------ read the parameter of the GLD from the data memory ---------------------- //r.------------ чтение параметра GLD из памяти данных ----------------------
{
	num_of_par = 1;     //e. 1 parameter output //r. выводить один параметр
	addr_param[0] = (void *)(&Device_blk.Str.My_Addres + rcv_buf[3]); //e. address of the needed parameter in the block //r. адрес требуемого параметра в блоке
	size_param[0] = 2;     //e. size of the buffer -  2 bytes //r. размер буфера  - 2 байта

	trm_ena = 1;        //e. allow operation of the transmitter //r. разрешить работу передатчика
} // M_Param_R

/******************************************************************************/
void M_Param_W(void)      //e.------------ write the parameter of the GLD from the data memory ----------------------- //r.------------ запись параметра GLD в память данных -----------------------
{
	int * ptr;
	
	ScopeMode = 0; //e. VB_DELAY_MEANDER allocate a mode of display for a control point of an scope //r. VB_DELAY_MEANDER назначить режим отображения для контрольной точки осциллографа
	
	ptr = (int *)&Device_blk.Str.My_Addres; // pointer to Parameters block
	ptr += rcv_buf[3];					// calculate offset
	*ptr = (rcv_buf[4] << 8) | (rcv_buf[5] & 0xFF); // write new parameter value

	trm_cycl = 0;      //e. periodic data transmission is not needed //r. периодическая передача ответа не требуется
	Mk_Ask1();
} // M_Param_W

/******************************************************************************/
void M_LdPar_F()      //e.============ procedure for load the GLD parameters from the flash-memory =========== //r.============ процедура загрузки параметров ГЛД из флэш-памяти ===========
{

	LoadFlashParam(FromFLASH);        //e. load the GLD parameters from the flash-memory //r. загрузить параметры ГЛД из фдэш-памяти
	blt_in_test = ((uint32_t)FIRMWARE_VER << 8) | (Device_blk.Str.Device_SerialNumber & 0x00FF);
//	Init_software();
	
	trm_cycl = 0;      //e. periodic data transmission is not needed //r. периодическая передача ответа не требуется
	Mk_Ask1();
} // M_LdPar_F

/******************************************************************************/
void M_LdPar_D(void)      	//e.============ procedure for set parameters of the GLD by default ============ //r.============ процедура установки параметров ГЛД по умолчанию ============
{
#if !defined COMMAND_DEBUG
	 LoadFlashParam(ByDefault);  	//e. define parameters of the GLD by default //r. определить параметры ГЛД по умолчанию (default)
	Init_software();
#endif	
	trm_cycl = 0;      		//e. periodic data transmission is not needed //r. периодическая передача ответа не требуется
	Mk_Ask1();
} // M_LdPar_D

/******************************************************************************/
void M_Start(void)        //e.============ initialization of the GLD switch on ================================= //r.============ инициализация запуска ГЛД =================================
{
	//start_Rq = 1;      	//e. set the flag of the GLD switch on request //r. установить флаг запроса старта ГЛД
	trm_cycl = 0;      	//e. periodic data transmission is not needed //r. периодическая передача ответа не требуется
	Mk_Ask1();
} // M_Start

/******************************************************************************/
void M_Stop(void)         //e.============ initialization of the GLD switch off ============================== //r.============ инициализация выключения ГЛД ==============================
{
	stop_Rq = 1;       	//e. set the flag of the GLD switch off request //r. установить флаг запроса выключения ГЛД
	trm_cycl = 0;      	//e. periodic data transmission is not needed //r. периодическая передача ответа не требуется
	Mk_Ask1();
} // M_Stop    

/******************************************************************************/
void M_Pulse(void)        //e.============ generetion of the light-up pulse ========================= //r.============ генерация импульса запуска лазера =========================
{
	//pulse_Rq = 1;      	//e. set the flag of the GLD switch on request //r. установить флаг запроса старта ГЛД
	trm_cycl = 0;      	//e. periodic data transmission is not needed //r. периодическая передача ответа не требуется
	Mk_Ask1();
} // M_Pulse

/******************************************************************************/
void B_Rate(void)         //e. === procedure of device operation in a mode of continuous output of raw data //r. === процедура работы прибора в режиме непрерывной выдачи "сырых" данных
{
} // B_Rate

/******************************************************************************/
void B_Delta(void)        //e. === procedure not used //r. === процедура не используется
{
} // B_Delta

/******************************************************************************/
void D_Period_W(void)     //e. === set the period of the angle increase output //r. === установка периода выдачи приращения угла
{
} // D_Period_W

/******************************************************************************/
void exec_CMD(void)       //e. === the final decoding and command execution procedure //r. === процедура окончательной дешифрации и исполнения команды
{
	uint32_t wcode;
	
	rx_buf_copy = 1;                 //e. initialization of the flag of copying of receiving buffer //r. предустанов флага копирования приемного буфера

	wcode = (rcv_buf[2] & 0xFF) << 8;
	CMD_Code = wcode | (rcv_buf[3] & 0xFF);	//e. save it in the memory for echo-transmission //r. сохранить его в памяти для обратной передачи
    
	if (wcode == CMD_RATE)			//e. is it Rate command? //r. это команда Rate?
	{
		B_Rate();
		return;
	}
	else if (wcode == CMD_DEV_MODE)	//e. is it the Device_Mode command? //r. это команда установки режима прибора?
	{
		CMD_Mode = 3;
		B_Dev_Mode();
		return;
	}
	else if (wcode == CMD_DELTA_BINS) //e. is it the B_DeltaBINS command (command of request for data transfer to the navigation system)? //r. это команда запроса передачи данных в навиг.систему?
	{
		RgConB = RATE_VIBRO_1;		  //e. set in the additional register of device control the mode of work with dither counters  and the filter of moving average //r. устанавливаем в дополнительном регистре управления режим работы с вибросчетчиками и фильтром скользящего среднего
		SetSpeedPeriod(); 
		UART_SwitchSpeed(trm_rate);
		CMD_Mode = 4;
		B_Delta_BINS();
		return;
	}
	else if (wcode == CMD_DELTA_PS)	//e.  is it the B_DeltaPS command (command of request for data transfer to the uPC)? //r. это команда запроса передачи данных в MkPC?
	{
 		SetSpeedPeriod();         //e. and set the answer transfer rate and its periodicity //r. и установить скорость передачи ответа и его периодичность (_trm_cycl)		  
		UART_SwitchSpeed(trm_rate);
		if (Device_Mode < 4)	 //e. work with internal latch
		   Device_Mode = DM_INT_LATCH_DELTA_PS;
		else
		   Device_Mode = DM_EXT_LATCH_DELTA_PS_PULSE;
		CMD_Mode = 1;
	//	B_Delta_PS();
		return;
	}
	else if (wcode == CMD_DELTA_SF)	//  is it the B_DeltaSF command?
	{
		SetSpeedPeriod();            //e. and set the answer transfer rate and its periodicity //r. и установить скорость передачи ответа и его периодичность (_trm_cycl)
		CMD_Mode = 7;
		if ((rcv_buf[3] & 0x0001) != 0) //e.reset of accumulated number is disabled //r. сброс накопленного между защелками числа запрещен 
		 Ext_Latch_ResetEnable = 0;
		else 
		 Ext_Latch_ResetEnable = 1;
		B_Delta_SF();
		return;
	}
	else if (wcode == CMD_BIT_MODE)         //e. is this the BIT mode command? //r. это команда тестирования?
	{
		CMD_Mode = 6;       //e. set the value of the Delta mode//r. установить значение режима Delta
		B_BIT_Mode();
		return;
	}	        
	else if (wcode == CMD_DELTA)         //e. is this the B_Delta command? //r. это команда B_Delta?
	{
		CMD_Mode = 2;       //e. set the value of the Delta mode//r. установить значение режима Delta
		B_Delta();           //e. output calculated value of angle increase //r. выводить вычисленное значение приращения угла
		return;
	}
	else if (CMD_Code == CMD_MAINT)         //e. is this the Maintenance mode command? //r. это команда Maintenance mode?
	{
	   if (!RgConB)
	   {
	    RgConB = RATE_VIBRO_1;
		SwitchRefMeandInt(RATE_VIBRO_1);   //e. disable interrupt from referense meander
	   }
		CMD_Mode = 5;       			   //e. yes, switch on present mode //r. да, включить данный режим
		M_Status();          //e. output the status stack of registers //r. вывести статусный набор регистров
		if (Device_Mode != DM_INT_10KHZ_LATCH)		//e. mode of internal latch 10 kHz //r. режим внутренней защелки 10 кГц	
		 {
		   Device_Mode = DM_INT_10KHZ_LATCH;
		   trm_ena = 0;
		 }
		 else
		 UART_SwitchSpeed(trm_rate);
		return;
	}
	else if (wcode == SUBCMD_M_RESET)       //e. is this subcommand for modification of the device control register? //r. это подкоманда модификации регистра управления у-ва?
			{
//				Mk_AskDev();
				M_Reset();
				return;
	}
	else // subcommand analyse
	{
		if (CMD_Mode == 2)	//e. is this the 2(Delta) mode? //r. это режим  2? (Delta)
		{
			if (CMD_Code == CMD_D_PERIOD_W)    //e. the Delta mode, check the subcommand ... //r. режим DElta, проверить подкоманду загрузки периода
			{				
				D_Period_W();        //e. otherwise - set the data transfer period //r. иначе установитиь период выдачи данных
				return;
			}
			else
			{
				line_sts = line_sts | CODE_ERR; // invalid command code
				return;
			}
		}
		else if (CMD_Mode == 5) //e. check the 5 mode (Maintenance) //r. проверка режима 5 (Maintenance)
		{
			if (CMD_Code == SUBCMD_M_CLEAR)       //e. is this subcommand of cleaning of the error register? //r. это подкоманда очистки регистра ошибок линии?
			{
		        M_Clear();           //e. yes, cleaning the error register //r. да, очистка регистра ошибок
				M_Status();          //e. preparing for trabsfer of the device status //r. подготовка передачи статуса устройства
				UART_SwitchSpeed(trm_rate);
				return;
			}
			else if (CMD_Code == SUBCMD_M_MIRR)        //e. is this subcommand of return of previous command? //r. это подкоманда возврата предыдущей команды?
			{
				rx_buf_copy = 0;                 //e. yes, forbid copying of command on saving previous //r. да, запретить копирование команды для сохранения предыдущей
		        M_Mirror();          //e. prepare transfer of the receiving buffer copy //r. подготовить передачу копии приемного буфера
				return;
			}
		/*	else if (wcode == SUBCMD_M_TMP_W)       //e. is this subcommand for writing instructions to the AD7714? //r. это подкоманда записи инструкций в AD7714?
			{
			//	M_Tmp_W();           //e. yes, procedure call of write of the command //r. да, вызов процедуры записи команды
				return;
			} */
			else if (wcode == SUBCMD_M_TMP_R)       //e. is this subcommand for data read from the AD7714? //r. это подкоманда чтения данных из AD7714?
			{
				M_Tmp_R();           //r. yes, call the procedure for data read //e. да, вызов процедуры чтения  данных				
				return;
			}
		/*	else if (wcode == SUBCMD_M_E5R_W)       //e. is this subcommand for write to the mode register? //r. это подкоманда записи в регистр режима?
			{
				M_e5r_W();           //e. yes, write data to the mode register of the Elio5 card //r. да, записать данные в регистр режима платы Elio5
				return;
			}
			else if (wcode == SUBCMD_M_E5RA_W)      //e. is this subcommand for write to the mode register? //r. это подкоманда записи в регистр режима?
			{
				M_e5rA_W();          //e. yes, write data to the mode register of the Elio5 card //r. да, записать данные в регистр режима платы Elio5
				return;
			}  */
			else if (wcode == SUBCMD_M_ADC_R)       //e. is t the subcommand for data read from ADCs ADS1250 and ADS8321 ? //r. это подкоманда чтения данных из АЦП ADS1250 и ADS8321?
			{
				M_ADC_R();           //e. read of ADC channels //r. чтение каналов АЦП
				return;
			}
			else if (wcode == SUBCMD_M_CNT_R)       //e. is this subcommand of data read from pulse counters? //r. это подкоманда чтения данных из счетчиков импульсов?
			{
				M_Cnt_R();           //e. read of counters //r. чтение счетчиков
				return;
			}
			else if (wcode == SUBCMD_M_FLG_R)       //e. is it the command of data read about input flags? //r. это подкоманда чтения данных о входных флагах?
			{
				M_Flg_R();           //e. read the input flags //r. читать входные флаги
				return;
			}
			else if (wcode == SUBCMD_M_VIB_W)       //e. is this subcommand for load the dither drive period? //r. это подкоманда загрузки периода вибропривода?
			{
				M_Vib_W();           //e. set the new value of the period //r. установить новое значение периода
				return;
			}
			else if (wcode == SUBCMD_M_GPH_W)       //e. is this subcommand for set the gain factor of photodetector channels? //r. это подкоманда установки усиления каналов фотоприемника ?
			{
				M_Gph_W();           //e. set the new values of gain factor //r. установить новые значения усиления
				return;
			}
			else if (wcode == SUBCMD_M_STIMUL)      //e. is this subcommand for set the values of the DACs and flags? //r. это подкоманда установки значения ЦАПов и фдагов ?
			{
				M_Stimul();          //e. set the new values //r. установить новые значения
				return;
			}
			else if (wcode == SUBCMD_M_RATE)        //e. is this subcommand for set the M_Rate output stack //r. это подкоманда установки выдачи набора M_Rate ?
			{
				M_Rate();            //e. switch on the M_Rate data transfer //r. включить передачу данных M_Rate
				return;
			}
			else if (wcode == SUBCMD_M_PARAM_W)     //e. is this subcommand for write the new value of the GLD parameter ... //r. это подкоманда записи нового значения параметра GLD
			{
				M_Param_W();         //e. write the new value of the parameter //r. записать новое значение параметра
				return;
			}
			else if (wcode == SUBCMD_M_PARAM_R)     //e. is this subcommand for read the value of the GLD parameter? //r. это подкоманда чтения значения параметра GLD?
			{
				M_Param_R();         //e. read the value from the data memory //r. считать значение параметра из памяти даных
				return;
			}
			else if (CMD_Code == SUBCMD_M_LDPAR_F)     //e. is this subcommand for load GLD parameters ... //r. это подкоманда загрузки параметров GLD
			{
				M_LdPar_F();         //e. start the loading of parameters //r. запустить загрузку параметров
				return;
			}
			else if (CMD_Code == SUBCMD_M_LDPAR_D)     //e. is this subcommand for direct setting of the GLD parameters ... //r. это подкоманда прямого назначения параметров GLD
			{
				M_LdPar_D();         //e. read the value of the parameter from the data memory //r. считать значение параметра из памяти даных
				return;
			}
			else if (CMD_Code == SUBCMD_M_START)       //e. is this subcommand for GLD starting? //r. это подкоманда запуска GLD?
			{
				M_Start();           //e. call the procedure of start //r. запустить процедуру старта
				return;
			}
			else if (CMD_Code == SUBCMD_M_PULSE)       //e. is this subcommand for GLD starting? //r. это подкоманда запуска GLD?
			{
				M_Pulse();           //e. call light-up function //r. выдать импульс поджига
				return;
			}
			else if (CMD_Code == SUBCMD_M_STOP)        //e. is this subcommand for GLD switch off? //r. это подкоманда выключения GLD?
			{
				M_Stop();            //e. otherwise- switch off all regulators and stop device //r. выключить все контуры и остановить прибор
				return;
			}
			else if (wcode == SUBCMD_M_CTL_R)       //e. is this subcommand for read the device control register? //r. это подкоманда чтения регистра управления у-ва?
			{
				M_Ctl_R();
				return;
			}
			else if (wcode == SUBCMD_M_CTL_M)       //e. is this subcommand for modification of the device control register? //r. это подкоманда модификации регистра управления у-ва?
			{
				M_Ctl_M();
				return;
			}
/*			else if (wcode == SUBCMD_M_RESET)       //e. is this subcommand for modification of the device control register? //r. это подкоманда модификации регистра управления у-ва?
			{
				M_Reset();
				return;
			}*/
			else
			{
		  	    line_sts = line_sts | MODE_ERR; // mode error
				return;
			}
		}
	}
} // exec_CMD
       
/******************************************************************************/
void decode_CMD(void)
{ 
	int size;
  
	if (!rcv_Rdy)	//e. is data in receive buffer? //r. в приемном буфере есть информация?
	{
		return; //e. if no, stop processing //r. если нет, закончить обработку
	}
	
	if (!line_sts) //e. Whether there were errors of receiving of start-bit? //r. были ли ошибки приема старт-бита ?
	{  
		//e. there were not errors of receiving of bytes, check the device address //r. ошибок приема байтов не было, проверить адрес обращения
		//Is_Brodcast_Req = 0;        //e. reset the flag of broadcasting request //r. сбросить флаг широковещательного запроса
		if (rcv_buf[1] != Device_blk.Str.My_Addres)
		{
			if (rcv_buf[1] != BROADCAST_ADDRESS)
			{
				goto end;
			}
		}
		
			if (rcv_buf[0] == COMMAND_PREFIX)
	  	    {					//e. there is new command in the receiver buffer, stop the transfer //r. в буфере приемника есть новая команда, прекратить передачу
				trm_ena = 0; 	//e. reset the flag of transmission allowing //r. сбросить флаг разрешения передачи
				exec_CMD();  
								//e. check up presence of errors in operation of this procedure //r. проверить, наличие ошибок в работе данной поцедуры
								//e. did is the "unknown code of a command" or "parameters were set incorrectly" error? //r. была ошибка "неизвестный код команды" или "неверно заданные параметры"?								
				if ( ((line_sts & CODE_ERR) == CODE_ERR) || ((line_sts & PARAM_ERR) == PARAM_ERR) )
				{
					line_err = line_sts;  //e. set error bits in the error register of the line //r.  установить биты ошибок в регистре ошибок линии
				}
			}
		  	else 
		  	{
		  		line_err = line_sts | NO_CMD_ERR; // добавление ошибки в регистр ошибок
		  	}
		  	
	}
	else
	{
		line_err = line_sts;
	}
end:
	if (rx_buf_copy)						//e. is copying of present received packet needed? //r. требуется копирование данного принятого пакета?
	{
		size = rcv_num_byt; 
		rcv_byt_copy = rcv_num_byt; 		//e. save the size of copy //r. запомнить размер копии
		if (rcv_num_byt > sizeof(rcv_buf))	//e. compare the size of receiving buffer with amount of received bytes //r. копирование требуется, сравнить размер приемного буфера с числом принятых байт
		{
			size = sizeof(rcv_buf);
		}
		memcpy(rcv_copy, rcv_buf, size);	//e. copy of received amount of bytes, but no more than buffer size //r. копировать принятое число байт, но не более размера буфера
	}
    rx_buf_copy = 0;						//e. reset the flag of necessity of copying of receiving buffer  //r. сбросить флаг необходимости копирования приемного буфера
   	
	do
	    rcv_buf[--rcv_num_byt] = 0;
	while(rcv_num_byt);
		 rcv_num_byt_old = rcv_num_byt;
    line_sts = 0;							//e.  reset the line status register //r. сбросить регистр статуса линии
    rcv_Rdy = 0;							//e. allow further data reception //r. разрешить дальнейший прием данных
} // decode_CMD

/******************************************************************************/
