/**
 * @file gps.h
 * @brief Header file for GPS parsing library.
 * @date 2024-03-04
 * 
 * @details This library is used to parse data from the Adafruit Ultimate GPS Breakout module.
 *          NMEA sentences are parsed and the latitude, longitude, speed, heading, and time
 *          are extracted. Specifically, we are interested in GPRMC sentences.
 * 
 * @author Vishal Billa (vbilla@usc.edu)
 */

#ifndef GPS_H
#define GPS_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// Define max length of NMEA fields
#define MAX_TIME_LEN 6 // hhmmss
#define MAX_LAT_LEN 9  // ddmm.mmmm
#define MAX_LON_LEN 10 // dddmm.mmmm
#define MAX_SPEED_LEN 6 // kk.kkk
#define MAX_HEADING_LEN 6 // ddd.dd

/**
 * @brief Struct to hold data received from GPS.
 */
typedef struct {
    char time[MAX_TIME_LEN + 1];
    char lat[MAX_LAT_LEN + 1];
    char lat_dir;
    char lon[MAX_LON_LEN + 1];
    char lon_dir;
    char speed[MAX_SPEED_LEN + 1];
    char heading[MAX_HEADING_LEN + 1];
    bool valid;
} GPSData;

/**
 * @brief Initializes values of GPS struct.
 * 
 * @param gps GPS object to initialize.
 */
void GPS_init(GPSData *gps);

/**
 * @brief Parses received NMEA sentence and stores necessary
 *        info in GPS struct.
 * 
 * @param gps Target object for data.
 * @param sentence Input GPRMC sentence.
 */
void GPS_parse_gprmc(GPSData *gps, char *sentence);

/**
 * @brief Sets status of GPS data structure to invalid.
 * 
 * @param gps GPS object to invalidate.
 */
void GPS_invalidate(GPSData *gps);

#endif // GPS_H
