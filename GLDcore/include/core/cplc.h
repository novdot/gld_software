#ifndef __CPLC_H_INCLUDED 
#define __CPLC_H_INCLUDED

/**
    @brief init_PLC
    Initialization procedure for PLC regulator
    @return 
*/
void cplc_init(void);

/**
    @brief Outgoing of the delayed meander signal for the PLC regulator
    @return 
*/
int PLC_MeanderDelay(int flag);

/**
    @brief clc_PLC
    Procedure of initial processing for the CPLC regulator
    @return 
*/
void cplc_regulator(void);

/**
    @brief clc_WP_sin
    Procedure of scan signal generating
    @return Current code for scan signal DAC of PLC
*/
int cplc_calc_modulator(void);

/**
    @brief Integartion of output of the PD of the CPLC regulator 
    for the technological output on the Rate command
    @param PhaseDetInput Current PD magnitude
    @param IntegrateTime period of integration
    @return Integrated magnitude of PD
*/
int WP_PhaseDetectorRate(int PhaseDetInput, int IntegrateTime);

#endif