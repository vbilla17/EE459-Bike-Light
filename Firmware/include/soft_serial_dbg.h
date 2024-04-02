/**
 * @file soft_serial_dbg.h
 *
 * @author Vishal Billa (vbilla@usc.edu)
 * @date 2024-03-23
 *
 * @brief Header file for software serial debug library. 
 *        Provides functions to send debug messages over software serial. 
 *

 * @copyright Copyright (c) 2024
 */

#ifndef SOFT_SERIAL_DBG_H
#define SOFT_SERIAL_DBG_H

#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>

/* Implementation-specific configurations */

// To use PE0
#define TX_PIN PE0
#define TX_PORT PORTE
#define TX_DDR DDRE

/* Constants */
#define TX_BUFFER_SIZE 256 // Maximum length of the transmit buffer.

/* External variables */
extern volatile char tx_buffer[TX_BUFFER_SIZE]; // Buffer for data to be transmitted.
extern volatile uint8_t tx_head; // Head index for the transmit buffer.
extern volatile uint8_t tx_tail; // Tail index for the transmit buffer.

/**
 * Initializes the software serial debug functionality.
 * Configures the timer and output pin for transmission.
 */
void dbg_init(void);

/**
 * Transmits a single character over software serial.
 * @param c The character to transmit.
 * @return true if the character was added to the transmit buffer successfully, false if the buffer is full.
 */
bool dbg_send_char(char c);

/**
 * Transmits a null-terminated string over software serial.
 * @param str Pointer to the string to be transmitted.
 * @return true if the entire string was successfully added to the transmit buffer, false if the buffer got full.
 */
bool dbg_send_string(const char *str);

#endif // SOFT_SERIAL_DBG_H
