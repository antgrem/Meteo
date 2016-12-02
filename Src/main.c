/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body

  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Terminal.h"


/* Private function prototypes -----------------------------------------------*/

void Morda (void);
void Error_Handler(void);
void First_Draw_Table (void);
void Hello_Screen(void);
void Sensor_test(void);


void Draw_table_ex (void);

	
extern uint16_t I2Cdev_readTimeout;
extern I2C_HandleTypeDef * I2Cdev_hi2c;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern SPI_HandleTypeDef hspi1;
extern RTC_HandleTypeDef hrtc;

extern float Average_pressure;
extern int16_t Averaga_temperature;
extern int16_t tempr_data;
extern uint8_t pointer_count;
extern uint8_t sec_count, minute_flag;
extern uint8_t button_was_pressed;
extern uint8_t one_sec_flag;
extern uint8_t min_count, hour_count;

extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;

RTC_TimeTypeDef sTime_temp;
RTC_DateTypeDef sDate_temp;

Messure_DataTypeDef All_data;
Messure_DataTypeDef Day_data_Array[DAY_DATA_ARRAY_LENGTH];

volatile float p, t, a;
volatile 	int count_store_data=0;

volatile uint8_t store_data = 0;
uint16_t count_time_store, count_time_store_en=0;
	
FRESULT result;
FATFS FATFS_Obj;
FIL file;
FILINFO fno;
UINT nWritten;
DSTATUS res;
char str_data_name[20];
char buffer[100];

uint16_t second_1_flag=0, minuts_12_flag=0;
uint8_t set_rtc_time=0, set_rtc_date=0, get_data=0;
uint8_t minuts_10=0, count_10_min=0;
uint8_t end_of_day_flag=0;
uint32_t count_32;

//uint8_t str_data[100];

void (* pfunction) (void);


int main(void)
{
  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
//  MX_USB_DEVICE_Init();
	

	RTC_Init();
	HAL_RTC_GetTime(&hrtc, &(sTime), RTC_FORMAT_BIN);
	sec_count = sTime.Seconds;	
	min_count = sTime.Minutes;
	hour_count = sTime.Hours;
	
	count_10_min = (hour_count*60 + min_count) / 10;
	
	LM75_Init();	
	
	Init_BMP085();
	
	Lcd_Init();
	
	LCD_LED_SET;
	//Lcd_Clear(LIGHTGREY);	
	
//		Terminal_shift_line();
		
	Hello_Screen();
	Sensor_test();
	Lcd_Clear(BLACK);
	
	pfunction = Draw_table_ex;	
	pfunction();
	
	Take_new_Messure(&All_data);
	Average_pressure = All_data.Pressure_p/1000;
	Averaga_temperature = All_data.T_in;

	//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);

	
	while(1)
	{
		if (one_sec_flag == 1)
		{// one second event
			one_sec_flag = 0;
			if (pointer_count == 0)
			{
				HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				sprintf(buffer, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
				PutStringRus11(0,87,buffer,BLUE,LIGHTGREY);
			}
		}// end if (one_sec_flag == 1)
		
		
		if (minute_flag == 1)
		{// one minut event
			minute_flag = 0;
			if (++min_count == 60)
			{
				min_count = 0;
				if (++hour_count == 24)
				{
					hour_count = 0;
					end_of_day_flag = 1;
				}
			}
			
			Gui_Circle(5, 5, 2, RED);
			
			Take_new_Messure(&All_data);
			
			if (minuts_10++ == 9)
			{
				minuts_10 = 0; 
				Day_data_Array[count_10_min].T_in = All_data.T_in;
				Day_data_Array[count_10_min].Pressure_p = All_data.Pressure_p;
				Day_data_Array[count_10_min].Time = All_data.Time;
				count_10_min++;
				if (count_10_min == DAY_DATA_ARRAY_LENGTH)
					count_10_min = 0;
			}
			
			if (pointer_count == 0)
				pfunction();// draw new data on screen if table
			
			
			Gui_Circle(5, 5, 2, LIGHTGREY);
		}// end if (minute_flag == 1)
			
		
		if (end_of_day_flag == 1)
		{// wow, midnight 
			end_of_day_flag = 0;
			//Store_data_in_new_file();
		}

		
		if (button_was_pressed == 1)
		{
			button_was_pressed = 0;
			pfunction();
		}
		

	}


}

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
	LCD_LED_SET;
	Lcd_Clear(BLACK);
	delay_ms(800);
	
	if (LM75_Temperature_ex(&tempr_data) != 0)
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
		
		PutStringRus11(10,90,"OUT T, SD: ERROR/OK?",BLUE,BLACK);
		//PutStringRus11(10,90,"45:10",BLUE,BLACK);
		
				
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);		
}

void First_Draw_Table (void)
{
	Lcd_Clear(LIGHTGREY);
	
	Gui_DrawLine(63, 0, 63, 40, DARKGREY);
	Gui_DrawLine(62, 0, 62, 40, DARKGREY);
	
	Gui_DrawLine(0, 40, 127, 40, DARKGREY);
	Gui_DrawLine(0, 41, 127, 41, DARKGREY);
	
	Gui_DrawLine(0, 85, 127, 85, DARKGREY);
	Gui_DrawLine(0, 86, 127, 86, DARKGREY);
	
	pfunction = Draw_table_ex;
}


void Draw_table_ex (void)
{
	
	Take_new_Messure(&All_data);
	
	
				sprintf(buffer, "%.2f", All_data.Pressure_p/1000);
				PutStringRus11(0,42,buffer,DARKGREY,LIGHTGREY);
	
        	
				delay_ms(50);

				if (All_data.T_in >= 0)
				{
					sprintf(buffer, "+%d", All_data.T_in/10);
					PutStringRus11(0,0,buffer,RED,LIGHTGREY);
					sprintf(buffer, "%.2f +%d %02d:%02d:%02d", All_data.Pressure_p/1000, All_data.T_in/10, All_data.Time.Hours, All_data.Time.Minutes, All_data.Time.Seconds);
					PutStringRus11(0,100,buffer,RED,LIGHTGREY);
				}
				else 
				{
					sprintf(buffer, "-%d", All_data.T_in/10);
					PutStringRus11(0,0,buffer,BLUE,LIGHTGREY);
					sprintf(buffer, "%.2f -%d %02d:%02d:%02d", All_data.Pressure_p/1000, All_data.T_in/10, All_data.Time.Hours, All_data.Time.Minutes, All_data.Time.Seconds);
					PutStringRus11(0,100,buffer,RED,LIGHTGREY);
				}
				
				//PutStringRus11(64,0,buffer,BLUE,LIGHTGREY);
		
				sprintf(buffer, "%02d:%02d:%02d", All_data.Time.Hours, All_data.Time.Minutes, All_data.Time.Seconds);
				PutStringRus11(0,87,buffer,BLUE,LIGHTGREY);
				
				
				
				//delay_ms(800);
}

void Morda (void)
{//draw lines for meteo station
	
	Gui_DrawLine(63, 0, 63, 40, DARKGREY);
	Gui_DrawLine(62, 0, 62, 40, DARKGREY);
	
	Gui_DrawLine(0, 40, 127, 40, DARKGREY);
	Gui_DrawLine(0, 41, 127, 41, DARKGREY);
	
	Gui_DrawLine(0, 85, 127, 85, DARKGREY);
	Gui_DrawLine(0, 86, 127, 86, DARKGREY);
	
}


void Take_new_Messure(Messure_DataTypeDef *data)
{
		BMP085_setControl(BMP085_MODE_TEMPERATURE);
		delay_ms(BMP085_getMeasureDelayMilliseconds(BMP085_MODE_TEMPERATURE));
		data->Temperature_p = BMP085_getTemperatureC();

		BMP085_setControl(BMP085_MODE_PRESSURE_3);
		delay_ms(BMP085_getMeasureDelayMilliseconds(BMP085_MODE_PRESSURE_3));
		data->Pressure_p = BMP085_getPressure();

		LM75_Temperature(&(data->T_in), LM75_ADDRESS_IN);
	
		if (data->Present_T_out == 1)
			LM75_Temperature(&(data->T_in), LM75_ADDRESS_OUT);
	
		HAL_RTC_GetTime(&hrtc, &(data->Time), RTC_FORMAT_BIN);
	
}


// Store data in new_file
SD_result_TypeDef Store_data_in_new_file(void)
{ //FRESULT result;
	uint8_t ij;
	count_time_store = 0;
	count_time_store_en = 1;
	SD_result_TypeDef res;
	
	res.SD_result = f_mount(&FATFS_Obj, "", 0);
	res.Stage = 0;

	if (res.SD_result == FR_OK) 
		{
				
			HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);	
			HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
			
			sprintf(str_data_name, "%04d%02d%02d.txt", sDate.Year+2000, sDate.Month, sDate.Date);
			res.SD_result = f_open(&file, str_data_name, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
			res.Stage = 1;
			if (res.SD_result == FR_OK)
				{//write redline

					sprintf(buffer, "Time\tT_in\tT_out\tPresure\t\n");
					res.SD_result = f_lseek(&file, f_size(&file));
					res.Stage = 2;
					if(res.SD_result == FR_OK)
						{}//go to end of file
						
							/* If we put more than 0 characters (everything OK) */
							count_store_data = f_puts(buffer, &file);
						if (count_store_data > 0) 
							{}//data were stored, but what to do I don't know
								
						for (ij=0; ij < DAY_DATA_ARRAY_LENGTH; ij++)
							{
								
								sprintf(buffer, "%02d:%02d:%02d\t%d\t%f\t\n", Day_data_Array[ij].Time.Hours, Day_data_Array[ij].Time.Minutes, Day_data_Array[ij].Time.Seconds, Day_data_Array[ij].T_in, Day_data_Array[ij].Pressure_p);
								
								count_store_data = f_puts(buffer, &file);
							}
							
					f_close(&file);	
				}			
					
						/* Unmount drive, don't forget this! */
						f_mount(0, "0:", 1);
			}//end mount SD
		
			if (count_store_data == EOF)
				store_data = 1;
		count_time_store_en = 0;//stop increment count_time_store value
			
		return res;
}



/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  
}


void delay_ms(uint16_t nms)
{
	HAL_Delay(nms);
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
