#include <thread>
#include "gpio_cxx.hpp"
#include "spi_host_cxx.hpp"

using namespace idf;
using namespace std;

extern "C" void app_main(void) {
    /* The functions of GPIO_Output throws exceptions in case of parameter errors or if there are underlying driver
       errors. */
    try {
        SPIMaster master(SPINum(2), // 2 means SPI3
                         MOSI(23),
                         MISO(19),
                         SCLK(18));

        shared_ptr<SPIDevice> spi_dev = master.create_dev(CS(5), Frequency::MHz(1));

        /* This line may throw an exception if the pin number is invalid.
         * Alternatively to 4, choose another output-capable pin. */
        GPIO_Output gpio(GPIONum(2));

        while (true) {
            // Connect pin 23 to pin 19 to check if SPI communication works.
            // 1. It should echo back the original message
            // 2. It should break and print empty things once the connection is severed
            printf("SPI full cicle:\n");
            for (auto &&value: spi_dev->transfer({'h', 'a', 'l', 'l', 'o'}).get()) {
                printf("%c, ", value);
            }
            printf("\n");

            printf("LED ON\n");
            gpio.set_high();
            this_thread::sleep_for(std::chrono::milliseconds(1000));
            printf("LED OFF\n");
            gpio.set_low();
            this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

    } catch (GPIOException &e) {
        printf("GPIO exception occurred: %s\n", esp_err_to_name(e.error));
        printf("stopping.\n");
    } catch (const SPIException &e) {
        printf("SPI exception occurred: %s\n", esp_err_to_name(e.error));
        printf("stopping.\n");
    }

}
