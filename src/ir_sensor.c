#include "ir_sensor.h"
#include "traffic_light.h"
#include "gpio.h"
#include "common.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static IRSensorConfig_t s_sensor_configs[MAX_APPROACHES] = {
    {GPIO_PORT_3, GPIO_PIN_0, GPIO_PIN_1},
    {GPIO_PORT_3, GPIO_PIN_2, GPIO_PIN_3},
    {GPIO_PORT_3, GPIO_PIN_4, GPIO_PIN_5},
    {GPIO_PORT_3, GPIO_PIN_6, GPIO_PIN_7}
};

static IRSensorStatus_t s_sensor_status[MAX_APPROACHES] = {0};
static uint8_t s_traffic_density[MAX_APPROACHES] = {2, 2, 2, 2};
static bool s_initialized = false;

static const char* approach_names[MAX_APPROACHES] = {"NORTH", "SOUTH", "EAST", "WEST"};

Status_t IRSensor_Init(void)
{
    if (s_initialized)
    {
        return STATUS_OK;
    }

    for (uint8_t i = 0; i < MAX_APPROACHES; i++)
    {
        IRSensorConfig_t* config = &s_sensor_configs[i];
        GPIO_SetDirection(config->port, config->entry_pin, GPIO_DIR_INPUT);
        GPIO_SetDirection(config->port, config->queue_pin, GPIO_DIR_INPUT);
        s_sensor_status[i].vehicle_present = false;
        s_sensor_status[i].queue_detected = false;
        s_sensor_status[i].vehicle_count = 0;
        s_traffic_density[i] = 2;
    }

    srand((unsigned int)time(NULL));
    s_initialized = true;
    printf("[IR_SENSOR] Initialized\n");
    return STATUS_OK;
}

Status_t IRSensor_Update(void)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    for (uint8_t i = 0; i < MAX_APPROACHES; i++)
    {
        IRSensorConfig_t* config = &s_sensor_configs[i];
        IRSensorStatus_t* status = &s_sensor_status[i];

        bool entry_active = (rand() % 100) < (s_traffic_density[i] * 10);
        bool queue_active = (rand() % 100) < (s_traffic_density[i] * 5);

        status->vehicle_present = entry_active;
        status->queue_detected = queue_active;

        if (entry_active)
        {
            status->vehicle_count++;
            if (status->vehicle_count > 20)
            {
                status->vehicle_count = 20;
            }
        }
        else if (status->vehicle_count > 0)
        {
            status->vehicle_count--;
        }

        GPIO_WritePin(config->port, config->entry_pin, entry_active);
        GPIO_WritePin(config->port, config->queue_pin, queue_active);
    }

    return STATUS_OK;
}

IRSensorStatus_t IRSensor_GetStatus(Approach_t approach)
{
    IRSensorStatus_t empty = {false, false, 0};

    if (!s_initialized || approach >= MAX_APPROACHES)
    {
        return empty;
    }

    return s_sensor_status[approach];
}

void IRSensor_SetTrafficDensity(Approach_t approach, uint8_t density)
{
    if (!s_initialized || approach >= MAX_APPROACHES)
    {
        return;
    }

    if (density > 10)
    {
        density = 10;
    }

    s_traffic_density[approach] = density;
}

uint8_t IRSensor_GetVehicleCount(Approach_t approach)
{
    if (!s_initialized || approach >= MAX_APPROACHES)
    {
        return 0;
    }

    return s_sensor_status[approach].vehicle_count;
}

TrafficLightState_t IRSensor_GetRecommendedGreenDuration(Approach_t approach)
{
    (void)approach;
    return LIGHT_GREEN;
}

uint16_t IRSensor_CalculateGreenDuration(Approach_t approach)
{
    if (!s_initialized || approach >= MAX_APPROACHES)
    {
        return TRAFFIC_GREEN_NORMAL_MS;
    }

    uint8_t count = s_sensor_status[approach].vehicle_count;
    bool queue = s_sensor_status[approach].queue_detected;

    if (count == 0 && !queue)
    {
        return TRAFFIC_GREEN_MIN_MS;
    }
    else if (count > 5 || queue)
    {
        return TRAFFIC_GREEN_MAX_MS;
    }
    else
    {
        return TRAFFIC_GREEN_NORMAL_MS;
    }
}

void IRSensor_PrintStatus(void)
{
    if (!s_initialized)
    {
        printf("[IR_SENSOR] Not initialized\n");
        return;
    }

    printf("[IR_SENSOR] Status:\n");
    for (uint8_t i = 0; i < MAX_APPROACHES; i++)
    {
        IRSensorStatus_t* status = &s_sensor_status[i];
        printf("  %s: vehicles=%u, present=%s, queue=%s, density=%u\n",
               approach_names[i],
               status->vehicle_count,
               status->vehicle_present ? "YES" : "NO",
               status->queue_detected ? "YES" : "NO",
               s_traffic_density[i]);
    }
}