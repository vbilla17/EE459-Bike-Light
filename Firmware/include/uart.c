/**
 * @file uart.c
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief 
 * @version 1.0
 * @date 2024-03-04
 * 
 */

#include "uart.h"

void uart_init() {
    // Set baud rate
    UBRR0H = (unsigned char)(BAUD_REG >> 8);
    UBRR0L = (unsigned char)BAUD_REG;

    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Frame format is already set to 8 data bits, no parity, 1 stop bit

    // Enable global interrupts
    sei();
}

void uart_transmit(uint8_t data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));

    // Put data into buffer, sends the data
    UDR0 = data;
}

void uart_transmit_string(const char *data) {
    // Transmit each character in the string
    for (uint8_t i = 0; data[i] != '\0'; i++) {
        uart_transmit(data[i]);
    }
}

uint8_t uart_receive_byte() {
    // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0)));

    // Get and return received data from buffer
    return UDR0;
}

void uart_receive_string(char *data) {
    // Receive each character in the string
    for (uint8_t i = 0; i < 255; i++) {
        data[i] = uart_receive_byte();
        if (data[i] == '\0') {
            break;
        }
    }
}

