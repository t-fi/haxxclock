#include <vector>
#include "HD108_driver.h"

std::vector<uint8_t> build_led_frame(led_brightness , led_value value) {
    std::vector<uint8_t> frame(8);

    // https://www.reddit.com/r/FastLED/comments/jq646t/debugging_new_16bit_strands_hd108_rgb_leds/
    // reddit says this is (1)(5bit)(5bit)(5bit) brightnesses
    frame[0] = 0xFF;
    frame[1] = 0xFF;
    //0 1 2 3 4 5 6 7
    //b b r r g g b b
    // for now only use the lower bits to limit power and be generally lazy
    frame[2] = value.r;
    frame[4] = value.g;
    frame[6] = value.b;

    // use SPI_SWAP_DATA in the future for 16 bit LSB MSB conversion

    return frame;
}
