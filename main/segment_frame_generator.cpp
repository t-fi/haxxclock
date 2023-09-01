#include <ctime>
#include <cstdio>
#include <sstream>
#include <ios>
#include "nvs_handle.hpp"
#include "inttypes.h"
#include "map"
#include "segment_frame_generator.h"

led_value led_color = {16536, 16536, 16536};
bool isOn = true;
static std::map<std::string, uint16_t &> color_name_to_ref{{"led_color_r", led_color.r},
                                                           {"led_color_g", led_color.g},
                                                           {"led_color_b", led_color.b}};

void add_segment(std::vector<led_value> &values, int leds, led_value color) {
    for (int i = 0; i < leds; ++i) {
        values.push_back(color);
    }
}

void set_color(char *color, double brightness) {
    int value = 0;
    std::stringstream ss;
    ss << std::hex << color;
    ss >> value;
    brightness *= brightness;

    led_color = {
            .r = (uint16_t) ((((value >> 16) & 0x0000FF) << 8) * brightness),
            .g = (uint16_t) ((((value >> 8) & 0x0000FF) << 8) * brightness),
            .b = (uint16_t) (((value & 0x0000FF) << 8) * brightness)
    };

    write_color_to_nvs();
}

void toggle_clock() {
    isOn = !isOn;
}

void write_color_to_nvs() {
    esp_err_t err;
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
// Handle will automatically close when going out of scope or when it's reset.
    std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle("storage", NVS_READWRITE, &err);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");
        for (const auto &[name, ref]: color_name_to_ref) {
            printf("writing %s to NVS ... ", name.c_str());
            err = handle->set_item(name.c_str(), ref);
            switch (err) {
                case ESP_OK:
                    printf("Done\n");
                    break;
                default :
                    printf("Error (%s) writing!\n", esp_err_to_name(err));
            }
        }
    }
}

void init_color_from_nvs() {
    esp_err_t err;
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    // Handle will automatically close when going out of scope or when it's reset.
    std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle("storage", NVS_READWRITE, &err);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");
        for (const auto &[name, ref]: color_name_to_ref) {
            printf("Reading %s from NVS ... ", name.c_str());
            err = handle->get_item(name.c_str(), ref);
            switch (err) {
                case ESP_OK:
                    printf("Done\n");
                    printf("%s= %" PRIu16 "\n", name.c_str(), ref);
                    break;
                case ESP_ERR_NVS_NOT_FOUND:
                    printf("The value is not initialized yet!\n");
                    break;
                default :
                    printf("Error (%s) reading!\n", esp_err_to_name(err));
            }
        }
    }
}

std::vector<led_value> get_digit_frame(char digit) {
    uint8_t segments = seven_segment_ascii[(uint8_t) (digit - 32)];
    uint8_t mask = 2;
    std::vector<led_value> values;

    for (int i = 0; i < 7; ++i) {
        if ((segments & mask) && isOn) {
            add_segment(values, 5, led_color);
        } else {
            add_segment(values, 5, led_value{0, 0, 0});
        }

        mask <<= 1;
    }

    return values;
}

std::vector<led_value> get_clock_frame(struct tm *time, suseconds_t microseconds) {
//    printf("human time: %d:%d:%d.%d\n", time->tm_hour, time->tm_min, time->tm_sec, microseconds);
    float sec_of_day = (float) (time->tm_hour * 3600 + time->tm_min * 60 + time->tm_sec) + microseconds * 1e-6;
//    printf("seconds of day: %f\n", sec_of_day);
    uint32_t hex_sec_of_day = sec_of_day / 1.318359375;
//    printf("hex sec of day: %x\n", hex_sec_of_day);
    std::vector<char> buffer(5);
    snprintf(buffer.data(), 5, "%04X", hex_sec_of_day);

    char hex_sec = buffer[3];
    char hex_min = buffer[2];
    char hex_maxime = buffer[1];
    char hex_hour = buffer[0];

//    printf("hex time: %c:%c:%c:%c\n", hex_hour, hex_maxime, hex_min, hex_sec);

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
