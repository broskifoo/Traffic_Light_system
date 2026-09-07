#ifndef LOGGER_H
#define LOGGER_H

#include "common.h"
#include "config.h"
#include "traffic_light.h"
#include "app.h"

typedef enum
{
    LOG_EVENT_SYSTEM_START = 0,
    LOG_EVENT_STATE_CHANGE = 1,
    LOG_EVENT_EMERGENCY = 2,
    LOG_EVENT_RFID_TAG = 3,
    LOG_EVENT_FAULT = 4,
    LOG_EVENT_WATCHDOG = 5,
    LOG_EVENT_TRAFFIC_PHASE = 6
} LogEventType_t;

Status_t Logger_Init(void);
void Logger_LogEvent(LogEventType_t type, SystemState_t system_state,
                     TrafficLightState_t north, TrafficLightState_t south,
                     TrafficLightState_t east, TrafficLightState_t west,
                     uint8_t traffic_density, const char* rfid_event,
                     uint32_t fault_status);
void Logger_LogEmergency(Approach_t approach, uint32_t response_time_ms);
void Logger_LogFault(uint32_t fault_flags);
void Logger_Close(void);
void Logger_PrintSummary(void);

#endif