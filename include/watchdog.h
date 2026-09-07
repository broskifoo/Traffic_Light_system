#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "common.h"
#include "config.h"

typedef void (*WatchdogCallback_t)(void);

Status_t Watchdog_Init(uint32_t timeout_ms);
Status_t Watchdog_Start(void);
Status_t Watchdog_Stop(void);
Status_t Watchdog_Kick(void);
Status_t Watchdog_SetCallback(WatchdogCallback_t callback);
void Watchdog_Update(void);
bool Watchdog_IsExpired(void);
void Watchdog_PrintStatus(void);

#endif