#ifndef UART_H
#define UART_H

#include "common.h"
#include "config.h"

typedef enum
{
    UART_PARITY_NONE = 0,
    UART_PARITY_EVEN = 1,
    UART_PARITY_ODD = 2
} UartParity_t;

typedef enum
{
    UART_STOP_BITS_1 = 0,
    UART_STOP_BITS_2 = 1
} UartStopBits_t;

typedef void (*UartRxCallback_t)(uint8_t byte);
typedef void (*UartTxCallback_t)(void);

Status_t UART_Init(uint32_t baud_rate, UartParity_t parity, UartStopBits_t stop_bits);
Status_t UART_SendByte(uint8_t byte);
Status_t UART_SendString(const char* string);
Status_t UART_SendBuffer(const uint8_t* buffer, uint16_t length);
Status_t UART_ReceiveByte(uint8_t* byte);
Status_t UART_SetRxCallback(UartRxCallback_t callback);
Status_t UART_SetTxCallback(UartTxCallback_t callback);
void UART_ProcessRxInterrupt(void);
void UART_ProcessTxInterrupt(void);
bool UART_IsRxBufferEmpty(void);
bool UART_IsTxBufferEmpty(void);
uint16_t UART_GetRxBufferCount(void);
uint16_t UART_GetTxBufferCount(void);
void UART_PrintStatus(void);
Status_t UART_InjectRxByte(uint8_t byte);

#endif