/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body

  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"





/* Private function prototypes -----------------------------------------------*/

void Create_new_file(void);
void Morda (void);
void Error_Handler(void);
void Draw_table (void);
void First_Draw_Table (void);
void Hello_Screen(void);
void Sensor_test(void);
void Take_new_Messure(Messure_DataTypeDef *data);

	
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
  MX_USB_DEVICE_Init();
	RTC_Init();
	
	LM75_Init();	
	
	Init_BMP085();
	
	delay_init(72);//Delay init.	
	
	Lcd_Init();
	
	LCD_LED_SET;
	//Lcd_Clear(LIGHTGREY);	
	
//	Hello_Screen();
	
//	Sensor_test();
//	Create_new_file();
		
	//pfunction = Draw_table;
	pfunction = First_Draw_Graph;	
	pfunction();
	
		BMP085_setControl(BMP085_MODE_TEMPERATURE);
		delay_ms(BMP085_getMeasureDelayMilliseconds(BMP085_MODE_TEMPERATURE));
		t = BMP085_getTemperatureC();

		BMP085_setControl(BMP085_MODE_PRESSURE_3);
		delay_ms(BMP085_getMeasureDelayMilliseconds(BMP085_MODE_PRESSURE_3));
		Average_pressure = BMP085_getPressure()/1000;

		LM75_Temperature(&tempr_data, LM75_ADDRESS_IN);
		Averaga_temperature = tempr_data;
		

	//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);

	
	while(1)
	{
		
		if (minute_flag == 1)
		{
			minute_flag = 0;
			PutStringRus(5,5,".",RED,BLACK);
			delay_ms(800);
			PutStringRus(5,5,".",BLACK,BLACK);
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
	
	PutStringRus(30,30,"METEO",GREEN,BLACK);
	
	PutStringRus(60,100,"ver 1.0",YELLOW,BLACK);
	
	delay_ms(800);
	delay_ms(800);
}

void Sensor_test(void)
{
	LCD_LED_SET;
	Lcd_Clear(BLACK);
	delay_ms(800);
	
	if (LM75_Temperature_ex(&tempr_data) != 0)
		{// we have problem with sensor
			PutStringRus(10,10,"IN T sensor: ERROR",RED,BLACK);	
		}
	else 
		{
			PutStringRus(10,10,"IN T sensor: OK",GREEN,BLACK);
		}
	delay_ms(800);	
	
		if (BMP085_testConnection() == 1)
		{
			PutStringRus(10,40,"P sensor: OK",GREEN,BLACK);
		}
		else 
		{
			PutStringRus(10,40,"P sensor: ERROR",RED,BLACK);
		}
	delay_ms(800);
		
		PutStringRus(10,80,"RTC: ERROR/OK?",YELLOW,BLACK);
		
		PutStringRus(10,120,"OUT T, SD: ERROR/OK?",BLUE,BLACK);
		
	delay_ms(800);
	delay_ms(800);
	delay_ms(800);		
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
	
	pfunction = Draw_table;
}

void Draw_table (void)
{
	      BMP085_setControl(BMP085_MODE_TEMPERATURE);
        delay_ms(BMP085_getMeasureDelayMilliseconds(BMP085_MODE_TEMPERATURE));
        t = BMP085_getTemperatureC();

        BMP085_setControl(BMP085_MODE_PRESSURE_3);
        delay_ms(BMP085_getMeasureDelayMilliseconds(BMP085_MODE_PRESSURE_3));
        p = BMP085_getPressure();

 				sprintf(buffer, "%.2f", p/1000);
				delay_ms(200);
				PutStringRus(0,42,buffer,DARKGREY,LIGHTGREY);
				
        a = BMP085_getAltitude(p, 101325);
			
				delay_ms(200);
				LM75_Temperature_ex(&tempr_data);
				if (tempr_data >= 0)
				{
					sprintf(buffer, "+%d", tempr_data/10);
					PutStringRus(0,0,buffer,RED,LIGHTGREY);
				}
				else 
				{
					sprintf(buffer, "-%d", tempr_data/10);
					PutStringRus(0,0,buffer,BLUE,LIGHTGREY);
				}
				
				PutStringRus(64,0,buffer,BLUE,LIGHTGREY);
				
				HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				sprintf(buffer, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
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
void Create_new_file(void)
{ uint8_t i;
	time_t time_temp;
	struct tm* time_tm_temp;
	char str_temp[20];

	
	if (f_mount(&FATFS_Obj, "", 0) == FR_OK) 
		{
				i = 0;	
			
			
				result = FR_OK;
				while ((result != FR_NO_FILE) && (i != 0xFF))
					{
						sprintf(str_data_name, "Data_%d.txt", i);
						result = f_stat(str_data_name, &fno);
						i++;
					}
				
				if (i == 0xFF)
					{	// all 255 files were created
						//RED_ON;
						//while(1);
					}
				else{
							if (f_open(&file, str_data_name, FA_CREATE_NEW | FA_READ | FA_WRITE) == FR_OK)
									{//write redline
										
										time_temp = 1456329855;//RTC_GetCounter();
										/* If we put more than 0 characters (everything OK) */
										if (f_puts(ctime(&time_temp), &file) > 0) 
											{}//data were stored, but what to do I don't know
																					
										sprintf(buffer, "i\tX\tY\tZ\t\tX_min\tX_max\n");
										if(f_lseek(&file, f_size(&file)) == FR_OK){};
											
										/* If we put more than 0 characters (everything OK) */
										if (f_puts(buffer, &file) > 0) 
											{}//data were stored, but what to do I don't know
										//stage_led_control = ONE_GREEN_FLASH;
											
										f_close(&file);	
									}
									else ;//stage_led_control = ONE_RED_FLASH;
									
										time_temp = 1456329855;//RTC_GetCounter();
										time_tm_temp = localtime(&time_temp);
										
										sprintf(str_temp, "%02d_%d_%d.txt", (time_tm_temp->tm_year)-100, (time_tm_temp->tm_mon)+1, time_tm_temp->tm_mday);
									result = f_open(&file, str_temp, FA_CREATE_NEW | FA_READ | FA_WRITE);
										if (result == FR_OK)
									{
										//stage_led_control = ONE_GREEN_FLASH;
										f_close(&file);
									}
										
						}
					
						/* Unmount drive, don't forget this! */
						f_mount(0, "0:", 1);
			}//end mount SD
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
