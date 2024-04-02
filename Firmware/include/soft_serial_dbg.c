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

/* Macro definitions */
#define TX_MASK (1 << TX_PIN) // Define pin mask for the transmit pin.

/* Global variables */
volatile char tx_buffer[TX_BUFFER_SIZE]; // Transmit buffer.
volatile uint8_t tx_head = 0;            // Head index for TX buffer.
volatile uint8_t tx_tail = 0;            // Tail index for TX buffer.
static volatile bool transmitting = false; // Flag to indicate if transmission is in progress.

/* Local variables */
static volatile uint8_t tx_byte = 0; // The current byte being transmitted.
static volatile uint8_t bit_pos = 0; // Position of the bit being transmitted.

/**
 * Initializes the software serial debug.
 */
void dbg_init() {
    TX_DDR |= TX_MASK; // Set the transmit pin as output.
    TX_PORT |= TX_MASK; // Set the transmit pin high.

    // Configure timer for software serial.
    TCCR1B |= (1 << WGM12); // Set timer to CTC mode.
    TIMSK1 |= (1 << OCIE1A); // Enable CTC interrupt.
    OCR1A = 767; // Set the compare match value for 9600 baud with a 7.3728 MHz clock.
    TCCR1B |= (1 << CS10); // Start the timer with no prescaler.
}

/**
 * Sends a character over software serial.
 * @param c The character to send.
 * @return true if character was queued successfully, false if buffer is full.
 */
bool dbg_send_char(char c) {
    uint8_t next = (tx_head + 1) % TX_BUFFER_SIZE; // Calculate next head index.
    
    if (next != tx_tail) { // Check if buffer is not full.
        tx_buffer[tx_head] = c; // Place character in buffer.
        tx_head = next; // Update head index.
        if (!transmitting) { // Start transmission if not already started.
            transmitting = true;
        }
        return true;
    } else {
        return false; // Buffer is full.
    }
}

/**
 * Sends a string over software serial.
 * @param str Pointer to the string to be sent.
 * @return true if entire string was queued successfully, false otherwise.
 */
bool dbg_send_string(const char *str) {
    bool success = true;
    while (*str) { // Send each character in the string.
        if (!dbg_send_char(*str++)) {
            success = false; // Failed to send a character.
            break;
        }
    }
    if (success) {
        dbg_send_char('\0'); // Optionally send null character to indicate end of string.
    }
    return success;
}

/**
 * Interrupt service routine for timer compare match.
 * Handles the transmission of serial data bit by bit.
 */
ISR(TIMER1_COMPA_vect) {
    if (transmitting) {
        if (bit_pos == 0) { // If starting a new byte.
            if (tx_head != tx_tail) { // If there's data in the buffer.
                tx_byte = tx_buffer[tx_tail]; // Fetch the next byte.
                tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE; // Update tail index.
                TX_PORT &= ~TX_MASK; // Start bit (pull TX pin low).
                bit_pos++; // Move to next bit position.
            } else {
                transmitting = false; // No more data to transmit.
            }
        } else if (bit_pos <= 8) { // Transmitting data bits.
            if (tx_byte & 1) {
                TX_PORT |= TX_MASK; // Send 1 (pull TX pin high).
            } else {
                TX_PORT &= ~TX_MASK; // Send 0 (pull TX pin low).
            }
            tx_byte >>= 1; // Prepare next bit for transmission.
            bit_pos++; // Move to next bit position.
        } else {
            TX_PORT |= TX_MASK; // Stop bit (pull TX pin high).
            bit_pos = 0; // Reset for next byte.
        }
    }
}
