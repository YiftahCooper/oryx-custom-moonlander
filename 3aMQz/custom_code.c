#ifndef ORYX_CUSTOM_MOONLANDER_CUSTOM_CODE_C
#define ORYX_CUSTOM_MOONLANDER_CUSTOM_CODE_C

#include "quantum.h"

enum user_custom_keycodes {
    MY_CUSTOM_MACRO = SAFE_RANGE,
};

static bool language_state_known = false;
static bool language_is_hebrew = false;
// LED 27 maps to the DANCE_1 key; avoid LED 32 because Oryx uses it for Caps Lock.
static const uint8_t LANGUAGE_INDICATOR_LED = 27;

void custom_language_toggled(void) {
    if (!language_state_known) {
        language_state_known = true;
        language_is_hebrew = true;
        return;
    }
    language_is_hebrew = !language_is_hebrew;
}

void custom_language_resync(void) {
    language_state_known = true;
    language_is_hebrew = false;
}

void custom_language_rgb_indicator(void) {
#ifdef RGB_MATRIX_ENABLE
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    if (!language_state_known) {
        r = 255;
        g = 140;
        b = 0;
    } else if (language_is_hebrew) {
        r = 255;
        g = 30;
        b = 30;
    } else {
        r = 40;
        g = 140;
        b = 255;
    }

    rgb_matrix_set_color(LANGUAGE_INDICATOR_LED, r, g, b);
#endif
}

bool process_record_user_custom(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return true;
    }

    switch (keycode) {
        case LALT(KC_LEFT_SHIFT):
            custom_language_toggled();
            break;
        // Example:
        // case MY_CUSTOM_MACRO:
        //     if (record->event.pressed) { SEND_STRING("Hello World"); }
        //     return false;
        default:
            break;
    }
    return true;
}

#endif  // ORYX_CUSTOM_MOONLANDER_CUSTOM_CODE_C
