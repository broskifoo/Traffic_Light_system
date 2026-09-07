#include "simulator.h"
#include "app.h"
#include "traffic_controller.h"
#include "ir_sensor.h"
#include "rfid.h"
#include "uart.h"
#include "fault_manager.h"
#include "watchdog.h"
#include "system_tick.h"
#include "common.h"
#include "config.h"
#include <stdio.h>
#include <string.h>

static SimScenario_t s_current_scenario = SIM_SCENARIO_NORMAL;
static bool s_scenario_running = false;

static uint32_t s_metrics_rfid_total = 0;
static uint32_t s_metrics_emergency_tags = 0;
static uint32_t s_metrics_normal_tags = 0;
static uint32_t s_metrics_faults = 0;
static uint32_t s_metrics_emergency_response_ms = 0;
static uint32_t s_metrics_emergency_duration_ms = 0;
static uint32_t s_metrics_unsafe_states = 0;

static void Scenario_Normal(void)
{
    printf("\n=== SCENARIO: NORMAL TRAFFIC ===\n");
    IRSensor_SetTrafficDensity(APPROACH_NORTH, 2);
    IRSensor_SetTrafficDensity(APPROACH_SOUTH, 2);
    IRSensor_SetTrafficDensity(APPROACH_EAST, 2);
    IRSensor_SetTrafficDensity(APPROACH_WEST, 2);
}

static void Scenario_HighTraffic(void)
{
    printf("\n=== SCENARIO: HIGH TRAFFIC ===\n");
    IRSensor_SetTrafficDensity(APPROACH_NORTH, 3);
    IRSensor_SetTrafficDensity(APPROACH_SOUTH, 3);
    IRSensor_SetTrafficDensity(APPROACH_EAST, 8);
    IRSensor_SetTrafficDensity(APPROACH_WEST, 6);
}

static void Scenario_EmergencyNorth(void)
{
    printf("\n=== SCENARIO: EMERGENCY NORTH ===\n");
    IRSensor_SetTrafficDensity(APPROACH_NORTH, 2);
    IRSensor_SetTrafficDensity(APPROACH_SOUTH, 2);
    IRSensor_SetTrafficDensity(APPROACH_EAST, 8);
    IRSensor_SetTrafficDensity(APPROACH_WEST, 4);
    Simulator_InjectEmergencyRFID(APPROACH_NORTH);
}

static void Scenario_EmergencySouth(void)
{
    printf("\n=== SCENARIO: EMERGENCY SOUTH ===\n");
    IRSensor_SetTrafficDensity(APPROACH_NORTH, 2);
    IRSensor_SetTrafficDensity(APPROACH_SOUTH, 2);
    IRSensor_SetTrafficDensity(APPROACH_EAST, 8);
    IRSensor_SetTrafficDensity(APPROACH_WEST, 4);
    Simulator_InjectEmergencyRFID(APPROACH_SOUTH);
}

static void Scenario_EmergencyEast(void)
{
    printf("\n=== SCENARIO: EMERGENCY EAST ===\n");
    IRSensor_SetTrafficDensity(APPROACH_NORTH, 2);
    IRSensor_SetTrafficDensity(APPROACH_SOUTH, 2);
    IRSensor_SetTrafficDensity(APPROACH_EAST, 8);
    IRSensor_SetTrafficDensity(APPROACH_WEST, 4);
    Simulator_InjectEmergencyRFID(APPROACH_EAST);
}

static void Scenario_EmergencyWest(void)
{
    printf("\n=== SCENARIO: EMERGENCY WEST ===\n");
    IRSensor_SetTrafficDensity(APPROACH_NORTH, 2);
    IRSensor_SetTrafficDensity(APPROACH_SOUTH, 2);
    IRSensor_SetTrafficDensity(APPROACH_EAST, 8);
    IRSensor_SetTrafficDensity(APPROACH_WEST, 4);
    Simulator_InjectEmergencyRFID(APPROACH_WEST);
}

static void Scenario_NormalTag(void)
{
    printf("\n=== SCENARIO: NORMAL RFID TAG ===\n");
    Simulator_InjectNormalRFID("123456789ABC");
}

static void Scenario_InvalidRFID(void)
{
    printf("\n=== SCENARIO: INVALID RFID ===\n");
    Simulator_InjectInvalidRFID();
}

static void Scenario_RFIDOverflow(void)
{
    printf("\n=== SCENARIO: RFID BUFFER OVERFLOW ===\n");
    for (uint8_t i = 0; i < RFID_BUFFER_SIZE + 5; i++)
    {
        UART_InjectRxByte('A');
    }
    UART_InjectRxByte('\r');
    UART_ProcessRxInterrupt();
    EventManager_ProcessEvents();
}

static void Scenario_SensorFault(void)
{
    printf("\n=== SCENARIO: SENSOR FAULT ===\n");
    FaultManager_SetFault(FAULT_SENSOR);
    FaultManager_PrintStatus();
}

static void Scenario_Watchdog(void)
{
    printf("\n=== SCENARIO: WATCHDOG TIMEOUT ===\n");
    Simulator_TriggerWatchdogTimeout();
}

static void Scenario_EmergencyDuringGreen(void)
{
    printf("\n=== SCENARIO: EMERGENCY DURING GREEN ===\n");
    Scenario_Normal();
    Simulator_InjectEmergencyRFID(APPROACH_EAST);
}

static void Scenario_EmergencyDuringYellow(void)
{
    printf("\n=== SCENARIO: EMERGENCY DURING YELLOW ===\n");
    Scenario_Normal();
    Simulator_InjectEmergencyRFID(APPROACH_EAST);
}

static void Scenario_EmergencyDuringTransition(void)
{
    printf("\n=== SCENARIO: EMERGENCY DURING TRANSITION ===\n");
    Scenario_Normal();
    Simulator_InjectEmergencyRFID(APPROACH_EAST);
}

static void Scenario_MultipleRFID(void)
{
    printf("\n=== SCENARIO: MULTIPLE RFID EVENTS ===\n");
    Simulator_InjectNormalRFID("111111111111");
    Simulator_InjectEmergencyRFID(APPROACH_NORTH);
    Simulator_InjectNormalRFID("222222222222");
    Simulator_InjectEmergencyRFID(APPROACH_WEST);
}

Status_t Simulator_Init(void)
{
    s_current_scenario = SIM_SCENARIO_NORMAL;
    s_scenario_running = false;

    s_metrics_rfid_total = 0;
    s_metrics_emergency_tags = 0;
    s_metrics_normal_tags = 0;
    s_metrics_faults = 0;
    s_metrics_emergency_response_ms = 0;
    s_metrics_emergency_duration_ms = 0;
    s_metrics_unsafe_states = 0;

    printf("[SIMULATOR] Initialized\n");
    return STATUS_OK;
}

void Simulator_RunScenario(SimScenario_t scenario)
{
    s_current_scenario = scenario;
    s_scenario_running = true;

    switch (scenario)
    {
        case SIM_SCENARIO_NORMAL:
            Scenario_Normal();
            break;
        case SIM_SCENARIO_HIGH_TRAFFIC:
            Scenario_HighTraffic();
            break;
        case SIM_SCENARIO_EMERGENCY_NORTH:
            Scenario_EmergencyNorth();
            break;
        case SIM_SCENARIO_EMERGENCY_SOUTH:
            Scenario_EmergencySouth();
            break;
        case SIM_SCENARIO_EMERGENCY_EAST:
            Scenario_EmergencyEast();
            break;
        case SIM_SCENARIO_EMERGENCY_WEST:
            Scenario_EmergencyWest();
            break;
        case SIM_SCENARIO_NORMAL_TAG:
            Scenario_NormalTag();
            break;
        case SIM_SCENARIO_INVALID_RFID:
            Scenario_InvalidRFID();
            break;
        case SIM_SCENARIO_RFID_OVERFLOW:
            Scenario_RFIDOverflow();
            break;
        case SIM_SCENARIO_SENSOR_FAULT:
            Scenario_SensorFault();
            break;
        case SIM_SCENARIO_WATCHDOG:
            Scenario_Watchdog();
            break;
        case SIM_SCENARIO_EMERGENCY_DURING_GREEN:
            Scenario_EmergencyDuringGreen();
            break;
        case SIM_SCENARIO_EMERGENCY_DURING_YELLOW:
            Scenario_EmergencyDuringYellow();
            break;
        case SIM_SCENARIO_EMERGENCY_DURING_TRANSITION:
            Scenario_EmergencyDuringTransition();
            break;
        case SIM_SCENARIO_MULTIPLE_RFID:
            Scenario_MultipleRFID();
            break;
        default:
            printf("[SIMULATOR] Unknown scenario: %d\n", scenario);
            break;
    }
}

void Simulator_InjectEmergencyRFID(Approach_t approach)
{
    (void)approach;
    const char* emergency_tag = EMERGENCY_TAG_ID;
    for (int i = 0; emergency_tag[i]; i++)
    {
        UART_InjectRxByte((uint8_t)emergency_tag[i]);
    }
    UART_InjectRxByte('\r');
    UART_ProcessRxInterrupt();
    EventManager_ProcessEvents();

    s_metrics_rfid_total++;
    s_metrics_emergency_tags++;
}

void Simulator_InjectNormalRFID(const char* tag_id)
{
    if (!tag_id)
    {
        return;
    }

    for (uint8_t i = 0; tag_id[i]; i++)
    {
        UART_InjectRxByte((uint8_t)tag_id[i]);
    }
    UART_InjectRxByte('\r');
    UART_ProcessRxInterrupt();
    EventManager_ProcessEvents();

    s_metrics_rfid_total++;
    s_metrics_normal_tags++;
}

void Simulator_InjectInvalidRFID(void)
{
    const char* invalid_tag = "INVALID!";
    for (uint8_t i = 0; invalid_tag[i]; i++)
    {
        UART_InjectRxByte((uint8_t)invalid_tag[i]);
    }
    UART_InjectRxByte('\r');
    UART_ProcessRxInterrupt();
    EventManager_ProcessEvents();

    s_metrics_rfid_total++;
}

void Simulator_TriggerWatchdogTimeout(void)
{
    Watchdog_Stop();
    FaultManager_SetFault(FAULT_WATCHDOG);
    s_metrics_faults++;
}

void Simulator_TriggerSensorFault(void)
{
    FaultManager_SetFault(FAULT_SENSOR);
    s_metrics_faults++;
}

void Simulator_PrintMetrics(void)
{
    printf("\n");
    printf("==================================================\n");
    printf("SIMULATION SUMMARY\n");
    printf("==================================================\n");
    printf("Total simulation time: %.2f sec\n", SystemTick_GetMs() / 1000.0);
    printf("RFID tags processed: %u\n", s_metrics_rfid_total);
    printf("Emergency tags: %u\n", s_metrics_emergency_tags);
    printf("Non-emergency tags: %u\n", s_metrics_normal_tags);
    printf("Emergency response time: %u ms\n", s_metrics_emergency_response_ms);
    printf("Emergency duration: %u ms\n", s_metrics_emergency_duration_ms);
    printf("Faults: %u\n", s_metrics_faults);
    printf("Unsafe conflicting-light states: %u\n", s_metrics_unsafe_states);
    printf("==================================================\n");
}