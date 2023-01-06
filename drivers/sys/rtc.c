#include "rtc.h"
#include "printf.h"

void rtc_init(void)
{
    RTC_WRITE(RTC_CONTROL_F, 4);
    RTC_WRITE(RTC_CONTROL_E, 0);
    RTC_WRITE(RTC_CONTROL_D, 4);
}

static void rtc_delay(unsigned long d)
{
    volatile unsigned long wait = d;
    while (wait--);
}

void rtc_wait_busy(void)
{
    RTC_WRITE(RTC_CONTROL_D, 1); // No interrupts, HOLD Bit=1

    uint8_t status = RTC_READ(RTC_CONTROL_D);

    while (status & 2)
    {
        RTC_WRITE(RTC_CONTROL_D, 0); // No interrupts, HOLD Bit=0
        rtc_delay(10);
        RTC_WRITE(RTC_CONTROL_D, 1); // No interrupts, HOLD Bit=1
        status = RTC_READ(RTC_CONTROL_D);
    }
    RTC_WRITE(RTC_CONTROL_D, 0);
}

uint8_t read_rtc_register(uint32_t reg)
{
    RTC_WRITE(RTC_CONTROL_D, 1); // No interrupts, HOLD Bit=1
    uint8_t status = RTC_READ(RTC_CONTROL_D);
    uint8_t data = 0;

    while (status & 2)
    {
        RTC_WRITE(RTC_CONTROL_D, 0); // No interrupts, HOLD Bit=0
        rtc_delay(10);
        RTC_WRITE(RTC_CONTROL_D, 1); // No interrupts, HOLD Bit=1
        status = RTC_READ(RTC_CONTROL_D);
    }
    data = RTC_READ(reg);
    RTC_WRITE(RTC_CONTROL_D, 0);

    return data;
}

void write_rtc_register(uint32_t reg, uint8_t data)
{
    RTC_WRITE(RTC_CONTROL_D, 1); // No interrupts, HOLD Bit=1
    uint8_t status = RTC_READ(RTC_CONTROL_D);

    while (status & 2)
    {
        RTC_WRITE(RTC_CONTROL_D, 0); // No interrupts, HOLD Bit=0
        rtc_delay(10);
        RTC_WRITE(RTC_CONTROL_D, 1); // No interrupts, HOLD Bit=1
        status = RTC_READ(RTC_CONTROL_D);
    }
    data = RTC_WRITE(reg, data);
    RTC_WRITE(RTC_CONTROL_D, 0);
}

char *get_dow(uint8_t dow)
{
    switch (dow)
    {
        case 0: return "Sunday";
        case 1: return "Monday";
        case 2: return "Tuesday";
        case 3: return "Wednesday";
        case 4: return "Thursday";
        case 5: return "Friday";
        case 6: return "Saturday";
    }
    return "Invalid DOW";
}

int dayofweek(int day,int month,int year)
{
	int adjustment, mm, yy;

	adjustment = (14 - month) / 12;
	mm = month + 12 * adjustment - 2;
	yy = year - adjustment;

	return (day + (13 * mm - 1) / 5 + yy + yy / 4 - yy / 100 + yy / 400) % 7;
}

void rtc_get_time(rtc_date_t *td)
{
	uint8_t h1 = 0, h10 = 0, mi1 = 0, mi10 = 0, s1 = 0, s10 = 0, d1 = 0, d10 = 0, mo1 = 0, mo10 = 0, y1 = 0, y10 = 0, dow = 0;

	// get current date and time from RTC
    h1    = read_rtc_register(RTC_HOUR1) & 0x0F;
    h10   = read_rtc_register(RTC_HOUR10) & 0x0F;
    mi1   = read_rtc_register(RTC_MINUTE1) & 0x0F;
    mi10  = read_rtc_register(RTC_MINUTE10) & 0x0F;
    s1    = read_rtc_register(RTC_SECOND1) & 0x0F;
    s10   = read_rtc_register(RTC_SECOND10) & 0x0F;
    d1    = read_rtc_register(RTC_DAY1) & 0x0F;
    d10   = read_rtc_register(RTC_DAY10) & 0x0F;
    mo1   = read_rtc_register(RTC_MONTH1) & 0x0F;
    mo10  = read_rtc_register(RTC_MONTH10) & 0x0F;
    y1    = read_rtc_register(RTC_YEAR1) & 0x0F;
    y10   = read_rtc_register(RTC_YEAR10) & 0x0F;
    dow   = read_rtc_register(RTC_DOW) & 0x0F;

    uint16_t hour = (h10 * 10) + h1;
    uint16_t min = (mi10 * 10) + mi1;
    uint16_t sec = (s10 * 10) + s1;
    uint16_t day = (d10 * 10) + d1;
    uint16_t mon = (mo10 * 10) + mo1;
    uint16_t year = (y10 * 10) + y1;

    td->tm_sec = sec;
    td->tm_min = min;
    td->tm_hour = hour;
    td->tm_day = day;
    td->tm_mon = mon;
    td->tm_year = year + 2000;
    td->tm_dow = dow;
}

void rtc_set_time(const rtc_date_t *td)
{
    int year = td->tm_year - 2000;

    // set date
    write_rtc_register(RTC_YEAR10,   year / 10);
	write_rtc_register(RTC_YEAR1,    year % 10);
    write_rtc_register(RTC_MONTH10,  td->tm_mon / 10);
	write_rtc_register(RTC_MONTH1,   td->tm_mon % 10);
    write_rtc_register(RTC_DAY10,    td->tm_day / 10);
	write_rtc_register(RTC_DAY1,     td->tm_day % 10);

    // set time
    write_rtc_register(RTC_HOUR10,   td->tm_hour / 10);
	write_rtc_register(RTC_HOUR1,    td->tm_hour % 10);
    write_rtc_register(RTC_MINUTE10, td->tm_min / 10);
	write_rtc_register(RTC_MINUTE1,  td->tm_min % 10);
    write_rtc_register(RTC_SECOND10, td->tm_sec / 10);
	write_rtc_register(RTC_SECOND1,  td->tm_sec % 10);

    // set weekday
    int dow = dayofweek(td->tm_day, td->tm_mon, td->tm_year);
    write_rtc_register(RTC_DOW, dow);
}

void rtc_print_time(void)
{
    rtc_date_t date;

    rtc_get_time(&date);
    printf("Time is: %s, %02d/%02d/%04d %02d:%02d:%02d\n", get_dow(date.tm_dow), date.tm_day, date.tm_mon, date.tm_year, date.tm_hour, date.tm_min, date.tm_sec);
}
