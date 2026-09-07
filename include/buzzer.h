#ifndef BUZZER_H
#define BUZZER_H

#include "common.h"
#include "config.h"

typedef enum
{
    BUZZER_OFF = 0,
    BUZZER_ON = 1,
    BUZZER_PULSE = 2,
    BUZZER_ALERT = 3
} BuzzerMode_t;

Status_t Buzzer_Init(void);
Status_t Buzzer_SetMode(BuzzerMode_t mode);
Status_t Buzzer_On(void);
Status_t Buzzer_Off(void);
Status_t Buzzer_Pulse(uint16_t on_ms, uint16_t off_ms, uint8_t count);
BuzzerMode_t Buzzer_GetMode(void);
void Buzzer_Update(void);
void Buzzer_PrintStatus(void);

#endif