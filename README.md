# Project Altair

Nano Satellite Simulator — Embedded STM32 Firmware + C++ Ground Station

---

## Overview

Project Altair simulates a NanoSat device:
- **STM32 firmware** collects sensor data, handles modes (Normal/Error/Safe), and communicates over UART.
- **C++ Gateway** relays UART communication to multiple TCP clients.
- **Client CLI** sends commands and receives telemetry.

---

## Main Features

- Beaconing and telemetry transmission
- Sensor sampling: Temperature, Humidity, Light, VBat
- SD card logging via FatFS
- Operational mode control (Normal/Error/Safe)
- UART packet-based protocol
- TCP server for client connections

---

## UART Packet Format

| Field      | Size  | Description         |
|------------|-------|---------------------|
| Length     | 1 B   | Payload + ID + CRC   |
| Packet ID  | 1 B   | Command/Response ID  |
| Payload    | 0-254B| Payload data         |
| Checksum   | 1 B   | XOR checksum         |
| End Byte   | 1 B   | 0x55 constant        |

---

## Main Tasks

| Task                  | Purpose                                              |
|-----------------------|-------------------------------------------------------|
| `Init_Task`            | Boot, wait for time sync, start subsystems            |
| `Sampler_Task`         | Sample sensors + format samples                      |
| `DHT_Task`             | Read DHT sensor (Temperature + Humidity)             |
| `LoggerFeeder_Task`    | Queue sensor logs for SD card writing                |
| `Logger_Task`          | Write logs/events to SD card                         |
| `Controller_Task`      | Evaluate system mode from latest sample              |
| `EventManager_Task`    | Handle system events + indicate with LED/Buzzer      |
| `Communicator_Task`    | Manage UART packet reception and transmission        |

---

## Build & Flash

### NanoSat Firmware
- STM32CubeIDE Project
- FreeRTOS based
- Flash binary to STM32 board
- UART configured at 115200 baud rate

### Gateway Server + Client
- C++17
- Build with `g++`
