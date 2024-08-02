/**
 * @file gps.c
 * @brief Implementation file for GPS parsing library.
 * @date 2024-03-04
 * 
 * @author Vishal Billa (vbilla@usc.edu)
 */

#include "gps.h"

void GPS_init(GPSData *gps) {
    // Initialize all fields to 0
    memset(gps->time, 0, sizeof(gps->time));
    memset(gps->lat, 0, sizeof(gps->lat));
    gps->lat_dir = '0';
    memset(gps->lon, 0, sizeof(gps->lon));
    gps->lon_dir = '0';
    memset(gps->speed, 0, sizeof(gps->speed));
    memset(gps->heading, 0, sizeof(gps->heading));
    gps->valid = false;
}

void GPS_parse_gprmc(GPSData *gps, char *sentence) {
    // Declare variables to hold parsed values, length needs to be larger than the max field length, I arbitrarily chose 15
    char utc[15] = {0};
    char status = '\0';
    char lat[15] = {0};
    char lat_dir = '\0';
    char lon[15] = {0};
    char lon_dir = '\0';
    char speed[15] = {0};
    char heading[15] = {0};
    char mode = '\0';

    // Check if the sentence is a valid RMC sentence, should be redundant but just in case
    if (strncmp(sentence, "$GPRMC", 6) != 0) {
        gps->valid = false;
        return;
    }

    // $GPRMC,utc,status,lat,N/S,lon,E/W,speed,hdg,date,mag,var,mode*cs
    uint8_t fields = sscanf(sentence, "$GPRMC,%15[^,],%c,%15[^,],%c,%15[^,],%c,%15[^,],%15[^,],%*[^,],,,%c",
                            utc, &status, lat, &lat_dir, lon, &lon_dir, speed, heading, &mode);

    // Check if we read all necessary fields, data is valid, and mode is not "Data not valid"
    if ((fields == 9) && ((status == 'A') && (mode != 'N'))) {
        // Copy parsed values to GPS struct and set valid flag
        strncpy(gps->time, utc, MAX_TIME_LEN);
        strncpy(gps->lat, lat, MAX_LAT_LEN);
        gps->lat_dir = lat_dir;
        strncpy(gps->lon, lon, MAX_LON_LEN);
        gps->lon_dir = lon_dir;
        strncpy(gps->speed, speed, MAX_SPEED_LEN);
        strncpy(gps->heading, heading, MAX_HEADING_LEN);
        gps->valid = true;
    } else {
        // Invalidate data
        gps->valid = false;
    }
}

void GPS_invalidate(GPSData *gps) {
    gps->valid = false;
}
