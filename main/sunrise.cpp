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
 * @return Julian day (days since 1.1.2000)
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
    float arg_perihelion = 102.9372;
    float earth_tilt = 23.4397;
    float center_coeff_earth = 1.9148;

    float julian_date = ts2j(cur_time);
    int julian_day = date2day(julian_date);
    float mean_solar_time = julian_day - lon / 360.0;
    float solar_mean_anomaly_deg = (int)std::ceil(357.5291 + 0.98560028 * mean_solar_time) % 360;
    float solar_mean_anomaly_rad = radians(solar_mean_anomaly_deg);

    float equation_of_center_deg = center_coeff_earth * std::sin(solar_mean_anomaly_rad) +
                                   0.0200 * std::sin(2 * solar_mean_anomaly_rad) +
                                   0.0003 * std::sin(3 * solar_mean_anomaly_rad);

    float ecliptic_longitude_deg = (int)std::ceil(solar_mean_anomaly_deg + equation_of_center_deg + 180.0 + arg_perihelion) % 360;
    float ecliptic_longitude_rad = radians(ecliptic_longitude_deg);

    float equation_of_time = 0.0053 * std::sin(solar_mean_anomaly_rad) - 0.0069 * std::sin(2 * ecliptic_longitude_rad);
    float solar_transit = 2451545.0 + mean_solar_time + equation_of_time;

    float sin_declination = std::sin(ecliptic_longitude_rad) * std::sin(radians(earth_tilt));
    float cos_declination = std::cos(std::asin(sin_declination));

    float ele_correction = 2.076 * std::sqrt(ele) / 60.0;
    float hour_angle_deg = (std::sin(radians(-0.833 - ele_correction)) - std::sin(radians(lat)) * sin_declination) /
            (std::cos(radians(lat)) * cos_declination);
    float w0;

    try {
        w0 = std::acos(hour_angle_deg);
    } catch (std::exception &e) {
        return 0;
    }

    float sunrise_time = solar_transit - (degrees(w0) / 360.0);
    printf("Sunrise time [julian]: %f\n", sunrise_time);
    return j2ts(sunrise_time);
}