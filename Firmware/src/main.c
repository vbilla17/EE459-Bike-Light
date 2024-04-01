/**
 * @file main.c
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief 
 * @version 1.0
 * @date 2024-03-04
 * 
 */

#include "uart.h"
#include "soft_serial_dbg.h"
#include "gps.h"

int main() {

    // Initialize UART communication
    uart_init();

    // Initialize software serial communication
    dbg_init();

    // Create GPSData struct and initialize it
    GPSData gps;
    GPS_init(&gps);

    // Create string to hold data to send over software serial
    char data[100];
    memset(data, 0, sizeof(data));

    // Create string to hold received NMEA sentence
    char nmea_received[85];
    memset(nmea_received, 0, sizeof(nmea_received));

    // Index to keep track of received NMEA sentence
    uint8_t nmea_index = 0;

    // Flag to indicate if we are receiving a GPRMC sentence
    bool gprmc_sentence = false;

    // Turn on global interrupts
    sei();

    // Main loop
    while (1) {
        // Check if there is data in the UART receive buffer
        if (uart_available() > 0) {
            // Read data from UART
            char c = uart_receive_byte();

            // Check if the character is the start of a NMEA sentence
            if (c == '$') {
                // Reset the received NMEA sentence
                memset(nmea_received, 0, sizeof(nmea_received));
                nmea_index = 0;
                gprmc_sentence = false;
            }

            // Add the character to the received NMEA sentence
            nmea_received[nmea_index++] = c;

            // Examine the first 6 characters of the NMEA sentence
            if (nmea_index == 6) {
                // Check if the NMEA sentence is a GPRMC sentence
                if (strncmp(nmea_received, "$GPRMC", 6) == 0) {
                    gprmc_sentence = true;
                }
            }

            // Check if the NMEA sentence is complete
            if ((c == '\n') && gprmc_sentence) {
                // Parse the NMEA sentence
                GPS_parse_gprmc(&gps, nmea_received);

                // Check if the GPS data is valid
                if (GPS_is_valid(&gps)) {
                    // Create string to hold GPS data
                    sprintf(data, "Time: %s, Lat: %s %s, Lon: %s %s, Speed: %s, Heading: %s\n",
                            gps.time, gps.lat, gps.lat_dir, gps.lon, gps.lon_dir, gps.speed, gps.heading);

                    // Send GPS data over software serial
                    dbg_send_string(data);

                    // Invalidate GPS data
                    GPS_invalidate(&gps);
                }
                else {
                    // Send invalid GPS data over software serial
                    dbg_send_string("Invalid GPS data\n");
                }
            } else if (c == '\n') {
                dbg_send_string("Not a GPRMC sentence\n");
            }
        }
    }

    return 0;
}
