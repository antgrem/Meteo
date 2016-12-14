#include "main.h"
#include "hello_screen.h"


extern RTC_HandleTypeDef hrtc;
extern FATFS FATFS_Obj;
extern FIL file;
extern uint16_t Global_Font_Color, Global_BG_Color;
extern uint16_t stage_sd;
extern System_TypeDef System;

void Hello_Screen(void)
{ 
	// function for draw Hello screen 
	// with firmware version
	LCD_LED_SET;
	Lcd_Clear(BLACK);	
	
	PutStringRus11(30,15,"METEO",GREEN,Global_BG_Color);// "METEO"
	
	PutStringRus11(30,80,"ver 1.0",YELLOW,Global_BG_Color); // "ver 1.0"
	
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);

}




void Sensor_test(void)
{
volatile	SD_result_TypeDef res;
	int16_t test;
	char df[20];
	
	System.Presure_Present = 0;
	System.RTC_Present = 0;
	System.SD_Card_Present = 0;
	System.T_in_Present = 0;
	System.T_out_Present = 0;
	System.Wifi_Present = 0;
	
	LCD_LED_SET;
	Lcd_Clear(BLACK);
	delay_ms(800);
	
	if (LM75_Temperature_ex(&test) != 0)
		{// we have problem with sensor
			PutStringRus11(10,10,"IN T sensor: ERROR",RED,Global_BG_Color);
			//PutStringRus11(10,00,"1:0",RED,BLACK);			
		}
	else 
		{
			PutStringRus11(10,10,"IN T sensor: OK",GREEN,Global_BG_Color);
			//PutStringRus11(10,00,"1:0",GREEN,BLACK);
			System.T_in_Present = 1;
		}
		delay_ms(500);	
	
		if (BMP085_testConnection() == 1)
		{
			PutStringRus11(10,30,"P sensor: OK",GREEN,Global_BG_Color);
			//PutStringRus11(10,35,"2:1",GREEN,BLACK);
			System.Presure_Present = 1;
		}
		else 
		{
			PutStringRus11(10,30,"P sensor: ERROR",RED,Global_BG_Color);
			//PutStringRus11(10,35,"2:0",RED,BLACK);
		}
		delay_ms(500);
		
		if (HAL_RTCEx_BKUPRead(&hrtc, 1) == RTC_IS_SET)
		{
			PutStringRus11(10,60,"RTC: OK",GREEN,Global_BG_Color);
			System.RTC_Present = 1;
		}
		else 
		{
			PutStringRus11(10,60,"RTC: ERROR",RED,Global_BG_Color);
			System.RTC_Present = 0;
		}
		
		delay_ms(500);
		
		PutStringRus11(10,90,"OUT T ERROR",RED,Global_BG_Color);
		
		res.SD_result = f_mount(&FATFS_Obj, "", 1);

	if (res.SD_result == FR_OK) 
		{
				PutStringRus11(10,110,"SD: OK",GREEN,Global_BG_Color);
				System.SD_Card_Present = 1;
				f_mount(0, "0:", 1);
		}
	else 
		{
			sprintf(df, "%d", res.SD_result);
			PutStringRus11(10,110,"SD: ERROR",RED,Global_BG_Color);
			PutStringRus11(10,120,df,RED,BLACK);
		}
				
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);		
}

