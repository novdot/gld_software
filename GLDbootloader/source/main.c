/***
* Основной модуль монитора прибора ГЛ-Д
 Обмен с PC по каналу RS485 (полный дуплекс, 38400 бод);
 Период главного цикла - 1 мс.
 
 Монитор, под кристал Alter'ы 1k30 uhost'a, с функцией адресации прибора в режиме
 монитора, что значит, что на стадии инициализации происходит загрузка парметров 
 из 4 сектора flash, и назначение адреса на основе первого считанного параметра.
 Кроме того:      
 Введена защита от некорректных данных 4-го сектора (а именно некорректного адреса):
 принудительное назначение переменной My_Addres=0x001f, в случае, если в 4-ом 
 секторе прошит адрес, превышающий значение 0x001f. Это сделанно для совместимости 
 с программирующим софтом uhost_Prog, т.к. для ее работы необходимо совпадение адреса
 запроса и адреса ответа.
* */
#include "bootloader/command_bootloader.h"
#include "hardware/hardware.h"
#include "core/config.h"
#include "core/global.h"
#include "core/const.h"
/******************************************************************************/
//инициализируем периферию для прошивки
void init()
{
    //e. clocking control initialization
    SystemInit();
    
    DMA_Init();
    
    //read params
    flash_dma_read(Device_blk.Array,sizeof(Device_blk.Array),CONST_PARAMS_FLASH_START);  
    
    //e. to calculate SystemCoreClock  for UART particularly
    SystemCoreClockUpdate();
    //e. initialization of UART on 38400
    UART_Init(CONFIG_COMMANDS_BAUDRATE);
    
}

/******************************************************************************/
//основной цикл. ждем подключения, если нет - переключаемся на основную программу
void loop()
{
    command_recieve();
    //decode
    command_transm();
}

/******************************************************************************/
int main(void)
{
    init();
    
    do {
        loop();
    } while(1);	    // main infinie loop            
}