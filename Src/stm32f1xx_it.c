/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"
#include "diskio.h"
#include "BMP_graph.h"
#include "Lcd_Driver.h"
#include "buttoms.h"


/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_FS;
extern RTC_HandleTypeDef hrtc;
extern void Draw_table (void);
extern void First_Draw_Table (void);

extern Buttom_struct BM_1, BM_2, BM_3;

extern uint16_t minuts_12_flag;

uint8_t pointer_count = 0, button_was_pressed = 0;
uint8_t sec_count=0, minute_flag=0;
uint8_t min_count, hour_count;
uint8_t one_sec_flag =0;

/******************************************************************************/
/*            Cortex-M3 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{

  HAL_IncTick();
	disk_timerproc();
  HAL_SYSTICK_IRQHandler();
	
	if(BM_1.State == BM_PRESSED)
		BM_1.time_presed++;
	
	if(BM_2.State == BM_PRESSED)
		BM_2.time_presed++;
	
	if(BM_3.State == BM_PRESSED)
		BM_3.time_presed++;

}

/**
* @brief This function handles RTC global interrupt.
*/
void RTC_IRQHandler(void)
{
	one_sec_flag = 1;
	
  if (++sec_count == 60)
		{//one minute passed
			minute_flag = 1;
			sec_count = 0;
		}
	
	HAL_RTCEx_RTCIRQHandler(&hrtc);
}


void RTC_Alarm_IRQHandler(void)
{
	
	HAL_RTC_AlarmIRQHandler(&hrtc);
}

// buttom
void EXTI1_IRQHandler (void)
{

	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)==0)
	{//buttom pressed
		BM_1.time_presed = 0;
		BM_1.State = BM_PRESSED;
	}
	else
	{
		BM_1.State = BM_RELEASED;
	}
	button_was_pressed = 1;
	
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

// buttom
void EXTI2_IRQHandler (void)
{

	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==0)
	{//buttom pressed
		BM_2.time_presed = 0;
		BM_2.State = BM_PRESSED;
	}
	else
	{
		BM_2.State = BM_RELEASED;
	}
	button_was_pressed = 1;
	
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

// buttom
void EXTI3_IRQHandler (void)
{
	HAL_GPIO_TogglePin(LCD_PORT_P, LCD_LED);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}


/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles USB low priority or CAN RX0 interrupts.
*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{

//  HAL_PCD_IRQHandler(&hpcd_USB_FS);

}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
