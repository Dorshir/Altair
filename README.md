# 🚀 Project Altair

Nano Satellite Simulator — Embedded STM32 Firmware + C++ Ground Station

---

## 📋 Overview

Project Altair simulates a NanoSat device:
- **STM32 firmware** collects sensor data, handles modes (Normal/Error/Safe), and communicates over UART.
- **C++ Gateway** relays UART communication to multiple TCP clients.
- **Client CLI** sends commands and receives telemetry.

---

## 🛰️ Main Features

- Beaconing and telemetry transmission
- Sensor sampling: Temperature, Humidity, Light, VBat
- SD card logging via FatFS
- Operational mode control (Normal/Error/Safe)
- UART packet-based protocol
- TCP server for client connections

---

## 📡 UART Packet Format

| Field      | Size  | Description         |
|------------|-------|---------------------|
| Length     | 1 B   | Payload + ID + CRC   |
| Packet ID  | 1 B   | Command/Response ID  |
| Payload    | 0-254B| Payload data         |
| Checksum   | 1 B   | XOR checksum         |
| End Byte   | 1 B   | 0x55 constant        |

---

## ⚙️ Build & Flash

### NanoSat Firmware
- STM32CubeIDE Project
- FreeRTOS based
- Flash binary to STM32 board
- UART configured at 115200 baud rate

### Gateway Server + Client
- C++17
- Build with `g++`
