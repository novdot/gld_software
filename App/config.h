#ifndef CONFIG
#define CONFIG


#define		DEBUG

//========== device selection ==========================
#define 	GLD_DEVICE
//#define 	OID_DEVICE
//#define 	OID_RF_DEVICE
//======================================================
#define COMMAND_DEBUG

#if defined(GLD_DEVICE)
	#define		FIRMWARE_VER		0xD	// 9.13 - firmware version
	#define     NEW_THERMO_ADC	
	#define		CONFIG_HFO_REG
//	#define		DELTA_SF_DELAY
	#define		CONFIG_PLC_SINUS
//	#define		CONFIG_HW_PLC
	//#define		CONFIG_SIMUL_HW_PLC	// simulation of part of PLC regulator in FPGA
	#define		CONFIG_PLC_LIDEM 		// PLC reg. works with demodulated LI signal (demodulation in analog IC)
//	#define		CONFIG_PLC_LIM			// PLC reg. works with modulated LI signal (demodulation inside FPGA)
	#define		WP_TRANSITION_ENA 		//e. allow the linear transition process for PLC reset //r. разрешен линейный переходной процесс при обнулении СРП
	
	#define		CONFIG_HFO_MOV_AVER_FILTER
	
	#define		CONFIG_SIX_THERMO_SENSORS
	
	#define		DEBUG_EXT_LAT_CNTR
	
	//#warning "=== GLD_DEVICE configuration selected ==="
	
#elif defined(OID_DEVICE)
	#define		FIRMWARE_VER		0x1C	// 1.12 - firmware version
	
	#define		CONFIG_CURRENT_REG
//	#define		CONFIG_IA_STABILIZING
	#define		CONFIG_LI_STABILIZING
//	#define		CONFIG_IB_EQUAL_IA
	#define		CONFIG_IB_IA_BALANCE
	
	#define		CONFIG_HW_PLC
//	#define		CONFIG_PLC_LIDEM 		// PLC reg. works with demodulated LI signal (demodulation in analog IC)
	#define		CONFIG_PLC_LIM			// PLC reg. works with modulated LI signal (demodulation inside FPGA)
	//#define		CONFIG_SIMUL_HW_PLC	// simulation of part of PLC regulator in FPGA
	#define		WP_TRANSITION_ENA 		//e. allow the linear transition process for PLC reset //r. разрешен линейный переходной процесс при обнулении СРП
	
	//#warning "=== OID_DEVICE configuration selected ==="
	
#elif defined(OID_RF_DEVICE)
	#define		FIRMWARE_VER		0x1C	// 1.12 - firmware version
	
	#define		CONFIG_HFO_REG
	
	#define		CONFIG_HW_PLC
//	#define		CONFIG_PLC_LIDEM 		// PLC reg. works with demodulated LI signal (demodulation in analog IC)
	#define		CONFIG_PLC_LIM			// PLC reg. works with modulated LI signal (demodulation inside FPGA)
	
	//#warning "=== OID_RF_DEVICE configuration selected ==="
	
#else
	#error "Not defined Device type"
#endif

// -------- conditional compiles errors messages ---------------------------
#if !(defined(GLD_DEVICE) ^ defined(OID_DEVICE) ^ defined(OID_RF_DEVICE))
	#error "Should be defined only one device, not several"
#endif

#if defined(CONFIG_PLC_LIDEM) & defined(CONFIG_PLC_LIM)
	#error "Should be defined CONFIG_PLC_LIDEM or CONFIG_PLC_LIM, not both"
#endif

#if !( defined(CONFIG_PLC_LIDEM) | defined(CONFIG_PLC_LIM) )
	#error "Not defined PLC mode: modulated / demodulated"
#endif

#if defined(OID_DEVICE)
#if defined(CONFIG_IA_STABILIZING) & defined(CONFIG_LI_STABILIZING)
	#error "Should be defined CONFIG_IA_STABILIZING or CONFIG_LI_STABILIZING, not both"
#endif

#if !( defined(CONFIG_IA_STABILIZING) | defined(CONFIG_LI_STABILIZING) )
	#error "Not defined IA regulator mode: select CONFIG_IA_STABILIZING or CONFIG_LI_STABILIZING"
#endif

#if defined(CONFIG_IB_EQUAL_IA) & defined(CONFIG_IB_IA_BALANCE)
	#error "Should be defined CONFIG_IB_EQUAL_IA or CONFIG_IB_IA_BALANCE, not both"
#endif

#if !( defined(CONFIG_IB_EQUAL_IA) | defined(CONFIG_IB_IA_BALANCE) )
	#error "Not defined IB regulator mode: select CONFIG_IB_EQUAL_IA or CONFIG_IB_IA_BALANCE"
#endif
#endif // OID_DEVICE

#endif

