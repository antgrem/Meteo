#include "Terminal.h"

uint8_t string_array_max=4;
char *string_array[] = {"First line", "+-*/", "Home", "HTML code", "Just English",
									"Very long line", "flash","light", "Happy", "dOREWD","qwer"};

									
									
void Terminal_shift_line (void)
{
	uint8_t i,j, a;
	
	while(1)
	{
		Lcd_Clear(BLACK);
		string_array[1] = "Hello";
	for(i=0; i<11; i++)
		{
			for (j=0; j<11; j++)
				{
					a = (i+j)%11;
					PutStringRus11(0,j*11+2,string_array[a],GREEN,BLACK);
				
				}
		
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		Lcd_Clear(BLACK);
		}
	}
	
}
