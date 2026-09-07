#include "system_tick.h"
#include "timer.h"
#include "common.h"
#include <stdio.h>

#define MAX_CALLBACKS 8

typedef struct
{
    Tick_t current_tick_ms;
    Tick_t tick_count;
    SystemTickCallback_t callbacks[MAX_CALLBACKS];
    uint8_t callback_count;
    bool initialized;
} SystemTick_t;

static SystemTick_t s_system_tick = {0};

static void Timer0_Callback(void)
{
    s_system_tick.current_tick_ms += SYSTEM_TICK_MS;
    s_system_tick.tick_count++;

    for (uint8_t i = 0; i < s_system_tick.callback_count; i++)
    {
        if (s_system_tick.callbacks[i])
        {
            s_system_tick.callbacks[i]();
        }
    }
}

Status_t SystemTick_Init(void)
{
    if (s_system_tick.initialized)
    {
        return STATUS_OK;
    }

    s_system_tick.current_tick_ms = 0;
    s_system_tick.tick_count = 0;
    s_system_tick.callback_count = 0;
    for (int i = 0; i < MAX_CALLBACKS; i++)
    {
        s_system_tick.callbacks[i] = NULL;
    }

    Status_t status = Timer_Init(TIMER_0, TIMER_MODE_16BIT, SYSTICK_TIMER_HZ);
    if (status != STATUS_OK)
    {
        return status;
    }

    status = Timer_SetCallback(TIMER_0, Timer0_Callback);
    if (status != STATUS_OK)
    {
        return status;
    }

    status = Timer_Start(TIMER_0);
    if (status != STATUS_OK)
    {
        return status;
    }

    s_system_tick.initialized = true;
    printf("[SYSTICK] Initialized (%d ms tick)\n", SYSTEM_TICK_MS);
    return STATUS_OK;
}

Status_t SystemTick_RegisterCallback(SystemTickCallback_t callback)
{
    if (!s_system_tick.initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (s_system_tick.callback_count >= MAX_CALLBACKS)
    {
        return STATUS_BUFFER_OVERFLOW;
    }

    s_system_tick.callbacks[s_system_tick.callback_count++] = callback;
    return STATUS_OK;
}

Status_t SystemTick_UnregisterCallback(SystemTickCallback_t callback)
{
    if (!s_system_tick.initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    for (uint8_t i = 0; i < s_system_tick.callback_count; i++)
    {
        if (s_system_tick.callbacks[i] == callback)
        {
            for (uint8_t j = i; j < s_system_tick.callback_count - 1; j++)
            {
                s_system_tick.callbacks[j] = s_system_tick.callbacks[j + 1];
            }
            s_system_tick.callbacks[--s_system_tick.callback_count] = NULL;
            return STATUS_OK;
        }
    }

    return STATUS_NOT_FOUND;
}

void SystemTick_Update(void)
{
    if (!s_system_tick.initialized)
    {
        return;
    }

    Timer_Tick(TIMER_0);
}

Tick_t SystemTick_GetMs(void)
{
    return s_system_tick.current_tick_ms;
}

Tick_t SystemTick_GetTicks(void)
{
    return s_system_tick.tick_count;
}

void SystemTick_PrintStatus(void)
{
    if (!s_system_tick.initialized)
    {
        printf("[SYSTICK] Not initialized\n");
        return;
    }

    printf("[SYSTICK] Current: %u ms, Ticks: %u, Callbacks: %u\n",
           s_system_tick.current_tick_ms, s_system_tick.tick_count, s_system_tick.callback_count);
}