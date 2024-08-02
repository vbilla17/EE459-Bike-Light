# Bike Light Firmware

## Introduction

The Smart Bike Light project aims to enhance the cycling experience by integrating nice-to-have features including GPS ride tracking and logging into an alrady necessary device - the bike light. The firmware is designed for the ATmega328PB microcontroller on the PCB designed for this project and includes libraries for UART communication, software serial, GPS data parsing, and ADC reading.

## Table of Contents

- [Introduction](#introduction)
- [Project Structure](#project-structure)
- [Firmware Overview](#firmware-overview)
  - [UART Communication](#uart-communication)
  - [Software Serial Debugging](#software-serial-debugging)
  - [GPS Data Parsing](#gps-data-parsing)
  - [ADC Reading](#adc-reading)
- [Main Loop Overview](#main-loop-overview)
- [Installation](#installation)

## Project Structure

```plaintext
Firmware/
├── Makefile
├── include/
│   ├── adc.h
│   ├── gps.h
│   ├── soft_serial.h
│   └── uart.h
└── src/
    ├── adc.c
    ├── gps.c
    ├── main.c
    ├── soft_serial.c
    └── uart.c
README.md
```

## Firmware Overview

### UART Communication

The UART library provides functionality for UART TX and buffered RX. The RX buffer size can be adjusted by changing the `RX_BUFFER_SIZE` macro.

#### Functions

- `void uart_init(void)`: Initializes the UART communication interface.
- `void uart_transmit_byte(uint8_t data)`: Transmits a byte of data through UART.
- `void uart_transmit_string(const uint8_t *data)`: Transmits a string of data through UART.
- `uint8_t uart_receive_byte(void)`: Receives a byte of data through UART.
- `uint8_t uart_available(void)`: Checks if data is available to be read from UART.
- `void uart_flush(void)`: Flushes the UART receive buffer.

### Software Serial

The software serial library provides a software-based serial communication solution for debugging and data transmission purposes using a timer interrupt to transmit data at a fixed baud rate.

#### Functions

- `void ss_init(void)`: Initializes the software serial functionality.
- `bool ss_send_char(char c)`: Transmits a single character over software serial.
- `bool ss_send_string(const char *str)`: Transmits a null-terminated string over software serial.
- `void ss_flush(void)`: Flushes the software serial transmit buffer.

### GPS Data Parsing

The GPS library parses data from the Adafruit Ultimate GPS Breakout module, extracting latitude, longitude, speed, heading, and time from NMEA sentences, specifically GPRMC sentences.

#### GPSData Struct

The `GPSData` struct stores parsed GPS information, including:

- `time`: The time in hhmmss format.
- `lat`: The latitude in ddmm.mmmm format.
- `lat_dir`: The latitude direction (N/S).
- `lon`: The longitude in dddmm.mmmm format.
- `lon_dir`: The longitude direction (E/W).
- `speed`: The speed over ground in knots.
- `heading`: The heading in degrees.
- `valid`: A flag indicating whether the data is valid.

#### Functions

- `void GPS_init(GPSData *gps)`: Initializes the GPS struct with default values and marks the data as invalid.
- `void GPS_parse_gprmc(GPSData *gps, char *sentence)`: Parses a received NMEA sentence and stores the extracted information in the `GPSData` struct, updating the validity flag based on the data.
- `void GPS_invalidate(GPSData *gps)`: Sets the status of the GPS data structure to invalid.

### ADC Reading

The ADC library provides functionality to read the ADC value from the ATmega328PB.

#### Functions

- `void adc_init(void)`: Initializes the ADC module.
- `uint8_t adc_read(void)`: Reads the ADC value.

### Main Loop Overview

The main loop integrates all the firmware libraries, handling inputs from the GPS, buttons, and light sensor, and managing outputs like the bike light and data transmission. Here's an intuitive explanation of the main loop:

1. **Initialization**:
   - The `GPSData` struct is created and initialized to hold the GPS data.
   - Strings and flags are set up to manage the NMEA sentence parsing and button states.
   - The microcontroller's I/O pins are configured: PD6 and PD7 as inputs for buttons, and PB7 as an output for the bike light.
   - Buffers for outgoing data are set up, and counters are initialized for timing control.
   - UART, software serial, and ADC functionalities are initialized.
   - Global interrupts are enabled to handle asynchronous events.

2. **Main Loop Execution**:
   - The loop continuously checks if there is new data available from the UART.
   - When a new byte is received, it is added to the `nmea_received` buffer, checking for the start (`$`) and end (`\n`) of an NMEA sentence.
   - Once a complete sentence is received, it is parsed to extract GPS data if it is a GPRMC sentence.
   - Button states are monitored, and their flags are updated accordingly.
   - Every 1000 iterations, the loop sends a summary of the current GPS data and button states via the software serial interface.
   - The ADC value is continuously checked to control the bike light: if the value is below a threshold, the light is turned on; otherwise, it is turned off.

This main loop ensures that all parts of the system work together seamlessly, providing real-time data processing and output control.

## Installation

### Prerequisites

Before installing and building the firmware, ensure you have the following toolchains and hardware ready:

- **AVR-GCC**: The GNU Compiler Collection for AVR microcontrollers.
- **AVRDUDE**: A utility to download/upload/manipulate the ROM and EEPROM contents of AVR microcontrollers.
- **Make**: A build automation tool.
- **USBtinyISP**: A USB programmer for AVR microcontrollers.

To install and flash the Bike Logger firmware to the device, follow these steps:

1. **Clone the Repository**: Clone the project repository to your local machine.

    ```sh
    git clone https://github.com/vbilla17/EE459-Bike-Light.git
    cd EE459-Bike-Light/Firmware
    ```

2. **Build the Project**: Use the `make` command to compile the source files and generate the firmware.

    ```sh
    make all
    ```

3. **Set the Fuses** (only required if it's the first time flashing the PCB): Set the microcontroller fuses to configure the device correctly.

    ```sh
    make fuse
    ```
    
4. **Flash the Firmware**: Flash the compiled firmware to the ATmega328PB microcontroller using the `make flash` command.

    ```sh
    make flash
    ```

5. **Install**: Optionally, you can run the `make install` command to combine the flash and fuse steps. This is useful if you are setting the fuses and flashing the firmware at the same time.

    ```sh
    make install
    ```

6. **Clean Build Artifacts**: To clean up the build artifacts, use the `make clean` command.

    ```sh
    make clean
    ```
