/* I2C to use for communications with LM75 */
#ifndef LM75_HAL_H
#define LM75_HAL_H

#include "stdint.h"
#include "stm32f1xx_hal.h"

#define LM75_ADDRESS 0x9F

#define LM75_ADDRESS_IN   0x9F
#define LM75_ADDRESS_OUT  0x90

/* LM75 defines */
#define LM75_ADDR                     0x9F // LM75 address

/* LM75 registers */
#define LM75_REG_TEMP                 0x00 // Temperature
#define LM75_REG_CONF                 0x01 // Configuration
#define LM75_REG_THYS                 0x02 // Hysteresis
#define LM75_REG_TOS                  0x03 // Overtemperature shutdown

void LM75_Init(void);
void LM75_Shutdown(FunctionalState newstate);
uint8_t LM75_Temperature(int16_t *temperature, uint8_t Addr);
uint8_t LM75_Temperature_ex(int16_t *temperature);
uint8_t LM75_Temperature_IN(int16_t *temperature);
uint8_t LM75_Temperature_OUT(int16_t *temperature);

#endif
