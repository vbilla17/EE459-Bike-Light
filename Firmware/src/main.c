/**
 * @file main.c
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief 
 * @version 1.0
 * @date 2024-03-04
 * 
 */

// #include "uart.h"
// #include "software_serial.h"
#include "soft_serial_dbg.h"
// #include "gps.h"

#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main() {

    // Initialize UART communication
    // uart_init();

    // Initialize software serial communication
    // software_serial_init();
    dbg_init();

    // Create GPSData struct and initialize it
    // GPSData gps;
    // GPS_init(&gps);

    // Create string to hold data to send over software serial
    // char data[100];

    // Create counter for life status over software serial
    uint64_t counter = 0;

    // Turn on global interrupts
    sei();

    // Main loop
    while (1) {
        // // String to hold received NMEA sentence
        // char nmea_received[85];

        // // Receive NMEA sentence from GPS module
        // if (uart_receive_byte() == '$') {
        //     // Receive first character
        //     char c;
        //     c = uart_receive_byte();
        //     // Receive rest of the sentence
        //     while (c != '\n') {
        //         // Add character to string and receive next character
        //         nmea_received[strlen(nmea_received)] = c;
        //         c = uart_receive_byte();
        //     }
        // }

        // // ss_transmit_string(nmea_received);
        // dbg_send_string(nmea_received);

        // // Pass received NMEA sentence to GPS parser
        // GPS_parse_gprmc(&gps, nmea_received);

        // // If GPS data is valid, send it over software serial
        // if (GPS_is_valid(&gps)) {
        //     // Create GPS summary data string
        //     sprintf(data, "Time: %s, Lat: %s, Lon: %s, Speed: %s, Heading: %s\n", 
        //             GPS_get_time(&gps), GPS_get_lat(&gps), GPS_get_lon(&gps), 
        //             GPS_get_speed(&gps), GPS_get_heading(&gps));

        //     // Send GPS data over software serial
        //     // ss_transmit_string(data);
        //     dbg_send_string(data);

        //     // Set GPS data to invalid
        //     GPS_invalidate(&gps);
        // } else {
        //     // ss_transmit_string("No GPS data available\n");
        //     dbg_send_string("No GPS data available\n");
        // }

        // Send life status over software serial
        if (counter == 1000) {
            // ss_transmit_string("Hello from software serial!\n");
            dbg_send_char('H');
            // dbg_send_string("Hello from software debug library!\n");
            // println("I'm alive!\n");
            counter = 0;
        }

        // Increment counter
        counter++;
    }

    return 0;
}
