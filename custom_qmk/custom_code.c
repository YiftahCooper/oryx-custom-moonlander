#ifndef ORYX_CUSTOM_MOONLANDER_CUSTOM_CODE_C
#define ORYX_CUSTOM_MOONLANDER_CUSTOM_CODE_C

#include "quantum.h"

enum user_custom_keycodes {
    MY_CUSTOM_MACRO = SAFE_RANGE,
};

bool process_record_user_custom(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
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
