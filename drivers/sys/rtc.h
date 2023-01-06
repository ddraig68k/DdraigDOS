#ifndef DDRAIG_OS_RTC_DRIVER_H
#define DDRAIG_OS_RTC_DRIVER_H

#include "ddraig.h"


#define RTC_READ(x)         (*((volatile uint8_t *) RTC_BASE + (x) ))
#define RTC_WRITE(x, y)     (*((uint8_t *) RTC_BASE + (x) ) = (y))

#define RTC_SECOND1         0
#define RTC_SECOND10        2
#define RTC_MINUTE1         4
#define RTC_MINUTE10        6
#define RTC_HOUR1           8
#define RTC_HOUR10          10
#define RTC_DAY1            12
#define RTC_DAY10           14
#define RTC_MONTH1          16
#define RTC_MONTH10         18
#define RTC_YEAR1           20
#define RTC_YEAR10          22
#define RTC_DOW             24 /* Day of week */
#define RTC_CONTROL_D       26 /* Control register D */
#define RTC_CONTROL_E       28 /* Control register E */
#define RTC_CONTROL_F       30 /* Control register F */

typedef struct
{
  int	tm_sec;
  int	tm_min;
  int	tm_hour;
  int	tm_day;
  int	tm_mon;
  int	tm_year;
  int	tm_dow;
} rtc_date_t;


void rtc_init(void);
void rtc_get_time(rtc_date_t *td);
void rtc_set_time(const rtc_date_t *td);
void rtc_print_time(void);

#endif
