#ifndef _RTC_MY_H
#define _RTC_MY_H

#include "stm32f1xx_hal.h"
#include <time.h>

/**
 * @brief  RTC Struct for date/time
 */
typedef struct {
	uint8_t seconds;     /*!< Seconds parameter, from 00 to 59 */
	uint16_t subseconds; /*!< Subsecond downcounter. When it reaches zero, it's reload value is the same as
                                 @ref RTC_SYNC_PREDIV, so in our case 0x3FF = 1023, 1024 steps in one second */
	uint8_t minutes;     /*!< Minutes parameter, from 00 to 59 */
	uint8_t hours;       /*!< Hours parameter, 24Hour mode, 00 to 23 */
	uint8_t day;         /*!< Day in a week, from 1 to 7 */
	uint8_t date;        /*!< Date in a month, 1 to 31 */
	uint8_t month;       /*!< Month in a year, 1 to 12 */
	uint8_t year;        /*!< Year parameter, 00 to 99, 00 is 2000 and 99 is 2099 */
	uint32_t unix;       /*!< Seconds from 01.01.1970 00:00:00 */
} RTC_t;

typedef struct {
	uint8_t seconds;              /*!< Alarm seconds value */
	uint8_t minutes;              /*!< Alarm minutes value */
	uint8_t hours;                /*!< Alarm hours value */
	uint8_t day;                  /*!< Alarm day value. If you select trigger for alarm every week, then this parameter has value between
                                          1 and 7, representing days in a week, Monday to Sunday
                                          If you select trigger for alarm every month, then this parameter has value between
                                          1 - 31, representing days in a month. */
} RTC_AlarmTime_t;

void RTC_my_Init(void);
void RTC_AlarmIRQ_function(void);
void RTC_IRQ_function(void);
	
#endif
