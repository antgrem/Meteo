#ifndef MAIN_H
#define MAIN_H


#include "stm32f1xx_hal.h"
#include "usb_device.h"
#include "LM75_hal.h"
#include "Lcd_Driver.h"
#include "delay.h"
#include "GUI.h"
#include "I2Cdev.h"
#include "BMP085.h"
#include "ffconf.h"
#include "diskio.h"
#include "ff.h"
#include "sd_spi_stm32.h"
#include "fattime.h"
#include <time.h>
#include "My_font.h"
#include "BMP_graph.h"
#include "Init.h"


#define DAY_DATA_ARRAY_LENGTH 144

//data structure definition  
typedef struct
{
  int16_t T_in;    

  int16_t T_out;	
	
	uint8_t Present_T_out;

  float Temperature_p;

	float Pressure_p;
  
  RTC_TimeTypeDef Time;   

  
}Messure_DataTypeDef; 


void Take_new_Messure(Messure_DataTypeDef *data);
void delay_ms(uint16_t nms);

#endif
