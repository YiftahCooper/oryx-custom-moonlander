#ifndef ORYX_CUSTOM_MOONLANDER_CUSTOM_CODE_C
#define ORYX_CUSTOM_MOONLANDER_CUSTOM_CODE_C

#include "quantum.h"

enum user_custom_keycodes {
    MY_CUSTOM_MACRO = SAFE_RANGE,
};

static bool language_is_hebrew = false;
// The language tap-dance key is on matrix [4,0] in this Moonlander layout.
static const uint8_t LANGUAGE_INDICATOR_ROW = 4;
static const uint8_t LANGUAGE_INDICATOR_COL = 0;
static const uint8_t LANGUAGE_ENGLISH_R = 40;
static const uint8_t LANGUAGE_ENGLISH_G = 140;
static const uint8_t LANGUAGE_ENGLISH_B = 255;
static const uint8_t LANGUAGE_HEBREW_R = 255;
static const uint8_t LANGUAGE_HEBREW_G = 0;
static const uint8_t LANGUAGE_HEBREW_B = 0;
static const uint16_t LANGUAGE_TOGGLE_GUARD_MS = 250;

// Suppress duplicate language flips caused by accidental re-triggering/bounce.
static bool language_toggle_guard_armed = false;
static uint16_t language_toggle_timer = 0;

static bool language_toggle_guard_allows_action(void) {
    if (language_toggle_guard_armed && timer_elapsed(language_toggle_timer) < LANGUAGE_TOGGLE_GUARD_MS) {
        return false;
    }
    language_toggle_timer = timer_read();
    language_toggle_guard_armed = true;
    return true;
}

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
    if (!language_toggle_guard_allows_action()) {
        return;
    }
    language_is_hebrew = !language_is_hebrew;
}

void custom_language_toggle(void) {
    if (!language_toggle_guard_allows_action()) {
        return;
    }
    tap_code16(LALT(KC_LEFT_SHIFT));
    language_is_hebrew = !language_is_hebrew;
}

void custom_language_resync(void) {
    // Force a known baseline: English + default indicator color.
    language_toggle_timer = timer_read();
    language_toggle_guard_armed = true;
    language_is_hebrew = false;
}

void custom_language_rgb_indicator(void) {
#ifdef RGB_MATRIX_ENABLE
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    if (language_is_hebrew) {
        r = LANGUAGE_HEBREW_R;
        g = LANGUAGE_HEBREW_G;
        b = LANGUAGE_HEBREW_B;
    } else {
        r = LANGUAGE_ENGLISH_R;
        g = LANGUAGE_ENGLISH_G;
        b = LANGUAGE_ENGLISH_B;
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

    // Language toggling is handled in patched tap-dance handlers.
    // Keep this hook available for future custom key logic.
    switch (keycode) {
        default:
            break;
    }
    return true;
}

#endif  // ORYX_CUSTOM_MOONLANDER_CUSTOM_CODE_C
