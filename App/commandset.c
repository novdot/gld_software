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
	RgConB = RATE_VIBRO_1;				//e. set in the additional register of device control the mode of work with dither counters  and the filter of moving average //r. ������������� � �������������� �������� ���������� ����� ������ � ��������������� � �������� ����������� ��������

	Valid_Data = 0;						// reset all bits of status word

	num_of_par = 5;//8;        			    //e. 8 parameters output //r. �������� 8 ���������                       
	addr_param[0] = &Output.Str.SF_dif;	    //e. set the first parameter address //r. ������ ����� ������� ���������
	addr_param[1] = &Out_main_cycle_latch;    //e. set the second paremeter address //r. ������ ����� ������� ���������
	addr_param[2] = &Out_T_latch;//F_ras;
	addr_param[3] = &Output.Str.WP_reg;
	addr_param[4] = &Output.Str.Tmp_Out;
	size_param[0] = 8;        		//e. the length of the 1st parameter is 4 bytes //r. �������� 1  ���e� ����� 4 �����
	size_param[1] = 4;        		//e. the length of the 2nd parameter is 1 byte (!! - the high byte, instead of low is transmitted) //r. �������� 2 ����� ����� 1 ���� (!!! ���������� ������� ����, � �� �������)
	size_param[2] = 2;
	size_param[3] = 2;
	size_param[4] = 12;
	trm_ena = 1;        			//e. allow operation of the transmitter //r. ��������� ������ �����������
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
 

	num_of_par = 2;        //e. 2 parameters output //r. �������� 2 ���������
	addr_param[0] = &Output.Str.PS_dif;           //e. set the first parameter address //r. ������ ����� ������� ���������
	addr_param[1] = &paramTmpWord;
	size_param[0] = 2;        		//e. the 1st parameter has 2 bytes length //r. �������� 1  ���e� ����� 2 ����
	size_param[1] = 2;        		//e. the 2nd and 3rd parameters have 1 bytes length//r. ��������� 2,3 ����� ����� 1 ����
	trm_ena = 1;        			//e. allow operation of the transmitter //r. ��������� ������ �����������

	index++;
	if (index > 21)
	{
		index = 0;
	}	
} // B_Delta_PS_execution

/******************************************************************************/
void M_Mirror(void)       //e.---------------- prepare to transmission of the copy of receiving buffer ------------- //r.---------------- ���������� �������� ����� ��������� ������ -------------
{
	num_of_par = 1;     //e. 1 parameter output //r. �������� 1 ��������

	addr_param[0] =  &rcv_copy;         //e. set the address of the receiving buffer //r. ��������� ������ ��������� ������

	//e. multiply the number of copied words on 2, since each //r. �������� ����� ������������� ���� �� 2, �.�. ������
	//e. will be trasferred in two steps: first zero, and then //r. ����� ������������ � ��� ������: ������� �������, � �����
	size_param[0] = rcv_byt_copy << 1; // multiplay by 2
	if (size_param[0] >= 64)
	{
		size_param[0] = 64;		//e. maximal amount - no more than double length of the copy buffer //r. ������������ ���������� - �� ����� ������� ����� ������-�����
	}
	trm_rate = 0;       //e. set the transfer rate to the 38400 bauds //r. ���������� �������� �������� 38400 ���
	trm_cycl = 0;       //e. forbid cyclic transmission of the parameter //r. ��������� �������� ��������� � �����
	trm_ena = 1;        //e. allow operation of the transmitter //r. ��������� ������ �����������
} // M_Mirror
  
/******************************************************************************/      
void Mk_Ask1(void)        //r.----------------- prepare of the standart answer 1 ---------------------- //r.----------------- ���������� ������������ ������ 1 ----------------------
{
	CMD_Code &= 0xff00;			//e. clear bits of errors //r. �������� � ��� ���� ������
	num_of_par = 1;             //e. total amount parameters in aswer - 1  //r. ����� ���������� � ������ - 1
	addr_param[0] = &CMD_Code;  //e. and this parameter - returnable command code //r. � ���� �������� - ������������ ��� �������
	size_param[0] = 2;          //e. and both its bytes //r. ������ ��� ��� �����
	trm_ena = 1;              	//e. allow operation of the transmitter of the device //r. ��������� ������ ����������� ����������
} // Mk_Ask1

/******************************************************************************/
void Mk_AskDev(void)        //r.----------------- answer on a command for device mode set ------------ //r.----------------- ����� �� ������� ��������� ������ ������� ------------
{
	num_of_par = 2;     //e. 2 parameters output //r. �������� ��� ���������
	addr_param[0] = &Device_Mode;	//e. address of the counter mode register (intenal latch, external latch, etc.) //r. ����� �������� ������ ��������� (�����., ������� ������� � �.�.)
	addr_param[1] = &SRgR;   	//e. address of the mode register of the processor card //r. ����� �������� ������ ����� ����������
	size_param[0] = 2;  			//e. size of the counter mode register - 2 bytes //r. ������ �������� ������ ��������� - 2 �����
	size_param[1] = 2;  			//e. size of the mode register of the processor card //r. ������ �������� ������ ����� ����������
	trm_cycl = 0;       			//e. forbid cyclic transmission of the parameter //r. ��������� �������� ��������� � �����
	trm_ena = 1;        			//e. allow operation of the transmitter //r. ��������� ������ �����������
} // Mk_AskDev

/******************************************************************************/
void GLD_Output(void)		//e. ----------- Output modes --------- //r. --------- ������ ������ ---------
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
			CMD_Code &= 0xff1f; //e. reset bits of current command code settings of periodicity and transfer rate //r. �������� � ������� ���� ������� ���� ��������� ������������� � �������� ��������
			if (CMD_Code == 0xdd02)	 //e. is it the Rate2 mode? //r. ��� ����� Rate2?
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
//e. set main device operation modes: //r. ��������� �������� ������� ������ �������:
//e. 1. acions with counters (internal latch, external latch with request over line, //r. 1. ������ �� ���������� (���������� �������, ������� ������� � �������� �� �����,
//e. external latch without request over line (instant answer after receiving of external latch pulse)) //r. ������� ������� ��� ������� �� ����� (����� ����� ����� ������� �������� �����. �������))
//e. 2. line receiving/transmission rate //r. 2. �������� ������/�������� �� �����
void B_Dev_Mode(void)  
{
	Device_Mode = rcv_buf[3] & 0x00ff; //e. read the byte of command parameter from the receiver buffer //r. ������� �� ������ ��������� ���� ��������� �������
										//e. and write it to the counter mode register  //r. � ���������� � ������� ������ ���������
	trm_cycl = 0;	//e. periodic data transmission is not needed //r. ������������� �������� ������ �� ���������
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
	trm_cycl = 0;	//e. periodic data transmission is not needed //r. ������������� �������� ������ �� ���������
	Mk_AskDev();	
} // B_BIT_Mode

/******************************************************************************/
void M_Stimul(void)       //e. === procedure of output of analog (DAC) and digital (flags) stimuluses //r. === ��������� ������ ���������� (���) � ���������� (�����) ��������
{
	uint32_t chan;
	
	ScopeMode = 4; //e. WP_PHASE_DETECTOR allocate a mode of display for a control point of an scope //r. WP_PHASE_DETECTOR ��������� ����� ����������� ��� ����������� ����� ������������

	chan = CMD_Code & 0x0007;           //e. extracting the number of stimulus //r. ��������� ������ �������

    Output.ArrayOut[chan] = (((int)rcv_buf[4] << 8) | (int)rcv_buf[5])-0x8000;
		 	
	if ((CMD_Code & (1 << 7)) == 0) //e. to estimate: whether the answer is required //r. �������: ��������� �� �����
	{
		return;			//e. if no - return //r. ���� ��� - �������
	}
	Mk_Ask1();           //e. otherwise - answer output //r. ����� - ������ �����
} // M_Stymul
   
/******************************************************************************/     
void M_Status(void)        //e. === procedure of initialization of transmission of the device status //r. === ��������� ������������� �������� ������� ����������
{
	SetSpeedPeriod();        //e. and set the answer transfer rate and its periodicity //r. � ���������� �������� �������� ������ � ��� �������������

	num_of_par = 2;     //e. 2 parameters output //r. �������� ��� ���������
	addr_param[0] = &blt_in_test;      //e. the register address of the self-testing result //r. ����� �������� ���������� ����������������
	addr_param[1] = &ser_num;   //e. address of the register of errors of line //r. ����� �������� ������ �����
	size_param[0] = 2;     //e. size of the self-test register - 2 bytes //r. ������ �������� ���������������� - 2 �����
	size_param[1] = 2;   //e. size of the register of errors of line //r. ������ �������� ������ �����

	trm_rate = 0;       //e. set the transfer rate to the 38400 bauds //r. ���������� �������� �������� 38400 ���
	trm_cycl = 0;       //e. forbid cyclic transmission of the parameter //r. ��������� �������� ��������� � �����
	trm_ena = 1;        //e. allow operation of the transmitter //r. ��������� ������ �����������
} // M_Status

/******************************************************************************/
void M_Clear(void)        //e.---------------- cleaning of the register of errors of line -------------------------- //r.---------------- ������� �������� ������ ����� --------------------------
{
	line_err = 0;
} // M_Clear

/******************************************************************************/
void M_Tmp_W(void)        //e.----------------- write the command to the AD7714 IC ------------------------- //r.----------------- ������ ������� � ����� AD7714 -------------------------
{
	Mk_Ask1();
}

/******************************************************************************/
void M_Tmp_R(void)        //e.----------------- read the data from the AD7714 IC -------------------------- //r.----------------- ������ ������ �� ���� AD7714 --------------------------
{

} // M_Tmp_R

/******************************************************************************/
void M_ADC_R(void)        //e.----------------- read the data from the ADCs ADS1250, ADS8321 --------- //r.----------------- ������ ������ �� ������� ��� ADS1250, ADS8321 ---------
{
	SetSpeedPeriod();        		//e. set the answer transfer rate and its periodicity //r. ���������� �������� �������� ������ � ��� �������������
	UART_SwitchSpeed(trm_rate);
	
	num_of_par = 4;     		//e. 4 parameters output //r. �������� ������ ���������
	addr_param[0] = 0; 			//e. //r. <!-- �������������� ��� ���, ������ �� ������������
	addr_param[1] = 0;			//e. //r.  -->
	addr_param[2] = 0;
	//addr_param[3] = &(Input.StrIn.HF_out); //e. set the 4th parameter address  _HF_out //r. ������ ����� ���������� ��������� _HF_out
	addr_param[3] = &(g_input.word.hf_out);
    size_param[0] = 2;     		//e. size of the parameters - 2 bytes //r. ������ ���������� - 2 �����
	size_param[1] = 2;
	size_param[2] = 2;
	size_param[3] = 2;
	trm_ena = 1;        		//e. allow operation of the transmitter //r. ��������� ������ �����������
} // M_ADC_R

/******************************************************************************/
void M_Cnt_R(void)        //r.----------------- read the counters of the Elio5 card -------------------------- //r.----------------- ������ ��������� ����� Elio5 --------------------------
{
	SetSpeedPeriod();        		//e. set the answer transfer rate and its periodicity //r. ���������� �������� �������� ������ � ��� �������������
	UART_SwitchSpeed(trm_rate);
	num_of_par = 2;     		//e. 2 parameters output //r. �������� ��� ���������

	addr_param[0] = &(Output.Str.Cnt_Pls);   //e. set the first parameter address //r. ������ ����� ������� ���������
	addr_param[1] = &(Output.Str.Cnt_Mns);   //e. set the second paremeter address //r. ������ ����� ������� ���������
	size_param[0] = 2;     		//e. size of the parameters - 2 bytes //r. ������ ���������� - 2 �����
	size_param[1] = 2;
	trm_ena = 1;        		//e. allow operation of the transmitter //r. ��������� ������ �����������
} // M_Cnt_R
    
/******************************************************************************  
/void M_e5r_W(void)        //r.----------------- write to the mode register of the Elio5 card ------------------ //r.----------------- ������ � ������� ������ ����� Elio5  ------------------
{
	//e. read the byte of command parameter from the receiver buffer //r. ������� �� ������ ��������� ���� ��������� �������
	//e. and write it to the card mode register and its copy //r. � �������� ��� � ������� ������ ����� � � ��� �����
#if !defined COMMAND_DEBUG  
	Copy_e5_RgR = rcv_buf[3];  
	io_space_write(E5_RGR, Copy_e5_RgR);  
#endif
	trm_cycl = 0;      //e. periodic data transmission is not needed //r. ������������� �������� ������ �� ���������
	Mk_Ask1();
} // M_e5r_W  */

/******************************************************************************
void M_e5rA_W(void)       //e.----- write to the 1st additional mode register of the Elio5 card  ---------- //r.----- ������ � 1-�� �������������� ������� ������ ����� Elio5  ----------
{
	//e. this command is used for switching a signal on which data counters are latched: //r. ��� ������� ������������ ��� ������������ �������, �� �������� �������������
	//e. on the Reper signal or on Sign Meander //r. �������������� ��������: ���� �� Reper`� ���� �� RefMeandr`�
	
	//e. read the byte of command parameter from the receiver buffer //r. ������� �� ������ ��������� ���� ��������� �������
	//e. and write it to the card mode register and its copy //r. � �������� ��� � ������� ������ ����� � � ��� �����        
	Copy_e5_RgRA = rcv_buf[3];    
//???	Device_Mode = Copy_e5_RgRA; //e. and write it to the counter mode register  //r. � ���������� � ������� ������ ���������
#if !defined COMMAND_DEBUG 
	io_space_write(E5_RGRA, Copy_e5_RgRA);
#endif
	trm_cycl = 0;      			//e. periodic data transmission is not needed //r. ������������� �������� ������ �� ���������
	Mk_Ask1();
} // M_e5rA_W

/******************************************************************************/ 
void M_Ctl_R(void)        //r.----------------- reading the control register of the device ----------------- //r.----------------- ������ �������� ���������� ���������� -----------------
{
	num_of_par = 2;                 //e. 2 parameters transfer //r. ���������� 2 ���������
	addr_param[0] = &CMD_Code;         //e. the first parameter in answer - returned command code //r. ������ �������� � ������ - ������������ ��� �������
	size_param[0] = 2;
	size_param[1] = 2;   //e. two bytes also have control registers //r. ��� ����� ����� ����� � �������� ����������
	if ((rcv_buf[3] & (1 << 4)) == 0) //e. is main control register needed? //r. ��������� �������� ������� ����������?
	{
		addr_param[1] = &RgConA;	//e. yes //r. ��
	}
	else
	{
		addr_param[1] = &RgConB;	//e. otherwise - load the address of the addititonal register //r. ����� - ��������� ����� ��������������� ��������
	}
	CMD_Code &= 0xff10;           //e. clear in it bit of errors and byte number //r. �������� � ��� ���� ������ � ������ ����
	trm_ena = 1;                 //e. allow operation of the transmitter of line //r. ��������� ������ ����������� �����
} // M_Ctl_R
       
/******************************************************************************/ 
void M_Ctl_M(void)        //r.----------------- modification of the control register of the device ------------ //r.----------------- ����������� �������� ���������� ���������� ------------
{
	uint32_t * ptr;
	uint32_t bit_numb;
	
	num_of_par = 2;                 //e. 2 parameters transfer //r. ���������� 2 ���������
	addr_param[0] = &CMD_Code;         //e. the first parameter in answer - returned command code //r. ������ �������� � ������ - ������������ ��� �������
	size_param[0] = 2;
	size_param[1] = 2;   //e. two bytes also have control registers //r. ��� ����� ����� ����� � �������� ����������
	if ((CMD_Code & (1 << 4)) == 0) //e. is main control register needed? //r. ��������� �������� ������� ����������?
	{
		ptr = &RgConA;	//e. yes //r. ��
	}
	else
	{
		ptr = &RgConB;	//e. otherwise - load the address of the addititonal register //r. ����� - ��������� ����� ��������������� ��������
	}
	addr_param[1] = ptr;         //e. the second parameter in answer - modified register //r. ������ �������� � ������ - ���������������� �������
	
	bit_numb = CMD_Code & 0x000f;    //e. extract the number of the changeable bit //r. �������� ����� ����������� ���� 
	if ((CMD_Code & (1 << 7)) == 0) // Is clear bit
	{
		*ptr &= ~(1 << bit_numb); 	// yes, clear bit
	}
	else
	{
		*ptr |= 1 << bit_numb;		// no, set bit
	}
	
	CMD_Code &= 0xff10;           //e. clear in command bit of errors and byte number //r. �������� � ������� ���� ������ � ������ ����
	trm_cycl = 0;       //e. forbid cyclic transmission of the parameter //r. ��������� �������� ��������� � �����
	trm_ena = 1;                 //e. allow operation of the transmitter of line //r. ��������� ������ ����������� �����
} // M_Ctl_M
       
/******************************************************************************/
void M_Flg_R(void)        //e.------------ reading the register of input flags --------------------------- //r.------------ ������ �������� ������� ������ ---------------------------
{
	num_of_par = 1;     //e. 1 parameter output //r. �������� ���� ��������
	addr_param[0] = &In_Flag;	//e. the address of the flag register copy //r. ����� ����� �������� ������
	size_param[0] = 2;     		//e. size of the buffer -  2 bytes //r. ������ ������  - 2 �����
	trm_ena = 1;        		//e. allow operation of the transmitter //r. ��������� ������ �����������
} // M_Flg_R
      
/******************************************************************************/  
void M_Vib_W(void)        //r.------------ set the oscillation period of the dither drive ----------------- //r.------------ ��������� ������� ��������� ������������ -----------------
{
	ScopeMode = 1; //e. VB_PHASE_DETECTOR allocate a mode of display for a control point of an scope //r. VB_PHASE_DETECTOR ��������� ����� ����������� ��� ����������� ����� ������������

	Output.Str.T_Vibro = (rcv_buf[4] << 8) | (rcv_buf[5] & 0xFF); //e. new variable of the period //r. ����� ���������� �������
	Output.Str.L_Vibro= (rcv_buf[6] << 8) | (rcv_buf[7] & 0xFF); //e. new variable of the pulse width //r. ����� ���������� ������������ ���������
	VibroDither_Set();       //e. and output its value to period registers on card  //r. ������� �� �������� � �������� ������� �� �����

	trm_cycl = 0;      //e. periodic data transmission is not needed //r. ������������� �������� ������ �� ���������
	Mk_Ask1();
} // M_Vib_W

/******************************************************************************/
void M_Gph_W(void)        //e.------------ set the gain factor of photodetector channels ------------------- //r.------------ ��������� �������� ������� ������������� -------------------
{
	Device_blk.Str.Gain_Ph_A = rcv_buf[4];     //e. read from the receiver buffer the value of the gain factor of the A channel //r. ������� �� ������ ��������� �������� �������� ������ �
	Device_blk.Str.Gain_Ph_B = rcv_buf[5];     //e. read from the receiver buffer the value of the gain factor of the B channel //r. ������� �� ������ ��������� �������� �������� ������ �

	Out_G_photo(Device_blk.Str.Gain_Ph_A, Device_blk.Str.Gain_Ph_B);       //e. display these values to digital potentiometers //r. ������� ��� �������� � �������� �������������

	trm_cycl = 0;      //e. periodic data transmission is not needed //r. ������������� �������� ������ �� ���������
	Mk_Ask1();
} // M_Gph_W

/******************************************************************************/
void M_Rate(void)         //e.------------ start of transfer of the M_Rate parameters stack ------------------- //r.------------ ������ �������� ������ ���������� M_Rate -------------------
{
	uint8_t mode;

	SetSpeedPeriod();        //e. set the answer transfer rate and its periodicity //r. ���������� �������� �������� ������ � ��� �������������
	UART_SwitchSpeed(trm_rate);

	mode = rcv_buf[3] & 0x001f;	//e. extract number of the parameters stack in the command //r. �������� ����� ������ ���������� � �������
	
    switch (mode) {
		case 1:											//e. it is the Rate stack //r. ��� ����� Rate
        	RgConB =  RATE_REPER_OR_REFMEANDR;			//e. counter latch every vibro period 
			SwitchRefMeandInt(RATE_REPER_OR_REFMEANDR); //e. enable reference meander interrupt
					
			wrk_period = 25000000; //e. load needed length of working period 1 �.//r. ��������� ��������� ������������ �������� �������	 1 �.
			num_of_par = 14;     //e. 9 parameters or groups of parameters output //r. �������� 9 ���������� ��� ����� ����������
	
			Valid_Data = 0;						// reset all bits of status word

	        addr_param[0] = &(Output.Str.Cnt_Pls);   	//e. set the address of the 1st parameter in the 1st group //r. ������ ����� ������� ���������  � 1-�� ������
	        addr_param[1] = &(Output.Str.Cnt_Mns);   	//e. set the address of the 1st parameter in the 2nd group //r. ������ ����� ������� ���������  � 2-�� ������
	        addr_param[2] = &(Output.Str.Cnt_Dif);   	//e. set the address of the 1st parameter in the 3rd group //r. ������ ����� ������� ���������  � 3-�� ������
	        addr_param[3] = &(Output.Str.F_ras);     	//e. set the address of the _F_ras parameter in the 4th group //r. ������ ����� ���������  _F_ras �� 4-�� ������
	        addr_param[4] = &(Output.Str.HF_reg);    	//e. set the address of the _HF_reg parameter in the 5th group //r. ������ ����� ���������  _HF_reg � 5-�� ������
	        addr_param[5] = &(Output.Str.HF_dif);    	//e. set the address of the _HF_dif parameter in the 6th group //r. ������ ����� ���������  _HF_dif � 6-�� ������
	        addr_param[6] = &(Output.Str.T_Vibro);   	//e. parameter _T_Vibro //r. �������� _T_Vibro
	        addr_param[7] = &(Output.Str.T_VB_pll);  	//e. parameter _T_VB_pll //r. ��������� _T_VB_pll
	        addr_param[8] = &(Output.Str.L_Vibro);   	//e. parameter _L_Vibro //r. �������� _L_Vibro     
	        //addr_param[9] = &(Input.StrIn.HF_out); 		//e. set the address of the _RI_reg parameter in the 8th group //r. ������ ����� ���������  _RI_reg � 8-�� ������      
	        addr_param[9] = &(g_input.word.hf_out);
            addr_param[10] = &(Output.Str.WP_reg);   	//e. parameters: _WP_reg, _WP_pll //r. ���������� _WP_reg, _WP_pll
	        addr_param[11] = &(Output.Str.WP_pll);   	//e. parameters: _WP_reg, _WP_pll //r. ���������� _WP_reg, _WP_pll
	        addr_param[12] = &(Output.Str.Tmp_Out);      //e. set the address of the temperature sensors array //r. ������ ����� ������� �������� �����������
	        addr_param[13] = &(Output.Str.WP_scope1);    //e. reserved //r. ��������������� ��� ���������� ����������
	
	        size_param[0] = 2;            	//e. groups 1-6, 8-10 consists of one parameter ... //r. ������ 1-6, 8-10 �������� �� ������ ���������
	        size_param[1] = 2;   			//e. and have 2 bytes length each //r. � ����� ����� 2 ����� ������
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
	
	        
	        trm_ena = 1;        //e. allow operation of the transmitter //r. ��������� ������ �����������
			break;
			
		case 2:			//e. it is the Rate2 stack //r. ��� ����� Rate2
		    RgConB = RATE_REPER_OR_REFMEANDR;
			SwitchRefMeandInt(RATE_REPER_OR_REFMEANDR);   //e. enable interrupt from ref. meander
			wrk_period = 0;     //e. frequency of output = fvibro //r. ������� ������ ����� ������� ��

			num_of_par = 2;     //e. 2 parameters output //r. �������� 2 ���������
	
	        addr_param[0] = &(Output.Str.Cnt_Pls);          //e. set the first parameter address //r. ������ ����� ������� ���������
	        addr_param[1] = &(Output.Str.Cnt_Mns);          //e. and the 2nd //r. � �������
	        size_param[0] = 2;     //e. parameters has 2 bytes length //r. ��������� ����� ����� 2 �����
	        size_param[1] = 2;
	        trm_ena = 1;        //e. allow operation of the transmitter //r. ��������� ������ �����������
			break;
			
		case 3:			//e. it is the Rate3 stack //r. ��� ����� Rate3

			wrk_period = 2500;     //e. frequency of output = 10000 Hz //r. n=1, ������� ������ = 10000 Hz
			num_of_par = 2;     //e. 2 parameters output //r. �������� 2 ���������
			
			addr_param[0] = &(Output.Str.WP_scope1);         //e. set the addresses of output parameters //r. ������ ������ ��������� ����������
			addr_param[1] = &(Output.Str.WP_scope2);
			size_param[0] = 2;     //e. all parameters has 2 bytes length //r. ��� ��������� ����� ����� 2 �����
			size_param[1] = 2;
			trm_ena = 1;        //e. allow operation of the transmitter //r. ��������� ������ �����������
			break;
			
		case 4:			//e. it is the Rate4 stack //r. ��� ����� Rate4
        	wrk_period = 20000;     //e. correspond to output frequency 1.25 kHz //r. ������������ ������� ������ 1.25 kHz
			num_of_par = 4;     //e. 4 parameters output //r. �������� 4 ���������

			//e. set the addresses of output parameters //r. ������ ������ ��������� ����������
        	// --- raw data array of numbers ---
		//	addr_param[0] = &Dif_Curr_Array;
        	// --- filtered array of numbers ---
	     //   addr_param[1] = &Dif_Filt_Array;
        	// ---------------------------------
	        addr_param[2] = &(Output.Str.HF_reg); 			//e. HFO regulator //r. ���������� ���
	        addr_param[3] = &(Output.Str.WP_reg); 			//e. CPLC heater regulator //r. ���������� �����������
	        size_param[0] = 16;     
	        size_param[1] = 32;     
	        size_param[2] = 2;
	        size_param[3] = 2;      
			trm_ena = 1;        //e. allow operation of the transmitter //r. ��������� ������ �����������
			break;
		case 7:
			wrk_period = 20000;     //e. correspond to output frequency 1250 Hz //r. ������������ ������� ������ 1.25 kHz
			num_of_par = 5;     //e. 4 parameters output //r. �������� 4 ���������

			//e. set the addresses of output parameters //r. ������ ������ ��������� ����������
        	// --- raw data array of PLC phase ---
			addr_param[0] = &(Output.Str.WP_Phase_Det_Array);
        	// --- filtered array of reference sin---
	        addr_param[1] = &(Output.Str.WP_sin_Array);
        	// ---------------------------------
	        addr_param[2] = &(Output.Str.WP_reg); 			//e. CPLC heater regulator //r. ���������� �����������
	        addr_param[3] = &(Output.Str.WP_pll); 			//e. CPLC phase  //r. ����
	        addr_param[4] = &(Output.Str.HF_reg); //e. set the address of the _HF_reg parameter in the 5th group //r. ������ ����� ���������  _HF_reg � 5-�� ������
	        size_param[0] = 16;     
	        size_param[1] = 16;     
	        size_param[2] = 2;
	        size_param[3] = 2;     
	        size_param[4] = 2;  
			trm_ena = 1;        //e. allow operation of the transmitter //r. ��������� ������ �����������
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
void M_Param_R(void)      //e. ------------ read the parameter of the GLD from the data memory ---------------------- //r.------------ ������ ��������� GLD �� ������ ������ ----------------------
{
	num_of_par = 1;     //e. 1 parameter output //r. �������� ���� ��������
	addr_param[0] = (void *)(&Device_blk.Str.My_Addres + rcv_buf[3]); //e. address of the needed parameter in the block //r. ����� ���������� ��������� � �����
	size_param[0] = 2;     //e. size of the buffer -  2 bytes //r. ������ ������  - 2 �����

	trm_ena = 1;        //e. allow operation of the transmitter //r. ��������� ������ �����������
} // M_Param_R

/******************************************************************************/
void M_Param_W(void)      //e.------------ write the parameter of the GLD from the data memory ----------------------- //r.------------ ������ ��������� GLD � ������ ������ -----------------------
{
	int * ptr;
	
	ScopeMode = 0; //e. VB_DELAY_MEANDER allocate a mode of display for a control point of an scope //r. VB_DELAY_MEANDER ��������� ����� ����������� ��� ����������� ����� ������������
	
	ptr = (int *)&Device_blk.Str.My_Addres; // pointer to Parameters block
	ptr += rcv_buf[3];					// calculate offset
	*ptr = (rcv_buf[4] << 8) | (rcv_buf[5] & 0xFF); // write new parameter value

	trm_cycl = 0;      //e. periodic data transmission is not needed //r. ������������� �������� ������ �� ���������
	Mk_Ask1();
} // M_Param_W

/******************************************************************************/
void M_LdPar_F()      //e.============ procedure for load the GLD parameters from the flash-memory =========== //r.============ ��������� �������� ���������� ��� �� ����-������ ===========
{

	LoadFlashParam(FromFLASH);        //e. load the GLD parameters from the flash-memory //r. ��������� ��������� ��� �� ����-������
	blt_in_test = ((uint32_t)FIRMWARE_VER << 8) | (Device_blk.Str.Device_SerialNumber & 0x00FF);
//	Init_software();
	
	trm_cycl = 0;      //e. periodic data transmission is not needed //r. ������������� �������� ������ �� ���������
	Mk_Ask1();
} // M_LdPar_F

/******************************************************************************/
void M_LdPar_D(void)      	//e.============ procedure for set parameters of the GLD by default ============ //r.============ ��������� ��������� ���������� ��� �� ��������� ============
{
#if !defined COMMAND_DEBUG
	 LoadFlashParam(ByDefault);  	//e. define parameters of the GLD by default //r. ���������� ��������� ��� �� ��������� (default)
	Init_software();
#endif	
	trm_cycl = 0;      		//e. periodic data transmission is not needed //r. ������������� �������� ������ �� ���������
	Mk_Ask1();
} // M_LdPar_D

/******************************************************************************/
void M_Start(void)        //e.============ initialization of the GLD switch on ================================= //r.============ ������������� ������� ��� =================================
{
	//start_Rq = 1;      	//e. set the flag of the GLD switch on request //r. ���������� ���� ������� ������ ���
	trm_cycl = 0;      	//e. periodic data transmission is not needed //r. ������������� �������� ������ �� ���������
	Mk_Ask1();
} // M_Start

/******************************************************************************/
void M_Stop(void)         //e.============ initialization of the GLD switch off ============================== //r.============ ������������� ���������� ��� ==============================
{
	stop_Rq = 1;       	//e. set the flag of the GLD switch off request //r. ���������� ���� ������� ���������� ���
	trm_cycl = 0;      	//e. periodic data transmission is not needed //r. ������������� �������� ������ �� ���������
	Mk_Ask1();
} // M_Stop    

/******************************************************************************/
void M_Pulse(void)        //e.============ generetion of the light-up pulse ========================= //r.============ ��������� �������� ������� ������ =========================
{
	//pulse_Rq = 1;      	//e. set the flag of the GLD switch on request //r. ���������� ���� ������� ������ ���
	trm_cycl = 0;      	//e. periodic data transmission is not needed //r. ������������� �������� ������ �� ���������
	Mk_Ask1();
} // M_Pulse

/******************************************************************************/
void B_Rate(void)         //e. === procedure of device operation in a mode of continuous output of raw data //r. === ��������� ������ ������� � ������ ����������� ������ "�����" ������
{
} // B_Rate

/******************************************************************************/
void B_Delta(void)        //e. === procedure not used //r. === ��������� �� ������������
{
} // B_Delta

/******************************************************************************/
void D_Period_W(void)     //e. === set the period of the angle increase output //r. === ��������� ������� ������ ���������� ����
{
} // D_Period_W

/******************************************************************************/
void exec_CMD(void)       //e. === the final decoding and command execution procedure //r. === ��������� ������������� ���������� � ���������� �������
{
	uint32_t wcode;
	
	rx_buf_copy = 1;                 //e. initialization of the flag of copying of receiving buffer //r. ����������� ����� ����������� ��������� ������

	wcode = (rcv_buf[2] & 0xFF) << 8;
	CMD_Code = wcode | (rcv_buf[3] & 0xFF);	//e. save it in the memory for echo-transmission //r. ��������� ��� � ������ ��� �������� ��������
    
	if (wcode == CMD_RATE)			//e. is it Rate command? //r. ��� ������� Rate?
	{
		B_Rate();
		return;
	}
	else if (wcode == CMD_DEV_MODE)	//e. is it the Device_Mode command? //r. ��� ������� ��������� ������ �������?
	{
		CMD_Mode = 3;
		B_Dev_Mode();
		return;
	}
	else if (wcode == CMD_DELTA_BINS) //e. is it the B_DeltaBINS command (command of request for data transfer to the navigation system)? //r. ��� ������� ������� �������� ������ � �����.�������?
	{
		RgConB = RATE_VIBRO_1;		  //e. set in the additional register of device control the mode of work with dither counters  and the filter of moving average //r. ������������� � �������������� �������� ���������� ����� ������ � ��������������� � �������� ����������� ��������
		SetSpeedPeriod(); 
		UART_SwitchSpeed(trm_rate);
		CMD_Mode = 4;
		B_Delta_BINS();
		return;
	}
	else if (wcode == CMD_DELTA_PS)	//e.  is it the B_DeltaPS command (command of request for data transfer to the uPC)? //r. ��� ������� ������� �������� ������ � MkPC?
	{
 		SetSpeedPeriod();         //e. and set the answer transfer rate and its periodicity //r. � ���������� �������� �������� ������ � ��� ������������� (_trm_cycl)		  
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
		SetSpeedPeriod();            //e. and set the answer transfer rate and its periodicity //r. � ���������� �������� �������� ������ � ��� ������������� (_trm_cycl)
		CMD_Mode = 7;
		if ((rcv_buf[3] & 0x0001) != 0) //e.reset of accumulated number is disabled //r. ����� ������������ ����� ��������� ����� �������� 
		 Ext_Latch_ResetEnable = 0;
		else 
		 Ext_Latch_ResetEnable = 1;
		B_Delta_SF();
		return;
	}
	else if (wcode == CMD_BIT_MODE)         //e. is this the BIT mode command? //r. ��� ������� ������������?
	{
		CMD_Mode = 6;       //e. set the value of the Delta mode//r. ���������� �������� ������ Delta
		B_BIT_Mode();
		return;
	}	        
	else if (wcode == CMD_DELTA)         //e. is this the B_Delta command? //r. ��� ������� B_Delta?
	{
		CMD_Mode = 2;       //e. set the value of the Delta mode//r. ���������� �������� ������ Delta
		B_Delta();           //e. output calculated value of angle increase //r. �������� ����������� �������� ���������� ����
		return;
	}
	else if (CMD_Code == CMD_MAINT)         //e. is this the Maintenance mode command? //r. ��� ������� Maintenance mode?
	{
	   if (!RgConB)
	   {
	    RgConB = RATE_VIBRO_1;
		SwitchRefMeandInt(RATE_VIBRO_1);   //e. disable interrupt from referense meander
	   }
		CMD_Mode = 5;       			   //e. yes, switch on present mode //r. ��, �������� ������ �����
		M_Status();          //e. output the status stack of registers //r. ������� ��������� ����� ���������
		if (Device_Mode != DM_INT_10KHZ_LATCH)		//e. mode of internal latch 10 kHz //r. ����� ���������� ������� 10 ���	
		 {
		   Device_Mode = DM_INT_10KHZ_LATCH;
		   trm_ena = 0;
		 }
		 else
		 UART_SwitchSpeed(trm_rate);
		return;
	}
	else if (wcode == SUBCMD_M_RESET)       //e. is this subcommand for modification of the device control register? //r. ��� ���������� ����������� �������� ���������� �-��?
			{
//				Mk_AskDev();
				M_Reset();
				return;
	}
	else // subcommand analyse
	{
		if (CMD_Mode == 2)	//e. is this the 2(Delta) mode? //r. ��� �����  2? (Delta)
		{
			if (CMD_Code == CMD_D_PERIOD_W)    //e. the Delta mode, check the subcommand ... //r. ����� DElta, ��������� ���������� �������� �������
			{				
				D_Period_W();        //e. otherwise - set the data transfer period //r. ����� ����������� ������ ������ ������
				return;
			}
			else
			{
				line_sts = line_sts | CODE_ERR; // invalid command code
				return;
			}
		}
		else if (CMD_Mode == 5) //e. check the 5 mode (Maintenance) //r. �������� ������ 5 (Maintenance)
		{
			if (CMD_Code == SUBCMD_M_CLEAR)       //e. is this subcommand of cleaning of the error register? //r. ��� ���������� ������� �������� ������ �����?
			{
		        M_Clear();           //e. yes, cleaning the error register //r. ��, ������� �������� ������
				M_Status();          //e. preparing for trabsfer of the device status //r. ���������� �������� ������� ����������
				UART_SwitchSpeed(trm_rate);
				return;
			}
			else if (CMD_Code == SUBCMD_M_MIRR)        //e. is this subcommand of return of previous command? //r. ��� ���������� �������� ���������� �������?
			{
				rx_buf_copy = 0;                 //e. yes, forbid copying of command on saving previous //r. ��, ��������� ����������� ������� ��� ���������� ����������
		        M_Mirror();          //e. prepare transfer of the receiving buffer copy //r. ����������� �������� ����� ��������� ������
				return;
			}
		/*	else if (wcode == SUBCMD_M_TMP_W)       //e. is this subcommand for writing instructions to the AD7714? //r. ��� ���������� ������ ���������� � AD7714?
			{
			//	M_Tmp_W();           //e. yes, procedure call of write of the command //r. ��, ����� ��������� ������ �������
				return;
			} */
			else if (wcode == SUBCMD_M_TMP_R)       //e. is this subcommand for data read from the AD7714? //r. ��� ���������� ������ ������ �� AD7714?
			{
				M_Tmp_R();           //r. yes, call the procedure for data read //e. ��, ����� ��������� ������  ������				
				return;
			}
		/*	else if (wcode == SUBCMD_M_E5R_W)       //e. is this subcommand for write to the mode register? //r. ��� ���������� ������ � ������� ������?
			{
				M_e5r_W();           //e. yes, write data to the mode register of the Elio5 card //r. ��, �������� ������ � ������� ������ ����� Elio5
				return;
			}
			else if (wcode == SUBCMD_M_E5RA_W)      //e. is this subcommand for write to the mode register? //r. ��� ���������� ������ � ������� ������?
			{
				M_e5rA_W();          //e. yes, write data to the mode register of the Elio5 card //r. ��, �������� ������ � ������� ������ ����� Elio5
				return;
			}  */
			else if (wcode == SUBCMD_M_ADC_R)       //e. is t the subcommand for data read from ADCs ADS1250 and ADS8321 ? //r. ��� ���������� ������ ������ �� ��� ADS1250 � ADS8321?
			{
				M_ADC_R();           //e. read of ADC channels //r. ������ ������� ���
				return;
			}
			else if (wcode == SUBCMD_M_CNT_R)       //e. is this subcommand of data read from pulse counters? //r. ��� ���������� ������ ������ �� ��������� ���������?
			{
				M_Cnt_R();           //e. read of counters //r. ������ ���������
				return;
			}
			else if (wcode == SUBCMD_M_FLG_R)       //e. is it the command of data read about input flags? //r. ��� ���������� ������ ������ � ������� ������?
			{
				M_Flg_R();           //e. read the input flags //r. ������ ������� �����
				return;
			}
			else if (wcode == SUBCMD_M_VIB_W)       //e. is this subcommand for load the dither drive period? //r. ��� ���������� �������� ������� ������������?
			{
				M_Vib_W();           //e. set the new value of the period //r. ���������� ����� �������� �������
				return;
			}
			else if (wcode == SUBCMD_M_GPH_W)       //e. is this subcommand for set the gain factor of photodetector channels? //r. ��� ���������� ��������� �������� ������� ������������� ?
			{
				M_Gph_W();           //e. set the new values of gain factor //r. ���������� ����� �������� ��������
				return;
			}
			else if (wcode == SUBCMD_M_STIMUL)      //e. is this subcommand for set the values of the DACs and flags? //r. ��� ���������� ��������� �������� ����� � ������ ?
			{
				M_Stimul();          //e. set the new values //r. ���������� ����� ��������
				return;
			}
			else if (wcode == SUBCMD_M_RATE)        //e. is this subcommand for set the M_Rate output stack //r. ��� ���������� ��������� ������ ������ M_Rate ?
			{
				M_Rate();            //e. switch on the M_Rate data transfer //r. �������� �������� ������ M_Rate
				return;
			}
			else if (wcode == SUBCMD_M_PARAM_W)     //e. is this subcommand for write the new value of the GLD parameter ... //r. ��� ���������� ������ ������ �������� ��������� GLD
			{
				M_Param_W();         //e. write the new value of the parameter //r. �������� ����� �������� ���������
				return;
			}
			else if (wcode == SUBCMD_M_PARAM_R)     //e. is this subcommand for read the value of the GLD parameter? //r. ��� ���������� ������ �������� ��������� GLD?
			{
				M_Param_R();         //e. read the value from the data memory //r. ������� �������� ��������� �� ������ �����
				return;
			}
			else if (CMD_Code == SUBCMD_M_LDPAR_F)     //e. is this subcommand for load GLD parameters ... //r. ��� ���������� �������� ���������� GLD
			{
				M_LdPar_F();         //e. start the loading of parameters //r. ��������� �������� ����������
				return;
			}
			else if (CMD_Code == SUBCMD_M_LDPAR_D)     //e. is this subcommand for direct setting of the GLD parameters ... //r. ��� ���������� ������� ���������� ���������� GLD
			{
				M_LdPar_D();         //e. read the value of the parameter from the data memory //r. ������� �������� ��������� �� ������ �����
				return;
			}
			else if (CMD_Code == SUBCMD_M_START)       //e. is this subcommand for GLD starting? //r. ��� ���������� ������� GLD?
			{
				M_Start();           //e. call the procedure of start //r. ��������� ��������� ������
				return;
			}
			else if (CMD_Code == SUBCMD_M_PULSE)       //e. is this subcommand for GLD starting? //r. ��� ���������� ������� GLD?
			{
				M_Pulse();           //e. call light-up function //r. ������ ������� �������
				return;
			}
			else if (CMD_Code == SUBCMD_M_STOP)        //e. is this subcommand for GLD switch off? //r. ��� ���������� ���������� GLD?
			{
				M_Stop();            //e. otherwise- switch off all regulators and stop device //r. ��������� ��� ������� � ���������� ������
				return;
			}
			else if (wcode == SUBCMD_M_CTL_R)       //e. is this subcommand for read the device control register? //r. ��� ���������� ������ �������� ���������� �-��?
			{
				M_Ctl_R();
				return;
			}
			else if (wcode == SUBCMD_M_CTL_M)       //e. is this subcommand for modification of the device control register? //r. ��� ���������� ����������� �������� ���������� �-��?
			{
				M_Ctl_M();
				return;
			}
/*			else if (wcode == SUBCMD_M_RESET)       //e. is this subcommand for modification of the device control register? //r. ��� ���������� ����������� �������� ���������� �-��?
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
  
	if (!rcv_Rdy)	//e. is data in receive buffer? //r. � �������� ������ ���� ����������?
	{
		return; //e. if no, stop processing //r. ���� ���, ��������� ���������
	}
	
	if (!line_sts) //e. Whether there were errors of receiving of start-bit? //r. ���� �� ������ ������ �����-���� ?
	{  
		//e. there were not errors of receiving of bytes, check the device address //r. ������ ������ ������ �� ����, ��������� ����� ���������
		//Is_Brodcast_Req = 0;        //e. reset the flag of broadcasting request //r. �������� ���� ������������������ �������
		if (rcv_buf[1] != Device_blk.Str.My_Addres)
		{
			if (rcv_buf[1] != BROADCAST_ADDRESS)
			{
				goto end;
			}
		}
		
			if (rcv_buf[0] == COMMAND_PREFIX)
	  	    {					//e. there is new command in the receiver buffer, stop the transfer //r. � ������ ��������� ���� ����� �������, ���������� ��������
				trm_ena = 0; 	//e. reset the flag of transmission allowing //r. �������� ���� ���������� ��������
				exec_CMD();  
								//e. check up presence of errors in operation of this procedure //r. ���������, ������� ������ � ������ ������ ��������
								//e. did is the "unknown code of a command" or "parameters were set incorrectly" error? //r. ���� ������ "����������� ��� �������" ��� "������� �������� ���������"?								
				if ( ((line_sts & CODE_ERR) == CODE_ERR) || ((line_sts & PARAM_ERR) == PARAM_ERR) )
				{
					line_err = line_sts;  //e. set error bits in the error register of the line //r.  ���������� ���� ������ � �������� ������ �����
				}
			}
		  	else 
		  	{
		  		line_err = line_sts | NO_CMD_ERR; // ���������� ������ � ������� ������
		  	}
		  	
	}
	else
	{
		line_err = line_sts;
	}
end:
	if (rx_buf_copy)						//e. is copying of present received packet needed? //r. ��������� ����������� ������� ��������� ������?
	{
		size = rcv_num_byt; 
		rcv_byt_copy = rcv_num_byt; 		//e. save the size of copy //r. ��������� ������ �����
		if (rcv_num_byt > sizeof(rcv_buf))	//e. compare the size of receiving buffer with amount of received bytes //r. ����������� ���������, �������� ������ ��������� ������ � ������ �������� ����
		{
			size = sizeof(rcv_buf);
		}
		memcpy(rcv_copy, rcv_buf, size);	//e. copy of received amount of bytes, but no more than buffer size //r. ���������� �������� ����� ����, �� �� ����� ������� ������
	}
    rx_buf_copy = 0;						//e. reset the flag of necessity of copying of receiving buffer  //r. �������� ���� ������������� ����������� ��������� ������
   	
	do
	    rcv_buf[--rcv_num_byt] = 0;
	while(rcv_num_byt);
		 rcv_num_byt_old = rcv_num_byt;
    line_sts = 0;							//e.  reset the line status register //r. �������� ������� ������� �����
    rcv_Rdy = 0;							//e. allow further data reception //r. ��������� ���������� ����� ������
} // decode_CMD

/******************************************************************************/
