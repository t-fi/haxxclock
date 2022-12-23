#include <thread>
#include "gpio_cxx.hpp"
#include "spi_host_cxx.hpp"
#include "HD108_driver.h"

using namespace idf;
using namespace std;

namespace {
    void cycle_onboard_led(GPIO_Output &onboard_led_gpio) {
        printf("LED ON\n");
        onboard_led_gpio.set_high();
        this_thread::sleep_for(chrono::milliseconds(1000));
        printf("LED OFF\n");
        onboard_led_gpio.set_low();
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    void send_led_color(led_value value, const shared_ptr<SPIDevice> &spi_dev) {
        spi_dev->transfer(start_frame).get();
        led_value black{};
        for (int i = 0; i < 34; ++i) {
            spi_dev->transfer(build_led_frame({255, 255, 255}, value)).get();
            spi_dev->transfer(build_led_frame({255, 255, 255}, black)).get();
        }
        spi_dev->transfer(end_frame).get();
    }
}


extern "C" void app_main(void) {
    try {
        SPIMaster master(SPINum(2), // 2 means SPI3
                         MOSI(23),
                         MISO(19),
                         SCLK(18));

        shared_ptr<SPIDevice> spi_dev = master.create_dev(CS(5), Frequency::MHz(1));

        GPIO_Output onboard_led_gpio(GPIONum(2));

        uint8_t i = 0;
        while (true) {
            i++;
            led_value v{i, static_cast<uint8_t>(i * 2), static_cast<uint8_t>(i * i)};
            printf("sending LED value: %03u %03u %03u\n", v.r, v.g, v.b);
            send_led_color(v, spi_dev);
            cycle_onboard_led(onboard_led_gpio);
        }

    } catch (GPIOException &e) {
        printf("GPIO exception occurred: %s\n", esp_err_to_name(e.error));
        printf("stopping.\n");
    } catch (const SPIException &e) {
        printf("SPI exception occurred: %s\n", esp_err_to_name(e.error));
        printf("stopping.\n");
    }
}
