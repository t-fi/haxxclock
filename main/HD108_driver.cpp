#include <vector>
#include "HD108_driver.h"
#include "driver/spi_common.h"

std::vector<uint8_t> build_led_frame(led_value value) {
    std::vector<uint8_t> frame(8);

    // https://www.reddit.com/r/FastLED/comments/jq646t/debugging_new_16bit_strands_hd108_rgb_leds/
    // reddit says this is (1)(5bit)(5bit)(5bit) brightnesses

    // setting the global brightness to 00011 like this:
    // 1 [...] [.|..] [...]
    // 1 00011 00|011 00011
    frame[0] = 0b10001100;
    frame[1] = 0b01100011;

    // 0 1 2 3 4 5 6 7
    // b b r r g g b b
    // need to change endianess
    frame[2] = (uint8_t)(value.r >> 8 & 0x00FF);
    frame[3] = (uint8_t)(value.r & 0x00FF);
    frame[4] = (uint8_t)(value.g >> 8 & 0x00FF);
    frame[5] = (uint8_t)(value.g & 0x00FF);
    frame[6] = (uint8_t)(value.b >> 8 & 0x00FF);
    frame[7] = (uint8_t)(value.b & 0x00FF);

    return frame;
}
