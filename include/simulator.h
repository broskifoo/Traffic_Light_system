#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "common.h"
#include "config.h"
#include "traffic_light.h"
#include "event_manager.h"

typedef enum
{
    SIM_SCENARIO_NORMAL = 0,
    SIM_SCENARIO_HIGH_TRAFFIC = 1,
    SIM_SCENARIO_EMERGENCY_NORTH = 2,
    SIM_SCENARIO_EMERGENCY_SOUTH = 3,
    SIM_SCENARIO_EMERGENCY_EAST = 4,
    SIM_SCENARIO_EMERGENCY_WEST = 5,
    SIM_SCENARIO_NORMAL_TAG = 6,
    SIM_SCENARIO_INVALID_RFID = 7,
    SIM_SCENARIO_RFID_OVERFLOW = 8,
    SIM_SCENARIO_SENSOR_FAULT = 9,
    SIM_SCENARIO_WATCHDOG = 10,
    SIM_SCENARIO_EMERGENCY_DURING_GREEN = 11,
    SIM_SCENARIO_EMERGENCY_DURING_YELLOW = 12,
    SIM_SCENARIO_EMERGENCY_DURING_TRANSITION = 13,
    SIM_SCENARIO_MULTIPLE_RFID = 14
} SimScenario_t;

Status_t Simulator_Init(void);
void Simulator_RunScenario(SimScenario_t scenario);
void Simulator_InjectEmergencyRFID(Approach_t approach);
void Simulator_InjectNormalRFID(const char* tag_id);
void Simulator_InjectInvalidRFID(void);
void Simulator_TriggerWatchdogTimeout(void);
void Simulator_TriggerSensorFault(void);
void Simulator_PrintMetrics(void);

#endif