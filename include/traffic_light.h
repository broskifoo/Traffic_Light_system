#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include "common.h"
#include "config.h"
#include "gpio.h"

typedef enum
{
    LIGHT_RED = 0,
    LIGHT_YELLOW = 1,
    LIGHT_GREEN = 2
} TrafficLightState_t;

typedef enum
{
    APPROACH_NORTH = 0,
    APPROACH_SOUTH = 1,
    APPROACH_EAST = 2,
    APPROACH_WEST = 3
} Approach_t;

typedef struct
{
    GpioPort_t port;
    GpioPin_t red_pin;
    GpioPin_t yellow_pin;
    GpioPin_t green_pin;
} TrafficLightConfig_t;

Status_t TrafficLight_Init(void);
Status_t TrafficLight_SetState(Approach_t approach, TrafficLightState_t state);
TrafficLightState_t TrafficLight_GetState(Approach_t approach);
Status_t TrafficLight_SetAllRed(void);
void TrafficLight_PrintStatus(void);
bool TrafficLight_IsSafeState(void);

#endif