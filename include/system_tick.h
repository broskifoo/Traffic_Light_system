#ifndef SYSTEM_TICK_H
#define SYSTEM_TICK_H

#include "common.h"
#include "config.h"

typedef void (*SystemTickCallback_t)(void);

Status_t SystemTick_Init(void);
Status_t SystemTick_RegisterCallback(SystemTickCallback_t callback);
Status_t SystemTick_UnregisterCallback(SystemTickCallback_t callback);
void SystemTick_Update(void);
Tick_t SystemTick_GetMs(void);
Tick_t SystemTick_GetTicks(void);
void SystemTick_PrintStatus(void);

#endif