#ifndef LCD_H
#define LCD_H

#include "common.h"
#include "config.h"
#include "traffic_light.h"

Status_t LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t column);
void LCD_Print(const char* text);
void LCD_PrintLine(uint8_t row, const char* text);
void LCD_PrintStatus(const char* line1, const char* line2);
void LCD_PrintEmergency(Approach_t approach);
void LCD_PrintNormal(uint8_t phase, uint32_t remaining_ms);
void LCD_PrintFault(uint32_t fault_flags);
void LCD_Update(void);
void LCD_PrintConsole(void);

#endif