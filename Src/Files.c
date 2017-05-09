#include "Files.h"
#include "stdio.h"

	FRESULT result;
	FATFS FATFS_Obj;
	FIL file;
	FSIZE_t offset;
	char buff[100];
	
extern RTC_HandleTypeDef hrtc;

// Store data in file
FRESULT Write_file(uint8_t Write_count)
{ 
	uint8_t ij;
	int16_t		count, lb;
	uint8_t try_mount=0, rewrite;

	result = f_mount(&FATFS_Obj, "0:", 1);
	while ((result != FR_OK) && (try_mount != 10))
	{
		result = f_mount(&FATFS_Obj, "0:", 1);
		delay_ms(200);
		try_mount++;
	}
	
	if (result == FR_OK) 
		{
			result = f_open(&file, str_file_name, FA_READ | FA_WRITE);
			if (result == FR_OK)
				{
					
						result = f_lseek(&file, offset);
								
						for (ij=0; ij < Write_count; ij++)
							{
								
								sprintf(buff, "%02d:%02d:%02d\t%.1f\t%.1f\t%.0f\t\n", Day_data_Array[ij].Time.Hours, Day_data_Array[ij].Time.Minutes, Day_data_Array[ij].Time.Seconds, (float) (Day_data_Array[ij].T_in/10), (float) (Day_data_Array[ij].T_out/10), Day_data_Array[ij].Pressure_p);
								lb = 0;
								while (buff[lb] != '\0')
									lb++;
								
								count = f_puts(buff, &file);
								if (count != lb)
								{// was written not all information
									rewrite = 10;
									while((count != lb) && (rewrite != 0))
									{
										result = f_lseek(&file, offset);//return to start position
										delay_ms(10);
										count = f_puts(buff, &file);
										rewrite--;
									}
								}
									
							}
						
						offset = f_tell(&file);
							
					f_close(&file);	
				}

//		// write to all_data file
//			result = f_open(&file, str_file_year_name, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
//			if (result == FR_OK)
//				{
//					result = f_lseek(&file, f_size(&file));
//					if(result == FR_OK)
//						{}//go to end of file
//								
//						for (ij=0; ij < Write_count; ij++)
//							{
//								
//								sprintf(buff, "%04d.%02d.%02d\t%02d:%02d:%02d\t%d\t%f\t\n", Day_data_Array[ij].Day.tm_year, Day_data_Array[ij].Day.tm_mon, Day_data_Array[ij].Day.tm_mday, Day_data_Array[ij].Time.Hours, Day_data_Array[ij].Time.Minutes, Day_data_Array[ij].Time.Seconds, Day_data_Array[ij].T_in, Day_data_Array[ij].Pressure_p);
//								
//								f_puts(buff, &file);
//							}
//							
//					f_close(&file);	
//				}
					
						/* Unmount drive, don't forget this! */
						f_mount(0, "0:", 1);
			}//end mount SD
			
		return result;
}

// Store one data in file
FRESULT Write_file_one(Messure_DataTypeDef *Data)
{ 
	int16_t		count, lb;
	uint8_t try_mount=0, rewrite;
	UINT count_written_data;

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
									lb++;
								if (((offset%512)+lb)>512)
								{
									rewrite = (512-(offset%512));
									f_write(&file, buff, rewrite, &count_written_data);
									f_write(&file, buff+rewrite, lb-rewrite, &count_written_data);
								}
								else 
									f_write(&file, buff, lb, &count_written_data);
									
								//count = f_puts(buff, &file);
//								if (count != lb)
//								{// was written not all information
//									rewrite = 10;
//									while((count != lb) && (rewrite != 0))
//									{
//										result = f_lseek(&file, offset);//return to start position
//										delay_ms(10);
//										count = f_puts(buff, &file);
//										rewrite--;
//									}
//								}
									
						offset = f_tell(&file);
							
					f_close(&file);	
				}

//		// write to all_data file
//			result = f_open(&file, str_file_year_name, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
//			if (result == FR_OK)
//				{
//					result = f_lseek(&file, f_size(&file));
//					if(result == FR_OK)
//						{}//go to end of file
//								
//						for (ij=0; ij < Write_count; ij++)
//							{
//								
//								sprintf(buff, "%04d.%02d.%02d\t%02d:%02d:%02d\t%d\t%f\t\n", Day_data_Array[ij].Day.tm_year, Day_data_Array[ij].Day.tm_mon, Day_data_Array[ij].Day.tm_mday, Day_data_Array[ij].Time.Hours, Day_data_Array[ij].Time.Minutes, Day_data_Array[ij].Time.Seconds, Day_data_Array[ij].T_in, Day_data_Array[ij].Pressure_p);
//								
//								f_puts(buff, &file);
//							}
//							
//					f_close(&file);	
//				}
					
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
					result = f_expand(&file, 10000, 1);
					while((result != FR_OK) && (try_expand++ !=10))
						{
							result = f_expand(&file, 10000, 1);
							delay_ms(200);
						}
					
					if (result)
					{
						f_close(&file);
						while(1);
					}
					f_close(&file);
				}
					
			result = f_open(&file, str_file_name, FA_WRITE | FA_READ);
			if (result == FR_OK)
			{
					sprintf(buff, "Time\tT_in\tT_out\tPresure\t\n");
					
					/* If we put more than 0 characters (everything OK) */
					count_store_data = f_puts(buff, &file);
						if (count_store_data > 0) 
							{
							  offset = f_tell(&file);
							}//data were stored, but what to do I don't know
																			
					f_close(&file);	
				}			
			

//				sprintf(str_file_year_name, "%04dyear.txt", time_tm_temp->tm_year+1900);
//				result = f_open(&file, str_file_year_name, FA_CREATE_NEW | FA_READ | FA_WRITE);
//				if (result == FR_OK)
//					{//write redline
//						
//						sprintf(buff, "Data\tTime\tT_in\tT_out\tPresure\t\n");
//						
//						/* If we put more than 0 characters (everything OK) */
//						count_store_data = f_puts(buff, &file);
//							if (count_store_data > 0) 
//								{}//data were stored, but what to do I don't know
//																				
//						f_close(&file);	
//					}		
//					
						/* Unmount drive, don't forget this! */
						f_mount(0, "0:", 1);
			}//end mount SD
		
		return result;
}

