# Haxx Clock
A clock from haxxors for haxxors with ❤️

See [Hardware Used](#hardware-used) for a list of hardware used in this project and how to connect it.

See [Project Setup](#project-setup) for instructions on how to set up the software project.

See [Trouble Shooting](#trouble-shooting) for common problems and their solutions.

## Hardware Used

- ESP32 with Az-Delivery ESP32 Dev Kit (This one has 5V output, which can be used to power the LED strip)
- HD108 LED Strip (Be wary of the faulty documentation, regarding the way the frame should be encoded)
- 3D printed digits to mount the LED strip on

## Hardware Setup

![ESP32 Wroom Pinout Diagram](https://cdn.shopify.com/s/files/1/1509/1638/files/ESP_-_32_NodeMCU_Developmentboard_Pinout_Diagram.png?7487926392378435209)

The code is configured to use the ports `G23` (data) and `G18` (clock) as spi lanes.
Please refer to the pinout diagram above to find the correct ports on your ESP32 board.
Also check `main.cpp` to see the rest of the pin configuration.

## Project setup
1. Have ESP32
2. Install `esp-idf`, follow the [official setup guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html), in short:
   ```bash
   sudo apt-get install git wget flex bison gperf python3 python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
   git clone --recursive https://github.com/espressif/esp-idf.git
   cd esp-idf
   ./install.sh esp32
   . ./export.sh
   cd ..
   ```
3. The IDF requires certain paths, set by sourcing the `export.sh` script. Keep this in mind when opening a new terminal. An IDE like CLion needs to know the IDF as well, specifically `$PATH` and `$IDF_PATH` need to be copied from a terminal.
4. Add the current user to the `dialout` linux group (to access the serial port):
   1. `sudo usermod -a -G dialout $USER`
   2. reboot

## Usage
For now, the wifi credentials are hardcoded in `init_wifi.cpp`, replace with your own. The ESP hosts a website with color controls, and knows the time via NTP.

## Trouble Shooting

### Haxx Clock does not display anything

- Check if the clock and data lanes are connected correctly
- Check if the correct wifi credentials are set in `init_wifi.cpp`
  - If there is no wifi connection, the clock will not display anything as it is unable to complete the startup sequence