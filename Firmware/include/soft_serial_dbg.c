/**
 * @file soft_serial_dbg.c
 *
 * @author Vishal Billa (vbilla@usc.edu)
 * @date 2024-03-23
 *
 * @brief Implementation file for software serial debug library.
 *
 * @copyright Copyright (c) 2024
 */

#include "soft_serial_dbg.h"

// TX pin mask
#define TX_MASK (1 << TX_PIN)

// Transmit buffer and head/tail pointers
volatile char tx_buffer[TX_BUFFER_SIZE];
volatile uint8_t tx_head = 0;
volatile uint8_t tx_tail = 0;

// Transmitting flag
static volatile bool transmitting = false;

// Transmitting byte and bit position for use in ISR
static volatile uint8_t tx_byte = 0;
static volatile uint8_t bit_pos = 0;

void dbg_init() {
    // Set transmit pin as output and pull high.
    TX_DDR |= TX_MASK;
    TX_PORT |= TX_MASK;

    // Configure timer for software serial.
    TCCR1B |= (1 << WGM12); // Set timer to CTC mode.
    TIMSK1 |= (1 << OCIE1A); // Enable CTC interrupt.
    OCR1A = 767; // Set the compare match value for 9600 baud with a 7.3728 MHz clock.
    // OCR1A = 63; // For 115200 baud with 7.3728 MHz clock.
    TCCR1B |= (1 << CS10); // Start the timer with no prescaler.
}

bool dbg_send_char(char c) {
    // Calculate next head index.
    uint8_t next = (tx_head + 1) % TX_BUFFER_SIZE;

    // If buffer is not full, add character to buffer and update head index.
    if (next != tx_tail) {
        tx_buffer[tx_head] = c;
        tx_head = next;
        // If not already transmitting, start transmission.
        if (!transmitting) transmitting = true;
        return true;
    }

    return false; // Buffer is full.
}

bool dbg_send_string(const char *str) {
    bool success = true;
    // Send each character in the string.
    while (*str) {
        if (!dbg_send_char(*str++)) {
            success = false; // Failed to send a character.
            break;
        }
    }
    if (success) {
        // dbg_send_char('\0'); // Send null character to indicate end of string.
    }
    return success;
}

ISR(TIMER1_COMPA_vect) {
    // If currently transmitting data.
    if (transmitting) {
        // If starting a new byte.
        if (bit_pos == 0) {
            // If there's data in the buffer.
            if (tx_head != tx_tail) {
                // Fetch the next byte. and update tail index
                tx_byte = tx_buffer[tx_tail];
                tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE;

                // Start bit (pull TX pin low) and move to next bit position.
                TX_PORT &= ~TX_MASK;
                bit_pos++;
            } // Otherwise, no more data to transmit.
            else {
                transmitting = false;
            }
        } // Transmitting data bits.
        else if (bit_pos <= 8) {
            if (tx_byte & 1) {
                TX_PORT |= TX_MASK; // Send 1 (pull TX pin high).
            } else {
                TX_PORT &= ~TX_MASK; // Send 0 (pull TX pin low).
            }
            // Shift data byte to get next bit.
            tx_byte >>= 1;
            bit_pos++;
        }
        else {
            // Stop bit and reset bit position.
            TX_PORT |= TX_MASK;
            bit_pos = 0;
        }
    }
}
