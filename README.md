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
> Make sure to press the reset button on the PCB or the microcontroller to enable flashing.

## Completed
- Layout of IoT Keyboard completed
- Working basic Bluetooth driver for Elite C V4 microcontroller and HC-05 Bluetooth module
- ASCII values are sent through Bluetooth

## TODO
- Clean up Bluetooth implementation
    - Make the IoT Keyboard use its own main (use a copy of LUFA framework)
    
## Limitations
- As certain keyboard functions such as shortcuts and Caps Lock are handled by the operating system, no ASCII values or ASCII modified values will be sent through Bluetooth with the following keys: Caps Lock, LCtrl, RCtrl, LAlt, RAlt, RWin/Function, LWin, Menu.
- Keycodes are sent through keyboard reports in QMK, whereby one is sent whenever the keyboard status chanages. This includes when you both press and lift a key on the keyboard. The Bluetooth firmware reads the keyboard report everytime it's sent and interprets it and decides what/when to send. Behavior when multiple keys is untested/unknown.