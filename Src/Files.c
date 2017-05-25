#include "Files.h"
#include "stdio.h"

	FRESULT result;
	FATFS FATFS_Obj;
	FIL file;
	FSIZE_t offset, offset_total;
	char buff[100];
	
  extern RTC_HandleTypeDef hrtc;

void Init_offset(void)
{
	offset = (FSIZE_t)HAL_RTCEx_BKUPRead(&hrtc, BKUP_OFFSET_ADRR);
	offset_total = (FSIZE_t)HAL_RTCEx_BKUPRead(&hrtc, BKUP_OFFSET_TOTAL_ADRR);
}


// Store one data in file
FRESULT Write_file(Messure_DataTypeDef *Data)
{ 
	int16_t		count, lb;
	uint8_t try_mount=0;
	int16_t byte_offset, i;

	result = f_mount(&FATFS_Obj, "0:", 1);
	while ((result != FR_OK) && (try_mount != 10))
	{
		result = f_mount(&FATFS_Obj, "0:", 1);
		delay_ms(100);
		try_mount++;
	}
	
	if (result == FR_OK) 
		{
			result = f_open(&file, str_file_name, FA_READ | FA_WRITE);
			if (result == FR_OK)
				{
						result = f_lseek(&file, offset);
								
						sprintf(buff, "%02d:%02d:%02d\t%.1f\t%.1f\t%.0f\t\n", Data->Time.Hours, Data->Time.Minutes, \
									Data->Time.Seconds, (float) (Data->T_in/10), (float) (Data->T_out/10), Data->Pressure_p);

						lb = 0;
						while (buff[lb] != '\0')
						{
							lb++;
						}
						
						byte_offset = offset % 512;
						if ((byte_offset + lb) >= 512)
						{
							for (i=0; i <= (512 - byte_offset); i++)
								f_putc(buff[i], &file);
							offset = f_tell(&file);
							f_close(&file);	
							delay_ms(100);
							
							result = f_open(&file, str_file_name, FA_READ | FA_WRITE);
							if (result == FR_OK)
								{
										result = f_lseek(&file, offset);
									for(i = (512 - byte_offset); i < lb; i++)
										f_putc(buff[i], &file);
									offset = f_tell(&file);
								}
						}//if
						else
						{
							count = 0;
							while(buff[count] != '\0')
							{
								f_putc(buff[count], &file);
								count++;
							}
						}//else							
																	
						offset = f_tell(&file);
						HAL_RTCEx_BKUPWrite(&hrtc, BKUP_OFFSET_ADRR, offset);
							
					f_close(&file);	
				}

		// write to all_data file
			result = f_open(&file, str_file_year_name, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
			if (result == FR_OK)
				{
						result = f_lseek(&file, offset_total);
											
						sprintf(buff, "%04d.%02d.%02d\t%02d:%02d:%02d\t%.1f\t%.1f\t%.0f\t\n", Data->Day.tm_year,\
								Data->Day.tm_mon, Data->Day.tm_mday, Data->Time.Hours,\
								Data->Time.Minutes, Data->Time.Seconds, (float) (Data->T_in/10), \
								(float) (Data->T_out/10), Data->Pressure_p);

						lb = 0;
						while (buff[lb] != '\0')
						{
							lb++;
						}
						
						byte_offset = offset_total % 512;
						if ((byte_offset + lb) >= 512)
						{
							for (i=0; i <= (512 - byte_offset); i++)
								f_putc(buff[i], &file);
							offset_total = f_tell(&file);
							f_close(&file);	
							delay_ms(100);
							
							result = f_open(&file, str_file_year_name, FA_READ | FA_WRITE);
							if (result == FR_OK)
								{
									result = f_lseek(&file, offset_total);
									for(i = (512 - byte_offset); i < lb; i++)
										f_putc(buff[i], &file);
								}
						}//if
						else
						{
							count = 0;
							while(buff[count] != '\0')
							{
								f_putc(buff[count], &file);
								count++;
							}
						}//else

						offset_total = f_tell(&file);
						HAL_RTCEx_BKUPWrite(&hrtc, BKUP_OFFSET_TOTAL_ADRR, offset_total);
												
					f_close(&file);	
				}
					
						/* Unmount drive, don't forget this! */
						f_mount(0, "0:", 1);
			}//end mount SD
			
		return result;
}


// create new file in midnight
FRESULT Create_new_file(void)
{ 
	int16_t count_store_data=0;
	time_t time_temp;
	struct tm* time_tm_temp;
	
	uint8_t try_mount, try_expand;
	
	try_mount = 0;
	result = f_mount(&FATFS_Obj, "0:", 1);
	while ((result != FR_OK) && (try_mount++ != 10))
	{
		result = f_mount(&FATFS_Obj, "0:", 1);
		delay_ms(200);
	}
	
	if (result == FR_OK) 
		{
				
		time_temp = ReadTimeCounter(&hrtc);
		time_tm_temp = localtime(&time_temp);
			
			sprintf(str_file_name, "%04d%02d%02d.txt", time_tm_temp->tm_year+1900, time_tm_temp->tm_mon + 1, time_tm_temp->tm_mday);
			
			
			result = f_open(&file, str_file_name, FA_CREATE_NEW | FA_READ | FA_WRITE);
			if (result == FR_OK)
				{//write redline
					try_expand = 0;
					result = f_expand(&file, 5000, 1);
					while((result != FR_OK) && (try_expand++ !=10))
						{
							result = f_expand(&file, 20000, 1);
							delay_ms(200);
						}
					
					if (result)
					{
						f_close(&file);
						while(1);
					}
					f_close(&file);
				
					
			result = f_open(&file, str_file_name, FA_WRITE | FA_READ);
			if (result == FR_OK)
			{
					sprintf(buff, "Time\tT_in\tT_out\tPresure\t\n");
					
					/* If we put more than 0 characters (everything OK) */
					count_store_data = f_puts(buff, &file);
						if (count_store_data > 0) 
							{
							  offset = f_tell(&file);
								HAL_RTCEx_BKUPWrite(&hrtc, BKUP_OFFSET_ADRR, offset);
							}//data were stored, but what to do I don't know
																			
					f_close(&file);	
				}			
			}

				sprintf(str_file_year_name, "%04dyear.txt", time_tm_temp->tm_year+1900);
				result = f_open(&file, str_file_year_name, FA_CREATE_NEW | FA_READ | FA_WRITE);
				if (result == FR_OK)
					{//write redline
						try_expand = 0;
						result = f_expand(&file, 2000000, 1);
						while((result != FR_OK) && (try_expand++ !=10))
							{
								result = f_expand(&file, 2000000, 1);
								delay_ms(100);
							}
						f_close(&file);
				
				result = f_open(&file, str_file_year_name, FA_READ | FA_WRITE);
				if (result == FR_OK)
				{
						sprintf(buff, "Data\tTime\tT_in\tT_out\tPresure\t\n");
					
						/* If we put more than 0 characters (everything OK) */
						count_store_data = f_puts(buff, &file);
							if (count_store_data > 0) 
								{
									offset_total = f_tell(&file);
									HAL_RTCEx_BKUPWrite(&hrtc, BKUP_OFFSET_TOTAL_ADRR, offset_total);
								}//data were stored, but what to do I don't know
																				
						f_close(&file);	
					}		
				}
						/* Unmount drive, don't forget this! */
						f_mount(0, "0:", 1);
			}//end mount SD
		
		return result;
}

