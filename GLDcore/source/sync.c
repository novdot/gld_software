#include "core/sync.h"

//TODO move to hardware
#include "lpc17xx.h"
#include "system_LPC17xx.h"

void latch_start_meas(void)
{
    LPC_TIM0->TCR = 1; //e. start timer	
}
void latch_reload_meas(void)
{
    LPC_TIM0->TCR = 3; //e. start and reset the multidrop delay timer	
}