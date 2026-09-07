#include "uart.h"
#include "event_manager.h"
#include "common.h"
#include "config.h"
#include <stdio.h>
#include <string.h>

static uint8_t s_rx_buffer[UART_RX_BUFFER_SIZE];
static uint16_t s_rx_head = 0;
static uint16_t s_rx_tail = 0;
static uint16_t s_rx_count = 0;

static uint8_t s_tx_buffer[UART_TX_BUFFER_SIZE];
static uint16_t s_tx_head = 0;
static uint16_t s_tx_tail = 0;
static uint16_t s_tx_count = 0;

static UartRxCallback_t s_rx_callback = NULL;
static UartTxCallback_t s_tx_callback = NULL;
static bool s_initialized = false;
static uint32_t s_baud_rate = 0;

static bool RxBuffer_Push(uint8_t byte)
{
    if (s_rx_count >= UART_RX_BUFFER_SIZE)
    {
        return false;
    }

    s_rx_buffer[s_rx_head] = byte;
    s_rx_head = (s_rx_head + 1) % UART_RX_BUFFER_SIZE;
    s_rx_count++;
    return true;
}

static bool RxBuffer_Pop(uint8_t* byte)
{
    if (s_rx_count == 0)
    {
        return false;
    }

    *byte = s_rx_buffer[s_rx_tail];
    s_rx_tail = (s_rx_tail + 1) % UART_RX_BUFFER_SIZE;
    s_rx_count--;
    return true;
}

static bool TxBuffer_Push(uint8_t byte)
{
    if (s_tx_count >= UART_TX_BUFFER_SIZE)
    {
        return false;
    }

    s_tx_buffer[s_tx_head] = byte;
    s_tx_head = (s_tx_head + 1) % UART_TX_BUFFER_SIZE;
    s_tx_count++;
    return true;
}

static bool TxBuffer_Pop(uint8_t* byte)
{
    if (s_tx_count == 0)
    {
        return false;
    }

    *byte = s_tx_buffer[s_tx_tail];
    s_tx_tail = (s_tx_tail + 1) % UART_TX_BUFFER_SIZE;
    s_tx_count--;
    return true;
}

Status_t UART_Init(uint32_t baud_rate, UartParity_t parity, UartStopBits_t stop_bits)
{
    (void)parity;
    (void)stop_bits;

    if (s_initialized)
    {
        return STATUS_OK;
    }

    s_baud_rate = baud_rate;
    s_rx_head = s_rx_tail = s_rx_count = 0;
    s_tx_head = s_tx_tail = s_tx_count = 0;
    s_rx_callback = NULL;
    s_tx_callback = NULL;
    s_initialized = true;

    printf("[UART] Initialized (baud: %u)\n", baud_rate);
    return STATUS_OK;
}

Status_t UART_SendByte(uint8_t byte)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (!TxBuffer_Push(byte))
    {
        return STATUS_BUFFER_OVERFLOW;
    }

    putchar(byte);
    fflush(stdout);

    if (s_tx_callback)
    {
        s_tx_callback();
    }

    return STATUS_OK;
}

Status_t UART_SendString(const char* string)
{
    if (!s_initialized || !string)
    {
        return STATUS_INVALID_PARAM;
    }

    while (*string)
    {
        Status_t status = UART_SendByte((uint8_t)*string++);
        if (status != STATUS_OK)
        {
            return status;
        }
    }

    return STATUS_OK;
}

Status_t UART_SendBuffer(const uint8_t* buffer, uint16_t length)
{
    if (!s_initialized || !buffer)
    {
        return STATUS_INVALID_PARAM;
    }

    for (uint16_t i = 0; i < length; i++)
    {
        Status_t status = UART_SendByte(buffer[i]);
        if (status != STATUS_OK)
        {
            return status;
        }
    }

    return STATUS_OK;
}

Status_t UART_ReceiveByte(uint8_t* byte)
{
    if (!s_initialized || !byte)
    {
        return STATUS_INVALID_PARAM;
    }

    return RxBuffer_Pop(byte) ? STATUS_OK : STATUS_ERROR;
}

Status_t UART_SetRxCallback(UartRxCallback_t callback)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    s_rx_callback = callback;
    return STATUS_OK;
}

Status_t UART_SetTxCallback(UartTxCallback_t callback)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    s_tx_callback = callback;
    return STATUS_OK;
}

void UART_ProcessRxInterrupt(void)
{
    while (s_rx_count > 0)
    {
        uint8_t byte;
        if (RxBuffer_Pop(&byte))
        {
            Event_t event = {0};
            event.type = EVENT_UART_RX;
            event.data.uart_rx.byte = byte;
            EventManager_PostEvent(&event);

            if (s_rx_callback)
            {
                s_rx_callback(byte);
            }
        }
        else
        {
            break;
        }
    }
}

void UART_ProcessTxInterrupt(void)
{
    if (s_tx_callback)
    {
        s_tx_callback();
    }
}

bool UART_IsRxBufferEmpty(void)
{
    return s_rx_count == 0;
}

bool UART_IsTxBufferEmpty(void)
{
    return s_tx_count == 0;
}

uint16_t UART_GetRxBufferCount(void)
{
    return s_rx_count;
}

uint16_t UART_GetTxBufferCount(void)
{
    return s_tx_count;
}

void UART_PrintStatus(void)
{
    if (!s_initialized)
    {
        printf("[UART] Not initialized\n");
        return;
    }

    printf("[UART] Status: RX=%u/%u, TX=%u/%u, Baud=%u\n",
           s_rx_count, UART_RX_BUFFER_SIZE, s_tx_count, UART_TX_BUFFER_SIZE, s_baud_rate);
}

Status_t UART_InjectRxByte(uint8_t byte)
{
    if (!s_initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (!RxBuffer_Push(byte))
    {
        return STATUS_BUFFER_OVERFLOW;
    }

    return STATUS_OK;
}