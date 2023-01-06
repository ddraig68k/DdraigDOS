#ifndef DDRAIG_OS_OSTIMER_H
#define DDRAIG_OS_OSTIMER_H

#include "ddraig.h"

// Timer interval in milliseconds
#define TIMER_INTERVAL  100
#define MAX_TIMERS     5

typedef void (*timer_callback)(uint32_t count);

typedef struct
{
    uint32_t interval;
    uint32_t count;
    uint32_t remaining;
    bool enabled;
    timer_callback callback;

} event_timer_t;

void init_timer(void);
uint32_t get_tick_count(void);

void cpu_delay(unsigned long d);

int16_t add_timer(uint16_t interval, bool start, timer_callback callback);
void delete_timer(uint16_t timer);

void start_timer(uint16_t timer);
void stop_timer(uint16_t timer);
void reset_timer(uint16_t timer);
void set_interval(uint16_t timer, uint32_t interval);

void update_timers(void);

#endif
