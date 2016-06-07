#ifndef __LCD_DRIVER_H
#define __LCD_DRIVER_H

#include "stm32f1xx_hal.h"

#define RED  	0xF800
#define GREEN	0x07E0
#define BLUE 	0x001F
#define WHITE	0xFFFF
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	//Grey 0 3165 00110 001011 00101
#define GRAY1   0x8410      	//Grey 1      00000 000000 00000
#define GRAY2   0x4208      	//Grey 2  1111111111011111

/* some RGB color definitions                                                 */
#define NAVY            0x000F      /*   0,   0, 128 */
#define DARKGREEN       0x03E0      /*   0, 128,   0 */
#define DARKCYAN				0x03EF      /*   0, 128, 128 */
#define MAROON          0x7800      /* 128,   0,   0 */
#define PURPLE          0x780F      /* 128,   0, 128 */
#define OLIVE           0x7BE0      /* 128, 128,   0 */
#define LIGHTGREY       0xC618      /* 192, 192, 192 */
#define DARKGREY        0x7BEF      /* 128, 128, 128 */
#define CYAN            0x07FF      /*   0, 255, 255 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define ORANGE          0xFD20      /* 255, 165,   0 */
#define GREENYELLOW     0xAFE5      /* 173, 255,  47 */
#define PINK            0xF81F




#define LCD_PORT_P			GPIOA
#define LCD_CTRL   	  	GPIOB		//The definition TFT data port
#define LCD_LED        	GPIO_PIN_9  //MCU_PB9--->>TFT --BL
#define LCD_RS         	GPIO_PIN_8	//PB11--->>TFT --RS/DC
#define LCD_CS        	GPIO_PIN_14  //MCU_PB14--->>TFT --CS/CE
#define LCD_RST     	GPIO_PIN_12	//PB10--->>TFT --RST
#define LCD_SCL        	GPIO_PIN_13	//PB13--->>TFT --SCL/SCK
#define LCD_SDA        	GPIO_PIN_15	//PB15 MOSI--->>TFT --SDA/DIN


//#define LCD_CS_SET(x) LCD_CTRL->ODR=(LCD_CTRL->ODR&~LCD_CS)|(x ? LCD_CS:0)

//LCD control port is set manipulation statements macro definition
#define	LCD_CS_SET  	LCD_CTRL->BSRR=LCD_CS    
#define	LCD_RS_SET  	LCD_PORT_P->BSRR=LCD_RS    
#define	LCD_SDA_SET  	LCD_CTRL->BSRR=LCD_SDA    
#define	LCD_SCL_SET  	LCD_CTRL->BSRR=LCD_SCL    
#define	LCD_RST_SET  	LCD_CTRL->BSRR=LCD_RST    
#define	LCD_LED_SET  	LCD_PORT_P->BSRR=LCD_LED   

//LCD control port is set to 0 manipulation statements macro definition
#define	LCD_CS_CLR  	LCD_CTRL->BRR=LCD_CS    
#define	LCD_RS_CLR  	LCD_PORT_P->BRR=LCD_RS    
#define	LCD_SDA_CLR  	LCD_CTRL->BRR=LCD_SDA    
#define	LCD_SCL_CLR  	LCD_CTRL->BRR=LCD_SCL    
#define	LCD_RST_CLR  	LCD_CTRL->BRR=LCD_RST    
#define	LCD_LED_CLR  	LCD_PORT_P->BRR=LCD_LED 


#define LCD_DATAOUT(x) LCD_DATA->ODR=x; //Data Output
#define LCD_DATAIN     LCD_DATA->IDR;   //Data Input

#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
LCD_DATAOUT(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 



void LCD_GPIO_Init(void);
void Lcd_WriteIndex(uint8_t Index);
void Lcd_WriteData(uint8_t Data);
void Lcd_WriteReg(uint8_t Index,uint8_t Data);
uint16_t Lcd_ReadReg(uint8_t LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(uint16_t Color);
void Lcd_SetXY(uint16_t x,uint16_t y);
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data);
unsigned int Lcd_ReadPoint(uint16_t x,uint16_t y);
void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end);
void LCD_WriteData_16Bit(uint16_t Data);

#endif
