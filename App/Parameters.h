#ifndef UHOST
#define UHOST
/*e.****************************************************
*        Constants for the uHost card                  *
********************************************************/

/*
#define FIRMWARE_VER	 0x00			 //e. firmware version number 

//e. registers of the system controller of the DSP card 
#define  Sys_RgE         0x07fb          //e. the line1_err register of line errors  
#define  Sys_RgR         0x07fc          //e. the register of a mode of the system (only write) 
#define  Sys_RgS         0x07fc          //e. the register of a status of the system (only read) 
#define  Trm_data        0x07fd          //e. the data stack of the transmitter 
#define  Rcv_data        0x07fd          //e. the data stack of the receiver 
#define  Rcv_cntr        0x07fe          //e. the counter of the received bytes 
*
//e.  bits of the Sys_RgS status register 
#define  RCV_RDY_MASK    0x0040          //e. a mask of the readiness bit of the receiver
#define  RCV_FERR_MASK   0x0020          //e. a mask of the "format error" bit
#define  RCV_TOUT_MASK   0x0010          //e. a mask of the "time-out" bit
#define  RCV_RFUL_MASK   0x0008          //e. a mask of the "stack is full" bit
#define  RCV_FERR        0x0020          //e. a mask of the "format error" bit
#define  RCV_TOUT        0x0010          //e. a mask of the "time-out" bit 
#define  RCV_RFUL        0x0008          //e. a mask of the "stack is full" bit

#define INT_ERR_TC		0x00000008 
*/
#define FromFLASH		0x00003
#define ByDefault		0x00001


void LoadFlashParam(unsigned );	
void FlashDMA_Init(void);
void init_DefaultParam(void);
#endif

