#include "event_manager.h"
#include "system_tick.h"
#include "common.h"
#include "config.h"
#include <stdio.h>
#include <string.h>

static Event_t s_event_queue[EVENT_QUEUE_SIZE];
static uint8_t s_queue_head = 0;
static uint8_t s_queue_tail = 0;
static uint8_t s_queue_count = 0;

static EventCallback_t s_callbacks[8] = {NULL};
static bool s_initialized = false;

static bool EventQueue_Push(const Event_t* event)
{
    if (s_queue_count >= EVENT_QUEUE_SIZE)
    {
        return false;
    }

    s_event_queue[s_queue_head] = *event;
    s_queue_head = (s_queue_head + 1) % EVENT_QUEUE_SIZE;
    s_queue_count++;
    return true;
}

static bool EventQueue_Pop(Event_t* event)
{
    if (s_queue_count == 0)
    {
        return false;
    }

    *event = s_event_queue[s_queue_tail];
    s_queue_tail = (s_queue_tail + 1) % EVENT_QUEUE_SIZE;
    s_queue_count--;
    return true;
}

Status_t EventManager_Init(void)
{
    if (s_initialized)
    {
        return STATUS_OK;
    }

    memset(s_event_queue, 0, sizeof(s_event_queue));
    s_queue_head = s_queue_tail = s_queue_count = 0;
    memset(s_callbacks, 0, sizeof(s_callbacks));
    s_initialized = true;

    printf("[EVENT_MGR] Initialized\n");
    return STATUS_OK;
}

Status_t EventManager_PostEvent(const Event_t* event)
{
    if (!s_initialized || !event)
    {
        return STATUS_INVALID_PARAM;
    }

    Event_t evt = *event;
    evt.timestamp = SystemTick_GetMs();

    if (!EventQueue_Push(&evt))
    {
        return STATUS_BUFFER_OVERFLOW;
    }

    return STATUS_OK;
}

Status_t EventManager_RegisterCallback(EventType_t type, EventCallback_t callback)
{
    if (!s_initialized || type >= 8)
    {
        return STATUS_INVALID_PARAM;
    }

    s_callbacks[type] = callback;
    return STATUS_OK;
}

void EventManager_ProcessEvents(void)
{
    if (!s_initialized)
    {
        return;
    }

    Event_t event;
    while (EventQueue_Pop(&event))
    {
        if (event.type < 8 && s_callbacks[event.type])
        {
            s_callbacks[event.type](&event);
        }
    }
}

void EventManager_PrintStatus(void)
{
    if (!s_initialized)
    {
        printf("[EVENT_MGR] Not initialized\n");
        return;
    }

    printf("[EVENT_MGR] Queue: %u/%u events\n", s_queue_count, EVENT_QUEUE_SIZE);
}