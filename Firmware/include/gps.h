/**
 * @file gps.h
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief 
 * @version 1.0
 * @date 2024-03-04
 * @
 * 
 * @copyright Copyright (c) 2024
 * 
 * @brief This AVR C library is used to interface with the Adafruit Ultimate GPS Breakout module.
 *        NMEA sentences are parsed and the latitude, longitude, speed, heading, and time 
 *        are extracted. Specifically, we are interested in RMC sentences.
 */

#ifndef GPS_H
#define GPS_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// Define max length of NMEA fields
#define MAX_TIME_LEN 6 // hhmmss
#define MAX_LAT_LEN 9 // ddmm.mmmm
#define MAX_LAT_DIR_LEN 1 // D
#define MAX_LON_LEN 10 // dddmm.mmmm
#define MAX_LON_DIR_LEN 1 // D
#define MAX_SPEED_LEN 6 // kk.kkk
#define MAX_HEADING_LEN 6 // ddd.dd

/**
 * @brief Struct to hold data received from GPS
 * 
 */
typedef struct {
    // Add 1 to all char arrays to include null termination
    char time[MAX_TIME_LEN + 1];
    char lat[MAX_LAT_LEN + 1];
    char lat_dir[MAX_LAT_DIR_LEN + 1];
    char lon[MAX_LON_LEN + 1];
    char lon_dir[MAX_LON_DIR_LEN + 1];
    char speed[MAX_SPEED_LEN + 1];
    char heading[MAX_HEADING_LEN + 1];
    bool valid;
} GPSData;

/**
 * @brief Initializes values of GPS struct
 * 
 * @param gps 
 */
void GPS_init(GPSData* gps);

/**
 * @brief Parses received NMEA sentence and stores necessary
 *        info in GPS struct
 * 
 * @param gps 
 * @param sentence 
 */
void GPS_parse_gprmc(GPSData* gps, char* sentence);

/**
 * @brief Sets status of GPS data structure to invalid
 * 
 * @param gps 
 */
void GPS_invalidate(GPSData* gps);

#endif // GPS_H