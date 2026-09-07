#include "logger.h"
#include "system_tick.h"
#include "common.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static FILE* s_log_file = NULL;
static bool s_initialized = false;
static uint32_t s_log_count = 0;
static Tick_t s_start_time = 0;

static const char* state_names[] = {
    "SYSTEM_INIT",
    "NORMAL_TRAFFIC",
    "EMERGENCY_OVERRIDE",
    "RECOVERY",
    "SYSTEM_FAULT"
};

static const char* light_names[] = {
    "RED",
    "YELLOW",
    "GREEN"
};

Status_t Logger_Init(void)
{
    if (s_initialized)
    {
        return STATUS_OK;
    }

    s_log_file = fopen("logs/simulation_log.csv", "w");
    if (!s_log_file)
    {
        return STATUS_ERROR;
    }

    fprintf(s_log_file, "timestamp_ms,system_state,north_light,south_light,east_light,west_light,traffic_density,rfid_event,fault_status\n");

    s_start_time = SystemTick_GetMs();
    s_log_count = 0;
    s_initialized = true;

    printf("[LOGGER] Initialized (logging to logs/simulation_log.csv)\n");
    return STATUS_OK;
}

void Logger_LogEvent(LogEventType_t type, SystemState_t system_state,
                     TrafficLightState_t north, TrafficLightState_t south,
                     TrafficLightState_t east, TrafficLightState_t west,
                     uint8_t traffic_density, const char* rfid_event,
                     uint32_t fault_status)
{
    (void)type;

    if (!s_initialized || !s_log_file)
    {
        return;
    }

    Tick_t timestamp = SystemTick_GetMs();
    Tick_t elapsed = Tick_Diff(timestamp, s_start_time);

    const char* rfid_str = rfid_event ? rfid_event : "none";

    fprintf(s_log_file, "%u,%s,%s,%s,%s,%s,%u,%s,0x%08X\n",
            elapsed,
            state_names[system_state],
            light_names[north],
            light_names[south],
            light_names[east],
            light_names[west],
            traffic_density,
            rfid_str,
            fault_status);

    fflush(s_log_file);
    s_log_count++;
}

void Logger_LogEmergency(Approach_t approach, uint32_t response_time_ms)
{
    if (!s_initialized || !s_log_file)
    {
        return;
    }

    const char* approach_names[] = {"NORTH", "SOUTH", "EAST", "WEST"};
    Tick_t timestamp = SystemTick_GetMs();
    Tick_t elapsed = Tick_Diff(timestamp, s_start_time);

    fprintf(s_log_file, "%u,EMERGENCY,%s,%u\n",
            elapsed, approach_names[approach], response_time_ms);

    fflush(s_log_file);
    s_log_count++;
}

void Logger_LogFault(uint32_t fault_flags)
{
    if (!s_initialized || !s_log_file)
    {
        return;
    }

    Tick_t timestamp = SystemTick_GetMs();
    Tick_t elapsed = Tick_Diff(timestamp, s_start_time);

    fprintf(s_log_file, "%u,FAULT,,,,,,0x%08X\n",
            elapsed, fault_flags);

    fflush(s_log_file);
    s_log_count++;
}

void Logger_Close(void)
{
    if (s_initialized && s_log_file)
    {
        fclose(s_log_file);
        s_log_file = NULL;
        s_initialized = false;
        printf("[LOGGER] Closed (%u entries logged)\n", s_log_count);
    }
}

void Logger_PrintSummary(void)
{
    if (!s_initialized)
    {
        printf("[LOGGER] Not initialized\n");
        return;
    }

    printf("[LOGGER] Entries logged: %u\n", s_log_count);
}