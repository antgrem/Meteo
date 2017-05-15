/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body

  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Terminal.h"
#include "Hello_screen.h"
#include "buttoms.h"
#include "Files.h"

/* Private function prototypes -----------------------------------------------*/

void Error_Handler(void);

void Draw_table_ex (void);
	
extern uint16_t I2Cdev_readTimeout;
extern I2C_HandleTypeDef * I2Cdev_hi2c;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern SPI_HandleTypeDef hspi1;
extern RTC_HandleTypeDef hrtc;

extern uint8_t pointer_count;
extern uint8_t sec_count, minute_flag;
extern uint8_t button_was_pressed;
extern uint8_t one_sec_flag;
extern uint8_t min_count, hour_count;

extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;

extern Buttom_struct BM_1, BM_2, BM_3;

Messure_DataTypeDef All_data;
Messure_DataTypeDef Day_data_Array[DAY_DATA_ARRAY_LENGTH];
Messure_DataTypeDef Avarage_data,
										Avarage_array[4];
uint16_t circle_array_count;

volatile float p, t, a;
//volatile 	int count_store_data=0;

uint16_t Global_Font_Color = RED, Global_BG_Color = BLACK;

uint8_t hour_flag = 0;


char str_file_name[20];
char str_file_year_name[20];
char buffer[10], dot[2]="~";
char BM_time_buffer[10] = "";
char T_in_string[10] = "", T_out_string[10] = "", Presure_string[10]="", Time_string[10]="";
char Day_string[12];
System_TypeDef System;
Coord_TypeDef Coordinate;

uint8_t set_rtc_time=0, set_rtc_date=0, get_data=0;
uint8_t minuts_10=0, count_10_min=0;
uint8_t end_of_day_flag=0;

time_t time_day_count;
struct tm* time_tm_day_count;

time_t time_temp;
struct tm* time_tm_temp;
struct tm Day;

int main(void)
{
  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
	
	Init_Variebles();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
//  MX_USB_DEVICE_Init();
	

	RTC_Init();
	HAL_RTC_GetTime(&hrtc, &(sTime), RTC_FORMAT_BIN);
	sec_count = sTime.Seconds;	
	min_count = sTime.Minutes;
	hour_count = sTime.Hours;
	
	LM75_Init();	
	
	Init_BMP085();
	
	Lcd_Init();
	
	LCD_LED_SET;
	//Lcd_Clear(LIGHTGREY);	
	
//		Terminal_shift_line();
	time_temp = ReadTimeCounter(&hrtc);
	time_tm_temp = localtime(&time_temp);
	sprintf(str_file_name, "%04d%02d%02d.txt", time_tm_temp->tm_year+1900, time_tm_temp->tm_mon + 1, time_tm_temp->tm_mday);
	sprintf(str_file_year_name, "%04d_year.txt", time_tm_temp->tm_year+1900);		
	Day.tm_year = time_tm_temp->tm_year+1900;
	Day.tm_mon = time_tm_temp->tm_mon + 1;
	Day.tm_mday = time_tm_temp->tm_mday;
	minuts_10 = time_tm_temp->tm_min % 10;//write data to sd every 10 minuts
	
	Hello_Screen();
	Sensor_test();
	Lcd_Clear(Global_BG_Color);

//end of initialls

		Create_new_file();
		Init_offset();			
		
		Take_new_Messure(&All_data);
		
		Avarage_array[0] = All_data;
		Avarage_array[1] = All_data;
		Avarage_array[2] = All_data;
		Avarage_array[3] = All_data;
		
		System_Status_Checked();
		Draw_table_ex();// redraw data on screen
			
			
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
					//	PutStringRus(Coordinate.Time_x,Coordinate.Time_y,Time_string,Global_BG_Color,Global_BG_Color);
						sprintf(Time_string, "%02d:%02d:%02d;", sTime.Hours, sTime.Minutes, sTime.Seconds);
						PutStringRus(Coordinate.Time_x,Coordinate.Time_y,Time_string,BLUE,Global_BG_Color);
					}
				
				System_Status_Update_Screen();
				
//				sprintf(buffer, "A");
//				PutStringRus(Coordinate.Weather_x,Coordinate.Weather_y,buffer,BLUE,Global_BG_Color);
//				sprintf(buffer, "B");
//				PutStringRus(Coordinate.Weather_x+16,Coordinate.Weather_y,buffer,BLUE,Global_BG_Color);
				
			}// end if (one_sec_flag == 1)
		
		
		if (minute_flag == 1)
		{// one minut event
			minute_flag = 0;
			if (++min_count == 60)
			{
				min_count = 0;
				hour_flag = 1;
				if (++hour_count == 24)
				{
					hour_count = 0;
					end_of_day_flag = 1;
				}
			}
			
			//draw red dot (start of measurement)
			PutStringRus11(Coordinate.Dot_x,Coordinate.Dot_y,dot,RED,Global_BG_Color);
			
			Take_new_Messure(&All_data);
			
			Avarage_array[circle_array_count++] = All_data;
			if (circle_array_count == 4) 
				circle_array_count = 0;
			Avarage_data.T_out = (Avarage_array[0].T_out + Avarage_array[1].T_out + \
																		Avarage_array[2].T_out + Avarage_array[3].T_out) >> 2;
			Avarage_data.T_in = (Avarage_array[0].T_in + Avarage_array[1].T_in + \
																		Avarage_array[2].T_in + Avarage_array[3].T_in) >> 2;
			Avarage_data.Pressure_p = (Avarage_array[0].Pressure_p + Avarage_array[1].Pressure_p + \
																		Avarage_array[2].Pressure_p + Avarage_array[3].Pressure_p)/4;
						
			if (minuts_10++ == 9)
			{
				minuts_10 = 0; 
				Avarage_data.Time = All_data.Time;
				Avarage_data.Day = Day;
			
				Write_file_one(&Avarage_data);
			}
			
			System_Status_Checked();
			Draw_table_ex();// redraw data on screen
			
						// draw new day data on left-bottom side of screen
			time_day_count = ReadTimeCounter(&hrtc);
			time_tm_day_count = localtime(&time_day_count);
			sprintf(Day_string, "%04d %02d %02d", time_tm_day_count->tm_year + 1900, time_tm_day_count->tm_mon + 1, time_tm_day_count->tm_mday);
			PutStringRus11(Coordinate.Day_x,Coordinate.Day_y,Day_string,YELLOW,Global_BG_Color); 
			
			PutStringRus11(Coordinate.Dot_x,Coordinate.Dot_y,dot,Global_BG_Color,Global_BG_Color);
		}// end if (minute_flag == 1)
			
		if (hour_flag == 1)
			{//one hour remain
				hour_flag = 0;
			}// end if (hour_flag == 1)
		
		if (end_of_day_flag == 1)
		{// wow, midnight 
			end_of_day_flag = 0;
			Create_new_file();

			// draw new day data on left-bottom side of screen
			time_day_count = ReadTimeCounter(&hrtc);
			time_tm_day_count = localtime(&time_day_count);
			sprintf(Day_string, "%04d %02d %02d", time_tm_day_count->tm_year + 1900, time_tm_day_count->tm_mon + 1, time_tm_day_count->tm_mday);
			PutStringRus11(Coordinate.Day_x,Coordinate.Day_y,Day_string,YELLOW,Global_BG_Color); 
			Day.tm_year = time_tm_temp->tm_year+1900;
			Day.tm_mon = time_tm_temp->tm_mon + 1;
			Day.tm_mday = time_tm_temp->tm_mday;
		}

		
		if (button_was_pressed == 1)
		{
			button_was_pressed = 0;
			PutStringRus11(10,110,BM_time_buffer,Global_BG_Color,Global_BG_Color);
			sprintf(BM_time_buffer, "%d", BM_1.time_presed);
			PutStringRus11(10,110,BM_time_buffer,BLUE,Global_BG_Color);
			//Store_data_in_new_file();
		}

	}// end while(1)


}



void Draw_table_ex (void)
{
	uint16_t color;
	
	//Take_new_Messure(&All_data);
	
	PutStringRus(Coordinate.Presure_x,Coordinate.Presure_y,Presure_string,Global_BG_Color,Global_BG_Color);
	sprintf(Presure_string, "%.2f", All_data.Pressure_p/1000);
	PutStringRus(Coordinate.Presure_x,Coordinate.Presure_y,Presure_string,DARKGREY,Global_BG_Color);
        	
				delay_ms(50);

				if (All_data.T_in >= 0)
				{
					
					if (All_data.T_in > 250)
					{
						color = RED;
					}
					else if (All_data.T_in > 100)
					{
						color = 0xFA8A;
					}
					else
					{
						color = 0xFE55;
					}
					PutStringRus(Coordinate.Tempr_in_x,Coordinate.Tempr_in_y,T_in_string,color,Global_BG_Color);
					sprintf(T_in_string, "+%d", All_data.T_in/10);
					PutStringRus(Coordinate.Tempr_in_x,Coordinate.Tempr_in_y,T_in_string,color,Global_BG_Color);
				}
				else 
				{
					
					if (All_data.T_in < -200)
					{
						color = BLUE;
					}
					else if (All_data.T_in <-100)
					{
						color = 0x6B5F;
					}
					else
					{
						color = 0xB65F;
					}
					PutStringRus(Coordinate.Tempr_in_x,Coordinate.Tempr_in_y,T_in_string,color,Global_BG_Color);
					sprintf(T_in_string, "-%d", All_data.T_in/10);
					PutStringRus(Coordinate.Tempr_in_x,Coordinate.Tempr_in_y,T_in_string,color,Global_BG_Color);
				}
				
				if (System.T_out_Present == 0)
				{
					sprintf(buffer, "---");
					PutStringRus(Coordinate.Tempr_out_x,Coordinate.Tempr_out_y,buffer,DARKGREY,Global_BG_Color);
				}
				else
				{
					if (All_data.T_out >= 0)
						{
							
							if (All_data.T_out > 250)
								{
									color = RED;
								}
								else if (All_data.T_out > 100)
								{
									color = 0xFA8A;
								}
								else
								{
									color = 0xFE55;
								}
							PutStringRus(Coordinate.Tempr_out_x,Coordinate.Tempr_out_y,T_out_string,color,Global_BG_Color);
							sprintf(T_out_string, "+%d", All_data.T_out/10);
							PutStringRus(Coordinate.Tempr_out_x,Coordinate.Tempr_out_y,T_out_string,color,Global_BG_Color);
						}
						else 
						{
							
							if (All_data.T_out < -200)
								{
									color = BLUE;
								}
								else if (All_data.T_out <-100)
								{
									color = 0x6B5F;
								}
								else
								{
									color = 0xB65F;
								}
							PutStringRus(Coordinate.Tempr_out_x,Coordinate.Tempr_out_y,T_out_string,color,Global_BG_Color);
							sprintf(T_out_string, "-%d", All_data.T_out/10);
							PutStringRus(Coordinate.Tempr_out_x,Coordinate.Tempr_out_y,T_out_string,color,Global_BG_Color);
						}
				}
				
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
	
		if (System.T_out_Present == 1)
			LM75_Temperature(&(data->T_out), LM75_ADDRESS_OUT);
	
		HAL_RTC_GetTime(&hrtc, &(data->Time), RTC_FORMAT_BIN);
	
}




/**
  * @brief  Read the time counter available in RTC_CNT registers.
  * @param  hrtc   pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval Time counter
  */
uint32_t ReadTimeCounter(RTC_HandleTypeDef* hrtc)
{
  uint16_t high1 = 0, high2 = 0, low = 0;
  uint32_t timecounter = 0;

  high1 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);
  low   = READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT);
  high2 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);

  if (high1 != high2)
  { /* In this case the counter roll over during reading of CNTL and CNTH registers, 
       read again CNTL register then return the counter value */
    timecounter = (((uint32_t) high2 << 16 ) | READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT));
  }
  else
  { /* No counter roll over during reading of CNTL and CNTH registers, counter 
       value is equal to first value of CNTL and CNTH */
    timecounter = (((uint32_t) high1 << 16 ) | low);
  }

  return timecounter;
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
