#ifndef ORYX_CUSTOM_MOONLANDER_CUSTOM_CODE_C
#define ORYX_CUSTOM_MOONLANDER_CUSTOM_CODE_C

#include "quantum.h"

enum user_custom_keycodes {
    MY_CUSTOM_MACRO = SAFE_RANGE,
};

static bool language_state_known = false;
static bool language_is_hebrew = false;
// DANCE_1 is on matrix [4,0] in this Moonlander layout.
static const uint8_t LANGUAGE_INDICATOR_ROW = 4;
static const uint8_t LANGUAGE_INDICATOR_COL = 0;

static uint8_t custom_language_indicator_led(void) {
#ifdef RGB_MATRIX_ENABLE
    if (LANGUAGE_INDICATOR_ROW >= MATRIX_ROWS || LANGUAGE_INDICATOR_COL >= MATRIX_COLS) {
        return NO_LED;
    }
    return g_led_config.matrix_co[LANGUAGE_INDICATOR_ROW][LANGUAGE_INDICATOR_COL];
#else
    return NO_LED;
#endif
}

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

    uint8_t led = custom_language_indicator_led();
    if (led != NO_LED) {
        rgb_matrix_set_color(led, r, g, b);
    }
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
