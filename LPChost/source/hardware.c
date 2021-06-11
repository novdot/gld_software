#include "hardware/hardware.h"
#include "hardware/ltc1622.h"
#include "hardware/config.h"

#include "lpc17xx.h"

/******************************************************************************/
void hardware_configure_lightup()
{
    LPC_PINCON->PINSEL0 &= ~(3<<8);		//e. P0.4 is GPIO pin
    LPC_PINCON->PINMODE0 |= (3<<8);		//e. P0.4 is GPIO pin
    LPC_GPIO0->FIODIR |= (1<<4);		//e. P0.4 is output
    hardware_lightup_off();
}
/******************************************************************************/
void hardware_lightup_on()
{
    LPC_GPIO0->FIOSET = (1<<4);
}
/******************************************************************************/
void hardware_lightup_off()
{
    LPC_GPIO0->FIOCLR = (1<<4);
}
/******************************************************************************/
void hardware_configure_backlight()
{
    LPC_PINCON->PINSEL2 &= ~(0x00<<24); 
    LPC_PINCON->PINSEL2 |= (0x00<<24); //e. P2.12 is GPIO pin
    LPC_PINCON->PINMODE3 |= (3<<24); //e. P1.28 is GPIO pin (запись ( 11 ) в бит PINMODE0  "для включения подтягивающего резистора")
    LPC_GPIO2->FIODIR |= (1<<12); //e. P1.28 is output   (запись ( 1 ) в  5  бит FIODIR    выбор P0.5 как выход)
    hardware_backlight_off();
}
/******************************************************************************/
void hardware_backlight_on()
{
    LPC_GPIO2->FIOCLR  = (1<<12);
}
/******************************************************************************/
void hardware_backlight_off()
{
    LPC_GPIO2->FIOSET  = (1<<12);
}
/******************************************************************************/
void hardware_modulator(x_uint32_t a_data)
{
    //float v_ampl = 0;
    //float v_ampl_mult = 0;
    //x_uint32_t delta_ref = 0;
    //float v_delta_range = 0;
    
    //множитель амплитуды
    //1.0 = 100% (max)
    //float v_amp_mult = 0.1;
    //коэффициент смещения. подобран вручную из-за нелинейной вых хар-ки ЦАП
    //float v_amp_shift = -(v_amp_mult-3.11)/9.84 ;//0.24;
    
    //приводим к 10 разрядам ЦАП
    a_data = a_data*0x3FF/0xFFFF;
    
    a_data += 30000;
    /*
    // amplitude = макс диапазон*коэф
    v_ampl = LPC_DAC_SIN_AMP_MAX*v_amp_mult;
    v_ampl_mult = (float)v_ampl/(float)(LPC_DAC_VOLT_MAX-0);
    //сжали характеристику по амплитуде
    a_data = (int)a_data*(v_ampl_mult);
    
    //вычислим на сколько нужно поднять(опустить) 
    //характеристику чтобы сответствовать опорному напряжению
    v_delta_range = (LPC_DAC_VOLT_MAX - v_ampl)/2 + v_amp_shift;
    delta_ref = MOD_CONVERT_VOLT2VAL(v_delta_range);
    a_data += delta_ref;
    */
    //запишем значение в ЦАП
    hardware_dac_send(a_data);
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
    spi_set_cs(PIN_DAC_CS);
}
/******************************************************************************/
void hardware_reset_dac()
{
    spi_reset_cs(PIN_DAC_CS);
}
/******************************************************************************/
void hardware_set_adc()
{
    spi_set_cs(PIN_ADC_CS);
}
/******************************************************************************/
void hardware_reset_adc()
{
    spi_reset_cs(PIN_ADC_CS);
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
void hardware_flash_read(x_uint32_t a_sector,x_uint8_t* a_pmemory)
{
    int start,end = 0;
    
    switch(a_sector){
        case 0:
            break;
        case 1:
            start = MEMORY_MAIN_MEM_START;
            end = MEMORY_MAIN_MEM_SIZE;
            break;
        case 2:
            break;
        case 3:
            start = MEMORY_COEF_MEM_START;
            end = MEMORY_COEF_MEM_SIZE;
            break;
        default:
            break;
    }
    memory_read(start,a_pmemory);
}
/******************************************************************************/
void hardware_flash_write(x_uint32_t a_sector,x_uint8_t* a_pmemory)
{
    int start,end = 0;
    x_uint32_t addr = 0;
    x_uint16_t size = 0;
    
    switch(a_sector){
        case 0:
            break;
        case 1:
            start = MEMORY_MAIN_SEC_START;
            end = MEMORY_MAIN_SEC_END;
            addr = MEMORY_MAIN_MEM_START;
            size = MEMORY_MAIN_MEM_SIZE;
            break;
        case 2:
            break;
        case 3:
            start = MEMORY_COEF_SEC_START;
            end = MEMORY_COEF_SEC_END;
            addr = MEMORY_COEF_MEM_START;
            size = MEMORY_COEF_MEM_SIZE;
            break;
        default:
            break;
    }
    memory_write(start,end,addr,a_pmemory,size);
}
/******************************************************************************/
void hardware_flash_erase(x_uint32_t a_sector)
{
    int start,end = 0;
    switch(a_sector){
        case 0:
            start = MEMORY_BOOT_SEC_START;
            end = MEMORY_BOOT_SEC_END;
            break;
        case 1:
            start = MEMORY_MAIN_SEC_START;
            end = MEMORY_MAIN_SEC_END;
            break;
        case 2:
            break;
        case 3:
            start = MEMORY_COEF_SEC_START;
            end = MEMORY_COEF_SEC_END;
            break;
        default:
            start = MEMORY_BOOT_SEC_START;
            end = MEMORY_COEF_SEC_END;
            break;
    }
    memory_erase(start,end);
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
    
    /* Power ON Timer0,1 */
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
