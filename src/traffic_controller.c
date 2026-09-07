#include "traffic_controller.h"
#include "traffic_light.h"
#include "system_tick.h"
#include "ir_sensor.h"
#include "common.h"
#include "config.h"
#include <stdio.h>

static TrafficController_t s_controller = {0};
static bool s_initialized = false;

static const uint16_t s_phase_durations[6] = {
    TRAFFIC_GREEN_NORMAL_MS,
    TRAFFIC_YELLOW_MS,
    TRAFFIC_ALL_RED_MS,
    TRAFFIC_GREEN_NORMAL_MS,
    TRAFFIC_YELLOW_MS,
    TRAFFIC_ALL_RED_MS
};

static const char* s_phase_names[6] = {
    "NS_GREEN",
    "NS_YELLOW",
    "ALL_RED_1",
    "EW_GREEN",
    "EW_YELLOW",
    "ALL_RED_2"
};

static void SetPhaseLights(TrafficControllerPhase_t phase)
{
    switch (phase)
    {
        case TC_PHASE_NS_GREEN:
            TrafficLight_SetState(APPROACH_NORTH, LIGHT_GREEN);
            TrafficLight_SetState(APPROACH_SOUTH, LIGHT_GREEN);
            TrafficLight_SetState(APPROACH_EAST, LIGHT_RED);
            TrafficLight_SetState(APPROACH_WEST, LIGHT_RED);
            break;

        case TC_PHASE_NS_YELLOW:
            TrafficLight_SetState(APPROACH_NORTH, LIGHT_YELLOW);
            TrafficLight_SetState(APPROACH_SOUTH, LIGHT_YELLOW);
            TrafficLight_SetState(APPROACH_EAST, LIGHT_RED);
            TrafficLight_SetState(APPROACH_WEST, LIGHT_RED);
            break;

        case TC_PHASE_ALL_RED_1:
        case TC_PHASE_ALL_RED_2:
            TrafficLight_SetAllRed();
            break;

        case TC_PHASE_EW_GREEN:
            TrafficLight_SetState(APPROACH_EAST, LIGHT_GREEN);
            TrafficLight_SetState(APPROACH_WEST, LIGHT_GREEN);
            TrafficLight_SetState(APPROACH_NORTH, LIGHT_RED);
            TrafficLight_SetState(APPROACH_SOUTH, LIGHT_RED);
            break;

        case TC_PHASE_EW_YELLOW:
            TrafficLight_SetState(APPROACH_EAST, LIGHT_YELLOW);
            TrafficLight_SetState(APPROACH_WEST, LIGHT_YELLOW);
            TrafficLight_SetState(APPROACH_NORTH, LIGHT_RED);
            TrafficLight_SetState(APPROACH_SOUTH, LIGHT_RED);
            break;
    }
}

static void AdvancePhase(void)
{
    s_controller.current_phase = (TrafficControllerPhase_t)((s_controller.current_phase + 1) % 6);
    s_controller.phase_start_time = SystemTick_GetMs();
    printf("[TRAFFIC_CTRL] Phase: %s\n", s_phase_names[s_controller.current_phase]);

    if (s_controller.current_phase == TC_PHASE_NS_GREEN)
    {
        s_controller.green_duration_ms = IRSensor_CalculateGreenDuration(APPROACH_NORTH);
        printf("[TRAFFIC_CTRL] N/S Green duration: %u ms\n", s_controller.green_duration_ms);
    }
    else if (s_controller.current_phase == TC_PHASE_EW_GREEN)
    {
        s_controller.green_duration_ms = IRSensor_CalculateGreenDuration(APPROACH_EAST);
        printf("[TRAFFIC_CTRL] E/W Green duration: %u ms\n", s_controller.green_duration_ms);
    }

    SetPhaseLights(s_controller.current_phase);
}

Status_t TrafficController_Init(void)
{
    if (s_initialized)
    {
        return STATUS_OK;
    }

    s_controller.current_phase = TC_PHASE_NS_GREEN;
    s_controller.phase_start_time = 0;
    s_controller.green_duration_ms = TRAFFIC_GREEN_NORMAL_MS;
    s_controller.emergency_override = false;
    s_controller.emergency_approach = APPROACH_NORTH;
    s_controller.emergency_start_time = 0;

    SetPhaseLights(s_controller.current_phase);
    s_controller.phase_start_time = SystemTick_GetMs();

    s_initialized = true;
    printf("[TRAFFIC_CTRL] Initialized\n");
    return STATUS_OK;
}

void TrafficController_Update(void)
{
    if (!s_initialized)
    {
        return;
    }

    IRSensor_Update();

    Tick_t now = SystemTick_GetMs();

    if (s_controller.emergency_override)
    {
        if (Tick_Elapsed(now, s_controller.emergency_start_time, EMERGENCY_DURATION_MS))
        {
            TrafficController_ClearEmergency();
        }
        return;
    }

    uint16_t phase_duration = s_phase_durations[s_controller.current_phase];

    if (s_controller.current_phase == TC_PHASE_NS_GREEN || s_controller.current_phase == TC_PHASE_EW_GREEN)
    {
        phase_duration = s_controller.green_duration_ms;
    }

    if (Tick_Elapsed(now, s_controller.phase_start_time, phase_duration))
    {
        AdvancePhase();
    }
}

void TrafficController_SetEmergency(Approach_t approach)
{
    if (!s_initialized)
    {
        return;
    }

    printf("[TRAFFIC_CTRL] EMERGENCY OVERRIDE: Approach %d\n", approach);

    s_controller.emergency_override = true;
    s_controller.emergency_approach = approach;
    s_controller.emergency_start_time = SystemTick_GetMs();

    TrafficLight_SetAllRed();

    switch (approach)
    {
        case APPROACH_NORTH:
        case APPROACH_SOUTH:
            TrafficLight_SetState(APPROACH_NORTH, LIGHT_GREEN);
            TrafficLight_SetState(APPROACH_SOUTH, LIGHT_GREEN);
            break;
        case APPROACH_EAST:
        case APPROACH_WEST:
            TrafficLight_SetState(APPROACH_EAST, LIGHT_GREEN);
            TrafficLight_SetState(APPROACH_WEST, LIGHT_GREEN);
            break;
    }

    printf("[TRAFFIC_CTRL] Emergency approach GREEN, others RED\n");
}

void TrafficController_ClearEmergency(void)
{
    if (!s_initialized || !s_controller.emergency_override)
    {
        return;
    }

    printf("[TRAFFIC_CTRL] Emergency complete, recovering...\n");

    s_controller.emergency_override = false;

    TrafficLight_SetAllRed();

    s_controller.current_phase = TC_PHASE_NS_GREEN;
    s_controller.phase_start_time = SystemTick_GetMs();
    SetPhaseLights(s_controller.current_phase);

    printf("[TRAFFIC_CTRL] Resumed normal operation: NS GREEN\n");
}

TrafficControllerPhase_t TrafficController_GetPhase(void)
{
    return s_controller.current_phase;
}

bool TrafficController_IsInEmergency(void)
{
    return s_controller.emergency_override;
}