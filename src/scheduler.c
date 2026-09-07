#include "scheduler.h"
#include "system_tick.h"
#include "common.h"
#include "config.h"
#include <stdio.h>

static Task_t s_tasks[SCHEDULER_MAX_TASKS];
static uint8_t s_task_count = 0;
static bool s_initialized = false;

Status_t Scheduler_Init(void)
{
    if (s_initialized)
    {
        return STATUS_OK;
    }

    s_task_count = 0;
    for (uint8_t i = 0; i < SCHEDULER_MAX_TASKS; i++)
    {
        s_tasks[i].function = NULL;
        s_tasks[i].period_ms = 0;
        s_tasks[i].last_run_ms = 0;
        s_tasks[i].enabled = false;
        s_tasks[i].name = NULL;
    }

    s_initialized = true;
    printf("[SCHEDULER] Initialized\n");
    return STATUS_OK;
}

Status_t Scheduler_AddTask(TaskFunction_t function, uint32_t period_ms, const char* name)
{
    if (!s_initialized || !function)
    {
        return STATUS_INVALID_PARAM;
    }

    if (s_task_count >= SCHEDULER_MAX_TASKS)
    {
        return STATUS_BUFFER_OVERFLOW;
    }

    s_tasks[s_task_count].function = function;
    s_tasks[s_task_count].period_ms = period_ms;
    s_tasks[s_task_count].last_run_ms = SystemTick_GetMs();
    s_tasks[s_task_count].enabled = true;
    s_tasks[s_task_count].name = name;
    s_task_count++;

    printf("[SCHEDULER] Task added: %s (period: %u ms)\n", name, period_ms);
    return STATUS_OK;
}

Status_t Scheduler_RemoveTask(TaskFunction_t function)
{
    if (!s_initialized || !function)
    {
        return STATUS_INVALID_PARAM;
    }

    for (uint8_t i = 0; i < s_task_count; i++)
    {
        if (s_tasks[i].function == function)
        {
            for (uint8_t j = i; j < s_task_count - 1; j++)
            {
                s_tasks[j] = s_tasks[j + 1];
            }
            s_task_count--;
            s_tasks[s_task_count].function = NULL;
            return STATUS_OK;
        }
    }

    return STATUS_NOT_FOUND;
}

void Scheduler_Update(void)
{
    if (!s_initialized)
    {
        return;
    }

    Tick_t now = SystemTick_GetMs();

    for (uint8_t i = 0; i < s_task_count; i++)
    {
        Task_t* task = &s_tasks[i];

        if (!task->enabled || !task->function)
        {
            continue;
        }

        if (Tick_Elapsed(now, task->last_run_ms, task->period_ms))
        {
            task->last_run_ms = now;
            task->function();
        }
    }
}

void Scheduler_PrintStatus(void)
{
    if (!s_initialized)
    {
        printf("[SCHEDULER] Not initialized\n");
        return;
    }

    printf("[SCHEDULER] Tasks: %u/%u\n", s_task_count, SCHEDULER_MAX_TASKS);

    for (uint8_t i = 0; i < s_task_count; i++)
    {
        Task_t* task = &s_tasks[i];
        Tick_t now = SystemTick_GetMs();
        Tick_t next_run = task->last_run_ms + task->period_ms;
        Tick_t remaining = (next_run > now) ? (next_run - now) : 0;

        printf("  [%u] %s: period=%u ms, next in %u ms, enabled=%s\n",
               i, task->name ? task->name : "unnamed",
               task->period_ms, remaining, task->enabled ? "YES" : "NO");
    }
}