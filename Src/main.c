/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body

  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "usb_device.h"
#include "LM75_hal.h"
#include "Lcd_Driver.h"
#include "delay.h"
#include "GUI.h"
#include "tm_stm32f4_bmp180.h"
#include "I2Cdev.h"
#include "BMP085.h"
#include "ffconf.h"
#include "diskio.h"
#include "ff.h"
#include "sd_spi_stm32.h"
#include "fattime.h"
#include <time.h>




/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

	TM_BMP180_t BMP180_Data;
	uint32_t avarage_preshure;
	TM_BMP180_Oversampling_t BMP180_Oversampling;
	
extern uint16_t I2Cdev_readTimeout;
extern I2C_HandleTypeDef * I2Cdev_hi2c;
extern I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;
extern SPI_HandleTypeDef hspi1;

void Bmp_180(void);
void Create_new_file(void);

volatile float p, t, a;
	FRESULT result;
	FATFS FATFS_Obj;
	FIL file;
	FILINFO fno;
	UINT nWritten;
	DSTATUS res;
	char str[20], str_data_name[20];
	char buffer[100];
	
int16_t data;
uint8_t str_data[100];
uint8_t file_created=0;


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
	
	LM75_Init();	
	
	LM75_Temperature_ex(&data);
		
//	Create_new_file();
	
//	if (LM75_Temperature_ex(&data) != 0)
//				while(1);//error with LM75. May be sensor not connected.
	
//	TM_BMP180_Init(&BMP180_Data);
//	 /* Initialize BMP180 pressure sensor */
//		if (TM_BMP180_Init(&BMP180_Data) == TM_BMP180_Result_Ok) {
//				/* Init OK */
//			 // TM_USART_Puts(USART1, "BMP180 configured and ready to use\n\n");
//		} else {
//				/* Device error */
//		}
		
	delay_init(72);//Delay init.
	
	Lcd_Init();
//	delay_init(72);//Delay init.
	
	LCD_LED_SET;
	Lcd_Clear(GRAY0);
		
	delay_ms(200);
	Gui_DrawFont_GBK16(16,0,BLUE,GRAY0,(uint8_t*)"Temperature");
	
		LM75_Temperature_ex(&data);
		sprintf(buffer, "%d.%d *C", data/10, data%10);
		delay_ms(200);
		Gui_DrawFont_GBK16(16,40,BLACK,GRAY0,(uint8_t*)buffer);
		

	//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);

//	while(1)
//	{
//				LM75_Temperature_ex(&data);
//				sprintf(buffer, "%d.%d *C", data/10, data%10);
//				delay_ms(200);
//				Gui_DrawFont_GBK16(16,40,BLACK,GRAY0,(uint8_t*)buffer);
//				
//				delay_ms(1500);
//	}
	
	
	Gui_DrawFont_GBK24(16,40,BLACK,GRAY0,(uint8_t*)" fds");
	Gui_DrawFont_GBK16(16,20,BLACK,GRAY0,(uint8_t*)" fds");
	
	sprintf(buffer, "123456");
	
	PutStringRus(0,80,buffer,BLACK,GRAY0);
		

	Bmp_180();
	

  
	/* Infinite loop */
  while (1)
  {
		
  }


}


void Bmp_180(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;


        __I2C1_CLK_ENABLE();
        __GPIOB_CLK_ENABLE();
				__HAL_RCC_AFIO_CLK_ENABLE();
	// Init I2C
        GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	__HAL_AFIO_REMAP_I2C1_ENABLE();

				hi2c2.Instance = I2C1;
        hi2c2.Init.ClockSpeed = 100000;
        hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
        hi2c2.Init.OwnAddress1 = 0;
        hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
        hi2c2.Init.OwnAddress2 = 0;
        hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
        hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
        HAL_I2C_Init(&hi2c2);

    
    I2Cdev_hi2c = &hi2c2; // init of i2cdevlib.  
    // You can select other i2c device anytime and 
    // call the same driver functions on other sensors

    while(!BMP085_testConnection()) ;

    BMP085_initialize();
  
      while (1)
    {
        BMP085_setControl(BMP085_MODE_TEMPERATURE);
        delay_ms(BMP085_getMeasureDelayMilliseconds(BMP085_MODE_TEMPERATURE));
        t = BMP085_getTemperatureC();

        BMP085_setControl(BMP085_MODE_PRESSURE_3);
        delay_ms(BMP085_getMeasureDelayMilliseconds(BMP085_MODE_PRESSURE_3));
        p = BMP085_getPressure();

 				sprintf(buffer, "%.2f Bar", p);
				delay_ms(200);
				Gui_DrawFont_GBK16(16,60,RED,GRAY0,(uint8_t*)buffer);

        a = BMP085_getAltitude(p, 101325);
			
				//LM75_Init();	
			
				LM75_Temperature_ex(&data);
				if (data >= 0)
					sprintf(buffer, "%d.%d *C", data/10, data%10);
				else 
					sprintf(buffer, "-%d.%d *C", data/10, data%10);
				delay_ms(200);
				//Gui_DrawFont_GBK16(16,40,BLACK,GRAY0,(uint8_t*)buffer);
//				Gui_DrawFont_Num32(6, 20, RED, GRAY0, data/100);
//				Gui_DrawFont_Num32(26, 20, RED, GRAY0, (data%100)/10);
//				Gui_DrawFont_Num32(56, 20, RED, GRAY0, 10);
//				Gui_DrawFont_Num32(76, 20, RED, GRAY0, data%10);
//				Gui_DrawFont_Num32(96, 20, RED, GRAY0, 13);
				
				delay_ms(1500);
    }
}


/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBPLLCLK_DIV1_5;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOC_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
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
										file_created = 10;
										//stage_led_control = ONE_GREEN_FLASH;
										f_close(&file);
									}
										
						}
					
						/* Unmount drive, don't forget this! */
						f_mount(0, "0:", 1);
			}//end mount SD
}
/* USER CODE END 4 */

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
