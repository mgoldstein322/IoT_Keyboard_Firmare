# IoT_Keyboard_Firmware
Firmware files for our IoT Keyboard senior design project --  pins are for ATMEGA32u4.

The firmware uses QMK version 0.12.29. All other versions have not been tested.

To use QMK, Follow the instructions and download/install QMK MSYS [here](https://beta.docs.qmk.fm/tutorial/newbs_getting_started) for your operating system.

## Installation and Usage
To use these files, do the following:
1. Copy the contents of both folders to your qmk_firmware directory, usually located at C:\Users\YOUR_NAME\qmk_firmware
> Note: This WILL replace files located in the tmk_core directory. Make sure to backup that folder before using the firmware.
2. If you haven't already or wish to compile the code, run `qmk compile -kb IoT_Keyboard -km default` in the QMK MSYS terminal in the qmk_firmware directory
3. In the QMK command line, run `make -j [NUM_THREADS] IoT_Keyboard:default:dfu` to upload the firmware to the keyboard.

## Completed
- Layout of IoT Keyboard completed
- Working basic Bluetooth driver for Elite C V4 microcontroller and HC-05 Bluetooth module
    - As of 3/29, the driver only sends raw keycodes through the `register_code()` function in `action.c`

## TODO
- Clean up Bluetooth implementation
    - Make the IoT Keyboard use its own main (use a copy of LUFA framework)
- Figure out how to send proper characters through Bluetooth