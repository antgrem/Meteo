/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "BMP085.h"
#include "LM75_hal.h"
#include "delay.h"
#include "BMP_graph.h"
#include "Lcd_Driver.h"
#include "GUI.h"

// array for pressure and temperature
float pressure[ARRAY_GRAPH];
int16_t temperature[ARRAY_GRAPH];

float MAX_pressure, MIN_pressure;
int16_t MAX_temperature, MIN_temperature;

float old_pressure;
int16_t old_temprature;

int16_t tempr_data;

uint8_t graph_i=0;
uint8_t changes_min_max_tempr = 0, changes_min_max_press = 0;

extern float p, t, a;


void Draw_graph (void)
{
	      
//take pressure data
		BMP085_setControl(BMP085_MODE_TEMPERATURE);
		delay_ms(BMP085_getMeasureDelayMilliseconds(BMP085_MODE_TEMPERATURE));
		t = BMP085_getTemperatureC();

		BMP085_setControl(BMP085_MODE_PRESSURE_3);
		delay_ms(BMP085_getMeasureDelayMilliseconds(BMP085_MODE_PRESSURE_3));
		old_pressure = pressure[graph_i];
		pressure[graph_i] = BMP085_getPressure()/1000;

		LM75_Temperature_ex(&tempr_data);
		old_temprature = temperature[graph_i];
		temperature[graph_i] = tempr_data;
	
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
		Gui_DrawLine(graph_i + 5, temperature[graph_i+1], graph_i + 4, old_temprature, LIGHTGREY);	
			
		//draw new dot on graph
		Gui_DrawPoint(graph_i + 4, pressure[graph_i], BLACK);
		//Gui_DrawPoint(graph_i + 4, temperature[graph_i], BLACK);
		Gui_DrawLine(graph_i + 3, temperature[graph_i-1], graph_i + 4, temperature[graph_i], BLACK);
		}
		else 
			if (graph_i == ARRAY_GRAPH - 1)
			{ // last point 
				//clear dot and draw line
				Gui_DrawPoint(graph_i + 4, old_temprature, LIGHTGREY);
				Gui_DrawLine(graph_i + 3, temperature[graph_i-1], graph_i + 4, temperature[graph_i], BLACK);
			}
			else 
			{//fraph_i == 0
				// clear line and draw dot
				Gui_DrawLine(graph_i + 5, temperature[graph_i+1], graph_i + 4, old_temprature, LIGHTGREY);
				
				Gui_DrawPoint(graph_i + 4, temperature[graph_i], BLACK);
			}
		
		
	
		
		graph_i++;
		if (graph_i == ARRAY_GRAPH) graph_i = 0;
}
