/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "BMP085.h"
#include "LM75_hal.h"
#include "delay.h"
#include "BMP_graph.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "My_font.h"
#include "main.h"

// array for pressure and temperature
float pressure[ARRAY_GRAPH];
int16_t temperature[ARRAY_GRAPH];

float Average_pressure = 0;
int16_t Averaga_temperature = 0;

float MAX_pressure, MIN_pressure;
int16_t MAX_temperature, MIN_temperature;

float old_pressure;
int16_t old_temprature;


int16_t temp_t=150;// half of full scale

uint8_t graph_i=0;
uint8_t changes_min_max_tempr = 0, changes_min_max_press = 0;

extern float p, t, a;
extern void (* pfunction) (void);
extern Messure_DataTypeDef All_data;


void First_Draw_Graph(void)
{//clear screen
		uint8_t i;

	//draw lines and fill dot from array
	Lcd_Clear(LIGHTGREY);
	Gui_DrawLine(0, 0, 128, 0, DARKGREY);
	Gui_DrawLine(0, 1, 128, 1, DARKGREY);
	
	Gui_DrawLine(0, 62, 128, 62, DARKGREY);
	Gui_DrawLine(0, 63, 128, 63, DARKGREY);
	Gui_DrawLine(0, 64, 128, 64, DARKGREY);
	Gui_DrawLine(0, 65, 128, 65, DARKGREY);
	
	Gui_DrawLine(0, 127, 128, 127, DARKGREY);
	Gui_DrawLine(0, 128, 128, 128, DARKGREY);
	
	Gui_DrawLine(0, 0, 0, 128, DARKGREY);
	Gui_DrawLine(1, 0, 1, 128, DARKGREY);
	
	Gui_DrawLine(127, 0, 127, 128, DARKGREY);
	Gui_DrawLine(128, 0, 128, 128, DARKGREY);	
	
			for (i=1; i<graph_i; i++)
				{
					//draw new dot on graph
					Gui_DrawPoint(i + 4, pressure[i], BLACK);
					Gui_DrawLine(i + 3, (60 - ((temperature[i-1]*2)/10))+2, i + 4, (60 - ((temperature[i]*2)/10))+2, BLACK);
				}
	
	pfunction = Draw_graph;
	
}


void Draw_graph (void)
{
   
//take pressure data
	
		old_pressure = pressure[graph_i];
		pressure[graph_i] = Average_pressure;
	
		old_temprature = temperature[graph_i];
		temperature[graph_i] = Averaga_temperature;
	
		if (graph_i == 0)
		{// prepare data for find min and max value
			MAX_pressure = pressure[graph_i];
			MIN_pressure = pressure[graph_i];
			
			MAX_temperature = temperature[graph_i];
			MIN_temperature = temperature[graph_i];
		}
		else 
		{
			if (MAX_pressure < pressure[graph_i])
			{
				MAX_pressure = pressure[graph_i];
				changes_min_max_press = 1;
			}
			
			if (MIN_pressure > pressure[graph_i])
			{
				MIN_pressure = pressure[graph_i];
				changes_min_max_press = 1;
			}
			
			if (MAX_temperature < temperature[graph_i])
			{
				MAX_temperature = temperature[graph_i];
				changes_min_max_tempr = 1;
			}
			
			if (MIN_temperature > temperature[graph_i])
			{
				MIN_temperature = temperature[graph_i];
				changes_min_max_tempr = 1;
			}
		}
		
		
		
		if ((graph_i != 0) && (graph_i != (ARRAY_GRAPH - 1)))
		{
		//clear old dot
		Gui_DrawPoint(graph_i + 4, old_pressure, LIGHTGREY);
		//Gui_DrawPoint(graph_i + 4, old_temprature, LIGHTGREY);
		Gui_DrawLine(graph_i + 5, (60 - ((temperature[graph_i+1]*2)/10))+2, graph_i + 4, (60 - ((old_temprature*2)/10))+2, LIGHTGREY);	
			
		//draw new dot on graph
		Gui_DrawPoint(graph_i + 4, pressure[graph_i], BLACK);
		//Gui_DrawPoint(graph_i + 4, temperature[graph_i], BLACK);
		Gui_DrawLine(graph_i + 3, (60 - ((temperature[graph_i-1]*2)/10))+2, graph_i + 4, (60 - ((temperature[graph_i]*2)/10))+2, BLACK);
			
		Gui_DrawLine(3, (60 - ((temp_t*2)/10))+2, 123, (60 - ((temp_t*2)/10))+2, RED);	
		}
		else 
			if (graph_i == ARRAY_GRAPH - 1)
			{ // last point 
				//clear dot and draw line
				//Gui_DrawPoint(graph_i + 4, old_temprature, LIGHTGREY);
				Gui_DrawLine(graph_i + 3, (60 - ((temperature[graph_i-1]*2)/10))+2, graph_i + 4, (60 - ((temperature[graph_i]*2)/10))+2, BLACK);
			}
			else 
			{//fraph_i == 0
				// clear line and draw dot
				Gui_DrawLine(graph_i + 5, (60 - ((temperature[graph_i+1]*2)/10))+2, graph_i + 4, (60 - ((old_temprature*2)/10))+2, LIGHTGREY);
				
				//Gui_DrawPoint(graph_i + 4, temperature[graph_i], BLACK);
			}
			
			
//		if ((graph_i != 0) && (graph_i != (ARRAY_GRAPH - 1)))
//		{
//		//clear old dot
//		Gui_DrawPoint(graph_i + 4, old_pressure, LIGHTGREY);
//		//Gui_DrawPoint(graph_i + 4, old_temprature, LIGHTGREY);
//		Gui_DrawLine(graph_i + 5, temperature[graph_i+1], graph_i + 4, old_temprature, LIGHTGREY);	
//			
//		//draw new dot on graph
//		Gui_DrawPoint(graph_i + 4, pressure[graph_i], BLACK);
//		//Gui_DrawPoint(graph_i + 4, temperature[graph_i], BLACK);
//		Gui_DrawLine(graph_i + 3, temperature[graph_i-1], graph_i + 4, temperature[graph_i], BLACK);
//			
//		Gui_DrawLine(3, (60 - ((temp_t*2)/10))+2, 123, (60 - ((temp_t*2)/10))+2, RED);	
//		}
//		else 
//			if (graph_i == ARRAY_GRAPH - 1)
//			{ // last point 
//				//clear dot and draw line
//				Gui_DrawPoint(graph_i + 4, old_temprature, LIGHTGREY);
//				Gui_DrawLine(graph_i + 3, temperature[graph_i-1], graph_i + 4, temperature[graph_i], BLACK);
//			}
//			else 
//			{//fraph_i == 0
//				// clear line and draw dot
//				Gui_DrawLine(graph_i + 5, temperature[graph_i+1], graph_i + 4, old_temprature, LIGHTGREY);
//				
//				Gui_DrawPoint(graph_i + 4, temperature[graph_i], BLACK);
//			}
	
		graph_i++;
		if (graph_i == ARRAY_GRAPH) graph_i = 0;
}
