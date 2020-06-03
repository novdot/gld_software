
#ifndef __HARDWARE_DAC_LTC1622_H_INCLUDED__
#define __HARDWARE_DAC_LTC1622_H_INCLUDED__

#include "core/types.h"

//commands
typedef enum{
    _ltc16xx_cmd_wr_input_reg = 0x0 //< Write to Input Register n
    , _ltc16xx_cmd_upd_dac_reg = 0x1 //< Update (Power Up) DAC Register n
    , _ltc16xx_cmd_wr_input_reg_upd = 0x2 //< Write to Input Register n, Update (Power Up) All n
    , _ltc16xx_cmd_wr_upd = 0x3 //< Write to and Update (Power Up) n
    , _ltc16xx_cmd_wr_power_down = 0x4 //< Power Down n
    , _ltc16xx_cmd_no_operation = 0xf //< No Operation
}ltc16xx_cmd;

//address
typedef enum{
    _ltc16xx_addr_dac_a = 0x0 //< DAC A
    , _ltc16xx_addr_dac_b = 0x1 //< DAC B
    , _ltc16xx_addr_all = 0xf //< All DACs
}ltc16xx_addr;

#define LTC1622_VOLT_REF (4) //< voltage at REF
#define LTC1622_RESOLUTION (4096) //< resolution

//The digital-to-analog transfer function is
//Vout(ideal) = Vref * k / pow(2,N);
// k is the decimal equivalent of the binary DAC input code
// N - resolution
#define LTC1622_TRANSFER_VAL2VOLT(k) LTC1622_VOLT_REF*k/LTC1622_RESOLUTION
#define LTC1622_TRANSFER_VOLT2VAL(v) LTC1622_RESOLUTION*v/LTC1622_VOLT_REF

/**
    @brief While the minimum input word is 24 bits, it may optionally
be extended to 32 bits to accommodate microprocessors
which have a minimum word width of 16 bits (2 bytes). To
use the 32-bit word width, 8 don’t-care bits are transferred
to the device first, followed by the 24-bit word as just
described. 
*/
x_uint32_t ltc16xx_create_input_word(ltc16xx_cmd cmd,ltc16xx_addr adr,x_uint16_t data);
x_uint16_t ltc16xx_create_input_word_hdr(ltc16xx_cmd cmd,ltc16xx_addr adr);
x_uint16_t ltc16xx_create_input_word_data(x_uint16_t a_data);

#endif