#ifndef INIT_H
#define INIT_H

#include "stm32f1xx_hal.h"
#include "BMP085.h"
#include "main.h"

#define RTC_IS_SET 	0xFFCA


void RTC_Init(void);
void Init_BMP085 (void);
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void Init_Variebles (void);

extern I2C_HandleTypeDef * I2Cdev_hi2c;



#endif
