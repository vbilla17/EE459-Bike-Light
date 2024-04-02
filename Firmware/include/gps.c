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
    char mode_indicator = 0;
    char pos_status = 0;
    int scan_result = sscanf(sentence, "$GPRMC,%6s,%c,%9s,%c,%10s,%c,%7s,%6s,%*s,%*s,%*s,%c,%*s",
                             GPS->time, &pos_status, GPS->lat, GPS->lat_dir, GPS->lon, GPS->lon_dir,
                             GPS->speed, GPS->heading, &mode_indicator);
    if ((scan_result == 9) && (pos_status == 'A') && (mode_indicator != 'N')) {
        GPS->valid = true;
    } else {
        GPS->valid = false;
    }
}

void GPS_invalidate(GPSData* gps) {
    gps->valid = false;
}
