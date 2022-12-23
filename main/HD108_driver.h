#pragma once

struct led_brightness {
    uint8_t r, g, b;
};

struct led_value {
    // change this to 16 bit eventually
    uint8_t r, g, b;
};

static const std::vector<uint8_t> start_frame(8);
static const std::vector<uint8_t> end_frame(0xFF, 8);


std::vector<uint8_t> build_led_frame(led_brightness brightness, led_value value);

