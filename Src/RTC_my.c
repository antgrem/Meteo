//File with RTC init and working function
#include "RTC_my.h"

///* Internal RTC defines */
//#define RTC_LEAP_YEAR(year) 			((((year) % 4 == 0) && ((year) % 100 != 0)) || ((year) % 400 == 0))
//#define RTC_DAYS_IN_YEAR(x)			RTC_LEAP_YEAR(x) ? 366 : 365
//#define RTC_OFFSET_YEAR				1970
//#define RTC_SECONDS_PER_DAY			86400
//#define RTC_SECONDS_PER_HOUR			3600
//#define RTC_SECONDS_PER_MINUTE		60
//#define RTC_BCD2BIN(x)				((((x) >> 4) & 0x0F) * 10 + ((x) & 0x0F))
//#define RTC_CHAR2NUM(x)				((x) - '0')
//#define RTC_CHARISNUM(x)				((x) >= '0' && (x) <= '9')

///* Days in a month */
//uint8_t RTC_Months[2][12] = {
//	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},	/* Not leap year */
//	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}	/* Leap year */
//};

void Set_default_RTC(void);


extern char i_count, flag_get_data;
extern uint16_t count_led_control, stage_led_control;

struct tm time_tm;
time_t time_t_32;

/* RTC init function */
void RTC_my_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	uint32_t temp32;
	
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);
	
	temp32 = BKP_ReadBackupRegister(BKP_DR1);
	if ((temp32 & 0x01) == 0x00)
		{// RTC not configure
			Set_default_RTC();
			BKP_WriteBackupRegister(BKP_DR1, 0x01); // set mask "RTC_now_work"
		}

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


//void GetDateTimeFromUnix(RTC_t* data, uint32_t unix) {
//	uint16_t year;
//	
//	/* Store unix time to unix in struct */
//	data->unix = unix;
//	/* Get seconds from unix */
//	data->seconds = unix % 60;
//	/* Go to minutes */
//	unix /= 60;
//	/* Get minutes */
//	data->minutes = unix % 60;
//	/* Go to hours */
//	unix /= 60;
//	/* Get hours */
//	data->hours = unix % 24;
//	/* Go to days */
//	unix /= 24;
//	
//	/* Get week day */
//	/* Monday is day one */
//	data->day = (unix + 3) % 7 + 1;

//	/* Get year */
//	year = 1970;
//	while (1) {
//		if (RTC_LEAP_YEAR(year)) {
//			if (unix >= 366) {
//				unix -= 366;
//			} else {
//				break;
//			}
//		} else if (unix >= 365) {
//			unix -= 365;
//		} else {
//			break;
//		}
//		year++;
//	}
//	/* Get year in xx format */
//	data->year = (uint8_t) (year - 2000);
//	/* Get month */
//	for (data->month = 0; data->month < 12; data->month++) {
//		if (RTC_LEAP_YEAR(year) && unix >= (uint32_t)RTC_Months[1][data->month]) {
//			unix -= RTC_Months[1][data->month];
//		} else if (unix >= (uint32_t)RTC_Months[0][data->month]) {
//			unix -= RTC_Months[0][data->month];
//		} else {
//			break;
//		}
//	}
//	/* Get month */
//	/* Month starts with 1 */
//	data->month++;
//	/* Get date */
//	/* Date starts with 1 */
//	data->date = unix + 1;
//}


void Set_default_RTC(void)
{
	
	time_tm.tm_hour = 18;
	time_tm.tm_min = 35;
	time_tm.tm_sec = 0;
	time_tm.tm_mon = 11-1;
	time_tm.tm_mday = 21;
	time_tm.tm_year = 115;
	
	time_t_32 = mktime(&time_tm);
	

  /* Enable the LSI OSC */
  RCC_LSICmd(ENABLE);
  /* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(40000);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
	
	RTC_SetCounter(time_t_32);
	
	/* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
	
	time_tm.tm_hour = 18;
	time_tm.tm_min = 36;
	time_tm.tm_sec = 0;
	time_tm.tm_mon = 11-1;
	time_tm.tm_mday = 21;
	time_tm.tm_year = 115;
	
	time_t_32 = mktime(&time_tm);
	
	RTC_SetAlarm(time_t_32);
	
	/* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_ALR, ENABLE);

  /* To output second signal on Tamper pin, the tamper functionality
       must be disabled (by default this functionality is disabled) */
  BKP_TamperPinCmd(DISABLE);

  /* Enable the RTC Second Output on Tamper Pin */
  BKP_RTCOutputConfig(BKP_RTCOutputSource_Second);
	
	
}

void RTC_AlarmIRQ_function(void)
{
	time_t time_temp;
	
	stage_led_control = TWO_RED_FLASH;
	time_temp = RTC_GetCounter();
	time_temp += 60; //add 5 minutes
	RTC_SetAlarm(time_temp);
}




void RTC_IRQ_function(void)
{
//	Red_Toggle();
//	Green_Toggle();
//	flag_get_data = 1;
}



