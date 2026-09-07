#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "common.h"
#include "config.h"

typedef enum
{
    EVENT_NONE = 0,
    EVENT_RFID_TAG_RECEIVED = 1,
    EVENT_EMERGENCY_DETECTED = 2,
    EVENT_TIMER_EXPIRED = 3,
    EVENT_SENSOR_CHANGED = 4,
    EVENT_FAULT = 5,
    EVENT_UART_RX = 6,
    EVENT_EMERGENCY_TIMEOUT = 7
} EventType_t;

typedef struct
{
    EventType_t type;
    uint32_t timestamp;
    union
    {
        struct
        {
            char tag_id[RFID_BUFFER_SIZE];
            uint8_t length;
        } rfid;
        struct
        {
            uint8_t approach;
        } emergency;
        struct
        {
            uint32_t fault_flags;
        } fault;
        struct
        {
            uint8_t byte;
        } uart_rx;
    } data;
} Event_t;

typedef void (*EventCallback_t)(const Event_t* event);

Status_t EventManager_Init(void);
Status_t EventManager_PostEvent(const Event_t* event);
Status_t EventManager_RegisterCallback(EventType_t type, EventCallback_t callback);
void EventManager_ProcessEvents(void);
void EventManager_PrintStatus(void);

#endif