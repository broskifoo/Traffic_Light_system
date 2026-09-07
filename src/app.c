#include "app.h"
#include "common.h"
#include "config.h"
#include "traffic_light.h"
#include "traffic_controller.h"
#include "ir_sensor.h"
#include "rfid.h"
#include "uart.h"
#include "event_manager.h"
#include "lcd.h"
#include "buzzer.h"
#include "fault_manager.h"
#include "watchdog.h"
#include "scheduler.h"
#include "simulator.h"
#include "logger.h"
#include "gpio.h"
#include "system_tick.h"
#include <stdio.h>

static SystemState_t s_system_state = SYSTEM_INIT;
static Scenario_t s_current_scenario = SCENARIO_NORMAL;
static bool s_initialized = false;
static bool s_tick_registered = false;

static void TrafficController_Callback(void)
{
    TrafficController_Update();
}

static void UartRxEventCallback(const Event_t* event)
{
    if (event->type == EVENT_UART_RX)
    {
        RFID_ProcessReceivedByte(event->data.uart_rx.byte);

        if (RFID_IsTagReady())
        {
            RFIDTagType_t tag_type = RFID_GetTagType();
            if (tag_type == RFID_TAG_EMERGENCY)
            {
                Event_t emergency_event = {0};
                emergency_event.type = EVENT_EMERGENCY_DETECTED;
                char tag_id[RFID_BUFFER_SIZE];
                RFID_GetTag(tag_id, RFID_BUFFER_SIZE);
                emergency_event.data.emergency.approach = APPROACH_EAST;
                EventManager_PostEvent(&emergency_event);
            }
            else if (tag_type == RFID_TAG_NORMAL)
            {
                Event_t rfid_event = {0};
                rfid_event.type = EVENT_RFID_TAG_RECEIVED;
                char tag_id[RFID_BUFFER_SIZE];
                RFID_GetTag(tag_id, RFID_BUFFER_SIZE);
                strncpy(rfid_event.data.rfid.tag_id, tag_id, RFID_BUFFER_SIZE - 1);
                rfid_event.data.rfid.length = strlen(tag_id);
                EventManager_PostEvent(&rfid_event);
            }
        }
    }
}

static void EmergencyEventCallback(const Event_t* event)
{
    if (event->type == EVENT_EMERGENCY_DETECTED)
    {
        printf("[APP] Emergency event received for approach %u\n", event->data.emergency.approach);
        TrafficController_SetEmergency(event->data.emergency.approach);
        LCD_PrintEmergency(event->data.emergency.approach);
        Buzzer_SetMode(BUZZER_ALERT);
        LCD_Update();

        uint32_t response_time = SystemTick_GetMs() - event->timestamp;
        Logger_LogEmergency(event->data.emergency.approach, response_time);
    }
}

static void RfidTagEventCallback(const Event_t* event)
{
    if (event->type == EVENT_RFID_TAG_RECEIVED)
    {
        printf("[APP] Normal RFID tag: %s\n", event->data.rfid.tag_id);
        Logger_LogEvent(LOG_EVENT_RFID_TAG, s_system_state,
                       TrafficLight_GetState(APPROACH_NORTH),
                       TrafficLight_GetState(APPROACH_SOUTH),
                       TrafficLight_GetState(APPROACH_EAST),
                       TrafficLight_GetState(APPROACH_WEST),
                       0, event->data.rfid.tag_id, FaultManager_GetFaultStatus());
    }
}

static void Scheduler_IRSensor_Update(void)
{
    (void)IRSensor_Update();
}

static void Scheduler_Watchdog_Update(void)
{
    Watchdog_Update();
}

static void Scheduler_EventManager_ProcessEvents(void)
{
    EventManager_ProcessEvents();
}

static void Scheduler_LCD_Update(void)
{
    LCD_Update();
}

static void Scheduler_Buzzer_Update(void)
{
    Buzzer_Update();
}

Status_t App_Init(void)
{
    printf("\n");
    printf("========================================\n");
    printf("SMART TRAFFIC LIGHT SYSTEM\n");
    printf("FIRMWARE SIMULATION\n");
    printf("========================================\n");
    printf("\n");
    printf("SYSTEM BOOT\n");
    printf("-----------\n");

    s_system_state = SYSTEM_INIT;
    s_initialized = true;

    Status_t status = TrafficLight_Init();
    if (status != STATUS_OK)
    {
        printf("TRAFFIC LIGHT INIT     [FAIL]\n");
        return status;
    }

    status = TrafficController_Init();
    if (status != STATUS_OK)
    {
        printf("TRAFFIC CTRL INIT      [FAIL]\n");
        return status;
    }

    status = IRSensor_Init();
    if (status != STATUS_OK)
    {
        printf("IR SENSOR INIT         [FAIL]\n");
        return status;
    }

    status = UART_Init(UART_BAUD_RATE, UART_PARITY_NONE, UART_STOP_BITS_1);
    if (status != STATUS_OK)
    {
        printf("UART INIT              [FAIL]\n");
        return status;
    }

    status = RFID_Init();
    if (status != STATUS_OK)
    {
        printf("RFID INIT              [FAIL]\n");
        return status;
    }

    status = EventManager_Init();
    if (status != STATUS_OK)
    {
        printf("EVENT MGR INIT         [FAIL]\n");
        return status;
    }

    status = LCD_Init();
    if (status != STATUS_OK)
    {
        printf("LCD INIT               [FAIL]\n");
        return status;
    }

    status = Buzzer_Init();
    if (status != STATUS_OK)
    {
        printf("BUZZER INIT            [FAIL]\n");
        return status;
    }

    status = FaultManager_Init();
    if (status != STATUS_OK)
    {
        printf("FAULT MGR INIT         [FAIL]\n");
        return status;
    }

    status = Watchdog_Init(WATCHDOG_TIMEOUT_MS);
    if (status != STATUS_OK)
    {
        printf("WATCHDOG INIT          [FAIL]\n");
        return status;
    }

    status = Watchdog_Start();
    if (status != STATUS_OK)
    {
        printf("WATCHDOG START         [FAIL]\n");
        return status;
    }

    status = Scheduler_Init();
    if (status != STATUS_OK)
    {
        printf("SCHEDULER INIT         [FAIL]\n");
        return status;
    }

    status = Simulator_Init();
    if (status != STATUS_OK)
    {
        printf("SIMULATOR INIT         [FAIL]\n");
        return status;
    }

    status = Logger_Init();
    if (status != STATUS_OK)
    {
        printf("LOGGER INIT            [FAIL]\n");
        return status;
    }

    status = SystemTick_Init();
    if (status != STATUS_OK)
    {
        printf("SYSTEM TICK INIT       [FAIL]\n");
        return status;
    }

    status = SystemTick_RegisterCallback(TrafficController_Callback);
    if (status != STATUS_OK)
    {
        printf("SYSTICK CALLBACK REG   [FAIL]\n");
        return status;
    }
    s_tick_registered = true;

    status = EventManager_RegisterCallback(EVENT_UART_RX, UartRxEventCallback);
    if (status != STATUS_OK)
    {
        printf("EVENT UART RX REG      [FAIL]\n");
        return status;
    }

    status = EventManager_RegisterCallback(EVENT_EMERGENCY_DETECTED, EmergencyEventCallback);
    if (status != STATUS_OK)
    {
        printf("EVENT EMERGENCY REG    [FAIL]\n");
        return status;
    }

    status = EventManager_RegisterCallback(EVENT_RFID_TAG_RECEIVED, RfidTagEventCallback);
    if (status != STATUS_OK)
    {
        printf("EVENT RFID REG         [FAIL]\n");
        return status;
    }

    status = Scheduler_AddTask(Scheduler_IRSensor_Update, 20, "IR Sensor");
    if (status != STATUS_OK)
    {
        printf("SCHEDULER IR SENSOR    [FAIL]\n");
        return status;
    }

    status = Scheduler_AddTask(Scheduler_Watchdog_Update, 50, "Watchdog");
    if (status != STATUS_OK)
    {
        printf("SCHEDULER WATCHDOG     [FAIL]\n");
        return status;
    }

    status = Scheduler_AddTask(Scheduler_EventManager_ProcessEvents, 10, "Event Manager");
    if (status != STATUS_OK)
    {
        printf("SCHEDULER EVENT MGR    [FAIL]\n");
        return status;
    }

    status = Scheduler_AddTask(Scheduler_LCD_Update, 200, "LCD Update");
    if (status != STATUS_OK)
    {
        printf("SCHEDULER LCD          [FAIL]\n");
        return status;
    }

    status = Scheduler_AddTask(Scheduler_Buzzer_Update, 10, "Buzzer");
    if (status != STATUS_OK)
    {
        printf("SCHEDULER BUZZER       [FAIL]\n");
        return status;
    }

    printf("GPIO INIT              [OK]\n");
    printf("TRAFFIC LIGHT INIT     [OK]\n");
    printf("TRAFFIC CTRL INIT      [OK]\n");
    printf("IR SENSOR INIT         [OK]\n");
    printf("UART INIT              [OK]\n");
    printf("RFID INIT              [OK]\n");
    printf("EVENT MGR INIT         [OK]\n");
    printf("LCD INIT               [OK]\n");
    printf("BUZZER INIT            [OK]\n");
    printf("FAULT MGR INIT         [OK]\n");
    printf("WATCHDOG INIT          [OK]\n");
    printf("SCHEDULER INIT         [OK]\n");
    printf("SIMULATOR INIT         [OK]\n");
    printf("LOGGER INIT            [OK]\n");
    printf("TIMER INIT             [OK]\n");
    printf("SYSTEM TICK INIT       [OK]\n");
    printf("\n");
    printf("System State: INIT\n");
    printf("Scenario: NORMAL\n");
    printf("\n");
    printf("Initialization complete.\n");
    printf("System ready.\n");

    TrafficLight_PrintStatus();
    SystemTick_PrintStatus();
    IRSensor_PrintStatus();
    printf("\n");
    printf("Setting traffic densities...\n");
    IRSensor_SetTrafficDensity(APPROACH_NORTH, 2);
    IRSensor_SetTrafficDensity(APPROACH_SOUTH, 2);
    IRSensor_SetTrafficDensity(APPROACH_EAST, 8);
    IRSensor_SetTrafficDensity(APPROACH_WEST, 4);
    printf("North: Low (2), South: Low (2), East: High (8), West: Medium (4)\n");
    printf("\n");
    printf("Testing safe transitions...\n");
    TrafficLight_SetState(APPROACH_NORTH, LIGHT_GREEN);
    TrafficLight_SetState(APPROACH_SOUTH, LIGHT_GREEN);
    printf("North/South: GREEN\n");
    printf("Safe state: %s\n", TrafficLight_IsSafeState() ? "YES" : "NO");

    TrafficLight_SetState(APPROACH_NORTH, LIGHT_YELLOW);
    TrafficLight_SetState(APPROACH_SOUTH, LIGHT_YELLOW);
    printf("North/South: YELLOW\n");

    TrafficLight_SetAllRed();
    printf("All Red\n");

    TrafficLight_SetState(APPROACH_EAST, LIGHT_GREEN);
    TrafficLight_SetState(APPROACH_WEST, LIGHT_GREEN);
    printf("East/West: GREEN\n");
    printf("Safe state: %s\n", TrafficLight_IsSafeState() ? "YES" : "NO");

    TrafficLight_SetAllRed();
    printf("All Red (safe transition)\n");

    printf("\n");
    printf("Testing emergency override...\n");
    TrafficController_SetEmergency(APPROACH_EAST);
    printf("Emergency active: %s\n", TrafficController_IsInEmergency() ? "YES" : "NO");
    TrafficController_ClearEmergency();
    printf("Emergency active: %s\n", TrafficController_IsInEmergency() ? "YES" : "NO");

    printf("\n");
    printf("Testing UART...\n");
    UART_SendString("UART TEST: Hello World\r\n");
    UART_PrintStatus();

    printf("\n");
    printf("Testing LCD...\n");
    LCD_PrintStatus("NORMAL MODE", "NS GREEN 3000ms");
    LCD_Update();

    printf("\n");
    printf("Testing Buzzer...\n");
    Buzzer_On();
    Buzzer_PrintStatus();
    Buzzer_Off();
    Buzzer_PrintStatus();
    Buzzer_SetMode(BUZZER_ALERT);
    Buzzer_PrintStatus();

    printf("\n");
    printf("Testing Fault Manager...\n");
    FaultManager_SetFault(FAULT_RFID_OVERFLOW);
    FaultManager_SetFault(FAULT_SENSOR);
    FaultManager_PrintStatus();
    FaultManager_ClearFault(FAULT_RFID_OVERFLOW);
    FaultManager_PrintStatus();
    FaultManager_ClearAllFaults();
    FaultManager_PrintStatus();

    printf("\n");
    printf("Testing Watchdog...\n");
    Watchdog_PrintStatus();
    Watchdog_Kick();
    Watchdog_PrintStatus();

    printf("\n");
    printf("Testing RFID...\n");
    printf("Injecting emergency tag...\n");
    const char* emergency_tag = EMERGENCY_TAG_ID;
    for (int i = 0; emergency_tag[i]; i++)
    {
        RFID_ProcessReceivedByte((uint8_t)emergency_tag[i]);
    }
    RFID_ProcessReceivedByte('\r');
    RFID_PrintStatus();

    printf("\nInjecting normal tag...\n");
    RFID_Reset();
    const char* normal_tag = "123456789ABC";
    for (int i = 0; normal_tag[i]; i++)
    {
        RFID_ProcessReceivedByte((uint8_t)normal_tag[i]);
    }
    RFID_ProcessReceivedByte('\r');
    RFID_PrintStatus();

    printf("\nInjecting invalid tag...\n");
    RFID_Reset();
    const char* invalid_tag = "INVALID!";
    for (int i = 0; invalid_tag[i]; i++)
    {
        RFID_ProcessReceivedByte((uint8_t)invalid_tag[i]);
    }
    RFID_ProcessReceivedByte('\r');
    RFID_PrintStatus();

    return STATUS_OK;
}

static void TestUartRxInterrupt(void)
{
    printf("\nTesting UART RX Interrupt -> RFID -> Emergency...\n");
    RFID_Reset();
    const char* emergency_tag = EMERGENCY_TAG_ID;
    for (int i = 0; emergency_tag[i]; i++)
    {
        UART_InjectRxByte((uint8_t)emergency_tag[i]);
    }
    UART_InjectRxByte('\r');
    UART_ProcessRxInterrupt();
    EventManager_ProcessEvents();
}

Status_t App_Run(void)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    s_system_state = NORMAL_TRAFFIC;
    printf("\nSystem State: NORMAL_TRAFFIC\n");
    printf("Running non-blocking traffic simulation with scheduler...\n");
    printf("Press Ctrl+C to exit.\n");

    Simulator_RunScenario((SimScenario_t)s_current_scenario);

    for (int i = 0; i < 5000; i++)
    {
        SystemTick_Update();
        Scheduler_Update();
    }

    TestUartRxInterrupt();

    SystemTick_PrintStatus();
    TrafficLight_PrintStatus();
    IRSensor_PrintStatus();
    UART_PrintStatus();
    RFID_PrintStatus();
    EventManager_PrintStatus();
    Buzzer_PrintStatus();
    FaultManager_PrintStatus();
    Watchdog_PrintStatus();
    Scheduler_PrintStatus();
    LCD_Update();
    Simulator_PrintMetrics();

    return STATUS_OK;
}

void App_Shutdown(void)
{
    printf("\nShutting down...\n");
    if (s_tick_registered)
    {
        SystemTick_UnregisterCallback(TrafficController_Callback);
    }
    Logger_Close();
    s_initialized = false;
}

SystemState_t App_GetState(void)
{
    return s_system_state;
}

void App_SetScenario(Scenario_t scenario)
{
    s_current_scenario = scenario;
}

void App_InjectEmergencyRFID(Approach_t approach)
{
    TrafficController_SetEmergency(approach);
}

void App_InjectNormalRFID(const char* tag_id)
{
    (void)tag_id;
}

void App_InjectInvalidRFID(void)
{
}

void App_TriggerWatchdogFault(void)
{
}

void App_TriggerSensorFault(void)
{
}