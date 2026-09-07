#include "traffic_light.h"
#include "traffic_controller.h"
#include "rfid.h"
#include "fault_manager.h"
#include "timer.h"
#include "system_tick.h"
#include "common.h"
#include "config.h"
#include <assert.h>
#include <stdio.h>

static int s_tests_passed = 0;
static int s_tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("  PASS: %s\n", message); \
            s_tests_passed++; \
        } else { \
            printf("  FAIL: %s\n", message); \
            s_tests_failed++; \
        } \
    } while(0)

void Test_TrafficLight_Init(void)
{
    printf("\n=== Traffic Light Tests ===\n");
    Status_t status = TrafficLight_Init();
    TEST_ASSERT(status == STATUS_OK, "TrafficLight_Init returns OK");
    
    TrafficLightState_t state = TrafficLight_GetState(APPROACH_NORTH);
    TEST_ASSERT(state == LIGHT_RED, "Initial state is RED");
}

void Test_TrafficLight_SetState(void)
{
    Status_t status = TrafficLight_SetState(APPROACH_NORTH, LIGHT_GREEN);
    TEST_ASSERT(status == STATUS_OK, "SetState returns OK");
    
    TrafficLightState_t state = TrafficLight_GetState(APPROACH_NORTH);
    TEST_ASSERT(state == LIGHT_GREEN, "State changed to GREEN");
}

void Test_TrafficLight_SafeState(void)
{
    TrafficLight_SetAllRed();
    TrafficLight_SetState(APPROACH_NORTH, LIGHT_GREEN);
    TrafficLight_SetState(APPROACH_SOUTH, LIGHT_GREEN);
    TEST_ASSERT(TrafficLight_IsSafeState() == true, "N/S Green is safe");
    
    TrafficLight_SetState(APPROACH_EAST, LIGHT_GREEN);
    TrafficLight_SetState(APPROACH_WEST, LIGHT_GREEN);
    TEST_ASSERT(TrafficLight_IsSafeState() == false, "All Green is unsafe");
}

void Test_TrafficController_Init(void)
{
    printf("\n=== Traffic Controller Tests ===\n");
    Status_t status = TrafficController_Init();
    TEST_ASSERT(status == STATUS_OK, "TrafficController_Init returns OK");
    
    TrafficControllerPhase_t phase = TrafficController_GetPhase();
    TEST_ASSERT(phase == TC_PHASE_NS_GREEN, "Initial phase is NS_GREEN");
}

void Test_TrafficController_Emergency(void)
{
    TrafficController_SetEmergency(APPROACH_EAST);
    TEST_ASSERT(TrafficController_IsInEmergency() == true, "Emergency mode active");
    
    TrafficLightState_t east = TrafficLight_GetState(APPROACH_EAST);
    TrafficLightState_t north = TrafficLight_GetState(APPROACH_NORTH);
    TEST_ASSERT(east == LIGHT_GREEN, "Emergency approach is GREEN");
    TEST_ASSERT(north == LIGHT_RED, "Other approaches are RED");
    
    TrafficController_ClearEmergency();
    TEST_ASSERT(TrafficController_IsInEmergency() == false, "Emergency mode cleared");
}

void Test_RFID_Init(void)
{
    printf("\n=== RFID Tests ===\n");
    Status_t status = RFID_Init();
    TEST_ASSERT(status == STATUS_OK, "RFID_Init returns OK");
}

void Test_RFID_EmergencyTag(void)
{
    const char* tag = EMERGENCY_TAG_ID;
    for (int i = 0; tag[i]; i++)
    {
        RFID_ProcessReceivedByte((uint8_t)tag[i]);
    }
    RFID_ProcessReceivedByte('\r');
    
    TEST_ASSERT(RFID_IsTagReady() == true, "Emergency tag ready");
    TEST_ASSERT(RFID_GetTagType() == RFID_TAG_EMERGENCY, "Tag type is EMERGENCY");
}

void Test_RFID_NormalTag(void)
{
    RFID_Reset();
    const char* tag = "123456789ABC";
    for (int i = 0; tag[i]; i++)
    {
        RFID_ProcessReceivedByte((uint8_t)tag[i]);
    }
    RFID_ProcessReceivedByte('\r');
    
    TEST_ASSERT(RFID_IsTagReady() == true, "Normal tag ready");
    TEST_ASSERT(RFID_GetTagType() == RFID_TAG_NORMAL, "Tag type is NORMAL");
}

void Test_RFID_InvalidTag(void)
{
    RFID_Reset();
    const char* tag = "INVALID!";
    for (int i = 0; tag[i]; i++)
    {
        RFID_ProcessReceivedByte((uint8_t)tag[i]);
    }
    RFID_ProcessReceivedByte('\r');
    
    TEST_ASSERT(RFID_GetTagType() == RFID_TAG_INVALID, "Tag type is INVALID");
}

void Test_FaultManager(void)
{
    printf("\n=== Fault Manager Tests ===\n");
    Status_t status = FaultManager_Init();
    TEST_ASSERT(status == STATUS_OK, "FaultManager_Init returns OK");
    
    FaultManager_SetFault(FAULT_RFID_OVERFLOW);
    TEST_ASSERT(FaultManager_IsFaultActive(FAULT_RFID_OVERFLOW) == true, "Fault set and active");
    
    FaultManager_ClearFault(FAULT_RFID_OVERFLOW);
    TEST_ASSERT(FaultManager_IsFaultActive(FAULT_RFID_OVERFLOW) == false, "Fault cleared");
    
    FaultManager_SetFault(FAULT_SENSOR);
    FaultManager_SetFault(FAULT_TIMER);
    uint32_t faults = FaultManager_GetFaultStatus();
    TEST_ASSERT((faults & FAULT_SENSOR) != 0, "Multiple faults tracked");
    TEST_ASSERT((faults & FAULT_TIMER) != 0, "Multiple faults tracked");
    
    FaultManager_ClearAllFaults();
    TEST_ASSERT(FaultManager_GetFaultStatus() == FAULT_NONE, "All faults cleared");
}

void Test_Timer(void)
{
    printf("\n=== Timer Tests ===\n");
    Status_t status = Timer_Init(TIMER_0, TIMER_MODE_16BIT, 100);
    TEST_ASSERT(status == STATUS_OK, "Timer_Init returns OK");
    
    status = Timer_Start(TIMER_0);
    TEST_ASSERT(status == STATUS_OK, "Timer_Start returns OK");
    
    uint16_t count = Timer_GetCount(TIMER_0);
    TEST_ASSERT(count <= 100, "Timer count initialized");
    
    Timer_Tick(TIMER_0);
    count = Timer_GetCount(TIMER_0);
    TEST_ASSERT(count < 100, "Timer tick decrements count");
}

void Test_SystemTick(void)
{
    printf("\n=== System Tick Tests ===\n");
    Status_t status = SystemTick_Init();
    TEST_ASSERT(status == STATUS_OK, "SystemTick_Init returns OK");
    
    Tick_t start = SystemTick_GetMs();
    SystemTick_Update();
    Tick_t after = SystemTick_GetMs();
    TEST_ASSERT(after >= start, "System tick advances");
}

int main(void)
{
    printf("========================================\n");
    printf("SMART TRAFFIC LIGHT - UNIT TESTS\n");
    printf("========================================\n");
    
    Test_TrafficLight_Init();
    Test_TrafficLight_SetState();
    Test_TrafficLight_SafeState();
    
    Test_TrafficController_Init();
    Test_TrafficController_Emergency();
    
    Test_RFID_Init();
    Test_RFID_EmergencyTag();
    Test_RFID_NormalTag();
    Test_RFID_InvalidTag();
    
    Test_FaultManager();
    Test_Timer();
    Test_SystemTick();
    
    printf("\n========================================\n");
    printf("TEST SUMMARY: %d passed, %d failed\n", s_tests_passed, s_tests_failed);
    printf("========================================\n");
    
    return s_tests_failed == 0 ? 0 : 1;
}