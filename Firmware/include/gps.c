/**
 * @file gps.c
 * @author Vishal Billa (vbilla@usc.edu)
 * @brief 
 * @version 1.0
 * @date 2024-03-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "gps.h"

void GPS_init(GPSData* gps) {
    memset (gps->time, 0, sizeof(gps->time));
    memset (gps->lat, 0, sizeof(gps->lat));
    memset (gps->lat_dir, 0, sizeof(gps->lat_dir));
    memset (gps->lon, 0, sizeof(gps->lon));
    memset (gps->lon_dir, 0, sizeof(gps->lon_dir));
    memset (gps->speed, 0, sizeof(gps->speed));
    memset (gps->heading, 0, sizeof(gps->heading));
    gps->valid = false;
}

void GPS_parse_gprmc(GPSData* GPS, char* sentence) {
    // Check if the sentence is a GPRMC sentence
    if (strncmp(sentence, "$GPRMC", 6) == 0) {
        // Split the sentence into tokens
        char* token = strtok(sentence, ",");
        uint8_t i = 0;
        while (token != NULL) {
            switch (i) {
                case 0: // Sentence type
                    // Don't care about sentence type
                    break;
                case 1: // Time
                    strncpy(GPS->time, token, MAX_TIME_LEN);
                    break;
                case 2: // Validity
                    if (token[0] == 'A') {
                        GPS->valid = true;
                    } else {
                        GPS->valid = false;
                    }
                    break;
                case 3: // Latitude
                    strncpy(GPS->lat, token, MAX_LAT_LEN);
                    break;
                case 4: // Latitude direction
                    strncpy(GPS->lat_dir, token, MAX_LAT_DIR_LEN);
                    break;
                case 5: // Longitude
                    strncpy(GPS->lon, token, MAX_LON_LEN);
                    break;
                case 6: // Longitude direction
                    strncpy(GPS->lon_dir, token, MAX_LON_DIR_LEN);
                    break;
                case 7: // Speed
                    strncpy(GPS->speed, token, MAX_SPEED_LEN);
                    break;
                case 8: // Heading
                    strncpy(GPS->heading, token, MAX_HEADING_LEN);
                    break;
                case 9: // Date
                    // Don't care about date
                    break;
                case 10: // Magnetic variation
                    // Don't care about magnetic variation
                    break;
                case 11: // Magnetic variation direction
                    // Don't care about magnetic variation direction
                    break;
                case 12: // Mode
                    // Don't care about mode
                    break;
                default:
                    break;
            }
            token = strtok(NULL, ",");
            i++;
        }
    }
}

void GPS_invalidate(GPSData* gps) {
    gps->valid = false;
}

char* GPS_get_time(GPSData* gps) {
    return gps->time;
}

char* GPS_get_lat(GPSData* gps) {
    // Combine latitude and direction
    static char lat[MAX_LAT_LEN + MAX_LAT_DIR_LEN + 1];
    strcpy(lat, gps->lat);
    strcat(lat, gps->lat_dir);
    return lat;
}

char* GPS_get_lon(GPSData* gps) {
    // Combine longitude and direction
    static char lon[MAX_LON_LEN + MAX_LON_DIR_LEN + 1];
    strcpy(lon, gps->lon);
    strcat(lon, gps->lon_dir);
    return lon;
}

char* GPS_get_speed(GPSData* gps) {
    return gps->speed;
}

char* GPS_get_heading(GPSData* gps) {
    return gps->heading;
}

bool GPS_is_valid(GPSData* gps) {
    return gps->valid;
}




