#ifndef GPIO_H
#define GPIO_H

#include "common.h"
#include "config.h"

typedef enum
{
    GPIO_PORT_0 = 0,
    GPIO_PORT_1 = 1,
    GPIO_PORT_2 = 2,
    GPIO_PORT_3 = 3
} GpioPort_t;

typedef enum
{
    GPIO_PIN_0 = 0,
    GPIO_PIN_1 = 1,
    GPIO_PIN_2 = 2,
    GPIO_PIN_3 = 3,
    GPIO_PIN_4 = 4,
    GPIO_PIN_5 = 5,
    GPIO_PIN_6 = 6,
    GPIO_PIN_7 = 7
} GpioPin_t;

typedef enum
{
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT = 1
} GpioDirection_t;

Status_t GPIO_Init(void);
Status_t GPIO_WritePin(GpioPort_t port, GpioPin_t pin, bool state);
bool GPIO_ReadPin(GpioPort_t port, GpioPin_t pin);
Status_t GPIO_WritePort(GpioPort_t port, uint8_t value);
uint8_t GPIO_ReadPort(GpioPort_t port);
Status_t GPIO_SetDirection(GpioPort_t port, GpioPin_t pin, GpioDirection_t dir);
Status_t GPIO_SetPortDirection(GpioPort_t port, uint8_t dir_mask);

void GPIO_PrintState(void);

#endif