#include "main.h"
#include "hello_screen.h"


extern RTC_HandleTypeDef hrtc;
extern FATFS FATFS_Obj;
extern FIL file;


void Hello_Screen(void)
{ 
	// function for draw Hello screen 
	// with firmware version
	LCD_LED_SET;
	Lcd_Clear(BLACK);	
	
	PutStringRus11(30,15,"METEO",GREEN,BLACK);// "METEO"
	
	PutStringRus11(30,80,"ver 1.0",YELLOW,BLACK); // "ver 1.0"
	
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);

}




void Sensor_test(void)
{
	SD_result_TypeDef res;
	FRESULT fr;    /* FatFs return code */
	int16_t test;
	
	LCD_LED_SET;
	Lcd_Clear(BLACK);
	delay_ms(800);
	
	if (LM75_Temperature_ex(&test) != 0)
		{// we have problem with sensor
			PutStringRus11(10,10,"IN T sensor: ERROR",RED,BLACK);
			//PutStringRus11(10,00,"1:0",RED,BLACK);			
		}
	else 
		{
			PutStringRus11(10,10,"IN T sensor: OK",GREEN,BLACK);
			//PutStringRus11(10,00,"1:0",GREEN,BLACK);
		}
		delay_ms(500);	
	
		if (BMP085_testConnection() == 1)
		{
			PutStringRus11(10,30,"P sensor: OK",GREEN,BLACK);
			//PutStringRus11(10,35,"2:1",GREEN,BLACK);
		}
		else 
		{
			PutStringRus11(10,30,"P sensor: ERROR",RED,BLACK);
			//PutStringRus11(10,35,"2:0",RED,BLACK);
		}
		delay_ms(500);
		
		if (HAL_RTCEx_BKUPRead(&hrtc, 1) == RTC_IS_SET)
			PutStringRus11(10,60,"RTC: OK",GREEN,BLACK);
		else PutStringRus11(10,60,"RTC: ERROR",RED,BLACK);
		
		delay_ms(500);
		
		PutStringRus11(10,90,"OUT T ERROR",RED,BLACK);
		
	res.SD_result = f_mount(&FATFS_Obj, "", 0);


	if (res.SD_result == FR_OK) 
		{

    /* Open a text file */
    fr = f_open(&file, "message.txt", FA_READ);
    if (fr) 
		{
			PutStringRus11(10,100,"SD: OK",GREEN,BLACK);
		}
		else 
		{
			PutStringRus11(10,100,"SD: ERROR",RED,BLACK);
		}
		f_mount(0, "0:", 1);
			}//end mount SD
			
				
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);		
}

