#include "gpio.h"
#include "common.h"
#include <stdio.h>

static uint8_t s_port_values[4] = {0};
static uint8_t s_port_directions[4] = {0};
static bool s_initialized = false;

static const char* port_names[4] = {"P0", "P1", "P2", "P3"};

Status_t GPIO_Init(void)
{
    if (s_initialized)
    {
        return STATUS_OK;
    }

    for (int i = 0; i < 4; i++)
    {
        s_port_values[i] = 0x00;
        s_port_directions[i] = 0x00;
    }

    s_initialized = true;
    printf("[GPIO] Initialized\n");
    return STATUS_OK;
}

Status_t GPIO_WritePin(GpioPort_t port, GpioPin_t pin, bool state)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (port > GPIO_PORT_3 || pin > GPIO_PIN_7)
    {
        return STATUS_INVALID_PARAM;
    }

    uint8_t mask = (1 << pin);
    if (state)
    {
        s_port_values[port] |= mask;
    }
    else
    {
        s_port_values[port] &= ~mask;
    }

    return STATUS_OK;
}

bool GPIO_ReadPin(GpioPort_t port, GpioPin_t pin)
{
    if (!s_initialized || port > GPIO_PORT_3 || pin > GPIO_PIN_7)
    {
        return false;
    }

    return (s_port_values[port] & (1 << pin)) != 0;
}

Status_t GPIO_WritePort(GpioPort_t port, uint8_t value)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (port > GPIO_PORT_3)
    {
        return STATUS_INVALID_PARAM;
    }

    s_port_values[port] = value;
    return STATUS_OK;
}

uint8_t GPIO_ReadPort(GpioPort_t port)
{
    if (!s_initialized || port > GPIO_PORT_3)
    {
        return 0;
    }

    return s_port_values[port];
}

Status_t GPIO_SetDirection(GpioPort_t port, GpioPin_t pin, GpioDirection_t dir)
{
    if (!s_initialized || port > GPIO_PORT_3 || pin > GPIO_PIN_7)
    {
        return STATUS_INVALID_PARAM;
    }

    uint8_t mask = (1 << pin);
    if (dir == GPIO_DIR_OUTPUT)
    {
        s_port_directions[port] |= mask;
    }
    else
    {
        s_port_directions[port] &= ~mask;
    }

    return STATUS_OK;
}

Status_t GPIO_SetPortDirection(GpioPort_t port, uint8_t dir_mask)
{
    if (!s_initialized || port > GPIO_PORT_3)
    {
        return STATUS_INVALID_PARAM;
    }

    s_port_directions[port] = dir_mask;
    return STATUS_OK;
}

void GPIO_PrintState(void)
{
    if (!s_initialized)
    {
        printf("[GPIO] Not initialized\n");
        return;
    }

    printf("[GPIO] Port States:\n");
    for (int i = 0; i < 4; i++)
    {
        printf("  %s: 0x%02X (dir: 0x%02X)\n", port_names[i], s_port_values[i], s_port_directions[i]);
    }
}