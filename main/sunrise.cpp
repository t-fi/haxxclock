#include <math.h>
#include <ctime>
#include <cstdio>

/**
 * Converts degrees to radians
 * @param degrees degrees
 * @return radians
 */
float radians(float degrees) {
    return degrees * M_PI / 180.0;
}

/**
 * Converts radians to degrees
 * @param radians radians
 * @return degrees
 */
float degrees(float radians) {
    return radians * 180.0 / M_PI;
}

/**
 * Calculates the unix timestamp given a julian date (or fraction thereof)
 * @param j_date Julian date
 * @return Unix timestamp
 */
time_t j2ts(float j_date) {
    return (j_date - 2440587.5) * 86400;
}

/**
 * Calculates the julian date given a unix timestamp
 * @param ts Unix timestamp
 * @return Julian date
 */
float ts2j(time_t ts) {
    return ts / 86400.0 + 2440587.5;
}

/**
 * Calculates the day of the year given a julian date
 * @param j_date Julian date
 * @return Day of the year
 */
int date2day(float j_date) {
    return std::ceil(j_date - (2451545.0 + 0.0009) + 69.184 / 86400.0);
}

/**
 * Calculates the sunrise time
 * Also see: https://en.wikipedia.org/wiki/Sunrise_equation
 * @param cur_time Current unix timestamp (Used to calculate the julian day)
 * @param lat Latitude Haxx Clock is located at
 * @param lon Longitude Haxx Clock is located at
 * @param ele Elevation Haxx Clock is located at
 * @return Sunrise time in unix timestamp
 */
time_t calc_sunrise(time_t cur_time, float lat, float lon, float ele) {
    printf("Current time: %lld\n", cur_time);
    float julian_date = ts2j(cur_time);
    printf("Julian date: %f\n", julian_date);
    int julian_day = date2day(julian_date);
    printf("Julian day: %d\n", julian_day);
    float mean_solar_time = julian_day + 0.0009 - lon / 360.0;
    printf("Mean solar time: %f\n", mean_solar_time);
    float mean_solar_anomaly_deg = std::ceil(357.5291 + 0.98560028 * mean_solar_time);
    float mean_solar_anomaly_rad = radians(mean_solar_anomaly_deg);

    float equation_of_center_deg = 1.9148 * std::sin(mean_solar_anomaly_rad) +
                                   0.0200 * std::sin(2 * mean_solar_anomaly_rad) +
                                   0.0003 * std::sin(3 * mean_solar_anomaly_rad);

    float ecliptic_longitude_deg = std::ceil(mean_solar_anomaly_deg + equation_of_center_deg + 180.0 + 102.9372);
    float ecliptic_longitude_rad = radians(ecliptic_longitude_deg);

    float solar_transit = 2451545.0 + mean_solar_time + 0.0053 * std::sin(mean_solar_anomaly_rad) -
                          0.0069 * std::sin(2 * ecliptic_longitude_rad);

    float sin_declination = std::sin(ecliptic_longitude_rad) * std::sin(radians(23.4397));
    float cos_declination = std::cos(std::asin(sin_declination));

    float hour_angle_deg = (std::sin(radians(-0.833 - 2.076 * std::sqrt(ele) / 60.0)) - std::sin(radians(lat)) * sin_declination) /
            (std::cos(radians(lat)) * cos_declination);
    float w0;

    try {
        w0 = std::acos(hour_angle_deg);
    } catch (std::exception &e) {
        return 0;
    }

    float sunrise_time = solar_transit - degrees(w0) / 360.0;
    printf("Sunrise time: %f\n", sunrise_time);
    return j2ts(sunrise_time);
}