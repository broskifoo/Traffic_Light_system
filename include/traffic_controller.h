#ifndef TRAFFIC_CONTROLLER_H
#define TRAFFIC_CONTROLLER_H

#include "common.h"
#include "config.h"
#include "traffic_light.h"
#include "system_tick.h"

typedef enum
{
    TC_PHASE_NS_GREEN = 0,
    TC_PHASE_NS_YELLOW = 1,
    TC_PHASE_ALL_RED_1 = 2,
    TC_PHASE_EW_GREEN = 3,
    TC_PHASE_EW_YELLOW = 4,
    TC_PHASE_ALL_RED_2 = 5
} TrafficControllerPhase_t;

typedef struct
{
    TrafficControllerPhase_t current_phase;
    Tick_t phase_start_time;
    uint16_t green_duration_ms;
    bool emergency_override;
    Approach_t emergency_approach;
    Tick_t emergency_start_time;
} TrafficController_t;

Status_t TrafficController_Init(void);
void TrafficController_Update(void);
void TrafficController_SetEmergency(Approach_t approach);
void TrafficController_ClearEmergency(void);
TrafficControllerPhase_t TrafficController_GetPhase(void);
bool TrafficController_IsInEmergency(void);

#endif