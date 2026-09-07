# Smart Traffic Light System with Emergency Vehicle Priority

## 8051/AT89S52 Firmware Simulation

A portfolio-quality embedded systems project demonstrating a smart traffic light controller with emergency vehicle priority, implemented in Embedded C with PC-based simulation.

---

## Project Overview

This project simulates a complete traffic light control system for a four-way intersection (North, South, East, West) with the following capabilities:

- **Normal Traffic Operation**: Configurable traffic light cycles with dynamic timing based on simulated IR sensor traffic density
- **Emergency Vehicle Priority**: RFID-based emergency vehicle detection with immediate traffic light override
- **Non-blocking Architecture**: Event-driven design using system ticks, no blocking delays
- **Hardware Abstraction**: Clean separation between application logic and hardware simulation
- **Fault Handling**: Comprehensive fault detection and management
- **Watchdog Timer**: Simulated watchdog with timeout recovery
- **Logging & Analysis**: CSV logging with Python analysis tools

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    APPLICATION LAYER                         │
│  ┌─────────────────┐  ┌─────────────────┐                   │
│  │ Emergency Mgr   │  │ Traffic Mgr     │                   │
│  └────────┬────────┘  └────────┬────────┘                   │
└───────────┼────────────────────┼────────────────────────────┘
            │                    │
            ▼                    ▼
┌─────────────────────────────────────────────────────────────┐
│                   DRIVER / HAL LAYER                         │
│  ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐     │
│  │ UART │ │TIMER │ │ GPIO │ │ LCD  │ │ RFID │ │  IR  │     │
│  └──────┘ └──────┘ └──────┘ └──────┘ └──────┘ └──────┘     │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                   SIMULATION LAYER                           │
│         (PC-based hardware behavior emulation)               │
└─────────────────────────────────────────────────────────────┘
```

---

## Key Features

### Traffic Light Control
- Four approaches: North, South, East, West
- Each approach: Red, Yellow, Green LEDs
- Safe state transitions (Green → Yellow → All Red → Next Green)
- Dynamic green duration based on traffic density (3-7 seconds)

### Emergency Vehicle Priority
- EM-18/RDM6300-style RFID reader simulation
- UART-based communication with interrupt-driven reception
- Emergency tag: `4E006D364C59`
- Immediate override: All Red → Emergency Approach Green
- Configurable emergency duration (5 seconds)
- Safe recovery to normal operation

### Peripheral Simulation
- **GPIO**: Port/pin abstraction with simulated registers
- **Timer**: 10ms system tick generation
- **UART**: Serial communication with RX/TX buffers
- **LCD**: 16x2 character display simulation
- **IR Sensors**: Vehicle presence and queue detection
- **Buzzer**: Audio alert simulation

### Software Architecture
- Finite State Machine: INIT → NORMAL → EMERGENCY → RECOVERY → NORMAL
- Event-driven design with event queue
- Cooperative scheduler for periodic tasks
- Fault management with bitmask flags
- No dynamic memory allocation

---

## Build Instructions

```bash
# Build the project
make

# Or compile manually
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude -g -c src/*.c -o build/src/
gcc build/src/*.o -o build/smart_traffic

# Run with default scenario (normal traffic)
./build/smart_traffic

# Run specific scenario
./build/smart_traffic emergency-east
./build/smart_traffic high-traffic
./build/smart_traffic watchdog

# Run unit tests
make test
# Or manually
./build/test_runner
```

---

## Scenarios

| Scenario | Description |
|----------|-------------|
| `normal` | Normal traffic operation with dynamic timing |
| `high-traffic` | High density traffic on all approaches |
| `emergency-north` | Emergency vehicle from North |
| `emergency-south` | Emergency vehicle from South |
| `emergency-east` | Emergency vehicle from East |
| `emergency-west` | Emergency vehicle from West |
| `normal-tag` | Non-emergency RFID tag detected |
| `invalid-rfid` | Invalid/malformed RFID data |
| `rfid-overflow` | RFID buffer overflow fault |
| `sensor-fault` | IR sensor fault detection |
| `watchdog` | Watchdog timeout and recovery |

---

## Project Structure

```
smart_traffic_embedded_simulator/
├── README.md
├── Makefile
├── .gitignore
├── include/
│   ├── config.h
│   ├── common.h
│   ├── app.h
│   ├── traffic_controller.h
│   ├── traffic_light.h
│   ├── gpio.h
│   ├── timer.h
│   ├── system_tick.h
│   ├── uart.h
│   ├── rfid.h
│   ├── lcd.h
│   ├── buzzer.h
│   ├── ir_sensor.h
│   ├── event_manager.h
│   ├── fault_manager.h
│   ├── scheduler.h
│   ├── watchdog.h
│   ├── simulator.h
│   └── logger.h
├── src/
│   ├── main.c
│   ├── app.c
│   ├── traffic_controller.c
│   ├── traffic_light.c
│   ├── gpio.c
│   ├── timer.c
│   ├── system_tick.c
│   ├── uart.c
│   ├── rfid.c
│   ├── lcd.c
│   ├── buzzer.c
│   ├── ir_sensor.c
│   ├── event_manager.c
│   ├── fault_manager.c
│   ├── scheduler.c
│   ├── watchdog.c
│   ├── simulator.c
│   └── logger.c
├── tests/
│   └── test_main.c
├── tools/
│   └── analyze_logs.py
└── logs/
```

---

## Example Output

### Normal Traffic
```
SYSTEM BOOT
-----------
GPIO INIT              [OK]
TRAFFIC LIGHT INIT     [OK]
TRAFFIC CTRL INIT      [OK]
IR SENSOR INIT         [OK]
UART INIT              [OK]
RFID INIT              [OK]
EVENT MGR INIT         [OK]
LCD INIT               [OK]
BUZZER INIT            [OK]
FAULT MGR INIT         [OK]
WATCHDOG INIT          [OK]
SCHEDULER INIT         [OK]
SIMULATOR INIT         [OK]
LOGGER INIT            [OK]

System State: NORMAL_TRAFFIC
Running non-blocking traffic simulation with scheduler...
[TRAFFIC_CTRL] Phase: NS_GREEN
[TRAFFIC_CTRL] Phase: NS_YELLOW
[TRAFFIC_CTRL] Phase: ALL_RED_1
[TRAFFIC_CTRL] Phase: EW_GREEN
```

### Emergency Override
```
=== SCENARIO: EMERGENCY EAST ===
[RFID] Emergency tag detected: 4E006D364C59
[APP] Emergency event received for approach 2
[TRAFFIC_CTRL] EMERGENCY OVERRIDE: Approach 2
[TRAFFIC_LIGHT] NORTH: GREEN -> RED
[TRAFFIC_LIGHT] SOUTH: GREEN -> RED
[TRAFFIC_LIGHT] EAST: RED -> GREEN
[TRAFFIC_LIGHT] WEST: RED -> GREEN
[BUZZER] ALERT mode (3 pulses)
+----------------+
| EMERGENCY        |
| EAST GREEN       |
+----------------+
```

---

## Simulation Summary

At the end of each scenario, a summary is printed:

```
==================================================
SIMULATION SUMMARY
==================================================
Total simulation time: 30.00 sec
RFID tags processed: 3
Emergency tags: 1
Non-emergency tags: 2
Emergency response time: 10 ms
Emergency duration: 5.00 sec
Faults: 0
Unsafe conflicting-light states: 0
==================================================
```

---

## CSV Logging

The system generates a CSV log file at `logs/simulation_log.csv`:

```csv
timestamp_ms,system_state,north_light,south_light,east_light,west_light,traffic_density,rfid_event,fault_status
0,NORMAL_TRAFFIC,GREEN,RED,RED,RED,0,none,0x00000000
5000,NORMAL_TRAFFIC,YELLOW,RED,RED,RED,0,none,0x00000000
6000,EMERGENCY,EAST,0,0x00000000
```

---

## Python Log Analysis

```bash
python tools/analyze_logs.py logs/simulation_log.csv
```

Output:
```
============================================================
LOG ANALYSIS: logs/simulation_log.csv
============================================================

Total events logged: 150
Normal traffic events: 120
Emergency events: 5
Fault events: 2

--- Emergency Events ---
  EAST: 3
  NORTH: 2

--- Traffic State Distribution ---
  NORMAL_TRAFFIC: 120 (80.0%)
  EMERGENCY_OVERRIDE: 25 (16.7%)
  RECOVERY: 5 (3.3%)

--- Traffic Light States ---
  north_light:
    GREEN: 60
    RED: 55
    YELLOW: 25
```

---

## Hardware Porting Plan

| Simulation Module | Real AT89S52 Implementation |
|-------------------|------------------------------|
| `GPIO_WritePin()` | Direct P0/P1/P2/P3 SFR access |
| `Timer_Tick()` | Timer 0/1 interrupt vector |
| `UART_SendByte()` | SBUF register + TI flag |
| `UART_RxInterruptHandler()` | Serial interrupt (0x23) |
| `LCD_Print()` | GPIO + HD44780 protocol |
| `RFID_ProcessReceivedByte()` | UART RX interrupt + buffer |
| `Buzzer_On/Off()` | GPIO pin control |

---

## Test Results

```
========================================
SMART TRAFFIC LIGHT - UNIT TESTS
========================================

=== Traffic Light Tests ===
  PASS: TrafficLight_Init returns OK
  PASS: Initial state is RED
  PASS: SetState returns OK
  PASS: State changed to GREEN
  PASS: N/S Green is safe
  PASS: All Green is unsafe

=== Traffic Controller Tests ===
  PASS: TrafficController_Init returns OK
  PASS: Initial phase is NS_GREEN
  PASS: Emergency mode active
  PASS: Emergency approach is GREEN
  PASS: Other approaches are RED
  PASS: Emergency mode cleared

=== RFID Tests ===
  PASS: RFID_Init returns OK
  PASS: Emergency tag ready
  PASS: Tag type is EMERGENCY
  PASS: Normal tag ready
  PASS: Tag type is NORMAL
  PASS: Tag type is INVALID

=== Fault Manager Tests ===
  PASS: FaultManager_Init returns OK
  PASS: Fault set and active
  PASS: Fault cleared
  PASS: Multiple faults tracked
  PASS: All faults cleared

=== Timer Tests ===
  PASS: Timer_Init returns OK
  PASS: Timer_Start returns OK
  PASS: Timer count initialized
  PASS: Timer tick decrements count

=== System Tick Tests ===
  PASS: SystemTick_Init returns OK
  PASS: System tick advances

========================================
TEST SUMMARY: 30 passed, 0 failed
========================================
```

---

## Interview Questions & Answers

### 1. Why did you choose 8051/AT89S52?
The AT89S52 is a classic 8051-family microcontroller with 8KB Flash, 256B RAM, 32 I/O lines, 3 timers, and UART - ideal for learning embedded fundamentals.

### 2. Why use interrupts for RFID?
The EM-18 RFID reader sends data asynchronously via UART. Interrupts allow immediate processing without polling, reducing latency and CPU overhead.

### 3. What happens inside the ISR?
The UART RX ISR captures the received byte, places it in a circular buffer, and sets a flag. It returns quickly - no protocol parsing in ISR.

### 4. Why should an ISR be short?
Long ISRs block other interrupts, increase interrupt latency, can cause stack overflow, and make timing unpredictable.

### 5. How does UART work?
UART uses asynchronous serial communication with start/stop bits. The 8051 uses SBUF register and RI/TI flags for RX/TX.

### 6. How does the RFID reader communicate?
EM-18 outputs 12-character hex tag ID via UART at 9600 baud when a tag is in range.

### 7. How does the timer work?
Timer 0 in 16-bit mode generates 10ms ticks. The ISR increments a system tick counter used by the scheduler.

### 8. Why avoid delay()?
Blocking delays prevent interrupt response, waste CPU cycles, and make concurrent tasks impossible.

### 9. What is a finite state machine?
An FSM models system behavior as discrete states with defined transitions triggered by events.

### 10. How does your state machine work?
States: INIT → NORMAL_TRAFFIC → EMERGENCY_OVERRIDE → RECOVERY → NORMAL_TRAFFIC. Transitions triggered by timer expiry, RFID events, emergency timeout.

### 11. How do you prevent conflicting green signals?
`TrafficLight_IsSafeState()` checks that at most 2 opposing approaches are GREEN simultaneously. All transitions go through ALL_RED.

### 12. How do IR sensors influence traffic timing?
Vehicle count and queue detection determine green duration: 3s (low), 5s (normal), 7s (high traffic).

### 13. How would this be implemented on a real AT89S52?
Replace simulation modules with SFR register access: P0-P3 for GPIO, TMOD/TH0/TL0 for timers, SCON/SBUF for UART.

### 14. What is an SFR?
Special Function Register - memory-mapped registers controlling hardware peripherals (e.g., P1, TMOD, SCON).

### 15. Difference between polling and interrupts?
Polling: CPU actively checks status in loop. Interrupts: Hardware signals CPU when event occurs.

### 16. What is volatile?
Tells compiler a variable can change unexpectedly (e.g., by ISR or hardware), preventing optimization.

### 17. Why use uint8_t instead of int?
Fixed-width types ensure portability across platforms. int size varies (16/32-bit).

### 18. Why use bitwise operations?
Efficient flag manipulation, register access, and bit-field operations - fundamental in embedded C.

### 19. How did you debug the system?
Console logging, LCD status display, CSV event logging with Python analysis, unit tests.

### 20. How would you test on actual hardware?
Use debugger (JTAG), logic analyzer for timing, oscilloscope for signals, hardware-in-loop simulation.

### 21. What are the limitations of your simulation?
No real-time constraints, no electrical effects, simplified sensor models, no power management.

### 22. How would you port to STM32?
Use HAL/LL libraries, CMSIS-RTOS, adapt peripheral abstractions to ARM Cortex-M registers.

### 23. How would you improve the system?
Add pedestrian crossing, adaptive timing with ML, V2X communication, redundant controllers.

### 24. How would you handle multiple emergency vehicles?
Priority queue, first-come-first-served with direction conflict resolution.

### 25. What happens if RFID communication fails?
Timeout detection, fallback to normal operation, fault logging, watchdog recovery.

---

## Important Notes

**This is a PC-based simulation** - The firmware logic was validated using software simulation scenarios. No physical hardware was used. The architecture is designed for later porting to an AT89S52/8051 microcontroller.

---

## License

MIT License - See LICENSE file for details.