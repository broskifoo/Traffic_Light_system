#include "traffic_light.h"
#include "gpio.h"
#include "common.h"
#include <stdio.h>
#include <string.h>

static TrafficLightConfig_t s_light_configs[MAX_APPROACHES] = {
    {GPIO_PORT_1, GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2},
    {GPIO_PORT_1, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5},
    {GPIO_PORT_2, GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2},
    {GPIO_PORT_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5}
};

static TrafficLightState_t s_current_states[MAX_APPROACHES] = {LIGHT_RED, LIGHT_RED, LIGHT_RED, LIGHT_RED};
static bool s_initialized = false;

static const char* approach_names[MAX_APPROACHES] = {"NORTH", "SOUTH", "EAST", "WEST"};
static const char* state_names[3] = {"RED", "YELLOW", "GREEN"};

static void SetLightPins(const TrafficLightConfig_t* config, TrafficLightState_t state)
{
    GPIO_WritePin(config->port, config->red_pin, state == LIGHT_RED);
    GPIO_WritePin(config->port, config->yellow_pin, state == LIGHT_YELLOW);
    GPIO_WritePin(config->port, config->green_pin, state == LIGHT_GREEN);
}

Status_t TrafficLight_Init(void)
{
    if (s_initialized)
    {
        return STATUS_OK;
    }

    Status_t status = GPIO_Init();
    if (status != STATUS_OK)
    {
        return status;
    }

    for (uint8_t i = 0; i < MAX_APPROACHES; i++)
    {
        const TrafficLightConfig_t* config = &s_light_configs[i];
        GPIO_SetDirection(config->port, config->red_pin, GPIO_DIR_OUTPUT);
        GPIO_SetDirection(config->port, config->yellow_pin, GPIO_DIR_OUTPUT);
        GPIO_SetDirection(config->port, config->green_pin, GPIO_DIR_OUTPUT);
        s_current_states[i] = LIGHT_RED;
    }

    TrafficLight_SetAllRed();
    s_initialized = true;

    printf("[TRAFFIC_LIGHT] Initialized\n");
    return STATUS_OK;
}

Status_t TrafficLight_SetState(Approach_t approach, TrafficLightState_t state)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (approach >= MAX_APPROACHES || state > LIGHT_GREEN)
    {
        return STATUS_INVALID_PARAM;
    }

    const TrafficLightConfig_t* config = &s_light_configs[approach];
    SetLightPins(config, state);

    TrafficLightState_t old_state = s_current_states[approach];
    s_current_states[approach] = state;

    printf("[TRAFFIC_LIGHT] %s: %s -> %s\n", approach_names[approach], state_names[old_state], state_names[state]);

    return STATUS_OK;
}

TrafficLightState_t TrafficLight_GetState(Approach_t approach)
{
    if (!s_initialized || approach >= MAX_APPROACHES)
    {
        return LIGHT_RED;
    }

    return s_current_states[approach];
}

Status_t TrafficLight_SetAllRed(void)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    for (uint8_t i = 0; i < MAX_APPROACHES; i++)
    {
        TrafficLight_SetState((Approach_t)i, LIGHT_RED);
    }

    return STATUS_OK;
}

void TrafficLight_PrintStatus(void)
{
    if (!s_initialized)
    {
        printf("[TRAFFIC_LIGHT] Not initialized\n");
        return;
    }

    printf("[TRAFFIC_LIGHT] Current States:\n");
    for (uint8_t i = 0; i < MAX_APPROACHES; i++)
    {
        printf("  %s: %s\n", approach_names[i], state_names[s_current_states[i]]);
    }
}

bool TrafficLight_IsSafeState(void)
{
    if (!s_initialized)
    {
        return false;
    }

    int green_count = 0;
    for (uint8_t i = 0; i < MAX_APPROACHES; i++)
    {
        if (s_current_states[i] == LIGHT_GREEN)
        {
            green_count++;
        }
    }

    return green_count <= 2;
}