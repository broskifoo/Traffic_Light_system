#include "fault_manager.h"
#include "common.h"
#include <stdio.h>

static uint32_t s_fault_flags = FAULT_NONE;
static bool s_initialized = false;

static const struct
{
    uint32_t flag;
    const char* name;
} s_fault_names[] = {
    {FAULT_RFID_OVERFLOW, "RFID_OVERFLOW"},
    {FAULT_INVALID_RFID, "INVALID_RFID"},
    {FAULT_TIMER, "TIMER"},
    {FAULT_SENSOR, "SENSOR"},
    {FAULT_INVALID_STATE, "INVALID_STATE"},
    {FAULT_WATCHDOG, "WATCHDOG"},
    {FAULT_UART, "UART"},
    {FAULT_LCD, "LCD"}
};

Status_t FaultManager_Init(void)
{
    if (s_initialized)
    {
        return STATUS_OK;
    }

    s_fault_flags = FAULT_NONE;
    s_initialized = true;
    printf("[FAULT_MGR] Initialized\n");
    return STATUS_OK;
}

void FaultManager_SetFault(uint32_t fault_flag)
{
    if (!s_initialized)
    {
        return;
    }

    if ((s_fault_flags & fault_flag) == 0)
    {
        s_fault_flags |= fault_flag;
        printf("[FAULT_MGR] Fault SET: 0x%08X\n", fault_flag);
    }
}

void FaultManager_ClearFault(uint32_t fault_flag)
{
    if (!s_initialized)
    {
        return;
    }

    if (s_fault_flags & fault_flag)
    {
        s_fault_flags &= ~fault_flag;
        printf("[FAULT_MGR] Fault CLEARED: 0x%08X\n", fault_flag);
    }
}

bool FaultManager_IsFaultActive(uint32_t fault_flag)
{
    if (!s_initialized)
    {
        return false;
    }

    return (s_fault_flags & fault_flag) != 0;
}

uint32_t FaultManager_GetFaultStatus(void)
{
    return s_fault_flags;
}

void FaultManager_ClearAllFaults(void)
{
    if (!s_initialized)
    {
        return;
    }

    s_fault_flags = FAULT_NONE;
    printf("[FAULT_MGR] All faults cleared\n");
}

void FaultManager_PrintStatus(void)
{
    if (!s_initialized)
    {
        printf("[FAULT_MGR] Not initialized\n");
        return;
    }

    printf("[FAULT_MGR] Active faults: 0x%08X\n", s_fault_flags);

    if (s_fault_flags == FAULT_NONE)
    {
        printf("  (none)\n");
        return;
    }

    for (size_t i = 0; i < sizeof(s_fault_names) / sizeof(s_fault_names[0]); i++)
    {
        if (s_fault_flags & s_fault_names[i].flag)
        {
            printf("  - %s (0x%08X)\n", s_fault_names[i].name, s_fault_names[i].flag);
        }
    }
}