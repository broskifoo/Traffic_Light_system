#ifndef FAULT_MANAGER_H
#define FAULT_MANAGER_H

#include "common.h"
#include "config.h"

#define FAULT_NONE                  0x00000000U
#define FAULT_RFID_OVERFLOW         0x00000001U
#define FAULT_INVALID_RFID          0x00000002U
#define FAULT_TIMER                 0x00000004U
#define FAULT_SENSOR                0x00000008U
#define FAULT_INVALID_STATE         0x00000010U
#define FAULT_WATCHDOG              0x00000020U
#define FAULT_UART                  0x00000040U
#define FAULT_LCD                   0x00000080U

Status_t FaultManager_Init(void);
void FaultManager_SetFault(uint32_t fault_flag);
void FaultManager_ClearFault(uint32_t fault_flag);
bool FaultManager_IsFaultActive(uint32_t fault_flag);
uint32_t FaultManager_GetFaultStatus(void);
void FaultManager_ClearAllFaults(void);
void FaultManager_PrintStatus(void);

#endif