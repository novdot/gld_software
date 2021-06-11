#include "hardware/i2c.h"

#include "lpc17xx.h"

#define TIMEOUT (1000)

#define I2C_WRITELENGTH		0x00000006	/*Buffer length*/
volatile x_uint32_t I2CMasterState = I2C_IDLE;
volatile x_uint32_t I2CMasterBuffer[I2C_WRITELENGTH];

/******************************************************************************/
void i2c_init(void)
{
    LPC_SC->PCONP |= (1 << 19);

    /* set PIO0.27 and PIO0.28 to I2C0 SDA and SCK */
    /* function to 01 on both SDA and SCK. */
    LPC_PINCON->PINSEL1 &= ~0x03C00000;
    LPC_PINCON->PINSEL1 |= 0x01400000;	

    /*--- Clear flags ---*/
    LPC_I2C0->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;    

    /*--- Reset registers ---*/
    LPC_I2C0->SCLL   = I2SCLL_SCLL;
    LPC_I2C0->SCLH   = I2SCLH_SCLH;

    I2CMasterBuffer[0] = A_ADDRESS;
    I2CMasterBuffer[1] = WRITE_CMD;
    I2CMasterBuffer[3] = B_ADDRESS;
    I2CMasterBuffer[4] = WRITE_CMD;

    LPC_I2C0->CONSET = I2CONSET_I2EN;
}
/******************************************************************************/
void i2c_send()
{
    uint32_t StatValue;
    static uint32_t WrIndex;
    static  int32_t time_out = TIMEOUT;

    //e.transmitting is not active, go away
    if (I2CMasterState) return;							

    //e. valid time period elapsed, go away
    if (--time_out < 0) {
	   	time_out = TIMEOUT;
	  	I2CMasterState = I2C_IDLE;	 	//e. timer elapsed, go away
		LPC_I2C0->CONSET = I2CONSET_STO;      //e. Set Stop flag 	
        LPC_I2C0->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC;  
		return;	
	}
	else if (!(LPC_I2C0->CONSET & I2CONSET_SI))		//e. state of I2C bus has not been changed
   		return;

	StatValue = LPC_I2C0->STAT;

    switch ( StatValue ) {
        case 0x08:			// A Start condition is issued (write data for the first potentiometer) 
            WrIndex = 0;
            time_out = TIMEOUT;  //e. enable countdown
            LPC_I2C0->DAT = I2CMasterBuffer[WrIndex++];	   //e. send address
            LPC_I2C0->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC); //e .clear interrupt bit and start bit
            break;
	
        case 0x10:			// A repeated started is issued (write data for the second potentiometer) 
            LPC_I2C0->DAT = I2CMasterBuffer[WrIndex++];	   //e. send address
            LPC_I2C0->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC); //e .clear interrupt bit and start bit
            break;

        case 0x18:			//e. Regardless, it's a ACK after slave address reading
            LPC_I2C0->DAT = I2CMasterBuffer[WrIndex++];  //e. send another byte
            LPC_I2C0->CONCLR = I2CONCLR_SIC;	//e. clear interrupt bit
            break;

        case 0x28:	//e. Regardless it's a ACK after data byte 
            //e. we have transmitted the data for the B potentiometer
            if  (WrIndex == I2C_WRITELENGTH) {  
                I2CMasterState = I2C_IDLE;
                LPC_I2C0->CONSET = I2CONSET_STO;      //e. Set Stop flag
                //*cnt_dif = 300;
            } else if (WrIndex == 3){
                LPC_I2C0->CONSET = I2CONSET_STA; 
                //*cnt_dif = 200;
            }else{
                LPC_I2C0->DAT = I2CMasterBuffer[WrIndex++];  //e. send another byte	
                //*cnt_dif = 100;
            }
            LPC_I2C0->CONCLR = I2CONCLR_SIC;	//e. clear interrupt bit
            break;

        case 0x20:							  //e. no aknowledgement after address transmitting
        case 0x30:							  //e. no aknowledgement after data block transmitting
            LPC_I2C0->CONSET = I2CONSET_STO;      //e. Set Stop flag 
            LPC_I2C0->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC); 
            I2CMasterState = I2C_IDLE;	  //e. fix new state
            break;
    }
    return;
}
/******************************************************************************/
void i2c_setData(x_uint32_t Ph_A, x_uint32_t Ph_B)
{
    I2CMasterBuffer[2] = Ph_A;
    I2CMasterBuffer[5] = Ph_B;	
    
    // Set Start flag
#if defined I2C0
    LPC_I2C0->CONSET = I2CONSET_STA;	 
#else
    LPC_I2C2->CONSET = I2CONSET_STA;	
#endif
    
    I2CMasterState = I2C_BUSY;  
}