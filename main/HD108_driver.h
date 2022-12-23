#pragma once

struct led_value {
    uint16_t r, g, b;
};

static const std::vector<uint8_t> start_frame(8);
static const std::vector<uint8_t> end_frame(0xFF, 8);
static const led_value black {};

std::vector<uint8_t> build_led_frame(led_value value);

