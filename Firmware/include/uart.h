/**
 * @file uart.h
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief Header file for buffered UART communication.
 * @date 2024-03-04
 *
 * @details This library provides functionality for UART TX and buffered RX. Designed for
 *          use with the ATmega328pb microcontroller on USART0. The RX buffer size can be
 *          adjusted by changing the RX_BUFFER_SIZE macro. F_CPU must be defined in the
 *          Makefile or in the source code before using this library.
 *
 */

#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

// Set the baud rate
#ifndef BAUD
#define BAUD 9600
#endif

// Calculate the baud rate register value
#define BAUD_REG ((F_CPU / (16UL * BAUD)) - 1)

// Set the receive buffer size
#define RX_BUFFER_SIZE 128

// Declare the receive buffer and head and tail pointers
extern volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
extern volatile uint8_t rx_buffer_head;
extern volatile uint8_t rx_buffer_tail;

/**
 * @brief Initializes the UART communication.
 */
void uart_init(void);

/**
 * @brief Transmits a byte of data through UART.
 * 
 * @param data The byte of data to be transmitted.
 */
void uart_transmit_byte(uint8_t data);

/**
 * @brief Transmit a string of data through UART.
 * 
 * @param data The string of data to be transmitted.
 */
void uart_transmit_string(const uint8_t *data);

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
