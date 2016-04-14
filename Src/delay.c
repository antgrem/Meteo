#include "stm32f1xx_hal.h"
#include "delay.h" 

static uint8_t  fac_us=0;//us Delay times multiplier
static uint16_t fac_ms=0;//ms Delay times multiplier

// Initialization delay function
// SYSTICK clock HCLK clock is fixed at 1/8
// SYSCLK: System Clock
void delay_init(uint8_t SYSCLK)
{
	SysTick->CTRL&=0xfffffffb;//bit2清空,选择外部时钟  HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(uint16_t)fac_us*1000;
}		
						    
// Delay nms
// Note nms range
// SysTick-> LOAD for the 24-bit register, so that the maximum delay is:
// Nms <= 0xffffff * 8 * 1000 / SYSCLK
// SYSCLK unit is Hz, nms in ms
// The conditions under 72M, nms <= 1864
void delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;		   
	SysTick->LOAD=(uint32_t)nms*fac_ms;//Loading time (SysTick->LOAD for 24bit)
	SysTick->VAL =0x00;           //Empty counter
	SysTick->CTRL=0x01 ;          //Start the countdown  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//Wait time is reached   
	SysTick->CTRL=0x00;       //Close Counter
	SysTick->VAL =0X00;       //Empty counter	  	    
}   

// Delay nus
// Nus the number of us to delay.	    								   
void delay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*fac_us; //Loading time	  		 
	SysTick->VAL=0x00;        //Empty counter
	SysTick->CTRL=0x01 ;      //Start the countdown
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //Close Counter
	SysTick->VAL =0X00;       //Empty counter	 
}




































