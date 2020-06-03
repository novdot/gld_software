/**
  ******************************************************************************
  * @file    dither.h
  *
  * @brief   Вибропривод
  *
  * @author  Дмитрий Новиков novikov@elektrooptika.ru
  *
  ******************************************************************************
  * @attention
  *
  * в разработке
  *
  * <h2><center>&copy; 2020 Электрооптика</center></h2>
  ******************************************************************************
  */
#ifndef __DITHER_H_INCLUDED 
#define __DITHER_H_INCLUDED

/**
    @brief Initialization of dither regulator.
    @return
*/
void init_Dither_reg();

/**
    @brief VibroDither initialization.
    @return
*/
void VibroDither_Init();

/**
    @brief noise regulator
    @return
*/
void clc_Noise_regulator(void);

/**
    @brief Routine for output frequency controller
    @return
*/
void clc_OutFreq_regulator(void);

/**
    @brief Routine for dither frequency controller
    @return
*/
void clc_Dith_regulator(void);

/**
    @brief Set period and pulse width for dither.
    @return
*/
void VibroDither_Set(void);

/**
    @brief Routine for addition of delay to meander
    @return
*/
int VB_MeanderDelay(int VB_Meander, int Delay100uS, int MaxDly);

/**
    @brief Routine for accumulation of dither error
    @return
*/
int VB_PhaseDetectorRate(int PhaseDetInput, int IntegrateTime);

void VibroDither_SwitchOn();
void VibroDither_SwitchOff();

#endif