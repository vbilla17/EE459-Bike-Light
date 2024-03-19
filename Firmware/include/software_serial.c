/**
 * @file software_serial.c
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief 
 * @version 1.0
 * @date 2024-03-07
 * 
 */

#include "software_serial.h"

// Global variables for tx pin info
static volatile uint8_t *tx_port, *tx_ddr;
static uint8_t tx_mask;

void software_serial_init() {
    tx_ddr = &SS_TX_DDR;    // Set the transmit pin DDR register
    tx_port = &SS_TX_PORT;  // Set the transmit pin port
    tx_mask = (1 << SS_TX_PIN);  // Set the transmit pin mask
    *tx_ddr |= tx_mask;     // Set the transmit pin as output
}

void ss_transmit_byte(unsigned char data) {
    // Start bit
    *tx_port &= ~tx_mask;
    _delay_us(BIT_TIME);

    // Data bits
    for (uint8_t i = 0; i < 8; i++) {
        if (data & 1) {
            *tx_port |= tx_mask;
        } else {
            *tx_port &= ~tx_mask;
        }
        data >>= 1;
        _delay_us(BIT_TIME);
    }

    // Stop bit
    *tx_port |= tx_mask;
    _delay_us(BIT_TIME);
}

void ss_transmit_string(const char *data) {
    // Transmit each character in the string
    for (uint8_t i = 0; data[i] != '\0'; i++) {
        ss_transmit_byte(data[i]);
    }
}