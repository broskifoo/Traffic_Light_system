#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "common.h"
#include "config.h"

typedef void (*TaskFunction_t)(void);

typedef struct
{
    TaskFunction_t function;
    uint32_t period_ms;
    uint32_t last_run_ms;
    bool enabled;
    const char* name;
} Task_t;

Status_t Scheduler_Init(void);
Status_t Scheduler_AddTask(TaskFunction_t function, uint32_t period_ms, const char* name);
Status_t Scheduler_RemoveTask(TaskFunction_t function);
void Scheduler_Update(void);
void Scheduler_PrintStatus(void);

#endif