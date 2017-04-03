//#include "stm32f1xx_hal.h"

#include <lm75_hal.h>

I2C_HandleTypeDef hi2c1;

// Init I2C with SPI_Clock_Speed (in Hz)
void LM75_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

		__I2C2_CLK_ENABLE();
		__GPIOB_CLK_ENABLE();
	// Init I2C
	
    /**I2C2 GPIO Configuration    
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		hi2c1.Instance = I2C2;
		hi2c1.Init.ClockSpeed = 100000;
		hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
		hi2c1.Init.OwnAddress1 = 0;
		hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
		hi2c1.Init.OwnAddress2 = 0;
		hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
		hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
		HAL_I2C_Init(&hi2c1);

}


// Set LM75 shutdown mode
// newstate:
//    ENABLE = put LM75 into powerdown mode
//    DISABLE = wake up LM75
void LM75_Shutdown(FunctionalState newstate) {
	uint8_t value;
        uint8_t adr = 0;
  
        adr = LM75_REG_CONF;	
	HAL_I2C_Master_Transmit(&hi2c1, LM75_ADDR, &adr, 1, 1);
        
        HAL_I2C_Master_Receive(&hi2c1, LM75_ADDR, &value, 1, 1);
        
	value = newstate == ENABLE ? value | 0x01 : value & 0xFE;
        //HAL_I2C_Master_Transmit(&hi2c1, LM75_ADDR, &adr, 1, 1);
        HAL_I2C_Master_Transmit(&hi2c1, LM75_ADDR, &value, 1, 1);

}


// Read temperature readings from LM75 in decimal format
// IIIF where:
//   III - integer part
//   F   - fractional part
// e.g. 355 means 35.5C
uint8_t LM75_Temperature(int16_t *temperature, uint8_t Addr) 
{
	uint16_t raw;
	int16_t temp;
	uint8_t adr = 0, tempr[2];
	uint8_t error = 0;

	adr = LM75_REG_TEMP;	
	error = HAL_I2C_Master_Transmit(&hi2c1, Addr, &adr, 1, 1);
	error = HAL_I2C_Master_Receive(&hi2c1, Addr, tempr, 2, 1);
	
	raw = ((tempr[0]<<8) | tempr[1])>>7;
	if (raw & 0x0100) {
		// Negative temperature
		temp = -10 * (((~(uint8_t)(raw & 0xFE) + 1) & 0x7F) >> 1) - (raw & 0x01) * 5;
	} else {
		// Positive temperature
		temp = ((raw & 0xFE) >> 1) * 10 + (raw & 0x01) * 5;
	}
	
	*temperature = temp;

	return error;
}


// Read temperature readings from LM75 in decimal format
// IIIF where:
//   III - integer part
//   F   - fractional part
// e.g. 355 means 35.5C
uint8_t LM75_Temperature_ex(int16_t *temperature) 
{
	uint16_t raw;
	int16_t temp;
	uint8_t adr = 0, tempr[2];
	uint8_t error = 0;

	adr = LM75_REG_TEMP;	
	error = HAL_I2C_Master_Transmit(&hi2c1, LM75_ADDR, &adr, 1, 1);
	error = HAL_I2C_Master_Receive(&hi2c1, LM75_ADDR, tempr, 2, 1);
	
	raw = ((tempr[0]<<8) | tempr[1])>>7;
	if (raw & 0x0100) {
		// Negative temperature
		temp = -10 * (((~(uint8_t)(raw & 0xFE) + 1) & 0x7F) >> 1) - (raw & 0x01) * 5;
	} else {
		// Positive temperature
		temp = ((raw & 0xFE) >> 1) * 10 + (raw & 0x01) * 5;
	}
	
	*temperature = temp;

	return error;
}

// Read temperature readings from LM75 in decimal format
// IIIF where:
//   III - integer part
//   F   - fractional part
// e.g. 355 means 35.5C
uint8_t LM75_Temperature_IN(int16_t *temperature) 
{
	uint16_t raw;
	int16_t temp;
	uint8_t adr = 0, tempr[2];
	uint8_t error = 0;

	adr = LM75_REG_TEMP;	
	error = HAL_I2C_Master_Transmit(&hi2c1, LM75_ADDRESS_IN, &adr, 1, 1);
	error = HAL_I2C_Master_Receive(&hi2c1, LM75_ADDRESS_IN, tempr, 2, 1);
	
	raw = ((tempr[0]<<8) | tempr[1])>>7;
	if (raw & 0x0100) {
		// Negative temperature
		temp = -10 * (((~(uint8_t)(raw & 0xFE) + 1) & 0x7F) >> 1) - (raw & 0x01) * 5;
	} else {
		// Positive temperature
		temp = ((raw & 0xFE) >> 1) * 10 + (raw & 0x01) * 5;
	}
	
	*temperature = temp;

	return error;
}

// Read temperature readings from LM75 in decimal format
// IIIF where:
//   III - integer part
//   F   - fractional part
// e.g. 355 means 35.5C
uint8_t LM75_Temperature_OUT(int16_t *temperature) 
{
	uint16_t raw;
	int16_t temp;
	uint8_t adr = 0, tempr[2];
	uint8_t error = 0;

	adr = LM75_REG_TEMP;	
	error = HAL_I2C_Master_Transmit(&hi2c1, LM75_ADDRESS_OUT, &adr, 1, 1);
	error = HAL_I2C_Master_Receive(&hi2c1, LM75_ADDRESS_OUT, tempr, 2, 1);
	
	raw = ((tempr[0]<<8) | tempr[1])>>7;
	if (raw & 0x0100) {
		// Negative temperature
		temp = -10 * (((~(uint8_t)(raw & 0xFE) + 1) & 0x7F) >> 1) - (raw & 0x01) * 5;
	} else {
		// Positive temperature
		temp = ((raw & 0xFE) >> 1) * 10 + (raw & 0x01) * 5;
	}
	
	*temperature = temp;

	return error;
}
