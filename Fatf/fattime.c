/* Martin Thomas 4/2009 */

#include "integer.h"
#include "fattime.h"
#include <time.h>
#include "main.h"
//#include "rtc.h"

extern RTC_HandleTypeDef hrtc;

DWORD get_fattime (void)
{
	DWORD res;
	time_t time_temp;
	struct tm* time_tm_temp;
	
//	res = 0;
//	RTC_t rtc;

		time_temp = 1456329855;//RTC_GetCounter();
//		time_temp = ReadTimeCounter(&hrtc);
		time_tm_temp = localtime(&time_temp);
	
	res =  (((DWORD)time_tm_temp->tm_year - 2000) << 25)
			| ((DWORD)(time_tm_temp->tm_mon+1) << 21)
			| ((DWORD)time_tm_temp->tm_mday << 16)
			| (WORD)(time_tm_temp->tm_hour << 11)
			| (WORD)(time_tm_temp->tm_min << 5)
			| (WORD)(time_tm_temp->tm_sec >> 1);

	return res;
}

