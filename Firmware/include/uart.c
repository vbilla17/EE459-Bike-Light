/**
 * @file uart.c
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief 
 * @version 1.0
 * @date 2024-03-04
 * 
 */

#include "uart.h"

volatile char rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_buffer_head = 0;
volatile uint8_t rx_buffer_tail = 0;

void uart_init() {
    // Set baud rate
    UBRR0H = (unsigned char)(BAUD_REG >> 8);
    UBRR0L = (unsigned char)BAUD_REG;

    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Enable receive interrupt
    UCSR0B |= (1 << RXCIE0);

    // Frame format is already set to 8 data bits, no parity, 1 stop bit

    // Make sure global interrupts are enabled
}

void uart_transmit_byte(uint8_t data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));

    // Put data into buffer, sends the data
    UDR0 = data;
}

void uart_transmit_string(const char *data) {
    // Transmit each character in the string
    while (*data) {
        uart_transmit_byte(*data++);
    }
}

uint8_t uart_receive_byte() {
    if (rx_buffer_head == rx_buffer_tail) {
        return 0; // No data available
    }

    // Get data from buffer
    uint8_t data = rx_buffer[rx_buffer_tail++];
    rx_buffer_tail %= RX_BUFFER_SIZE;

    return data; // Return received data
}

uint8_t uart_available() {
    // Return number of bytes available in buffer
    return (RX_BUFFER_SIZE + rx_buffer_head - rx_buffer_tail) % RX_BUFFER_SIZE;
}

void uart_flush() {
    // Reset buffer head and tail
    rx_buffer_head = rx_buffer_tail = 0;
}

ISR(USART_RX_vect) {
    // Add received data to buffer
    rx_buffer[rx_buffer_head++] = UDR0;

    // Wrap around buffer head
    rx_buffer_head %= RX_BUFFER_SIZE;
}
