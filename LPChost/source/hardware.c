#include "hardware/hardware.h"
#include  "hardware/ltc1622.h"

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
    float v_ampl = 0;
    float v_ampl_mult = 0;
    x_uint32_t delta_ref = 0;
    float v_delta_range = 0;
    
    //множитель амплитуды
    //1.0 = 100% (max)
    float v_amp_mult = 0.1;
    //коэффициент смещения. подобран вручную из-за нелинейной вых хар-ки ЦАП
    float v_amp_shift = -(v_amp_mult-3.11)/9.84 ;//0.24;
    
    //приводим к 10 разрядам ЦАП
    a_data = a_data*0x3FF/0xFFFF;
    
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
#define IN_VAL_MAX (0xd23)
#define IN_VAL_REF (0x722)

void hardware_regul_data_write(x_uint32_t flag, int*pExchangeErr, int a_HF_reg, int a_WP_reg)
{
    int data[6] = {0,0,0,0,0,0};
    float v_ampl = 0.0;
    float v_ampl_mult = 0.0;
    x_uint32_t a_data = 0;
    x_uint32_t delta_ref = 0;
    float v_delta_range = 0;
    static x_uint32_t testData = 0x122;
    
    x_uint32_t nDataRef = 2286;
    x_int32_t nDelta = 0;
    x_uint32_t nAmpAvail = 0;
 
    if (flag & 1) {
        data[4] = ltc16xx_create_input_word_hdr(
                _ltc16xx_cmd_wr_upd
                , _ltc16xx_addr_dac_a
            );
        a_data = a_HF_reg;
    } else {
        data[4] = ltc16xx_create_input_word_hdr(
                _ltc16xx_cmd_wr_upd
                , _ltc16xx_addr_dac_b
            );
        a_data = a_WP_reg;
    }
    testData = 1826 - 512*1;
    //0xD23 0x722 0x122
    //if(testData>=0xD23) testData = 0x122;
    //a_data = sqrt(a_data);
    
    nDelta = LTC1622_RESOLUTION/2 - nDataRef;
    if(nDelta>=0) nAmpAvail = 2*nDataRef;
    else nAmpAvail = 2*(LTC1622_RESOLUTION - nDataRef);
    
    //диапазон значений 0x122..0xD23 приведем к 0x0..0xFFF
    a_data = (a_data - 0x122)*0xFFF/(0xD23 - 0x122);
    
    //сожмем характеристику(из-за опорника для ОУ)
    a_data *= (float)nAmpAvail/(float)(LTC1622_RESOLUTION);
    //v_ampl = (LTC1622_VOLT_REF-SPI_DAC_AMPL_VOLT_REF)*2;
    //v_ampl_mult = v_ampl/LTC1622_VOLT_REF;
    //a_data = (int)a_data*(v_ampl_mult);
    
    //поднимем характеристику до Vref ОУ
    a_data -= nDelta*2;
    //v_delta_range = (LTC1622_VOLT_REF - v_ampl)/2;
    //delta_ref = LTC1622_TRANSFER_VOLT2VAL(v_delta_range);
    //a_data -= delta_ref;
    
    
    //в хосте4 в ОУ отрицание - необходимо перевернуть значение
    //a_data = 0xFFF - a_data;
    
    //a_data = 2286;
    
    data[5] = ltc16xx_create_input_word_data(a_data);
    spi_write(data,6,pExchangeErr);
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
    i2c_read(pCntDif);
}
/******************************************************************************/
void hardware_photo_out(x_uint32_t Ph_A, x_uint32_t Ph_B)
{
    i2c_write(Ph_A,Ph_B);
}
/******************************************************************************/

