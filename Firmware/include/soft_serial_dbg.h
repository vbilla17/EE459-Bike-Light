/**
 * @file soft_serial_dbg.h
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief Header file for software serial debug library.
 * @date 2024-03-23
 * 
 * @details This library provides a software serial implementation for debugging purposes.
 *          It uses a timer interrupt to transmit data at a fixed baud rate. This should
 *          work on any digital pin on the ATmega328pb microcontroller. The transmit buffer
 *          can be configured to any size by changing the TX_BUFFER_SIZE macro.
 *
 */

#ifndef SOFT_SERIAL_DBG_H
#define SOFT_SERIAL_DBG_H

#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

// TX pin configuration
#define TX_PIN PE0
#define TX_PORT PORTE
#define TX_DDR DDRE

// Buffer size for transmit buffer
#define TX_BUFFER_SIZE 128 // Maximum length of the transmit buffer.

// TX buffer and head/tail pointers
extern volatile char tx_buffer[TX_BUFFER_SIZE]; // Buffer for data to be transmitted.
extern volatile uint8_t tx_head; // Head index for the transmit buffer.
extern volatile uint8_t tx_tail; // Tail index for the transmit buffer.


/**
 * @brief Initializes the software serial debug functionality.
 */
void dbg_init(void);

/**
 * @brief Transmits a single character over software serial.
 *
 * @param c The character to transmit.
 *
 * @return true if the character was added to the transmit buffer successfully, false if the buffer is full.
 */
bool dbg_send_char(char c);

/**
 * @brief Transmits a null-terminated string over software serial.
 *
 * @param str Pointer to the string to be transmitted.
 *
 * @return true if the entire string was successfully added to the transmit buffer, false if the buffer got full.
 */
bool dbg_send_string(const char *str);

void dbg_flush(void);

#endif // SOFT_SERIAL_DBG_H
