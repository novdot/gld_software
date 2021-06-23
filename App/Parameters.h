#ifndef _PARAMS_UHOST_H_INCLUDED_
#define _PARAMS_UHOST_H_INCLUDED_
/*e.****************************************************
*        Constants for the uHost card                  *
********************************************************/

typedef enum{
      _params_load_default = 0x00001
    , _params_load_fash = 0x00003
}params_load_src;

void params_load(unsigned source_code);	
void params_load_default(void);
void params_load_flash(void);

void params_FlashDMA_Init(void);
void params_save2flash();

#endif //_PARAMS_UHOST_H_INCLUDED_

