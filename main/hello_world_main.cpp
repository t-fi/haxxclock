#include <time.h>
#include <thread>
#include "gpio_cxx.hpp"
#include "spi_host_cxx.hpp"
#include "HD108_driver.h"
#include "segment_frame_generator.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "nvs_handle.hpp"
#include "init_wifi.h"
#include "lwip/apps/sntp.h"
#include "http_server.h"

using namespace idf;

using namespace std;

namespace {
    void init_nvm() {
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);
    }

    void send_led_color(std::vector<led_value>& values, const shared_ptr<SPIDevice> &spi_dev) {
        spi_dev->transfer(start_frame).get();
        for (int i = 0; i < values.size(); ++i) {
            led_value v = values[i];
            spi_dev->transfer(build_led_frame(v)).get();
        }
        spi_dev->transfer(end_frame).get();
    }
}

extern "C" void app_main(void) {
    try {
        SPIMaster master(
                SPINum(2), // 2 means SPI3
                MOSI(23),
                MISO(19),
                SCLK(18)
        );

        shared_ptr<SPIDevice> spi_dev = master.create_dev(CS(5), Frequency::MHz(1));

        init_nvm();

        printf("init color from NVS\n");
        init_color_from_nvs();

        printf("init wifi\n");
        wifi_init_sta();

        printf("init sntp\n");
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, "pool.ntp.org");
        sntp_init();

        printf("init webserver\n");
        start_webserver();

        printf("init clock\n");
        struct timeval tv_now;
        struct tm time_info;
        setenv("TZ", "CEST", 1);
        tzset();

        printf("start main loop\n");
        while (true) {
            gettimeofday(&tv_now, &time_info);
            struct tm *local_time = std::localtime(&tv_now.tv_sec);
            std::vector<led_value> digits = get_clock_frame(local_time, tv_now.tv_usec);
            send_led_color(digits, spi_dev);

            this_thread::sleep_for(chrono::milliseconds(20));
        }
    } catch (GPIOException &e) {
        printf("GPIO exception occurred: %s\n", esp_err_to_name(e.error));
        printf("stopping.\n");
    } catch (const SPIException &e) {
        printf("SPI exception occurred: %s\n", esp_err_to_name(e.error));
        printf("stopping.\n");
    }
}
