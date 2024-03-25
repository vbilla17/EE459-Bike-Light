/**
 * @file soft_serial_dbg.c
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief 
 * @version 1.0
 * @date 2024-03-23
 * 
 */

#include "soft_serial_dbg.h"

// Define pin mask for the transmit pin
#define TX_MASK (1 << TX_PIN)

// Initialize tx buffer
volatile char tx_buffer[TX_BUFFER_SIZE];
volatile uint8_t tx_head = 0;
volatile uint8_t tx_tail = 0;

// Bool to check if transmission is in progress
static volatile bool transmitting = false;

// Variables to store the current byte being transmitted
static volatile uint8_t tx_byte = 0;
static volatile uint8_t bit_pos = 0;

void dbg_init() {
    // Set the transmit pin as output
    TX_DDR |= TX_MASK;

    // Set the transmit pin high
    TX_PORT |= TX_MASK;

    // Configure timer for software serial
    TCCR1B |= (1 << WGM12); // CTC mode
    TIMSK1 |= (1 << OCIE1A); // Enable CTC interrupt
    OCR1A = 768; // 9600 baud with 7.3728 MHz clock
    TCCR1B |= (1 << CS10);  // No prescaler
}

bool dbg_send_char(char c) {
    uint8_t next = (tx_head + 1) % TX_BUFFER_SIZE;
    if (next != tx_tail) {
        tx_buffer[tx_head] = c;
        tx_head = next;
        transmitting = true;
        return true;
    } else {
        return false;
    }
}

bool dbg_send_string(const char *str) {
    bool success = true;
    while (*str) {
        if (!dbg_send_char(*str++)) {
            success = false;
            break;
        }
    }
    return success;
}

ISR(TIMER1_COMPA_vect) {
    if (transmitting) {
        if (bit_pos == 0) {
            if (tx_head != tx_tail) {
                tx_byte = tx_buffer[tx_tail];
                tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE;
                TX_PORT &= ~TX_MASK; // Start bit
                bit_pos++;
            } else {
                // Buffer is empty, stop transmitting
                transmitting = false;
            }

        } else if (bit_pos <= 8) {
            // Send data bits
            if (tx_byte & 1) {
                TX_PORT |= TX_MASK;
            } else {
                TX_PORT &= ~TX_MASK;
            }
            tx_byte >>= 1;
            bit_pos++;
        } else {
            // Send stop bit
            TX_PORT |= TX_MASK;
            bit_pos = 0;
        }
    }
}
