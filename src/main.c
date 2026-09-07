#include "app.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void PrintUsage(const char* program_name)
{
    printf("Usage: %s [scenario]\n", program_name);
    printf("\nScenarios:\n");
    printf("  normal              - Normal traffic operation\n");
    printf("  high-traffic        - High traffic density\n");
    printf("  emergency-north     - Emergency vehicle from North\n");
    printf("  emergency-south     - Emergency vehicle from South\n");
    printf("  emergency-east      - Emergency vehicle from East\n");
    printf("  emergency-west      - Emergency vehicle from West\n");
    printf("  normal-tag          - Non-emergency RFID tag\n");
    printf("  invalid-rfid        - Invalid RFID data\n");
    printf("  rfid-overflow       - RFID buffer overflow\n");
    printf("  sensor-fault        - IR sensor fault\n");
    printf("  watchdog            - Watchdog timeout test\n");
    printf("\n");
}

static Scenario_t ParseScenario(const char* arg)
{
    if (strcmp(arg, "normal") == 0) return SCENARIO_NORMAL;
    if (strcmp(arg, "high-traffic") == 0) return SCENARIO_HIGH_TRAFFIC;
    if (strcmp(arg, "emergency-north") == 0) return SCENARIO_EMERGENCY_NORTH;
    if (strcmp(arg, "emergency-south") == 0) return SCENARIO_EMERGENCY_SOUTH;
    if (strcmp(arg, "emergency-east") == 0) return SCENARIO_EMERGENCY_EAST;
    if (strcmp(arg, "emergency-west") == 0) return SCENARIO_EMERGENCY_WEST;
    if (strcmp(arg, "normal-tag") == 0) return SCENARIO_NORMAL_TAG;
    if (strcmp(arg, "invalid-rfid") == 0) return SCENARIO_INVALID_RFID;
    if (strcmp(arg, "rfid-overflow") == 0) return SCENARIO_RFID_OVERFLOW;
    if (strcmp(arg, "sensor-fault") == 0) return SCENARIO_SENSOR_FAULT;
    if (strcmp(arg, "watchdog") == 0) return SCENARIO_WATCHDOG;
    return SCENARIO_NORMAL;
}

int main(int argc, char* argv[])
{
    Scenario_t scenario = SCENARIO_NORMAL;
    
    if (argc > 1)
    {
        scenario = ParseScenario(argv[1]);
        if (scenario == SCENARIO_NORMAL && strcmp(argv[1], "normal") != 0)
        {
            PrintUsage(argv[0]);
            return 1;
        }
    }
    
    Status_t status = App_Init();
    if (status != STATUS_OK)
    {
        printf("Initialization failed: %d\n", status);
        return 1;
    }
    
    App_SetScenario(scenario);
    
    status = App_Run();
    if (status != STATUS_OK)
    {
        printf("Run failed: %d\n", status);
        App_Shutdown();
        return 1;
    }
    
    App_Shutdown();
    return 0;
}