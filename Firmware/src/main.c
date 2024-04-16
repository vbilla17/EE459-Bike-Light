/**
 * @file main.c
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief Main loop for the project
 * @date 2024-03-04
 *
 */

#include "uart.h"
#include "soft_serial_dbg.h"
#include "gps.h"
#include "adc.h"

#include <avr/io.h>
#include <stdio.h>

#define MAX_NMEA_LENGTH 128
#define OUTPUT_BUFFER_SIZE 128
#define ADC_THRESHOLD 128

int main() {
    // Create GPSData struct and initialize it
    GPSData gps;
    GPS_init(&gps);

    // Create string to hold received NMEA sentence
    char nmea_received[MAX_NMEA_LENGTH];

    // Index to keep track of received NMEA sentence
    uint8_t nmea_index = 0;

    // Flag to indicate if we are done receiving a sentence
    bool sentence_complete = false;

    // Set up PD6 and PD7 as inputs for buttons, active low
    DDRD &= ~((1 << PD6) | (1 << PD7));

    // Set PB7 as output for bike light
    DDRB |= (1 << PB7);

    // Buffer to hold outgoing data
    char outgoing_data[OUTPUT_BUFFER_SIZE];

    // Flags to keep track of button state
    bool but1_pressed = false;
    bool but2_pressed = false;
    bool but1_state = false;
    bool but2_state = false;

    // Counter to control the rate of outgoing data
    uint16_t counter = 0;

    // Initialize UART communication
    uart_init();

    // Initialize software serial communication
    dbg_init();

    // Initialize ADC
    adc_init();

    // Enable global interrupts
    sei();

    // Main loop
    while (1) {
        // If there is data in the receive buffer
        if (uart_available() > 0) {
            // Read a single byte (character) from the receive buffer
            char c = uart_receive_byte();

            // Check if the character is the start of a new sentence
            if ((c == '$') && (nmea_index == 0)) {
                nmea_received[nmea_index++] = c;
                sentence_complete = false;
            } // Check if the character is the end of the sentence
            else if ((c == '\n') && (nmea_index > 0)) {
                nmea_received[nmea_index] = '\n';
                nmea_received[nmea_index + 1] = '\0';
                sentence_complete = true;
            } // Add the character to the sentence
            else if ((nmea_index > 0) && (nmea_index < MAX_NMEA_LENGTH - 1)) {
                nmea_received[nmea_index++] = c;
            }

            // If we have a complete sentence
            if (sentence_complete) {
                // Reset flags and index
                sentence_complete = false;
                nmea_index = 0;

                // Check if the sentence is a GPRMC sentence
                if (strncmp(nmea_received, "$GPRMC", 6) == 0) {
                    // Parse the GPRMC sentence
                    GPS_parse_gprmc(&gps, nmea_received);
                }
            }
        }

        if (PIND & (1 << PD6)) {
            if (!but1_pressed) {
                but1_state = !but1_state;
                but1_pressed = true;
            }
        } else {
            but1_pressed = false;
        }

        if (PIND & (1 << PD7)) {
            if (!but2_pressed) {
                but2_state = !but2_state;
                but2_pressed = true;
            }
        } else {
            but2_pressed = false;
        }

        // Check if it is time to send data summary
        if (counter == 1000) {
            // gps_valid, time, lat, lat_dir, lon, lon_dir, speed, heading, btn1, btn2
            snprintf(outgoing_data, OUTPUT_BUFFER_SIZE, "$%c,%s,%s,%c,%s,%c,%s,%s,%c,%c\n",
                     gps.valid ? '1' : '0', gps.time, gps.lat, gps.lat_dir, gps.lon, gps.lon_dir,
                     gps.speed, gps.heading, but1_state ? '1' : '0', but2_state ? '1' : '0');
            dbg_send_string(outgoing_data);
            counter = 0;
        } else counter++;

        // If ADC value is below threshold
        if (adc_read() < ADC_THRESHOLD) {
            // Turn on bike light
            PORTB |= (1 << PB7);
        } else {
            // Turn off bike light
            PORTB &= ~(1 << PB7);
        }
    }

    return 0;
}
