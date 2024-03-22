/**
 * @file uart.h
 * @brief Header file for UART library for ATmega328P processor.
 * @author Vishal Billa (vbilla@usc.edu)
 * @version 1.0
 * @date 2024-03-04
 * 
 * @details This library provides functions to initialize and use UART communication
 *          on the ATmega328P processor.
 * 
 */

#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <avr/interrupt.h>

// Define baud rate
#ifndef BAUD
#define BAUD 9600
#endif

// Define baud rate register value
#define BAUD_REG ((F_CPU / (16UL * BAUD)) - 1)

// Buffer size for received data
#define RX_BUFFER_SIZE 128

// Circular buffer for received data
extern volatile char rx_buffer[RX_BUFFER_SIZE];
extern volatile uint8_t rx_buffer_head;
extern volatile uint8_t rx_buffer_tail;

/**
 * @brief Initializes the UART communication.
 * 
 * This function initializes the UART communication by setting the baud rate and enabling
 * the necessary UART registers.
 */
void uart_init(void);

/**
 * @brief Transmits a byte of data through UART.
 * 
 * @param data The byte of data to be transmitted.
 */
void uart_transmit_byte(unsigned char data);

/**
 * @brief Transmit a string of data through UART.
 * 
 * @param data The string of data to be transmitted.
 */
void uart_transmit_string(const char *data);

/**
 * @brief Receives a byte of data through UART.
 * 
 * @return The byte of data received.
 */
uint8_t uart_receive_byte(void);

/**
 * @brief Checks if data is available to be read from UART.
 * 
 * @return The number of bytes available to be read.
 */
uint8_t uart_available(void);

/**
 * @brief Flushes the UART receive buffer.
 */
void uart_flush(void);

#endif // UART_H
