#include "hardware/hardware.h"
#include "hardware/ltc1622.h"
#include "hardware/config.h"

#include "lpc17xx.h"

/******************************************************************************/
void hardware_configure_lightup()
{
#ifdef HOST4
    LPC_PINCON->PINSEL0 &= ~(3<<8);		//e. P0.4 is GPIO pin
    LPC_PINCON->PINMODE0 |= (3<<8);		//e. P0.4 is GPIO pin
    LPC_GPIO0->FIODIR |= (1<<4);		//e. P0.4 is output
#else
    LPC_PINCON->PINSEL4 &= ~(3<<4);		//e. P2.2 is GPIO pin
    LPC_PINCON->PINMODE4 |= (3<<4);		//e. P2.2 is GPIO pin
    LPC_GPIO2->FIODIR |= (1<<2);		//e. P2.2 is output	
#endif
    hardware_lightup_off();
}
/******************************************************************************/
void hardware_lightup_on()
{
#ifdef HOST4
    LPC_GPIO0->FIOSET = (1<<4);
#else
	  LPC_GPIO2->FIOSET = (1<<2);
#endif
}
/******************************************************************************/
void hardware_lightup_off()
{
#ifdef HOST4
    LPC_GPIO0->FIOCLR = (1<<4);
#else
	LPC_GPIO2->FIOCLR = (1<<2);
#endif
}
/******************************************************************************/
void hardware_configure_backlight()
{
#ifdef HOST4
    LPC_PINCON->PINSEL2 &= ~(0x00<<24); //??????????????????????????????
    LPC_PINCON->PINSEL2 |= (0x00<<24); //e. P2.12 is GPIO pin?????????????????????
    LPC_PINCON->PINMODE3 |= (3<<24); //e. P1.28 is GPIO pin (запись ( 11 ) в бит PINMODE0  "для включения подтягивающего резистора")
    LPC_GPIO2->FIODIR |= (1<<12); //e. P1.28 is output   (запись ( 1 ) в  5  бит FIODIR    выбор P0.5 как выход)
#else
   LPC_PINCON->PINSEL4 &= ~(0x00<<6); 
    LPC_PINCON->PINSEL4 |= (0x00<<6); //e. P2.3 is GPIO pin
    LPC_PINCON->PINMODE4 |= (3<<6); //e. P2.3 is GPIO pin (запись ( 11 ) в бит PINMODE0  "для включения подтягивающего резистора")
    LPC_GPIO2->FIODIR |= (1<<3); //e. P2.3 is output   (запись ( 1 ) в  5  бит FIODIR    выбор P2.3 как выход)	
#endif
	hardware_backlight_off();
}
/******************************************************************************/
void hardware_backlight_on()
{
#ifdef HOST4
    LPC_GPIO2->FIOCLR  = (1<<12);
#else
	LPC_GPIO2->FIOSET  = (1<<3);
#endif
}
/******************************************************************************/
void hardware_backlight_off()
{
#ifdef HOST4
    LPC_GPIO2->FIOSET  = (1<<12);
#else
	 LPC_GPIO2->FIOCLR  = (1<<3);
#endif
}
/******************************************************************************/
void hardware_modulator(x_int32_t a_data)
{
    //x_uint32_t udata = 0;
       
    //поднимем 0
    //a_data += INT16_MAX;
    //приведем к регистру
    //udata = (x_uint32_t)(a_data); 

    //запишем значение в ЦАП
    hardware_dac_send(a_data );
}

/******************************************************************************/
void hardware_regul_data_init()
{
    spi_init();
}
/******************************************************************************/
//опорник операционного усилителя
#define SPI_DAC_AMPL_VOLT_REF (2.23)
#define OUT_VOLT_MAX (15.0)

void hardware_regul_data_write(
    x_uint32_t a_ch
    , int*a_pExchangeErr
    , int a_reg
        )
{
    int data[6] = {0,0,0,0,0,0};
    /*float v_ampl = 0.0;
    float v_ampl_mult = 0.0;
    int a_data = 0;
    x_uint32_t a_data_max = 0;
    x_uint32_t a_data_min = 0;
    x_uint32_t delta_ref = 0;
    float v_delta_range = 0;
    //float mult, shift = 0.0;
    
    x_uint32_t nDataRef = 2330;//2286;
    x_int32_t nDelta = 0;
    x_uint32_t nAmpAvail = 0;*/
    
    //a_reg = (a_reg>>4)+2048;//преобразуем код из математики
 
    if (a_ch == 1) {
        data[4] = ltc16xx_create_input_word_hdr(
                _ltc16xx_cmd_wr_upd
                , _ltc16xx_addr_dac_a
            );
    } else {
        data[4] = ltc16xx_create_input_word_hdr(
                _ltc16xx_cmd_wr_upd
                , _ltc16xx_addr_dac_b
            );
    }
    
    //проверим допустимый диапазон
    //if(a_reg>a_reg_max) a_reg = a_reg_max;
    //else if(a_reg<a_reg_min) a_reg = a_reg_min;
    //a_data = a_reg;
    
    //рассчитаем 0 из-за опроника
    //получим разницу между средним значением регистра и фактическим нулем
    //nDelta = LTC1622_RESOLUTION/2 - nDataRef;
    //определим максимальную амплитуду
    //if(nDelta>=0) nAmpAvail = 2*nDataRef;
    //else nAmpAvail = 2*(LTC1622_RESOLUTION - nDataRef);
    
    //диапазон значений 0x122(IN_VAL_MIN)..0xD23(IN_VAL_MAX) приведем к 0x0..0xFFF
    //a_data = (a_data - a_reg_min)*0xFFF/(a_reg_max - a_reg_min);
    
    //сожмем характеристику(из-за опорника для ОУ)
    //a_data *= (float)nAmpAvail/(float)(LTC1622_RESOLUTION);
    //a_data_max = nDataRef + nAmpAvail/2;
    //a_data_min = nDataRef - nAmpAvail/2;
    
    //поднимем характеристику до Vref ОУ
    //a_data -= nDelta*2;
    
    ////////////////////////////
    //a_data = ( (a_data_max-a_data_min)*(a_reg_max-a_reg_min) )/(a_reg-a_reg_min) + a_data_min;
    //if(a_data>a_data_max) a_data = a_data_max;
    //else f(a_data<a_data_min) a_data = a_data_min;
    
    //a_data = a_reg - nDelta;
    //a_data *= (float)nAmpAvail/(float)(LTC1622_RESOLUTION);
    //a_data = -41400424/1363383 + 3076852/4090149*a_reg + 2764/4090149*a_reg*a_reg;
    //a_data = (44401 + 1603*a_reg - 218*a_reg*a_reg/100 - 185*a_reg*a_reg*a_reg/10000)/1000;
    
    data[5] = ltc16xx_create_input_word_data( a_reg );
    spi_write(data,6,a_pExchangeErr);
}
/******************************************************************************/
void hardware_regul_data_read(int*a_pBuffer, int cnt, int*pExchangeErr)
{
    spi_read(a_pBuffer,cnt,pExchangeErr);
}

/******************************************************************************/
void hardware_set_dac()
{
    spi_set_cs_DAC(PIN_DAC_CS);
}
/******************************************************************************/
void hardware_reset_dac()
{
    spi_reset_cs_DAC(PIN_DAC_CS);
}
/******************************************************************************/
void hardware_set_adc()
{
    spi_set_cs_ADC(PIN_ADC_CS);
}
/******************************************************************************/
void hardware_reset_adc()
{
    spi_reset_cs_ADC(PIN_ADC_CS);
}

/******************************************************************************/
void hardware_photo_init(void) 
{
    i2c_init();
}
/******************************************************************************/
void hardware_photo_exchange(int*pCntDif) 
{
    i2c_send();
}
/******************************************************************************/
void hardware_photo_set(x_uint32_t Ph_A, x_uint32_t Ph_B)
{
    i2c_setData(255 - Ph_A,255 - Ph_B);
}

/******************************************************************************/
x_bool_t compare_str(x_uint8_t* a_filename, char* a_reference)
{
    x_uint32_t icnt = 0;
    x_uint32_t a_cnt = strlen((char*)a_reference);
    
    for(icnt=0;icnt<a_cnt;icnt++){
        if(a_filename[icnt]!=a_reference[icnt]) return _x_false;
    }
    return _x_true;
}
void hardware_flash_convert_name2sector(
    x_uint8_t* a_filename
    , x_uint32_t* a_sector
)
{
    *a_sector = MEMORY_VOID_SEC_NUM;
    
    if(compare_str(a_filename,"gld")==_x_true){
        *a_sector = MEMORY_MAIN_SEC_NUM;
        
    }else if(compare_str(a_filename,"params")==_x_true){
        *a_sector = MEMORY_COEF_SEC_NUM;
        
    }
end:;
    return;
}
/******************************************************************************/
x_bool_t hardware_flash_read(x_uint32_t a_sector,x_uint32_t* a_pmemory, x_uint32_t a_cnt)
{
    int start,end = 0;
    
    switch(a_sector){
        case MEMORY_BOOT_SEC_NUM:
            start = MEMORY_BOOT_MEM_START;
            //end = MEMORY_BOOT_MEM_SIZE;
            break;
        
        case MEMORY_MAIN_SEC_NUM:
            start = MEMORY_MAIN_MEM_START;
            //end = MEMORY_MAIN_MEM_SIZE;
            break;
        
        case MEMORY_FPGA_SEC_NUM:
            break;
        
        case MEMORY_COEF_SEC_NUM:
            start = MEMORY_COEF_MEM_START;
            //end = a_cnt;
            break;
        
        default:
            break;
    }
    memory_read(start,a_pmemory,a_cnt);
    return _x_true;
}
/******************************************************************************/
x_bool_t hardware_flash_write_f(
    x_uint8_t* a_file
    , x_uint32_t a_size
    , x_uint8_t *buf
    , x_uint32_t shift)
{
    x_uint32_t a_sector=0;
    hardware_flash_convert_name2sector(a_file,&a_sector);
    
    if(a_sector==MEMORY_VOID_SEC_NUM) {
        return _x_false;
    }
    return hardware_flash_write(a_sector,buf,a_size,shift);
}

x_bool_t hardware_flash_write(
    x_uint32_t a_sector
    , x_uint8_t* a_pmemory
    , x_uint32_t a_size
    , x_uint32_t a_shift_memory
)
{
    int sec_start,sec_end = 0;
    //x_uint32_t addr = 0;
    //x_uint16_t size = 0;
    
    switch(a_sector){
        case MEMORY_MAIN_SEC_NUM:
            sec_start = MEMORY_MAIN_SEC_START;
            sec_end = MEMORY_MAIN_SEC_END;
            //addr = MEMORY_MAIN_MEM_START + a_shift_sector*MEMORY_PAGE_SIZE;
            //size = (x_uint16_t)MEMORY_MAIN_MEM_SIZE;
            break;
        
        case MEMORY_COEF_SEC_NUM:
            sec_start = MEMORY_COEF_SEC_START;
            sec_end = MEMORY_COEF_SEC_END;
            //addr = MEMORY_COEF_MEM_START + a_shift_sector*MEMORY_PAGE_SIZE;
            //size = (x_uint16_t)MEMORY_COEF_MEM_SIZE;
            break;
        
        case MEMORY_BOOT_SEC_NUM:
        case MEMORY_FPGA_SEC_NUM:
        default:
            return _x_false;
    }
    //check overflow sectors mapping
    //if( (sec_start+a_shift_sector) >= sec_end ) 
    //    return _x_false;
    
    return memory_write(
            sec_start 
            , sec_end
            , a_shift_memory
            , a_pmemory
            , a_size/4 + (a_size%4==0?0:1) 
        );
}
/******************************************************************************/
x_bool_t hardware_flash_erase_f(x_uint8_t* a_file, x_uint16_t a_size)
{
    x_uint32_t a_sector=0;
    hardware_flash_convert_name2sector(a_file,&a_sector);
    
    if(a_sector==MEMORY_VOID_SEC_NUM) return _x_false;
    return hardware_flash_erase(a_sector,a_size);
}
x_bool_t hardware_flash_erase(x_uint32_t a_sector, x_uint16_t a_size)
{
    int start,end = 0;
    //x_uint16_t size = 0;
    
    if(a_sector==MEMORY_VOID_SEC_NUM) return _x_false;
    
    switch(a_sector){
        case MEMORY_BOOT_SEC_NUM:
            start = MEMORY_BOOT_SEC_START;
            end = MEMORY_BOOT_SEC_END;
            break;
        
        case MEMORY_MAIN_SEC_NUM:
            start = MEMORY_MAIN_SEC_START;
            end = MEMORY_MAIN_SEC_END;
            //size = (x_uint16_t)MEMORY_MAIN_MEM_SIZE;
            break;
        
        case MEMORY_FPGA_SEC_NUM:
            break;
        
        case MEMORY_COEF_SEC_NUM:
            start = MEMORY_COEF_SEC_START;
            end = MEMORY_COEF_SEC_END;
            //size = (x_uint16_t)MEMORY_COEF_MEM_SIZE;
            break;
        
        default:
            //full erase
            start = MEMORY_BOOT_SEC_START;
            end = MEMORY_COEF_SEC_END;
            break;
    }
    memory_erase(start,end);
    return _x_true;
}
/******************************************************************************/
void hardware_flash_load_main()
{
    memory_load(MEMORY_MAIN_MEM_START);
}
/******************************************************************************/
#define SBIT_TIMER0  1
#define SBIT_TIMER1  2

#define SBIT_MR0I    0
#define SBIT_MR0R    1

#define SBIT_CNTEN   0

#define PCLK_TIMER0  2
#define PCLK_TIMER1  4   

/* ms is multiplied by 1000 to get us*/
#define MiliToMicroSec(x)  (x*1000)  

x_uint32_t* g_mcs_cnt;
extern unsigned int SystemCoreClock;
unsigned int getPrescalarForUs(uint8_t timerPclkBit)
{
    unsigned int pclk,prescalarForUs;
    /* get the pclk info for required timer */
    pclk = (LPC_SC->PCLKSEL0 >> timerPclkBit) & 0x03;  

    /* Decode the bits to determine the pclk*/
    switch ( pclk ){
    case 0x00:
        pclk = SystemCoreClock/4;
        break;

    case 0x01:
        pclk = SystemCoreClock;
        break; 

    case 0x02:
        pclk = SystemCoreClock/2;
        break; 

    case 0x03:
        pclk = SystemCoreClock/8;
        break;

    default:
        pclk = SystemCoreClock/4;
        break;  
    }

    /* Prescalar for 1us (1000000Counts/sec) */
    prescalarForUs = pclk/1000000 - 1;                    

    return prescalarForUs;
}

void hardware_tim_init(x_uint32_t* mcs_cnt)
{
    g_mcs_cnt = mcs_cnt;
    
    /* Power ON Timer1 */
    LPC_SC->PCONP |= (1<<SBIT_TIMER1);
    
    /* Clear TC on MR0 match and Generate Interrupt*/
    LPC_TIM1->MCR  = (1<<SBIT_MR0I) | (1<<SBIT_MR0R);
    /* Prescalar for 1us */
    LPC_TIM1->PR   = getPrescalarForUs(PCLK_TIMER1);
    /* Load timer value to generate 1000ms delay*/
    LPC_TIM1->MR0  = MiliToMicroSec(1000);
}
void hardware_tim_start()
{
    /* Start timer by setting the Counter Enable*/
    LPC_TIM1->TCR  = (1 << SBIT_CNTEN);
    /* Enable Timer1 Interrupt */
    NVIC_EnableIRQ(TIMER1_IRQn);
}
void hardware_tim_stop()
{
    NVIC_DisableIRQ(TIMER1_IRQn);
}

__irq void TIMER1_IRQHandler(void)
{
    unsigned int isrMask;

    isrMask = LPC_TIM1->IR; 
    /* Clear the Interrupt Bit */
    LPC_TIM1->IR = isrMask;
    
    (*g_mcs_cnt)+=1000;
}
/******************************************************************************/
