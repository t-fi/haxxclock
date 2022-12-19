#include <cstdlib>
#include <thread>
#include "esp_log.h"
#include "gpio_cxx.hpp"

using namespace idf;
using namespace std;

extern "C" void app_main(void)
{
    /* The functions of GPIO_Output throws exceptions in case of parameter errors or if there are underlying driver
       errors. */
    try {
        /* This line may throw an exception if the pin number is invalid.
         * Alternatively to 4, choose another output-capable pin. */
        GPIO_Output gpio(GPIONum(2));

        while (true) {
            printf("LED ON\n");
            gpio.set_high();
            this_thread::sleep_for(std::chrono::milliseconds(200));
            printf("LED OFF\n");
            gpio.set_low();
            this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    } catch (GPIOException &e) {
        printf("GPIO exception occurred: %s\n", esp_err_to_name(e.error));
        printf("stopping.\n");
    }
}
