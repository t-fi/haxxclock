# Haxx Clock
A clock from haxxors for haxxors with ❤️

## Project setup
1. Have ESP32
2. Install `esp-idf`, follow the [official setup guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html), in short:
   ```bash
   sudo apt-get install git wget flex bison gperf python3 python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
   git clone --recursive https://github.com/espressif/esp-idf.git
   ./install.sh esp32
   . ./export.sh
   ```
3. The IDF requires certain paths, set by sourcing the `export.sh` script. Keep this in mind when opening a new terminal. An IDE like CLion needs to know the IDF as well, specifically `$PATH` and `$IDF_PATH` need to be copied from a terminal.
4. Add the current user to the `dialout` linux group:
   1. `sudo usermod -a -G dialout $USER`
   2. reboot

## Usage
For now, the wifi credentials are hardcoded in `init_wifi.cpp`, replace with your own. The ESP hosts a website with color controls, and knows the time via NTP.