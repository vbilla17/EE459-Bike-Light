/**
 * @file uart.c
 * @brief Implementation file for UART communication library.
 * @date 2024-03-04
 * 
 * @author Vishal Billa (vbilla@usc.edu)
 */

#include "uart.h"

// Initialize receive buffer and head and tail pointers
volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_buffer_head = 0;
volatile uint8_t rx_buffer_tail = 0;

void uart_init(void) {
    // Set baud rate based on the calculated value
    UBRR0H = (uint8_t)(BAUD_REG >> 8);
    UBRR0L = (uint8_t)BAUD_REG;

    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Enable receive interrupt
    UCSR0B |= (1 << RXCIE0);

    // Frame format defaults to 8 data bits, no parity, 1 stop bit
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

uint8_t uart_receive_byte(void) {
    uint8_t data = 0;

    // Protect critical section using shared variables
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // Check if there is data in the receive buffer
        if (rx_buffer_head != rx_buffer_tail) {
            // Read a single byte from the receive buffer and increment the tail pointer
            data = rx_buffer[rx_buffer_tail++];
            rx_buffer_tail %= RX_BUFFER_SIZE;
        }
    }

    return data;
}

uint8_t uart_available(void) {
    uint8_t available_bytes;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // Calculate the number of bytes available in the receive buffer
        available_bytes = (uint8_t)((RX_BUFFER_SIZE + rx_buffer_head - rx_buffer_tail) % RX_BUFFER_SIZE);
    }

    return available_bytes;
}

void uart_flush(void) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        rx_buffer_head = rx_buffer_tail = 0;
    }
}

/**
 * @brief ISR for UART receive complete interrupt.
 * 
 * @details This ISR is called when a byte is received on the UART. It stores the received
 *          byte in the receive buffer, handling buffer overflow by discarding new data if
 *          the buffer is full.
 */
ISR(USART0_RX_vect) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
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
}
