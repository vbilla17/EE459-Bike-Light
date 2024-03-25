/**
 * @file soft_serial_dbg.h
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief 
 * @version 1.0
 * @date 2024-03-23
 * 
 */

#ifndef SOFT_SERIAL_DBG_H
#define SOFT_SERIAL_DBG_H

#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>

// Uncomment for 328P Processor
// #define TX_PIN PD3
// #define TX_PORT PORTD
// #define TX_DDR DDRD

// Uncomment for 328PB Processor
#define TX_PIN PE0
#define TX_PORT PORTE
#define TX_DDR DDRE


// Define max length of transmit buffer
#define TX_BUFFER_SIZE 64

// Circular buffer for data to be transmitted
extern volatile char tx_buffer[TX_BUFFER_SIZE];
extern volatile uint8_t tx_buffer_head;
extern volatile uint8_t tx_buffer_tail;

/**
 * @brief Initializes the software serial communication by configuring timemr and output pin.
 * 
 */
void dbg_init(void);

/**
 * @brief Transmits a byte of data through software serial.
 * 
 * @param data The byte of data to be transmitted.
 * 
 * @return true if the data is successfully transmitted, false otherwise.
 */
bool dbg_send_char(char c);

/**
 * @brief Transmit a string of data through software serial.
 * 
 * @param data The string of data to be transmitted.
 * 
 * @return true if the data is successfully transmitted, false otherwise.
 */
bool dbg_send_string(const char *str);

#endif // SOFT_SERIAL_DBG_H