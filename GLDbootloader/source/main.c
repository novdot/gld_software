/***
* Основной модуль монитора прибора ГЛ-Д
 Обмен с PC по каналу RS485 (полный дуплекс, 38400 бод);
 Период главного цикла - 1 мс.
 
 Описание под HOST3:
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
#include "bootloader/global.h"
#include "hardware/hardware.h"
#include "core/config.h"
#include "core/global.h"
#include "xlib/ring_buffer.h"

bootloader_global g_bootloader;
/******************************************************************************/
//инициализируем периферию для прошивки
void init()
{
    int i=0;
    char dbg[64];
    
    //program variables
    x_ring_init(&g_gld.cmd.dbg.ring_in, g_gld.cmd.dbg.buf_in, GLD_RINGBUFFER_SIZE);
    x_ring_init(&g_gld.cmd.dbg.ring_out, g_gld.cmd.dbg.buf_out, GLD_RINGBUFFER_SIZE);
    
    x_ring_init(&g_gld.cmd.ask.ring_in, g_gld.cmd.ask.buf_in, GLD_RINGBUFFER_SIZE);
    x_ring_init(&g_gld.cmd.ask.ring_out, g_gld.cmd.ask.buf_out, GLD_RINGBUFFER_SIZE);
    g_gld.cmd.recieve_cmd_size = 0;
    
    //e. clocking control initialization
    SystemInit();
    
    DMA_Init();
    
    //for tests
    hardware_configure_backlight();
    
    //e. to calculate SystemCoreClock  for UART particularly
    SystemCoreClockUpdate();
    
    //e. initialization of UART on 38400
    UART_Init(CONFIG_COMMANDS_BAUDRATE);
    
    //e. initialize DMA channel for UART
    //uart_dma_init(trm_buf);
    
    //initialize software values
    global_bootloader_init();
    
    //init timer
    hardware_tim_init(&g_bootloader.nTimerCnt);
    
    DBG2(&g_gld.cmd.dbg.ring_out,dbg,64,"Build in:%s %s\n\r",__DATE__, __TIME__);
         
    for(i=0;i<64;i++){x_ring_put(dbg[i],&g_gld.cmd.ask.ring_out);}
}

/******************************************************************************/
//основной цикл. ждем подключения, если нет - переключаемся на основную программу
void loop()
{
    //uart_recieve_unblocked(0,&g_gld.cmd.dbg.ring_in);
    //UART_DBG_SEND(&g_gld.cmd.dbg.ring_out);
    
    uart_recieve_unblocked(1,&g_gld.cmd.ask.ring_in);
    if(x_ring_get_count(&g_gld.cmd.ask.ring_out)>0) 
        uart_send_blocked(1,&g_gld.cmd.ask.ring_out);
    
    //обработка команд
    command_recieve(_command_recieve_flag_bootloader);
    command_decode();
    command_transm();
    
    //если прибор не введен в режим монитора, то проверим, что прошло время 
    //ожидания и запустим основную программу
    if(g_bootloader.bMonitorMode == 0) {
        if(g_bootloader.nTimerCnt>5000) {
            hardware_tim_stop();
            //hardware_flash_load_main();
        }
    }
}

/******************************************************************************/
int main(void)
{
    init();
    
    do {
        loop();
    } while(1); //main infinie loop            
}