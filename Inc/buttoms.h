#ifndef __BUTTOMS_H
#define __BUTTOMS_H

#include "stm32f1xx_hal.h"

/** 
  * @brief  GPIO Bit SET and Bit RESET enumeration 
  */
typedef enum
{ 
  BM_RELEASED = 0,
  BM_PRESSED
}BM_State;

typedef struct
{
	uint8_t State;//
	uint32_t time_presed;//
}Buttom_struct;

#endif
