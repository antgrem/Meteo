#ifndef __MY_FONT_H
#define __MY_FONT_H

//WARNING: This Font Require X-GLCD Lib.
//         You can not use it with MikroE GLCD Lib.

//Font Generated by MikroElektronika GLCD Font Creator 1.2.0.0
//MikroElektronika 2011 
//http://www.mikroe.com 

//GLCD FontName : Verdana17x16
//GLCD FontSize : 17 x 16

#include "stm32f1xx_hal.h"
#include "GUI.h"
#include "Lcd_Driver.h"

//void PutStringRus(uint8_t ix, uint8_t iy, char* iString, uint8_t iLength, uint16_t fc, uint16_t bc);
void PutString6159Rus(uint8_t ix, uint8_t iy, char* iString, uint16_t fc, uint16_t bc);	


#endif

