#ifndef RFID_H
#define RFID_H

#include "common.h"
#include "config.h"

typedef enum
{
    RFID_STATE_IDLE = 0,
    RFID_STATE_RECEIVING = 1,
    RFID_STATE_COMPLETE = 2,
    RFID_STATE_ERROR = 3
} RFIDState_t;

typedef enum
{
    RFID_TAG_VALID = 0,
    RFID_TAG_INVALID = 1,
    RFID_TAG_INCOMPLETE = 2,
    RFID_TAG_EMERGENCY = 3,
    RFID_TAG_NORMAL = 4
} RFIDTagType_t;

typedef struct
{
    char tag_id[RFID_BUFFER_SIZE];
    uint8_t length;
    RFIDState_t state;
    RFIDTagType_t tag_type;
    bool tag_ready;
    uint32_t last_byte_time;
} RFID_t;

Status_t RFID_Init(void);
void RFID_ProcessReceivedByte(uint8_t byte);
bool RFID_IsTagReady(void);
Status_t RFID_GetTag(char* buffer, uint8_t buffer_size);
bool RFID_IsEmergencyTag(const char* tag);
RFIDTagType_t RFID_GetTagType(void);
void RFID_Reset(void);
void RFID_PrintStatus(void);

#endif