#include "rfid.h"
#include "common.h"
#include "config.h"
#include <stdio.h>
#include <string.h>

static RFID_t s_rfid = {0};
static bool s_initialized = false;

static const char* s_emergency_tag = EMERGENCY_TAG_ID;

static bool IsValidHexChar(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

static void RFID_ResetInternal(void)
{
    memset(s_rfid.tag_id, 0, RFID_BUFFER_SIZE);
    s_rfid.length = 0;
    s_rfid.state = RFID_STATE_IDLE;
    s_rfid.tag_type = RFID_TAG_INVALID;
    s_rfid.tag_ready = false;
    s_rfid.last_byte_time = 0;
}

Status_t RFID_Init(void)
{
    if (s_initialized)
    {
        return STATUS_OK;
    }

    RFID_ResetInternal();
    s_initialized = true;
    printf("[RFID] Initialized (emergency tag: %s)\n", s_emergency_tag);
    return STATUS_OK;
}

void RFID_ProcessReceivedByte(uint8_t byte)
{
    if (!s_initialized)
    {
        return;
    }

    char c = (char)byte;

    if (s_rfid.state == RFID_STATE_IDLE)
    {
        if (IsValidHexChar(c))
        {
            s_rfid.state = RFID_STATE_RECEIVING;
            s_rfid.tag_id[s_rfid.length++] = c;
            s_rfid.last_byte_time = 0;
        }
        return;
    }

    if (s_rfid.state == RFID_STATE_RECEIVING)
    {
        if (IsValidHexChar(c))
        {
            if (s_rfid.length < RFID_BUFFER_SIZE - 1)
            {
                s_rfid.tag_id[s_rfid.length++] = c;
                s_rfid.last_byte_time = 0;
            }
            else
            {
                s_rfid.state = RFID_STATE_ERROR;
                s_rfid.tag_type = RFID_TAG_INVALID;
                printf("[RFID] Buffer overflow!\n");
            }
        }
        else if (c == '\r' || c == '\n')
        {
            if (s_rfid.length >= RFID_TAG_LENGTH)
            {
                s_rfid.tag_id[s_rfid.length] = '\0';
                s_rfid.state = RFID_STATE_COMPLETE;

                if (strcmp(s_rfid.tag_id, s_emergency_tag) == 0)
                {
                    s_rfid.tag_type = RFID_TAG_EMERGENCY;
                    printf("[RFID] Emergency tag detected: %s\n", s_rfid.tag_id);
                }
                else
                {
                    s_rfid.tag_type = RFID_TAG_NORMAL;
                    printf("[RFID] Normal tag detected: %s\n", s_rfid.tag_id);
                }
            }
            else if (s_rfid.length > 0)
            {
                s_rfid.state = RFID_STATE_ERROR;
                s_rfid.tag_type = RFID_TAG_INCOMPLETE;
                printf("[RFID] Incomplete tag (%u chars): %s\n", s_rfid.length, s_rfid.tag_id);
            }
            else
            {
                s_rfid.state = RFID_STATE_IDLE;
            }

            s_rfid.tag_ready = true;
        }
        else
        {
            s_rfid.state = RFID_STATE_ERROR;
            s_rfid.tag_type = RFID_TAG_INVALID;
            printf("[RFID] Invalid character: 0x%02X\n", byte);
        }
    }
}

bool RFID_IsTagReady(void)
{
    if (!s_initialized)
    {
        return false;
    }

    return s_rfid.tag_ready;
}

Status_t RFID_GetTag(char* buffer, uint8_t buffer_size)
{
    if (!s_initialized || !buffer || buffer_size == 0)
    {
        return STATUS_INVALID_PARAM;
    }

    if (!s_rfid.tag_ready)
    {
        return STATUS_ERROR;
    }

    if (s_rfid.length >= buffer_size)
    {
        return STATUS_BUFFER_OVERFLOW;
    }

    strncpy(buffer, s_rfid.tag_id, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';

    RFID_ResetInternal();
    return STATUS_OK;
}

bool RFID_IsEmergencyTag(const char* tag)
{
    if (!tag)
    {
        return false;
    }

    return strcmp(tag, s_emergency_tag) == 0;
}

RFIDTagType_t RFID_GetTagType(void)
{
    return s_rfid.tag_type;
}

void RFID_Reset(void)
{
    RFID_ResetInternal();
}

void RFID_PrintStatus(void)
{
    if (!s_initialized)
    {
        printf("[RFID] Not initialized\n");
        return;
    }

    const char* state_names[] = {"IDLE", "RECEIVING", "COMPLETE", "ERROR"};
    const char* type_names[] = {"VALID", "INVALID", "INCOMPLETE", "EMERGENCY", "NORMAL"};

    printf("[RFID] State: %s, Type: %s, Length: %u, Ready: %s\n",
           state_names[s_rfid.state],
           type_names[s_rfid.tag_type],
           s_rfid.length,
           s_rfid.tag_ready ? "YES" : "NO");

    if (s_rfid.length > 0)
    {
        printf("  Tag ID: %s\n", s_rfid.tag_id);
    }
}