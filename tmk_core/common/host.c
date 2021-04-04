/*
Copyright 2011,2012 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
//#include <avr/interrupt.h>
#include "keycode.h"
#include "host.h"
#include "util.h"
#include "debug.h"

// IoT: Import to send data over bluetooth
#include "../protocol/serial.h"

#ifdef NKRO_ENABLE
#    include "keycode_config.h"
extern keymap_config_t keymap_config;
#endif

static host_driver_t *driver;
static uint16_t       last_system_report   = 0;
static uint16_t       last_consumer_report = 0;

void host_set_driver(host_driver_t *d) { driver = d; }

host_driver_t *host_get_driver(void) { return driver; }

uint8_t host_keyboard_leds(void) {
    if (!driver) return 0;
    return (*driver->keyboard_leds)();
}

led_t host_keyboard_led_state(void) {
    if (!driver) return (led_t){0};
    return (led_t)((*driver->keyboard_leds)());
}

// IoT: Converts keycode to ASCII value
// NOTE: Only converts alphanumeric keys (plus some punctuation) found in 60% layout
uint8_t convert_keycode(uint8_t code, bool shifted) {
    uint8_t c = 0;

    // Convert letters
    if (code >= 0x04 && code <= 0x1D) {
        // Convert keycode to lowercase letter, then add uppercase modifier if shifted
        c = code - 0x04 + 0x61;
        if (shifted) c -= 0x20;
        return c;
    }
    // Individually convert every other non-modifier key in 60% layout
    switch (code) {
        case 0x1E:
            c = (shifted) ? 0x21 : 0x31;
            break;
        case 0x1F:
            c = (shifted) ? 0x40 : 0x32;
            break;
        case 0x20:
            c = (shifted) ? 0x23 : 0x33;
            break;
        case 0x21:
            c = (shifted) ? 0x24 : 0x34;
            break;
        case 0x22:
            c = (shifted) ? 0x25 : 0x35;
            break;
        case 0x23: //6
            c = (shifted) ? 0x5E : 0x36;
            break;
        case 0x24:
            c = (shifted) ? 0x26 : 0x37;
            break;
        case 0x25:
            c = (shifted) ? 0x2A : 0x38;
            break;
        case 0x26:
            c = (shifted) ? 0x28 : 0x39;
            break;
        case 0x27:
            c = (shifted) ? 0x29 : 0x30;
            break;
        case 0x28: // Enter
            c = 0x0D;
            break;
        case 0x29:
            c = 0x1B;
            break;
        case 0x2A:
            c = 0x08;
            break;
        case 0x2B:
            c = 0x09;
            break;
        case 0x2C:
            c = 0x20;
            break;
        case 0x2D:
            c = (shifted) ? 0x5F : 0x2D;
            break;
        case 0x2E:
            c = (shifted) ? 0x2B : 0x3D;
            break;
        case 0x2F:
            c = (shifted) ? 0x7B : 0x5B;
            break;
        case 0x30:
            c = (shifted) ? 0x7D : 0x5D;
            break;
        case 0x31:
            c = (shifted) ? 0x7C : 0x5C;
            break;
        case 0x33:
            c = (shifted) ? 0x3A : 0x3B;
            break;
        case 0x34:
            c = (shifted) ? 0x22 : 0x27;
            break;
        case 0x36: // Comma
            c = (shifted) ? 0x3C : 0x2C;
            break;
        case 0x37:
            c = (shifted) ? 0x3E : 0x2E;
            break;
        case 0x38:
            c = (shifted) ? 0x3F : 0x2F;
            break;
        default:
            break;
    }
    return c;

}

// Variables to keep track of previous values
uint8_t prev_length = 0;
uint8_t prev_keys[KEYBOARD_REPORT_KEYS];


// IoT: Updates stats
void update_stats(report_keyboard_t *report, uint8_t len) {
    // Copy length
    prev_length = len;

    // Copy Array
    for (int i = 0; i < KEYBOARD_REPORT_KEYS; i++) {
        prev_keys[i] = report->keys[i];
    }

}

// IoT: Quick function to check if value exists in another.
bool in_array(uint8_t num) {
    for (int i = 0; i < KEYBOARD_REPORT_KEYS; i++) {
        if (prev_keys[i] == num) return true;
    }
    return false;
}
// IoT: Function that sends keyboard data over bluetooth
void send_bluetooth_report(report_keyboard_t *report) {
    // Determine if shift is pressed
    bool shifted = ((0b00100010 & report->mods) > 0) ? true : false;

    // Track if key wasn't seen previously and last actual keycode in array
    bool found_new = false;
    uint8_t last_char = 0;

    // Consider the length of the keys array (number of non-zero entries)
    // If less than the previous time function was called and no new characters, ignore send
    // Else, send last character in array

    // Determine length of current array
    uint8_t current_length = 0;
    for (int i = 0; i < KEYBOARD_REPORT_KEYS; i++) {
        if (!report->keys[i]) {
            break;
        } else {
            // Save char
            last_char = report->keys[i];
            // Increment count
            current_length++;
            // Check if key isn't present in previous array
            if (!in_array(report->keys[i])) found_new = true;
        }
    }

    // If length smaller or no new characters, don't send
    if (current_length < prev_length || !found_new) {
    // Else convert and send last char
    } else {
        uint8_t c = convert_keycode(last_char,shifted);
        serial_send(c);
    }

    // Update previous values to current value
    update_stats(report, current_length);

}

/*
IoT: Old function that just sends the keycode values
void send_bluetooth_report(report_keyboard_t *report) {
    serial_send(report->mods);
    for (int i = 0; i < 6; i++) serial_send(report->keys[i]);
}
*/

/* send report */
void host_keyboard_send(report_keyboard_t *report) {
    // IoT: Send bluetooth data
    send_bluetooth_report(report);
    if (!driver) return;
#if defined(NKRO_ENABLE) && defined(NKRO_SHARED_EP)
    if (keyboard_protocol && keymap_config.nkro) {
        /* The callers of this function assume that report->mods is where mods go in.
         * But report->nkro.mods can be at a different offset if core keyboard does not have a report ID.
         */
        report->nkro.mods      = report->mods;
        report->nkro.report_id = REPORT_ID_NKRO;
    } else
#endif
    {
#ifdef KEYBOARD_SHARED_EP
        report->report_id = REPORT_ID_KEYBOARD;
#endif
    }
    (*driver->send_keyboard)(report);

    if (debug_keyboard) {
        dprint("keyboard_report: ");
        for (uint8_t i = 0; i < KEYBOARD_REPORT_SIZE; i++) {
            dprintf("%02X ", report->raw[i]);
        }
        dprint("\n");
    }
}

void host_mouse_send(report_mouse_t *report) {
    if (!driver) return;
#ifdef MOUSE_SHARED_EP
    report->report_id = REPORT_ID_MOUSE;
#endif
    (*driver->send_mouse)(report);
}

void host_system_send(uint16_t report) {
    if (report == last_system_report) return;
    last_system_report = report;

    if (!driver) return;
    (*driver->send_system)(report);
}

void host_consumer_send(uint16_t report) {
    if (report == last_consumer_report) return;
    last_consumer_report = report;

    if (!driver) return;
    (*driver->send_consumer)(report);
}

uint16_t host_last_system_report(void) { return last_system_report; }

uint16_t host_last_consumer_report(void) { return last_consumer_report; }
