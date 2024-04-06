/**
 * @file uart.c
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief Implementation file for UART communication library
 * @date 2024-03-04
 * 
 */

#include "uart.h"

// Initialize receive buffer and head and tail pointers
volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_buffer_head = 0;
volatile uint8_t rx_buffer_tail = 0;

void uart_init() {
    // Set baud rate based on the calculated value
    UBRR0H = (uint8_t)(BAUD_REG >> 8);
    UBRR0L = (uint8_t)BAUD_REG;

    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Enable receive interrupt
    UCSR0B |= (1 << RXCIE0);

    // Frame format is already set to 8 data bits, no parity, 1 stop bit

    // Make sure global interrupts are enabled in the main program
}

void uart_transmit_byte(uint8_t data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));

    // Put data into buffer, sends the data
    UDR0 = data;
}

void uart_transmit_string(const uint8_t *data) {
    // Transmit each character in the string
    while (*data) {
        uart_transmit_byte(*data++);
    }
}

uint8_t uart_receive_byte() {
    uint8_t data = 0;
    // Check if there is data in the receive buffer
    if (rx_buffer_head != rx_buffer_tail) {
        // Read a single byte from the receive buffer and increment the tail pointer
        data = rx_buffer[rx_buffer_tail++];
        rx_buffer_tail %= RX_BUFFER_SIZE;
    }
    return data;
}

uint8_t uart_available() {
    return (uint8_t)((RX_BUFFER_SIZE + rx_buffer_head - rx_buffer_tail) % RX_BUFFER_SIZE);
}

void uart_flush() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        rx_buffer_head = rx_buffer_tail = 0;
    }
}

ISR(USART0_RX_vect) {
    // Read the received data
    uint8_t data = UDR0;
    // Calculate the next head pointer
    unsigned int next_head = (rx_buffer_head + 1) % RX_BUFFER_SIZE;

    // Check for overflow
    if (next_head != rx_buffer_tail) {
        rx_buffer[rx_buffer_head] = data;
        rx_buffer_head = next_head;
    } else {
        // Buffer overflow, ignore data
    }
}
