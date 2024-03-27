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
    OCR1A = 767; // 9600 baud with 7.3728 MHz clock
    TCCR1B |= (1 << CS10);  // No prescaler
}

bool dbg_send_char(char c) {
    // Next idx is where the head will point to after this write
    uint8_t next = (tx_head + 1) % TX_BUFFER_SIZE;
    // Make sure buffer is not full
    if (next != tx_tail) {
        // Write data to head of buffer and update head
        tx_buffer[tx_head] = c;
        tx_head = next;
        // Start transmitting if not already transmitting
        if (!transmitting) {
            transmitting = true;
        }
        return true;
    } else {
        // Buffer is full
        return false;
    }
}

bool dbg_send_string(const char *str) {
    bool success = true;
    // Send each character in the string
    while (*str) {
        // If a character fails to send, break and return false
        if (!dbg_send_char(*str++)) {
            success = false;
            break;
        }
    }
    return success;
}

ISR(TIMER1_COMPA_vect) {
    // If transmitting, send the next bit
    if (transmitting) {
        // If bit position is 0, that means we're starting a new byte
        if (bit_pos == 0) {
            // If there is data in the buffer, get the next byte
            if (tx_head != tx_tail) {
                // Get next byte from buffer
                tx_byte = tx_buffer[tx_tail];
                // Move tail to next byte
                tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE;
                // Pull TX pin low to start transmission
                TX_PORT &= ~TX_MASK;
                // Increment bit position
                bit_pos++;
            } else {
                // Buffer is empty, stop transmitting
                transmitting = false;
            }
        // If bit position is less than 9, send data bits
        } else if (bit_pos <= 8) {
            // Send data bits
            // If LSB is 1, pull TX pin high
            if (tx_byte & 1) {
                TX_PORT |= TX_MASK;
            } else { // If LSB is 0, pull TX pin low
                TX_PORT &= ~TX_MASK;
            }
            // Shift data byte right and increment bit position
            tx_byte >>= 1;
            bit_pos++;
        } else {
            // Pull TX pin high to stop transmission
            TX_PORT |= TX_MASK;
            // Reset bit position
            bit_pos = 0;
        }
    }

    // To test baud rate, toggle tx pin
    // TX_PORT ^= TX_MASK;
}
