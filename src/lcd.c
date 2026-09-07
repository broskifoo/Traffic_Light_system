#include "lcd.h"
#include "traffic_light.h"
#include "common.h"
#include "config.h"
#include <stdio.h>
#include <string.h>

static char s_display[LCD_ROWS][LCD_COLS + 1];
static bool s_initialized = false;
static bool s_dirty = true;

Status_t LCD_Init(void)
{
    if (s_initialized)
    {
        return STATUS_OK;
    }

    LCD_Clear();
    s_initialized = true;
    printf("[LCD] Initialized (16x2)\n");
    return STATUS_OK;
}

void LCD_Clear(void)
{
    for (uint8_t i = 0; i < LCD_ROWS; i++)
    {
        memset(s_display[i], ' ', LCD_COLS);
        s_display[i][LCD_COLS] = '\0';
    }
    s_dirty = true;
}

void LCD_SetCursor(uint8_t row, uint8_t column)
{
    (void)row;
    (void)column;
}

void LCD_Print(const char* text)
{
    if (!s_initialized || !text)
    {
        return;
    }

    s_dirty = true;
}

void LCD_PrintLine(uint8_t row, const char* text)
{
    if (!s_initialized || !text || row >= LCD_ROWS)
    {
        return;
    }

    size_t len = strlen(text);
    if (len > LCD_COLS)
    {
        len = LCD_COLS;
    }

    memset(s_display[row], ' ', LCD_COLS);
    memcpy(s_display[row], text, len);
    s_display[row][LCD_COLS] = '\0';
    s_dirty = true;
}

void LCD_PrintStatus(const char* line1, const char* line2)
{
    if (line1)
    {
        LCD_PrintLine(0, line1);
    }
    if (line2)
    {
        LCD_PrintLine(1, line2);
    }
}

void LCD_PrintEmergency(Approach_t approach)
{
    const char* approach_names[] = {"NORTH", "SOUTH", "EAST", "WEST"};
    char line1[LCD_COLS + 1];
    char line2[LCD_COLS + 1];

    snprintf(line1, LCD_COLS + 1, "EMERGENCY");
    snprintf(line2, LCD_COLS + 1, "%s GREEN", approach_names[approach]);

    LCD_PrintStatus(line1, line2);
    s_dirty = true;
}

void LCD_PrintNormal(uint8_t phase, uint32_t remaining_ms)
{
    char line1[LCD_COLS + 1];
    char line2[LCD_COLS + 1];

    const char* phase_names[] = {"NS GREEN", "NS YELLOW", "ALL RED", "EW GREEN", "EW YELLOW", "ALL RED"};

    snprintf(line1, LCD_COLS + 1, "NORMAL: %s", phase_names[phase]);
    snprintf(line2, LCD_COLS + 1, "Time: %u ms", remaining_ms);

    LCD_PrintStatus(line1, line2);
    s_dirty = true;
}

void LCD_PrintFault(uint32_t fault_flags)
{
    char line1[LCD_COLS + 1];
    char line2[LCD_COLS + 1];

    snprintf(line1, LCD_COLS + 1, "SYSTEM FAULT");
    snprintf(line2, LCD_COLS + 1, "Flags: 0x%04X", (uint16_t)fault_flags);

    LCD_PrintStatus(line1, line2);
    s_dirty = true;
}

void LCD_Update(void)
{
    if (!s_initialized || !s_dirty)
    {
        return;
    }

    LCD_PrintConsole();
    s_dirty = false;
}

void LCD_PrintConsole(void)
{
    printf("+----------------+\n");
    printf("| %-14s |\n", s_display[0]);
    printf("| %-14s |\n", s_display[1]);
    printf("+----------------+\n");
}