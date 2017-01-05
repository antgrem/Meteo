#include "main.h"
#include "hello_screen.h"


extern RTC_HandleTypeDef hrtc;
extern FATFS FATFS_Obj;
extern FIL file;
extern uint16_t Global_Font_Color, Global_BG_Color;
extern uint16_t stage_sd;
extern System_TypeDef System;
extern Coord_TypeDef Coordinate;

void Hello_Screen(void)
{ 
	time_t time_temp;
	struct tm* time_tm_temp;
	char str_temp[15];
	
	
	// function for draw Hello screen 
	// with firmware version
	LCD_LED_SET;
	Lcd_Clear(BLACK);	
	
	PutStringRus11(30,15,"METEO",GREEN,Global_BG_Color);// "METEO"
	
	PutStringRus11(30,80,"ver 1.0",YELLOW,Global_BG_Color); // "ver 1.0"

	time_temp = ReadTimeCounter(&hrtc);
	time_tm_temp = localtime(&time_temp);
	
	sprintf(str_temp, "%04d%02d%02d.txt", time_tm_temp->tm_year, time_tm_temp->tm_mon, time_tm_temp->tm_mday);
	
	PutStringRus11(10,100,str_temp,YELLOW,Global_BG_Color); 
	
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
	uint8_t dy=10;
	

	
	LCD_LED_SET;
	Lcd_Clear(BLACK);
	delay_ms(800);
	
	if (LM75_Temperature_ex(&test) != 0)
		{// we have problem with sensor
			PutStringRus11(10,dy,"IN T sensor: ERROR",RED,Global_BG_Color);
		}
	else 
		{
			PutStringRus11(10,dy,"IN T sensor: OK",GREEN,Global_BG_Color);
			System.T_in_Present = 1;
		}
		delay_ms(250);	
		dy += 22;
	
		if (BMP085_testConnection() == 1)
		{
			PutStringRus11(10,dy,"P sensor: OK",GREEN,Global_BG_Color);
			System.Presure_Present = 1;
		}
		else 
		{
			PutStringRus11(10,dy,"P sensor: ERROR",RED,Global_BG_Color);
		}
		delay_ms(250);
		dy += 22;
		
		if (HAL_RTCEx_BKUPRead(&hrtc, 1) == RTC_IS_SET)
		{
			PutStringRus11(10,dy,"RTC: OK",GREEN,Global_BG_Color);
			System.RTC_Present = 1;
		}
		else 
		{
			PutStringRus11(10,dy,"RTC: ERROR",RED,Global_BG_Color);
			System.RTC_Present = 0;
		}
		
		delay_ms(250);
		dy += 22;
		
		PutStringRus11(10,dy,"OUT T ERROR",RED,Global_BG_Color);
		
		dy += 22;
		
		res.SD_result = f_mount(&FATFS_Obj, "", 1);

	if (res.SD_result == FR_OK) 
		{
				PutStringRus11(10,dy,"SD: OK",GREEN,Global_BG_Color);
				System.SD_Card_Present = 1;
				f_mount(0, "0:", 1);
		}
	else 
		{
			sprintf(df, "%d", res.SD_result);
			PutStringRus11(10,dy,"SD: ERROR",RED,Global_BG_Color);
			dy += 11;
			PutStringRus11(10,dy,df,RED,BLACK);
		}
				
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);		
}

void System_Status_Checked(void)
{
	volatile	SD_result_TypeDef res;
	int16_t test;
	
		if (LM75_Temperature_ex(&test) != 0)
		{// we have problem with sensor
			System.T_in_Present = 0;
		}
	else 
		{
			System.T_in_Present = 1;
		}
	
		if (BMP085_testConnection() == 1)
		{
			System.Presure_Present = 1;
		}
		else 
		{
			System.Presure_Present = 0;
		}
		
		
		System.T_out_Present = 0;
		
		res.SD_result = f_mount(&FATFS_Obj, "", 1);

	if (res.SD_result == FR_OK) 
		{
			System.SD_Card_Present = 1;
			f_mount(0, "0:", 1);
		}
	else 
		{
			System.SD_Card_Present = 0;
		}
}

void System_Status_Update_Screen(void)
{	
	char buffer[5];
	// system states on screen
			if (System.SD_Card_Present)
			{
				sprintf(buffer, "$");
				PutStringRus11(Coordinate.SD_char_x,Coordinate.SD_char_y,buffer,GREEN,Global_BG_Color);
			}
			else
			{
			sprintf(buffer, "#");
			PutStringRus11(Coordinate.SD_char_x,Coordinate.SD_char_y,buffer,RED,Global_BG_Color);
			}
			
			if (System.Wifi_Present)
			{
				sprintf(buffer, "&");
				PutStringRus11(Coordinate.Wifi_char_x,Coordinate.Wifi_char_y,buffer,GREEN,Global_BG_Color);
			}
			else
			{
				sprintf(buffer, "&");
				PutStringRus11(Coordinate.Wifi_char_x,Coordinate.Wifi_char_y,buffer,Global_BG_Color,Global_BG_Color);
			}
}

