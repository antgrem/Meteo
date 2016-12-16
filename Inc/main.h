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
#include "stm32f1xx_hal_rtc.h"

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

//data structure definition  
typedef struct
{
    
  FRESULT SD_result; 

	uint8_t	Stage;
  
}SD_result_TypeDef; 

typedef struct
{
	uint8_t SD_Card_Present;
	uint8_t Wifi_Present;
	uint8_t T_in_Present;
	uint8_t T_out_Present;
	uint8_t Presure_Present;
	uint8_t RTC_Present;
	uint8_t Light_Status;
} System_TypeDef;

typedef struct
{
	uint8_t Dot_x, Dot_y;
	uint8_t Tempr_in_x, Tempr_in_y;
	uint8_t Tempr_out_x, Tempr_out_y;
	uint8_t Presure_x, Presure_y;
	uint8_t Time_x, Time_y;
	uint8_t SD_char_x, SD_char_y;
	uint8_t Wifi_char_x, Wifi_char_y;
	uint8_t Weather_x, Weather_y;
}Coord_TypeDef;

void Take_new_Messure(Messure_DataTypeDef *data);
void delay_ms(uint16_t nms);
SD_result_TypeDef Store_data_in_new_file(void);

#endif
