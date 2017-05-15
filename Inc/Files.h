#ifndef __FILES_H
#define __FILES_H


#include "main.h"

#define BKUP_OFFSET_ADRR 2
#define BKUP_OFFSET_TOTAL_ADRR 3

extern Messure_DataTypeDef All_data;
extern Messure_DataTypeDef Day_data_Array[DAY_DATA_ARRAY_LENGTH];
extern char str_file_name[20];
extern char str_file_year_name[20];

void Init_offset(void);
FRESULT Create_new_file(void);
FRESULT Write_file(uint8_t Write_count);
FRESULT Write_file_one(Messure_DataTypeDef *Data);



#endif
