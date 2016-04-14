/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32f4_bmp180.h"
#include <stdio.h>

I2C_HandleTypeDef hi2c2;

/* EEPROM values */
int16_t AC1, AC2, AC3, B1, B2, MB, MC, MD;
uint16_t AC4, AC5, AC6, UT;
/* OK */
int32_t X1, X2, X3, B3, B5, B6, T, p;
uint32_t B4, B7, UP;
uint8_t lib_initialized = 0;
uint8_t adr = 0;

TM_BMP180_Result_t TM_BMP180_Init(TM_BMP180_t* BMP180_Data) {
	uint8_t data[22];
	uint8_t i = 0;
	GPIO_InitTypeDef GPIO_InitStruct;

	
	/* Initialize I2C */
	
		/* Peripheral clock enable */
    __I2C2_CLK_ENABLE();
		__GPIOB_CLK_ENABLE();
	 /**I2C2 GPIO Configuration    
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    hi2c2.Instance = I2C2;
		hi2c2.Init.ClockSpeed = 100000;
		hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
		hi2c2.Init.OwnAddress1 = 0;
		hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
		hi2c2.Init.OwnAddress2 = 0;
		hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
		hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
		HAL_I2C_Init(&hi2c2);
	

	
	/* Get default values from EEPROM */
	/* EEPROM starts at 0xAA address, read 22 bytes */
	adr = BMP180_REGISTER_EEPROM;
	HAL_I2C_Master_Transmit(&hi2c2, BMP180_I2C_ADDRESS, &adr, 1, 10);
	HAL_I2C_Master_Receive(&hi2c2, BMP180_I2C_ADDRESS, data, 22, 10);
		
	/* Set configuration values */
	AC1 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC2 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC3 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC4 = (uint16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC5 = (uint16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC6 = (uint16_t)(data[i] << 8 | data[i + 1]); i += 2;
	B1 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	B2 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	MB = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	MC = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	MD = (int16_t)(data[i] << 8 | data[i + 1]);
	
	/* Initialized OK */
	lib_initialized = 1;
	
	/* Return OK */
	return TM_BMP180_Result_Ok;
}

TM_BMP180_Result_t TM_BMP180_StartTemperature(TM_BMP180_t* BMP180_Data) 
{
	uint8_t data[2];
	/* Check for library initialization */
	if (!lib_initialized) {
		return TM_BMP180_Result_LibraryNotInitialized;
	}
	/* Send to device */
	data[0] = BMP180_REGISTER_CONTROL;
	data[1] = BMP180_COMMAND_TEMPERATURE;
	HAL_I2C_Master_Transmit(&hi2c2, BMP180_I2C_ADDRESS, data, 2, 1);
	//adr = BMP180_COMMAND_TEMPERATURE;
	//HAL_I2C_Master_Transmit(&hi2c2, BMP180_I2C_ADDRESS, &adr, 1, 10);
	
	/* Set minimum delay */
	BMP180_Data->Delay = BMP180_TEMPERATURE_DELAY;
	/* Return OK */
	return TM_BMP180_Result_Ok;
}

TM_BMP180_Result_t TM_BMP180_ReadTemperature(TM_BMP180_t* BMP180_Data) {
	uint8_t data[2];
	
	/* Check for library initialization */
	if (!lib_initialized) {
		return TM_BMP180_Result_LibraryNotInitialized;
	}
	
	/* Read multi bytes from I2C */
	adr = BMP180_REGISTER_RESULT;
	HAL_I2C_Master_Transmit(&hi2c2, BMP180_I2C_ADDRESS, &adr, 1, 10);
	HAL_I2C_Master_Receive(&hi2c2, BMP180_I2C_ADDRESS, data, 2, 10);
	
	/* Get uncompensated temperature */
	UT = data[0] << 8 | data[1];
	
	/* Calculate true temperature */
	X1 = (UT - AC6) * AC5 / (float)32768;
	X2 = MC * 2048 / (X1 + MD);
	B5 = X1 + X2;
	
	/* Get temperature in degrees */
	BMP180_Data->Temperature = (B5 + 8) / ((float)160);
	
	/* Return OK */
	return TM_BMP180_Result_Ok;
}

TM_BMP180_Result_t TM_BMP180_StartPressure(TM_BMP180_t* BMP180_Data, TM_BMP180_Oversampling_t Oversampling) {
	uint8_t command;
	
	/* Check for library initialization */
	if (!lib_initialized) {
		return TM_BMP180_Result_LibraryNotInitialized;
	}
	
	switch (Oversampling) {
		case TM_BMP180_Oversampling_UltraLowPower :
			command = BMP180_COMMAND_PRESSURE_0;
			BMP180_Data->Delay = BMP180_PRESSURE_0_DELAY;
			break;
		case TM_BMP180_Oversampling_Standard:
			command = BMP180_COMMAND_PRESSURE_1;
			BMP180_Data->Delay = BMP180_PRESSURE_1_DELAY;
			break;
		case TM_BMP180_Oversampling_HighResolution:
			command = BMP180_COMMAND_PRESSURE_2;
			BMP180_Data->Delay = BMP180_PRESSURE_2_DELAY;
			break;
		case TM_BMP180_Oversampling_UltraHighResolution:
			command = BMP180_COMMAND_PRESSURE_3;
			BMP180_Data->Delay = BMP180_PRESSURE_3_DELAY;
			break;
		default:
			command = BMP180_COMMAND_PRESSURE_0;
			BMP180_Data->Delay = BMP180_PRESSURE_0_DELAY;
			break;
	}
	/* Send to device */
	adr = BMP180_REGISTER_CONTROL;
	HAL_I2C_Master_Transmit(&hi2c2, BMP180_I2C_ADDRESS, &adr, 1, 10);
	HAL_I2C_Master_Transmit(&hi2c2, BMP180_I2C_ADDRESS, &command, 1, 10);
	
	/* Save selected oversampling */
	BMP180_Data->Oversampling = Oversampling;
	/* Return OK */
	return TM_BMP180_Result_Ok;
}

TM_BMP180_Result_t TM_BMP180_ReadPressure(TM_BMP180_t* BMP180_Data) {
	uint8_t data[3];
	
	/* Check for library initialization */
	if (!lib_initialized) {
		return TM_BMP180_Result_LibraryNotInitialized;
	}
	
	/* Read multi bytes from I2C */
	adr = BMP180_REGISTER_RESULT;
	HAL_I2C_Master_Transmit(&hi2c2, BMP180_I2C_ADDRESS, &adr, 1, 10);
	HAL_I2C_Master_Receive(&hi2c2, BMP180_I2C_ADDRESS, data, 3, 10);
	
	/* Get uncompensated pressure */
	UP = (data[0] << 16 | data[1] << 8 | data[2]) >> (8 - (uint8_t)BMP180_Data->Oversampling);

	/* Calculate true pressure */
	B6 = B5 - 4000;
	X1 = (B2 * (B6 * B6 / (float)4096)) / (float)2048;
	X2 = AC2 * B6 / (float)2048;
	X3 = X1 + X2;
	B3 = (((AC1 * 4 + X3) << (uint8_t)BMP180_Data->Oversampling) + 2) / 4;
	X1 = AC3 * B6 / (float)8192;
	X2 = (B1 * (B6 * B6 / (float)4096)) / (float)65536;
	X3 = ((X1 + X2) + 2) / 4;
	B4 = AC4 * (uint32_t)(X3 + 32768) / (float)32768;
	B7 = ((uint32_t)UP - B3) * (50000 >> (uint8_t)BMP180_Data->Oversampling);
	if (B7 < 0x80000000) {
		p = (B7 * 2) / B4;
	} else {
		p = (B7 / B4) * 2;
	}
	X1 = ((float)p / (float)256) * ((float)p / (float)256);
	X1 = (X1 * 3038) / (float)65536;
	X2 = (-7357 * p) / (float)65536;
	p = p + (X1 + X2 + 3791) / (float)16;
	
	/* Save pressure */
	BMP180_Data->Pressure = p;
	
	/* Calculate altitude */
	BMP180_Data->Altitude = (float)44330.0 * (float)((float)1.0 - (float)pow((float)p / (float)101325.0, 0.19029495));
	
	/* Return OK */
	return TM_BMP180_Result_Ok;
}

uint32_t TM_BMP180_GetPressureAtSeaLevel(uint32_t pressure, float altitude) {
	return (uint32_t)((float)pressure / ((float)pow(1 - (float)altitude / (float)44330, 5.255)));
}
