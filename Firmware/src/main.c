/**
 * @file main.c
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief 
 * @version 1.0
 * @date 2024-03-04
 * 
 */

#include "uart.h"
#include "software_serial.h"
#include "gps.h"

#include <stdio.h>

int main() {

    // Initialize UART communication
    uart_init();

    // Initialize software serial communication
    software_serial_init();

    // Create GPSData struct and initialize it
    GPSData gps;
    GPS_init(&gps);

    // Create string to hold data to send over software serial
    char data[100];

    // Main loop
    while (1) {
        // String to hold received NMEA sentence
        char nmea_received[85];

        // Receive NMEA sentence from GPS module
        if (uart_receive_byte() == '$') {
            // Receive first character
            char c;
            c = uart_receive_byte();
            // Receive rest of the sentence
            while (c != '\n') {
                // Add character to string and receive next character
                nmea_received[strlen(nmea_received)] = c;
                c = uart_receive_byte();
            }
        }

        GPS_parse_gprmc(&gps, nmea_received);

        if (GPS_is_valid(&gps)) {
            // Create GPS data string to send over software serial
            sprintf(data, "Time: %s, Lat: %s, Lon: %s, Speed: %s, Heading: %s\n", 
                    GPS_get_time(&gps), GPS_get_lat(&gps), GPS_get_lon(&gps), 
                    GPS_get_speed(&gps), GPS_get_heading(&gps));

            // Send GPS data over software serial
            ss_transmit_string(data);

            // Set GPS data to invalid
            GPS_invalidate(&gps);
        }
    }

    return 0;
}
