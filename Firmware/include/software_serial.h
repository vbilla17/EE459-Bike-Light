/**
 * @file software_serial.h
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief Software Serial Library for the ATmega328P
 * @version 1.0
 * @date 2024-03-07
 * 
 * @details This library provides functions to initialize and use software serial communication
 */

#ifndef SOFTWARE_SERIAL_H
#define SOFTWARE_SERIAL_H

#include <avr/io.h>
#include <util/delay.h>

#define SS_BAUD 9600
#define BIT_TIME (1000000UL / SS_BAUD)

// #define SS_TX_PORT PORTB
// #define SS_TX_DDR DDRB
// #define SS_TX_PIN PB5

// USE PD2 for software serial
#define SS_TX_PORT PORTD
#define SS_TX_DDR DDRD
#define SS_TX_PIN PD3

/**
 * @brief Initializes software serial on the ATmega328P
 * 
 */
void software_serial_init();

/**
 * @brief Transmits a byte over software serial
 * 
 * @param data 
 */
void ss_transmit_byte(unsigned char data);

/**
 * @brief Transmits a string over software serial
 * 
 * @param data 
 * 
 */
void ss_transmit_string(const char *data);

#endif // SOFTWARE_SERIAL_H