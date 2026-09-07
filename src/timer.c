#include "timer.h"
#include "common.h"
#include <stdio.h>

typedef struct
{
    uint16_t reload_value;
    uint16_t current_count;
    TimerMode_t mode;
    TimerCallback_t callback;
    bool running;
    bool initialized;
} Timer_t;

static Timer_t s_timers[2] = {0};
static uint32_t s_total_ticks = 0;

Status_t Timer_Init(TimerId_t timer, TimerMode_t mode, uint16_t reload_value)
{
    if (timer > TIMER_1)
    {
        return STATUS_INVALID_PARAM;
    }

    Timer_t* t = &s_timers[timer];
    t->reload_value = reload_value;
    t->current_count = reload_value;
    t->mode = mode;
    t->callback = NULL;
    t->running = false;
    t->initialized = true;

    printf("[TIMER] Timer %d initialized (mode: %d, reload: %u)\n", timer, mode, reload_value);
    return STATUS_OK;
}

Status_t Timer_Start(TimerId_t timer)
{
    if (timer > TIMER_1)
    {
        return STATUS_INVALID_PARAM;
    }

    Timer_t* t = &s_timers[timer];
    if (!t->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    t->current_count = t->reload_value;
    t->running = true;
    return STATUS_OK;
}

Status_t Timer_Stop(TimerId_t timer)
{
    if (timer > TIMER_1)
    {
        return STATUS_INVALID_PARAM;
    }

    Timer_t* t = &s_timers[timer];
    if (!t->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    t->running = false;
    return STATUS_OK;
}

Status_t Timer_SetCallback(TimerId_t timer, TimerCallback_t callback)
{
    if (timer > TIMER_1)
    {
        return STATUS_INVALID_PARAM;
    }

    Timer_t* t = &s_timers[timer];
    if (!t->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    t->callback = callback;
    return STATUS_OK;
}

uint16_t Timer_GetCount(TimerId_t timer)
{
    if (timer > TIMER_1)
    {
        return 0;
    }

    Timer_t* t = &s_timers[timer];
    if (!t->initialized)
    {
        return 0;
    }

    return t->current_count;
}

void Timer_Tick(TimerId_t timer)
{
    if (timer > TIMER_1)
    {
        return;
    }

    Timer_t* t = &s_timers[timer];
    if (!t->initialized || !t->running)
    {
        return;
    }

    if (t->current_count > 0)
    {
        t->current_count--;
    }

    if (t->current_count == 0)
    {
        t->current_count = t->reload_value;
        s_total_ticks++;

        if (t->callback)
        {
            t->callback();
        }
    }
}

void Timer_PrintStatus(void)
{
    printf("[TIMER] Status:\n");
    for (int i = 0; i < 2; i++)
    {
        Timer_t* t = &s_timers[i];
        printf("  Timer %d: %s, count: %u/%u, mode: %d\n",
               i, t->running ? "RUNNING" : "STOPPED",
               t->current_count, t->reload_value, t->mode);
    }
    printf("  Total ticks: %u\n", s_total_ticks);
}

uint32_t Timer_GetTotalTicks(void)
{
    return s_total_ticks;
}