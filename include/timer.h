#ifndef TIMER_H
#define TIMER_H

#include "common.h"
#include "config.h"

typedef enum
{
    TIMER_0 = 0,
    TIMER_1 = 1
} TimerId_t;

typedef enum
{
    TIMER_MODE_16BIT = 0,
    TIMER_MODE_8BIT_AUTO = 1
} TimerMode_t;

typedef void (*TimerCallback_t)(void);

Status_t Timer_Init(TimerId_t timer, TimerMode_t mode, uint16_t reload_value);
Status_t Timer_Start(TimerId_t timer);
Status_t Timer_Stop(TimerId_t timer);
Status_t Timer_SetCallback(TimerId_t timer, TimerCallback_t callback);
uint16_t Timer_GetCount(TimerId_t timer);
void Timer_Tick(TimerId_t timer);

void Timer_PrintStatus(void);

#endif