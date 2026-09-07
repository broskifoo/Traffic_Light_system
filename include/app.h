#ifndef APP_H
#define APP_H

#include "common.h"
#include "config.h"
#include "traffic_light.h"

typedef enum
{
    SYSTEM_INIT = 0,
    NORMAL_TRAFFIC = 1,
    EMERGENCY_OVERRIDE = 2,
    RECOVERY = 3,
    SYSTEM_FAULT = 4
} SystemState_t;

typedef enum
{
    SCENARIO_NORMAL = 0,
    SCENARIO_HIGH_TRAFFIC = 1,
    SCENARIO_EMERGENCY_NORTH = 2,
    SCENARIO_EMERGENCY_SOUTH = 3,
    SCENARIO_EMERGENCY_EAST = 4,
    SCENARIO_EMERGENCY_WEST = 5,
    SCENARIO_NORMAL_TAG = 6,
    SCENARIO_INVALID_RFID = 7,
    SCENARIO_RFID_OVERFLOW = 8,
    SCENARIO_SENSOR_FAULT = 9,
    SCENARIO_WATCHDOG = 10
} Scenario_t;

Status_t App_Init(void);
Status_t App_Run(void);
void App_Shutdown(void);
SystemState_t App_GetState(void);
void App_SetScenario(Scenario_t scenario);
void App_InjectEmergencyRFID(Approach_t approach);
void App_InjectNormalRFID(const char* tag_id);
void App_InjectInvalidRFID(void);
void App_TriggerWatchdogFault(void);
void App_TriggerSensorFault(void);

#endif