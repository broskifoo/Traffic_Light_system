#include "watchdog.h"
#include "system_tick.h"
#include "fault_manager.h"
#include "common.h"
#include "config.h"
#include <stdio.h>

static uint32_t s_timeout_ms = 0;
static Tick_t s_last_kick = 0;
static bool s_running = false;
static bool s_initialized = false;
static WatchdogCallback_t s_callback = NULL;
static bool s_expired = false;

Status_t Watchdog_Init(uint32_t timeout_ms)
{
    if (s_initialized)
    {
        return STATUS_OK;
    }

    s_timeout_ms = timeout_ms;
    s_last_kick = 0;
    s_running = false;
    s_callback = NULL;
    s_expired = false;
    s_initialized = true;

    printf("[WATCHDOG] Initialized (timeout: %u ms)\n", timeout_ms);
    return STATUS_OK;
}

Status_t Watchdog_Start(void)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    s_running = true;
    s_last_kick = SystemTick_GetMs();
    s_expired = false;
    printf("[WATCHDOG] Started\n");
    return STATUS_OK;
}

Status_t Watchdog_Stop(void)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    s_running = false;
    s_expired = false;
    printf("[WATCHDOG] Stopped\n");
    return STATUS_OK;
}

Status_t Watchdog_Kick(void)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (!s_running)
    {
        return STATUS_ERROR;
    }

    s_last_kick = SystemTick_GetMs();
    s_expired = false;
    return STATUS_OK;
}

Status_t Watchdog_SetCallback(WatchdogCallback_t callback)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    s_callback = callback;
    return STATUS_OK;
}

void Watchdog_Update(void)
{
    if (!s_initialized || !s_running)
    {
        return;
    }

    Tick_t now = SystemTick_GetMs();
    Tick_t elapsed = Tick_Diff(now, s_last_kick);

    if (elapsed >= s_timeout_ms)
    {
        s_expired = true;
        printf("[WATCHDOG] TIMEOUT! System reset triggered.\n");

        if (s_callback)
        {
            s_callback();
        }

        FaultManager_SetFault(FAULT_WATCHDOG);
    }
}

bool Watchdog_IsExpired(void)
{
    return s_expired;
}

void Watchdog_PrintStatus(void)
{
    if (!s_initialized)
    {
        printf("[WATCHDOG] Not initialized\n");
        return;
    }

    Tick_t now = SystemTick_GetMs();
    Tick_t elapsed = s_running ? Tick_Diff(now, s_last_kick) : 0;

    printf("[WATCHDOG] Running: %s, Timeout: %u ms, Elapsed: %u ms, Expired: %s\n",
           s_running ? "YES" : "NO", s_timeout_ms, elapsed, s_expired ? "YES" : "NO");
}