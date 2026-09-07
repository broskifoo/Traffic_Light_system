#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include "common.h"
#include "config.h"
#include "traffic_light.h"

typedef enum
{
    IR_SENSOR_ENTRY = 0,
    IR_SENSOR_QUEUE = 1
} IRSensorType_t;

typedef struct
{
    bool vehicle_present;
    bool queue_detected;
    uint8_t vehicle_count;
} IRSensorStatus_t;

typedef struct
{
    GpioPort_t port;
    GpioPin_t entry_pin;
    GpioPin_t queue_pin;
} IRSensorConfig_t;

Status_t IRSensor_Init(void);
Status_t IRSensor_Update(void);
IRSensorStatus_t IRSensor_GetStatus(Approach_t approach);
void IRSensor_SetTrafficDensity(Approach_t approach, uint8_t density);
uint8_t IRSensor_GetVehicleCount(Approach_t approach);
uint16_t IRSensor_CalculateGreenDuration(Approach_t approach);
void IRSensor_PrintStatus(void);

#endif