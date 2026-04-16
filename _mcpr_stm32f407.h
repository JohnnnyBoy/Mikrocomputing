/**
  ******************************************************************************
  * @file    _mcpr_stm32f407.h
  * @author  L. Gaul
  * @version V2.1.0
  * @date    24-May-2020
  * @brief   Publish functions of _mcpr_stm32f407_all.c
	*           
  ******************************************************************************
  */
	

 	
	extern void mcpr_SetSystemCoreClock(void);
	extern void mcpr_ConfigSystemTick(void);
	extern void mcpr_SetSysClockTick(void);
	
  extern void Emulate_LEDsLCD (unsigned int rows, unsigned int columns);

  extern void Emulate_RKTYC (signed short);
	

/*****************************END OF FILE***************************************/
