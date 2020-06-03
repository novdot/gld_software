#include  "hardware/ltc1622.h"
#include  "hardware/spi.h"

/******************************************************************************/
x_uint32_t ltc16xx_create_input_word(ltc16xx_cmd cmd,ltc16xx_addr adr,x_uint16_t a_data)
{
    return ((x_uint32_t)ltc16xx_create_input_word_hdr(cmd,adr)<<16) 
        + ltc16xx_create_input_word_data(a_data);
}

/******************************************************************************/
x_uint16_t ltc16xx_create_input_word_hdr(ltc16xx_cmd cmd,ltc16xx_addr adr)
{
    //COMMAND(4bits) ADDRESS(4bits)
    return (cmd<<4) + adr;
}

/******************************************************************************/
x_uint16_t ltc16xx_create_input_word_data(x_uint16_t a_data)
{
    //DATA (12 BITS + 4 DON’T-CARE BITS)
    return ((a_data&0xFFF)<<4);
}