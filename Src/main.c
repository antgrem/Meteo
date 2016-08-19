/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body

  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"





/* Private function prototypes -----------------------------------------------*/

FRESULT Create_new_file(void);
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

extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;

Messure_DataTypeDef All_data;

volatile float p, t, a;
	
FRESULT result;
FATFS FATFS_Obj;
FIL file;
FILINFO fno;
UINT nWritten;
DSTATUS res;
char str_data_name[20];
char buffer[100];

uint16_t second_1_flag=0, minuts_12_flag=0;
	
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
	
	if (HAL_RTCEx_BKUPRead(&hrtc, 1) != RTC_IS_SET)
			RTC_Init();// check RTC is init or no
	
	LM75_Init();	
	
	Init_BMP085();
	
	delay_init(72);//Delay init.	
	
	Lcd_Init();
	
	LCD_LED_SET;
	//Lcd_Clear(LIGHTGREY);	
	
	Hello_Screen();
	Sensor_test();
	
	Create_new_file();
		
	pfunction = Draw_table_ex;	
	pfunction();
	
	Take_new_Messure(&All_data);
	Average_pressure = All_data.Pressure_p/1000;
	Averaga_temperature = All_data.T_in;

	//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);

	
	while(1)
	{
		
		if (minute_flag == 1)
		{
			minute_flag = 0;
			Gui_Circle(5, 5, 2, RED);
			
			Take_average_data();
			
			HAL_RTC_GetTime(&hrtc, &(sTime), RTC_FORMAT_BIN);
			if ((sTime.Seconds == 0) && (sTime.Minutes == 0) && (sTime.Hours == 0))
			{//we have new day/ time to create new file for data
				Create_new_file();
			}
			
			Gui_Circle(5, 5, 2, LIGHTGREY);
		}
			
		
		
		if (second_1_flag >= 1)
		{//every seconds
			second_1_flag = 0;
			minuts_12_flag++;
			
			if (pointer_count == 0)
				pfunction();
			
			Take_average_data();
		}
		
		if (minuts_12_flag == 300)
		{//every 12 minuts if == 120
			// 
			pfunction();
			minuts_12_flag = 0;
		}
		

	}


}

void Hello_Screen(void)
{ 
	// function for draw Hello screen 
	// with firmware version
	LCD_LED_SET;
	Lcd_Clear(BLACK);	
	
	PutStringRus(30,15,"123",GREEN,BLACK);// "METEO"
	
	PutStringRus(30,80,"1.0",YELLOW,BLACK); // "ver 1.0"
	
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
			//PutStringRus(10,10,"IN T sensor: ERROR",RED,BLACK);
			PutStringRus(10,00,"1:0",RED,BLACK);			
		}
	else 
		{
			//PutStringRus(10,10,"IN T sensor: OK",GREEN,BLACK);
			PutStringRus(10,00,"1:0",GREEN,BLACK);
		}
	delay_ms(500);	
	
		if (BMP085_testConnection() != 1)
		{
			//PutStringRus(10,40,"P sensor: OK",GREEN,BLACK);
			PutStringRus(10,35,"2:1",GREEN,BLACK);
		}
		else 
		{
			//PutStringRus(10,40,"P sensor: ERROR",RED,BLACK);
			PutStringRus(10,35,"2:0",RED,BLACK);
		}
	delay_ms(500);
		
		//PutStringRus(10,80,"RTC: ERROR/OK?",YELLOW,BLACK);
		PutStringRus(10,70,"3:10",YELLOW,BLACK);
		
		delay_ms(500);
		
		//PutStringRus(10,120,"OUT T, SD: ERROR/OK?",BLUE,BLACK);
		PutStringRus(10,90,"45:10",BLUE,BLACK);
		
				
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
				delay_ms(200);
				PutStringRus(0,42,buffer,DARKGREY,LIGHTGREY);
	
        	
				delay_ms(200);

				if (All_data.T_in >= 0)
				{
					sprintf(buffer, "+%d", All_data.T_in/10);
					PutStringRus(0,0,buffer,RED,LIGHTGREY);
				}
				else 
				{
					sprintf(buffer, "-%d", All_data.T_in/10);
					PutStringRus(0,0,buffer,BLUE,LIGHTGREY);
				}
				
				PutStringRus(64,0,buffer,BLUE,LIGHTGREY);
		
				sprintf(buffer, "%02d:%02d:%02d", All_data.Time.Hours, All_data.Time.Minutes, All_data.Time.Seconds);
				PutStringRus(0,87,buffer,BLUE,LIGHTGREY);
				
				delay_ms(1500);
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
		data->Pressure_p = BMP085_getPressure()/1000;

		LM75_Temperature(&(data->T_in), LM75_ADDRESS_IN);
	
		if (data->Present_T_out == 1)
			LM75_Temperature(&(data->T_in), LM75_ADDRESS_OUT);
	
		HAL_RTC_GetTime(&hrtc, &(data->Time), RTC_FORMAT_BIN);
	
}

// try to created new file "DataX.txt", were X = 0...254
// if no, one red flash.
// if all 254 files already exist, then turn on Red Led and endless while();
FRESULT Create_new_file(void)
{ FRESULT result;
	
	result = f_mount(&FATFS_Obj, "", 0);

	if (result == FR_OK) 
		{
				
			HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);	
			
			sprintf(str_data_name, "%d_%d_%d.txt", sDate.Year+2000, sDate.Month, sDate.Date);
			if (f_stat(str_data_name, &fno) == FR_OK)
				{// File already exist
					return FR_OK;
				}
			
			if (f_open(&file, str_data_name, FA_CREATE_NEW | FA_READ | FA_WRITE) == FR_OK)
				{//write redline

					sprintf(buffer, "Time\tT_in\tT_out\tPresure\t\n");
					if(f_lseek(&file, f_size(&file)) == FR_OK)
						{}//go to end of file
						
							/* If we put more than 0 characters (everything OK) */
						if (f_puts(buffer, &file) > 0) 
							{}//data were stored, but what to do I don't know
					f_close(&file);	
				}			
					
						/* Unmount drive, don't forget this! */
						f_mount(0, "0:", 1);
			}//end mount SD
		
		return result;
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
