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
#include <avr/io.h>

#include <util/delay.h>

#define MAX_NMEA_LENGTH 128

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

    char outgoing_data[128];

    // Initialize UART communication
    uart_init();

    // Initialize software serial communication
    dbg_init();

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

                    // If the GPS data is valid, print a summary
                    if (gps.valid) {
                        // char summary[128];
                        // snprintf(summary, 128, "Lat: %s %c, Lon: %s %c, Speed: %s, Heading: %s\n",
                        //             gps.lat, gps.lat_dir, gps.lon, gps.lon_dir,
                        //             gps.speed, gps.heading);
                        // dbg_send_string(summary);

                        // $GPS,time,lat,lat_dir,lon,lon_dir,speed,heading,
                        snprintf(outgoing_data, 128, "$GPS,%s,%s,%c,%s,%c,%s,%s",
                                 gps.time, gps.lat, gps.lat_dir, gps.lon, gps.lon_dir,
                                 gps.speed, gps.heading);
                        dbg_send_string(outgoing_data);
                        GPS_invalidate(&gps);
                    } else {
                        // If the GPS data is invalid, print an error message
                        // dbg_send_string("Invalid GPS data!\n");
                    }
                } else {
                    // Not a GPRMC sentence, ignore
                }
            }
        }
        if (!(PIND & (1 << PD6))) {
            dbg_send_string("$BTN,1");
        }
        if (!(PIND & (1 << PD7))) {
            dbg_send_string("$BTN,2");
        }
    }

    return 0;
}
