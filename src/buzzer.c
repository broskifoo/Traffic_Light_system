#include "buzzer.h"
#include "gpio.h"
#include "system_tick.h"
#include "common.h"
#include "config.h"
#include <stdio.h>

static BuzzerMode_t s_mode = BUZZER_OFF;
static bool s_initialized = false;
static Tick_t s_pulse_start = 0;
static uint16_t s_pulse_on_ms = 0;
static uint16_t s_pulse_off_ms = 0;
static uint8_t s_pulse_count = 0;
static uint8_t s_pulse_current = 0;
static bool s_pulse_state = false;

#define BUZZER_PORT GPIO_PORT_1
#define BUZZER_PIN  GPIO_PIN_6

Status_t Buzzer_Init(void)
{
    if (s_initialized)
    {
        return STATUS_OK;
    }

    GPIO_SetDirection(BUZZER_PORT, BUZZER_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, false);

    s_mode = BUZZER_OFF;
    s_initialized = true;
    printf("[BUZZER] Initialized\n");
    return STATUS_OK;
}

Status_t Buzzer_SetMode(BuzzerMode_t mode)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    s_mode = mode;

    switch (mode)
    {
        case BUZZER_OFF:
            GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, false);
            printf("[BUZZER] OFF\n");
            break;
        case BUZZER_ON:
            GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, true);
            printf("[BUZZER] ON\n");
            break;
        case BUZZER_PULSE:
            printf("[BUZZER] PULSE mode\n");
            break;
        case BUZZER_ALERT:
            s_pulse_on_ms = 200;
            s_pulse_off_ms = 200;
            s_pulse_count = 3;
            s_pulse_current = 0;
            s_pulse_state = true;
            s_pulse_start = SystemTick_GetMs();
            GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, true);
            printf("[BUZZER] ALERT mode (3 pulses)\n");
            break;
    }

    return STATUS_OK;
}

Status_t Buzzer_On(void)
{
    return Buzzer_SetMode(BUZZER_ON);
}

Status_t Buzzer_Off(void)
{
    return Buzzer_SetMode(BUZZER_OFF);
}

Status_t Buzzer_Pulse(uint16_t on_ms, uint16_t off_ms, uint8_t count)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    s_mode = BUZZER_PULSE;
    s_pulse_on_ms = on_ms;
    s_pulse_off_ms = off_ms;
    s_pulse_count = count;
    s_pulse_current = 0;
    s_pulse_state = true;
    s_pulse_start = SystemTick_GetMs();
    GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, true);

    return STATUS_OK;
}

BuzzerMode_t Buzzer_GetMode(void)
{
    return s_mode;
}

void Buzzer_Update(void)
{
    if (!s_initialized)
    {
        return;
    }

    if (s_mode == BUZZER_PULSE || s_mode == BUZZER_ALERT)
    {
        Tick_t now = SystemTick_GetMs();
        Tick_t elapsed = Tick_Diff(now, s_pulse_start);

        if (s_pulse_state && elapsed >= s_pulse_on_ms)
        {
            GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, false);
            s_pulse_state = false;
            s_pulse_start = now;
            s_pulse_current++;
        }
        else if (!s_pulse_state && elapsed >= s_pulse_off_ms)
        {
            if (s_pulse_current < s_pulse_count)
            {
                GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, true);
                s_pulse_state = true;
                s_pulse_start = now;
            }
            else
            {
                s_mode = BUZZER_OFF;
                GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, false);
                printf("[BUZZER] Pulse sequence complete\n");
            }
        }
    }
}

void Buzzer_PrintStatus(void)
{
    if (!s_initialized)
    {
        printf("[BUZZER] Not initialized\n");
        return;
    }

    const char* mode_names[] = {"OFF", "ON", "PULSE", "ALERT"};
    printf("[BUZZER] Mode: %s, Pin: %s\n",
           mode_names[s_mode],
           GPIO_ReadPin(BUZZER_PORT, BUZZER_PIN) ? "HIGH" : "LOW");
}