#include "ostimer.h"

#include <malloc.h>
#include <string.h>
#include "drivers/sys/pit.h"
#include "drivers/sys/duart.h"


event_timer_t *g_timers[MAX_TIMERS];
uint32_t g_tick_count;

void cpu_delay(unsigned long d)
{
    volatile unsigned long wait = d;
    while (wait--);
}

void pit_start_timer()
{
    PIT_WRITE(PIT_TCR, 0xE1);
}

void pit_stop_timer()
{
	PIT_WRITE(PIT_TCR, 0xE0);
}

void __attribute__((interrupt)) pit_timer(void)
{
    g_tick_count++;
    PIT_WRITE(PIT_TSR, (uint8_t)0x01);       // Reset the ZDS bit
}

void init_timer()
{
    g_tick_count = 0;
    memset(g_timers, 0, sizeof(event_timer_t) * MAX_TIMERS);

    // Initialise PIT
	PIT_WRITE(PIT_PGCR, 0x30);	        // Mode0, H34 & H12 Enable

    ISR_VECT_PIT(pit_timer);

    // Reload value = (time ms * 10000) / 32 (prescaler)
    uint32_t reload = (TIMER_INTERVAL * 10000) / 32;
	PIT_WRITE(PIT_CPRH, (uint8_t)(reload >> 16));
	PIT_WRITE(PIT_CPRM, (uint8_t)(reload >> 8));
	PIT_WRITE(PIT_CPRL, (uint8_t)reload);

    ISR_VECT_PIT(pit_timer);
    pit_start_timer();

}

uint32_t get_tick_count()
{
    return g_tick_count;
}

int16_t add_timer(uint16_t interval, bool start, timer_callback callback)
{
    for (int16_t i = 0; i < MAX_TIMERS; i++)
    {
        if (g_timers[i] == NULL)
        {
            event_timer_t *timer = malloc(sizeof(event_timer_t));
            g_timers[i] = timer;
            timer->interval = interval;
            timer->remaining = interval;
            timer->count = 0;
            timer->callback = callback;
            timer->enabled = start;
            return i;
        }
    }

    return -1;
}
void delete_timer(uint16_t timer)
{
    if (g_timers[timer] != NULL)
    {
        free(g_timers[timer]);
        g_timers[timer] = NULL;
    }
}

void start_timer(uint16_t timer)
{
    if (g_timers[timer] == NULL)
        return;

    g_timers[timer]->remaining = g_timers[timer]->interval;
    g_timers[timer]->count = 0;
    g_timers[timer]->enabled = true;
}

void stop_timer(uint16_t timer)
{
    if (g_timers[timer] == NULL)
        return;

    g_timers[timer]->enabled =false;
}

void reset_timer(uint16_t timer)
{
    if (g_timers[timer] == NULL)
        return;

    g_timers[timer]->remaining = g_timers[timer]->interval;
}

void set_interval(uint16_t timer, uint32_t interval)
{
    if (g_timers[timer] == NULL)
        return;

    g_timers[timer]->interval = interval;
}

void update_timers(void)
{
    static uint32_t last_tick = 0;

    if (last_tick != g_tick_count)
    {
        last_tick = g_tick_count;
        for (int i = 0; i < MAX_TIMERS; i++)
        {
            if (g_timers[i] != NULL)
            {
                event_timer_t *timer = g_timers[i];
                if (timer->enabled)
                {
                    timer->remaining--;
                    if (timer->remaining == 0)
                    {
                        timer->count++;
                        timer->callback(timer->count);
                        timer->remaining = timer->interval;
                    }
                }
            }
        }
    }
}