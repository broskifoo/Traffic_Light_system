#ifndef CONFIG_H
#define CONFIG_H

#define SYSTEM_TICK_MS              10U
#define SYSTICK_TIMER_HZ            (1000U / SYSTEM_TICK_MS)

#define TRAFFIC_GREEN_MIN_MS        3000U
#define TRAFFIC_GREEN_NORMAL_MS     5000U
#define TRAFFIC_GREEN_MAX_MS        7000U
#define TRAFFIC_YELLOW_MS           2000U
#define TRAFFIC_ALL_RED_MS          1000U

#define EMERGENCY_DURATION_MS       5000U
#define EMERGENCY_RECOVERY_MS       2000U

#define RFID_TAG_LENGTH             12U
#define RFID_BUFFER_SIZE            16U
#define EMERGENCY_TAG_ID            "4E006D364C59"

#define LCD_ROWS                    2U
#define LCD_COLS                    16U

#define UART_BAUD_RATE              9600U
#define UART_RX_BUFFER_SIZE         32U
#define UART_TX_BUFFER_SIZE         64U

#define MAX_APPROACHES              4U

#define SCHEDULER_MAX_TASKS         10U

#define WATCHDOG_TIMEOUT_MS         2000U

#define LOG_BUFFER_SIZE             256U
#define MAX_LOG_ENTRIES             1000U

#define FAULT_NONE                  0x00000000U
#define FAULT_RFID_OVERFLOW         0x00000001U
#define FAULT_INVALID_RFID          0x00000002U
#define FAULT_TIMER                 0x00000004U
#define FAULT_SENSOR                0x00000008U
#define FAULT_INVALID_STATE         0x00000010U
#define FAULT_WATCHDOG              0x00000020U
#define FAULT_UART                  0x00000040U
#define FAULT_LCD                   0x00000080U

#define EVENT_QUEUE_SIZE            16U

#endif