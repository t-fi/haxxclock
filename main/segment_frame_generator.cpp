#include <ctime>
#include <cstdio>
#include "segment_frame_generator.h"

void add_segment(std::vector<led_value>& values, int leds, led_value color) {
    for (int i = 0; i < leds; ++i) {
        values.push_back(color);
    }
}

std::vector<led_value> get_digit_frame(char digit) {
    uint8_t segments = seven_segment_ascii[(uint8_t)(digit - 32)];
    uint8_t mask = 1;
    std::vector<led_value> values;

    for (int i = 0; i < 8; ++i) {
        if (segments & mask) {
            add_segment(values, 5, led_value{16536, 16536, 16536});
        } else {
            add_segment(values, 5, led_value{0, 0, 0});
        }

        mask <<= 1;
    }

    return values;
}

std::vector<led_value> get_clock_frame(struct tm* time, suseconds_t microseconds) {
    float sec_of_day = (float)(time->tm_hour * 3600 + time->tm_min * 60 + time->tm_sec) + microseconds * 1e-6;
//    printf("seconds of day: %f\n", sec_of_day);
    uint32_t hex_sec_of_day = sec_of_day / 1.318359375;
//    printf("hex sec of day: %x\n", hex_sec_of_day);
    std::vector<char> buffer(5);
    snprintf(buffer.data(), 5, "%X", hex_sec_of_day);

    char hex_sec = buffer[3];
    char hex_min = buffer[2];
    char hex_maxime = buffer[1];
    char hex_hour = buffer[0];

    printf("hex time: %c:%c:%c:%c\n", hex_hour, hex_maxime, hex_min, hex_sec);

    std::vector<led_value> hex_sec_values = get_digit_frame(hex_sec);
    std::vector<led_value> hex_min_values = get_digit_frame(hex_min);
    std::vector<led_value> hex_maxime_values = get_digit_frame(hex_maxime);
    std::vector<led_value> hex_hour_values = get_digit_frame(hex_hour);

    std::vector<led_value> values;
    values.insert(values.end(), hex_sec_values.begin(), hex_sec_values.end());
    values.insert(values.end(), hex_min_values.begin(), hex_min_values.end());
    values.insert(values.end(), hex_maxime_values.begin(), hex_maxime_values.end());
    values.insert(values.end(), hex_hour_values.begin(), hex_hour_values.end());

    return values;
}
