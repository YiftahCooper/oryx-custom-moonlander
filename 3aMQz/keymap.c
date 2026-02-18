#include QMK_KEYBOARD_H
#include "version.h"

// --- Custom language hooks (injected) ---
void custom_language_toggled(void);
void custom_language_resync(void);
void custom_language_rgb_indicator(void);
// ----------------------------------------
#define MOON_LED_LEVEL LED_LEVEL
#ifndef ZSA_SAFE_RANGE
#define ZSA_SAFE_RANGE SAFE_RANGE
#endif

enum custom_keycodes {
  RGB_SLD = ZSA_SAFE_RANGE,
  HSV_0_0_0,
  HSV_0_137_115,
  ST_MACRO_0,
  ST_MACRO_1,
  ST_MACRO_2,
  ST_MACRO_3,
  ST_MACRO_4,
  ST_MACRO_5,
  ST_MACRO_6,
  ST_MACRO_7,
  ST_MACRO_8,
  ST_MACRO_9,
  ST_MACRO_10,
  ST_MACRO_11,
  ST_MACRO_12,
  ST_MACRO_13,
  ST_MACRO_14,
  ST_MACRO_15,
  ST_MACRO_16,
  ST_MACRO_17,
};



enum tap_dance_codes {
  DANCE_0,
  DANCE_1,
  DANCE_2,
  DANCE_3,
  DANCE_4,
  DANCE_5,
  DANCE_6,
};

#define DUAL_FUNC_0 LT(15, KC_F1)
#define DUAL_FUNC_1 LT(8, KC_F9)
#define DUAL_FUNC_2 LT(9, KC_F23)
#define DUAL_FUNC_3 LT(14, KC_A)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_moonlander(
    KC_ESCAPE,      KC_1,           KC_2,           KC_3,           KC_4,           KC_5,           KC_EQUAL,                                       MEH_T(KC_PAGE_UP),KC_6,           KC_7,           KC_8,           KC_9,           KC_0,           KC_HOME,        
    TD(DANCE_0),    KC_Q,           KC_W,           KC_E,           KC_R,           KC_T,           KC_MINUS,                                       ALL_T(KC_PGDN), KC_Y,           KC_U,           KC_I,           KC_O,           KC_P,           KC_END,         
    DUAL_FUNC_0,    KC_A,           KC_S,           KC_D,           KC_F,           KC_G,           KC_GRAVE,                                                                       TG(1),          KC_H,           KC_J,           KC_K,           KC_L,           KC_SCLN,        KC_BSLS,        
    KC_LEFT_SHIFT,  KC_Z,           KC_X,           KC_C,           KC_V,           KC_B,                                           KC_N,           KC_M,           KC_COMMA,       KC_DOT,         KC_UP,          OSL(1),         
    TD(DANCE_1),    KC_LEFT_GUI,    KC_LEFT_ALT,    KC_LBRC,        KC_RBRC,        MT(MOD_RALT, KC_TAB),                                                                                                DUAL_FUNC_1,    KC_QUOTE,       KC_SLASH,       KC_LEFT,        KC_DOWN,        KC_RIGHT,       
    TD(DANCE_2),    MT(MOD_RCTL, KC_PSCR),LCTL(KC_LEFT_SHIFT),                TD(DANCE_3),    TD(DANCE_4),    TD(DANCE_5)
  ),
  [1] = LAYOUT_moonlander(
    KC_ESCAPE,      KC_F1,          KC_F2,          KC_F3,          KC_F4,          KC_F5,          KC_F6,                                          KC_EXLM,        KC_KP_PLUS,     KC_KP_7,        KC_KP_8,        KC_KP_9,        KC_KP_EQUAL,    KC_AMPR,        
    KC_TRANSPARENT, KC_F7,          KC_F8,          KC_F9,          KC_F10,         KC_F11,         KC_F12,                                         KC_PERC,        KC_KP_MINUS,    KC_KP_4,        KC_KP_5,        KC_KP_6,        KC_CIRC,        KC_DLR,         
    KC_TRANSPARENT, KC_LCBR,        KC_RCBR,        KC_LPRN,        KC_RPRN,        KC_PIPE,        TG(2),                                                                          KC_TRANSPARENT, KC_KP_ASTERISK, KC_KP_1,        KC_KP_2,        KC_KP_3,        KC_HASH,        ST_MACRO_0,     
    KC_TRANSPARENT, KC_GRAVE,       KC_DQUO,        KC_LBRC,        KC_RBRC,        KC_TILD,                                        KC_KP_SLASH,    KC_KP_COMMA,    KC_KP_0,        KC_KP_DOT,      KC_UP,          KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_LABK,        KC_RABK,        KC_TRANSPARENT,                                                                                                 KC_ENTER,       KC_LPRN,        KC_RPRN,        KC_LEFT,        KC_DOWN,        KC_RIGHT,       
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_DELETE,      KC_BSPC,        KC_SPACE
  ),
  [2] = LAYOUT_moonlander(
    HSV_0_0_0,      HSV_0_137_115,  TOGGLE_LAYER_COLOR,RGB_TOG,        TG(3),          HSV_0_0_0,      QK_BOOT,                                        KC_TRANSPARENT, ST_MACRO_12,    ST_MACRO_13,    ST_MACRO_14,    ST_MACRO_15,    ST_MACRO_16,    ST_MACRO_17,    
    KC_TAB,         ST_MACRO_1,     LCTL(KC_BSPC),  LCTL(KC_DELETE),ST_MACRO_2,     ST_MACRO_3,     ST_MACRO_4,                                     KC_TRANSPARENT, KC_KP_COMMA,    KC_COMMA,       KC_DOT,         KC_KP_DOT,      KC_INTERNATIONAL_1,KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_LEFT_CTRL,   KC_LEFT_ALT,    KC_LEFT_GUI,    KC_LEFT_SHIFT,  LCTL(KC_K),     KC_TRANSPARENT,                                                                 KC_TRANSPARENT, DUAL_FUNC_2,    RCTL(KC_LEFT),  KC_END,         RCTL(KC_RIGHT), DUAL_FUNC_3,    KC_TRANSPARENT, 
    ST_MACRO_5,     ST_MACRO_6,     ST_MACRO_7,     ST_MACRO_8,     ST_MACRO_9,     ST_MACRO_10,                                    MT(MOD_LCTL, KC_R),TD(DANCE_6),    KC_LEFT,        KC_RIGHT,       LCTL(LSFT(KC_MINUS)),KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, ST_MACRO_11,    LALT(KC_LEFT_SHIFT),KC_UP,                                                                                                          KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_LEFT,        KC_DOWN,        KC_RIGHT,                       KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [3] = LAYOUT_moonlander(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [4] = LAYOUT_moonlander(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, 
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),
};



uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TD(DANCE_1):
            return TAPPING_TERM -80;
        case MT(MOD_RALT, KC_TAB):
            return TAPPING_TERM -60;
        case TD(DANCE_2):
            return TAPPING_TERM -110;
        case MT(MOD_RCTL, KC_PSCR):
            return TAPPING_TERM -110;
        case MEH_T(KC_PAGE_UP):
            return TAPPING_TERM -120;
        case ALL_T(KC_PGDN):
            return TAPPING_TERM -120;
        case DUAL_FUNC_1:
            return TAPPING_TERM -80;
        case TD(DANCE_3):
            return TAPPING_TERM -120;
        case TD(DANCE_4):
            return TAPPING_TERM -120;
        case TD(DANCE_5):
            return TAPPING_TERM -134;
        default:
            return TAPPING_TERM;
    }
}

bool capslock_active = false;

bool led_update_user(led_t led_state) {
  capslock_active = led_state.caps_lock;
  return true;
}

extern rgb_config_t rgb_matrix_config;

RGB hsv_to_rgb_with_value(HSV hsv) {
  RGB rgb = hsv_to_rgb( hsv );
  float f = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
  return (RGB){ f * rgb.r, f * rgb.g, f * rgb.b };
}

void keyboard_post_init_user(void) {
  rgb_matrix_enable();
}

const uint8_t PROGMEM ledmap[][RGB_MATRIX_LED_COUNT][3] = {
    [0] = { {22,251,241}, {22,251,241}, {22,251,241}, {185,244,212}, {131,252,242}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {185,244,212}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {185,244,212}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {185,244,212}, {185,244,212}, {185,244,212}, {185,244,212}, {22,251,241}, {22,251,241}, {83,219,200}, {239,255,255}, {22,251,241}, {83,219,200}, {83,219,200}, {83,219,200}, {22,251,241}, {22,251,241}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {22,251,241}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {83,219,200}, {185,244,212}, {185,244,212}, {239,255,255}, {22,251,241}, {22,251,241}, {22,251,241}, {22,251,241} },

    [1] = { {22,251,241}, {22,251,241}, {22,251,241}, {185,244,212}, {185,244,212}, {137,252,242}, {137,252,242}, {154,255,255}, {154,255,255}, {185,244,212}, {137,252,242}, {137,252,242}, {154,255,255}, {154,255,255}, {185,244,212}, {137,252,242}, {137,252,242}, {154,255,255}, {154,255,255}, {154,255,255}, {137,252,242}, {137,252,242}, {154,255,255}, {154,255,255}, {154,255,255}, {137,252,242}, {137,252,242}, {154,255,255}, {154,255,255}, {137,252,242}, {137,252,242}, {239,255,255}, {185,244,212}, {185,244,212}, {185,244,212}, {185,244,212}, {154,255,255}, {154,255,255}, {154,255,255}, {154,255,255}, {22,251,241}, {110,252,242}, {110,252,242}, {110,252,242}, {22,251,241}, {22,251,241}, {191,255,255}, {191,255,255}, {191,255,255}, {110,252,242}, {22,251,241}, {191,255,255}, {191,255,255}, {191,255,255}, {191,255,255}, {154,255,255}, {191,255,255}, {191,255,255}, {191,255,255}, {110,252,242}, {154,255,255}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {239,255,255}, {22,251,241}, {22,251,241}, {22,251,241}, {22,251,241} },

    [2] = { {239,255,255}, {0,192,254}, {154,255,255}, {10,225,255}, {85,203,158}, {239,255,255}, {10,225,255}, {0,192,254}, {10,225,255}, {85,203,158}, {239,255,255}, {10,225,255}, {0,192,254}, {10,225,255}, {85,203,158}, {239,255,255}, {10,225,255}, {85,203,158}, {154,255,255}, {10,225,255}, {239,255,255}, {10,225,255}, {85,203,158}, {10,225,255}, {137,252,242}, {22,251,241}, {10,225,255}, {10,225,255}, {10,225,255}, {45,255,255}, {0,192,254}, {239,255,255}, {198,215,255}, {198,215,255}, {198,215,255}, {198,215,255}, {0,192,254}, {0,192,254}, {0,0,255}, {0,192,254}, {0,192,254}, {0,192,254}, {0,0,255}, {10,225,255}, {10,225,255}, {0,192,254}, {0,192,254}, {10,225,255}, {10,225,255}, {0,0,255}, {0,192,254}, {0,192,254}, {10,225,255}, {10,225,255}, {0,0,255}, {0,192,254}, {0,192,254}, {0,192,254}, {10,225,255}, {10,225,255}, {154,255,255}, {0,192,254}, {10,225,255}, {10,225,255}, {10,225,255}, {0,192,254}, {0,192,254}, {0,192,254}, {154,255,255}, {85,203,158}, {85,203,158}, {85,203,158} },

    [3] = { {22,251,241}, {22,251,241}, {22,251,241}, {185,244,212}, {185,244,212}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {185,244,212}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {185,244,212}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {137,252,242}, {185,244,212}, {185,244,212}, {185,244,212}, {185,244,212}, {22,251,241}, {22,251,241}, {185,244,212}, {185,244,212}, {22,251,241}, {110,252,242}, {110,252,242}, {110,252,242}, {22,251,241}, {22,251,241}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {22,251,241}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {110,252,242}, {22,251,241}, {22,251,241}, {239,255,255}, {22,251,241}, {22,251,241}, {22,251,241}, {22,251,241} },

    [4] = { {22,251,241}, {22,251,241}, {22,251,241}, {185,244,212}, {185,244,212}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {185,244,212}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {185,244,212}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {253,252,242}, {185,244,212}, {185,244,212}, {185,244,212}, {185,244,212}, {22,251,241}, {22,251,241}, {185,244,212}, {185,244,212}, {22,251,241}, {131,252,242}, {131,252,242}, {131,252,242}, {22,251,241}, {22,251,241}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {22,251,241}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {131,252,242}, {22,251,241}, {22,251,241}, {131,252,242}, {22,251,241}, {22,251,241}, {22,251,241}, {22,251,241} },

};

void set_layer_color(int layer) {
  for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
    HSV hsv = {
      .h = pgm_read_byte(&ledmap[layer][i][0]),
      .s = pgm_read_byte(&ledmap[layer][i][1]),
      .v = pgm_read_byte(&ledmap[layer][i][2]),
    };
    if (!hsv.h && !hsv.s && !hsv.v) {
        rgb_matrix_set_color( i, 0, 0, 0 );
    } else {
        RGB rgb = hsv_to_rgb_with_value(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
  }
}

bool rgb_matrix_indicators_user(void) {
  if (rawhid_state.rgb_control) {
      return false;
  }
  if (!keyboard_config.disable_layer_led) { 
    switch (biton32(layer_state)) {
      case 0:
        set_layer_color(0);
        break;
      case 1:
        set_layer_color(1);
        break;
      case 2:
        set_layer_color(2);
        break;
      case 3:
        set_layer_color(3);
        break;
      case 4:
        set_layer_color(4);
        break;
     default:
        if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
          rgb_matrix_set_color_all(0, 0, 0);
        }
    }
  } else {
    if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
      rgb_matrix_set_color_all(0, 0, 0);
    }
  }

  if (capslock_active && biton32(layer_state) == 0) {
    RGB rgb = hsv_to_rgb_with_value((HSV) { 110, 252, 242 });
    rgb_matrix_set_color( 32, rgb.r, rgb.g, rgb.b );
  } 
  custom_language_rgb_indicator(); /* ORYX_LANG_RGB_PATCH */
  return true;
}



typedef struct {
    bool is_press_action;
    uint8_t step;
} tap;

enum {
    SINGLE_TAP = 1,      
    SINGLE_HOLD,         
    DOUBLE_TAP,          
    DOUBLE_HOLD,         
    DOUBLE_SINGLE_TAP,   
    MORE_TAPS            
};

static tap dance_state[7];

uint8_t dance_step(tap_dance_state_t *state);

uint8_t dance_step(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return SINGLE_TAP;
        else return SINGLE_HOLD;
    } else if (state->count == 2) {
        if (state->interrupted) return DOUBLE_SINGLE_TAP;
        else if (state->pressed) return DOUBLE_HOLD;
        else return DOUBLE_TAP;
    }
    return MORE_TAPS;
}


void on_dance_0(tap_dance_state_t *state, void *user_data);
void dance_0_finished(tap_dance_state_t *state, void *user_data);
void dance_0_reset(tap_dance_state_t *state, void *user_data);

void on_dance_0(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_DELETE);
        tap_code16(KC_DELETE);
        tap_code16(KC_DELETE);
    }
    if(state->count > 3) {
        tap_code16(KC_DELETE);
    }
}

void dance_0_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[0].step = dance_step(state);
    switch (dance_state[0].step) {
        case SINGLE_TAP: register_code16(KC_DELETE); break;
        case DOUBLE_TAP: register_code16(LCTL(KC_DELETE)); break;
        case DOUBLE_SINGLE_TAP: tap_code16(KC_DELETE); register_code16(KC_DELETE);
    }
}

void dance_0_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[0].step) {
        case SINGLE_TAP: unregister_code16(KC_DELETE); break;
        case DOUBLE_TAP: unregister_code16(LCTL(KC_DELETE)); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_DELETE); break;
    }
    dance_state[0].step = 0;
}
void on_dance_1(tap_dance_state_t *state, void *user_data);
void dance_1_finished(tap_dance_state_t *state, void *user_data);
void dance_1_reset(tap_dance_state_t *state, void *user_data);

void on_dance_1(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(LALT(KC_LEFT_SHIFT)); custom_language_toggled(); /* ORYX_LANG_TOGGLE_PATCH */
        tap_code16(LALT(KC_LEFT_SHIFT)); custom_language_toggled(); /* ORYX_LANG_TOGGLE_PATCH */
        tap_code16(LALT(KC_LEFT_SHIFT)); custom_language_toggled(); /* ORYX_LANG_TOGGLE_PATCH */
    }
    if(state->count > 3) {
        tap_code16(LALT(KC_LEFT_SHIFT)); custom_language_toggled(); /* ORYX_LANG_TOGGLE_PATCH */
    }
}

void dance_1_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[1].step = dance_step(state);
    switch (dance_state[1].step) {
        case SINGLE_TAP: register_code16(LALT(KC_LEFT_SHIFT)); custom_language_toggled(); /* ORYX_LANG_TOGGLE_PATCH */ break;
        case SINGLE_HOLD: register_code16(KC_LEFT_CTRL); break;
        case DOUBLE_TAP: register_code16(LALT(KC_LEFT_SHIFT)); custom_language_toggled(); /* ORYX_LANG_TOGGLE_PATCH */ register_code16(LALT(KC_LEFT_SHIFT)); custom_language_toggled(); /* ORYX_LANG_TOGGLE_PATCH */ break;
        case DOUBLE_HOLD: custom_language_resync(); break; /* ORYX_LANG_RESYNC_PATCH */
        case DOUBLE_SINGLE_TAP: tap_code16(LALT(KC_LEFT_SHIFT)); custom_language_toggled(); /* ORYX_LANG_TOGGLE_PATCH */ register_code16(LALT(KC_LEFT_SHIFT)); custom_language_toggled(); /* ORYX_LANG_TOGGLE_PATCH */
    }
}

void dance_1_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[1].step) {
        case SINGLE_TAP: unregister_code16(LALT(KC_LEFT_SHIFT)); break;
        case SINGLE_HOLD: unregister_code16(KC_LEFT_CTRL); break;
        case DOUBLE_TAP: unregister_code16(LALT(KC_LEFT_SHIFT)); break;
        case DOUBLE_HOLD: break; /* ORYX_LANG_RESYNC_PATCH */
        case DOUBLE_SINGLE_TAP: unregister_code16(LALT(KC_LEFT_SHIFT)); break;
    }
    dance_state[1].step = 0;
}
void on_dance_2(tap_dance_state_t *state, void *user_data);
void dance_2_finished(tap_dance_state_t *state, void *user_data);
void dance_2_reset(tap_dance_state_t *state, void *user_data);

void on_dance_2(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_SPACE);
        tap_code16(KC_SPACE);
        tap_code16(KC_SPACE);
    }
    if(state->count > 3) {
        tap_code16(KC_SPACE);
    }
}

void dance_2_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[2].step = dance_step(state);
    switch (dance_state[2].step) {
        case SINGLE_TAP: register_code16(KC_SPACE); break;
        case SINGLE_HOLD: register_code16(KC_LEFT_SHIFT); break;
        case DOUBLE_TAP: register_code16(KC_CAPS); break;
        case DOUBLE_SINGLE_TAP: tap_code16(KC_SPACE); register_code16(KC_SPACE);
    }
}

void dance_2_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[2].step) {
        case SINGLE_TAP: unregister_code16(KC_SPACE); break;
        case SINGLE_HOLD: unregister_code16(KC_LEFT_SHIFT); break;
        case DOUBLE_TAP: unregister_code16(KC_CAPS); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_SPACE); break;
    }
    dance_state[2].step = 0;
}
void on_dance_3(tap_dance_state_t *state, void *user_data);
void dance_3_finished(tap_dance_state_t *state, void *user_data);
void dance_3_reset(tap_dance_state_t *state, void *user_data);

void on_dance_3(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_DELETE);
        tap_code16(KC_DELETE);
        tap_code16(KC_DELETE);
    }
    if(state->count > 3) {
        tap_code16(KC_DELETE);
    }
}

void dance_3_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[3].step = dance_step(state);
    switch (dance_state[3].step) {
        case SINGLE_TAP: register_code16(KC_DELETE); break;
        case DOUBLE_TAP: register_code16(LCTL(KC_DELETE)); break;
        case DOUBLE_SINGLE_TAP: tap_code16(KC_DELETE); register_code16(KC_DELETE);
    }
}

void dance_3_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[3].step) {
        case SINGLE_TAP: unregister_code16(KC_DELETE); break;
        case DOUBLE_TAP: unregister_code16(LCTL(KC_DELETE)); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_DELETE); break;
    }
    dance_state[3].step = 0;
}
void on_dance_4(tap_dance_state_t *state, void *user_data);
void dance_4_finished(tap_dance_state_t *state, void *user_data);
void dance_4_reset(tap_dance_state_t *state, void *user_data);

void on_dance_4(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_BSPC);
        tap_code16(KC_BSPC);
        tap_code16(KC_BSPC);
    }
    if(state->count > 3) {
        tap_code16(KC_BSPC);
    }
}

void dance_4_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[4].step = dance_step(state);
    switch (dance_state[4].step) {
        case SINGLE_TAP: register_code16(KC_BSPC); break;
        case DOUBLE_TAP: register_code16(LCTL(KC_BSPC)); break;
        case DOUBLE_SINGLE_TAP: tap_code16(KC_BSPC); register_code16(KC_BSPC);
    }
}

void dance_4_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[4].step) {
        case SINGLE_TAP: unregister_code16(KC_BSPC); break;
        case DOUBLE_TAP: unregister_code16(LCTL(KC_BSPC)); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_BSPC); break;
    }
    dance_state[4].step = 0;
}
void on_dance_5(tap_dance_state_t *state, void *user_data);
void dance_5_finished(tap_dance_state_t *state, void *user_data);
void dance_5_reset(tap_dance_state_t *state, void *user_data);

void on_dance_5(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_SPACE);
        tap_code16(KC_SPACE);
        tap_code16(KC_SPACE);
    }
    if(state->count > 3) {
        tap_code16(KC_SPACE);
    }
}

void dance_5_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[5].step = dance_step(state);
    switch (dance_state[5].step) {
        case SINGLE_TAP: register_code16(KC_SPACE); break;
        case DOUBLE_TAP: tap_code16(KC_KP_DOT); register_code16(KC_SPACE); break; /* ORYX_FN24_NUMDOT_SPACE_PATCH */
        case DOUBLE_SINGLE_TAP: tap_code16(KC_SPACE); register_code16(KC_SPACE);
    }
}

void dance_5_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[5].step) {
        case SINGLE_TAP: unregister_code16(KC_SPACE); break;
        case DOUBLE_TAP: unregister_code16(KC_SPACE); break; /* ORYX_FN24_NUMDOT_SPACE_PATCH */
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_SPACE); break;
    }
    dance_state[5].step = 0;
}
void on_dance_6(tap_dance_state_t *state, void *user_data);
void dance_6_finished(tap_dance_state_t *state, void *user_data);
void dance_6_reset(tap_dance_state_t *state, void *user_data);

void on_dance_6(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(LCTL(KC_S));
        tap_code16(LCTL(KC_S));
        tap_code16(LCTL(KC_S));
    }
    if(state->count > 3) {
        tap_code16(LCTL(KC_S));
    }
}

void dance_6_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[6].step = dance_step(state);
    switch (dance_state[6].step) {
        case SINGLE_TAP: register_code16(LCTL(KC_S)); break;
        case DOUBLE_TAP: register_code16(LCTL(KC_S)); register_code16(LCTL(KC_S)); break;
        case DOUBLE_HOLD: register_code16(LALT(LCTL(LSFT(KC_5)))); break;
        case DOUBLE_SINGLE_TAP: tap_code16(LCTL(KC_S)); register_code16(LCTL(KC_S));
    }
}

void dance_6_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[6].step) {
        case SINGLE_TAP: unregister_code16(LCTL(KC_S)); break;
        case DOUBLE_TAP: unregister_code16(LCTL(KC_S)); break;
        case DOUBLE_HOLD: unregister_code16(LALT(LCTL(LSFT(KC_5)))); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(LCTL(KC_S)); break;
    }
    dance_state[6].step = 0;
}

tap_dance_action_t tap_dance_actions[] = {
        [DANCE_0] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_0, dance_0_finished, dance_0_reset),
        [DANCE_1] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_1, dance_1_finished, dance_1_reset),
        [DANCE_2] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_2, dance_2_finished, dance_2_reset),
        [DANCE_3] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_3, dance_3_finished, dance_3_reset),
        [DANCE_4] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_4, dance_4_finished, dance_4_reset),
        [DANCE_5] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_5, dance_5_finished, dance_5_reset),
        [DANCE_6] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_6, dance_6_finished, dance_6_reset),
};

bool process_record_user_oryx(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case ST_MACRO_0:
    if (record->event.pressed) {
      SEND_STRING(SS_TAP(X_U)SS_DELAY(100)  SS_TAP(X_2)SS_DELAY(100)  SS_TAP(X_0)SS_DELAY(100)  SS_TAP(X_A)SS_DELAY(100)  SS_TAP(X_C)  SS_DELAY(100) SS_TAP(X_ENTER));
    }
    break;
    case ST_MACRO_1:
    if (record->event.pressed) {
      SEND_STRING(SS_LCTL(SS_TAP(X_X))SS_DELAY(100)  SS_TAP(X_B));
    }
    break;
    case ST_MACRO_2:
    if (record->event.pressed) {
      SEND_STRING(SS_LCTL(SS_TAP(X_X))SS_DELAY(100)  SS_LCTL(SS_TAP(X_W)));
    }
    break;
    case ST_MACRO_3:
    if (record->event.pressed) {
      SEND_STRING(SS_RCTL(SS_TAP(X_X))SS_DELAY(100)  SS_LCTL(SS_TAP(X_S)));
    }
    break;
    case ST_MACRO_4:
    if (record->event.pressed) {
      SEND_STRING(SS_TAP(X_DOT)SS_DELAY(100)  SS_TAP(X_SPACE));
    }
    break;
    case ST_MACRO_5:
    if (record->event.pressed) {
      SEND_STRING(SS_RCTL(SS_TAP(X_X))SS_DELAY(100)  SS_LCTL(SS_TAP(X_MINUS))SS_DELAY(100)  SS_LCTL(SS_TAP(X_G)));
    }
    break;
    case ST_MACRO_6:
    if (record->event.pressed) {
      SEND_STRING(SS_LCTL(SS_TAP(X_X))SS_DELAY(100)  SS_LCTL(SS_LSFT(SS_TAP(X_EQUAL)))SS_DELAY(100)  SS_LCTL(SS_TAP(X_G)));
    }
    break;
    case ST_MACRO_7:
    if (record->event.pressed) {
      SEND_STRING(SS_LCTL(SS_TAP(X_X))SS_DELAY(100)  SS_TAP(X_K));
    }
    break;
    case ST_MACRO_8:
    if (record->event.pressed) {
      SEND_STRING(SS_LCTL(SS_TAP(X_X))SS_DELAY(100)  SS_TAP(X_0));
    }
    break;
    case ST_MACRO_9:
    if (record->event.pressed) {
      SEND_STRING(SS_LCTL(SS_TAP(X_X))SS_DELAY(100)  SS_LCTL(SS_TAP(X_F)));
    }
    break;
    case ST_MACRO_10:
    if (record->event.pressed) {
      SEND_STRING(SS_LCTL(SS_TAP(X_X))SS_DELAY(100)  SS_LCTL(SS_TAP(X_F)));
    }
    break;
    case ST_MACRO_11:
    if (record->event.pressed) {
      SEND_STRING(SS_LCTL(SS_TAP(X_X))SS_DELAY(100)  SS_TAP(X_2));
    }
    break;
    case ST_MACRO_12:
    if (record->event.pressed) {
      SEND_STRING(SS_LALT(SS_TAP(X_X) SS_TAP(X_KP_0) SS_TAP(X_KP_0) SS_TAP(X_KP_2) SS_TAP(X_E) ));
    }
    break;
    case ST_MACRO_13:
    if (record->event.pressed) {
      SEND_STRING(SS_LALT(SS_TAP(X_U) SS_TAP(X_KP_0) SS_TAP(X_KP_0) SS_TAP(X_KP_2) SS_TAP(X_E) ));
    }
    break;
    case ST_MACRO_14:
    if (record->event.pressed) {
      SEND_STRING(SS_LCTL(SS_LSFT(SS_TAP(X_U)))SS_DELAY(100)  SS_TAP(X_KP_0)SS_DELAY(100)  SS_TAP(X_KP_0)SS_DELAY(100)  SS_TAP(X_KP_2)SS_DELAY(100)  SS_TAP(X_E));
    }
    break;
    case ST_MACRO_15:
    if (record->event.pressed) {
      SEND_STRING(SS_LALT(SS_TAP(X_X) SS_TAP(X_KP_0) SS_TAP(X_KP_0) SS_TAP(X_KP_2) SS_TAP(X_E) ) SS_DELAY(100) SS_TAP(X_ENTER));
    }
    break;
    case ST_MACRO_16:
    if (record->event.pressed) {
      SEND_STRING(SS_LALT(SS_TAP(X_U) SS_TAP(X_KP_0) SS_TAP(X_KP_0) SS_TAP(X_KP_2) SS_TAP(X_E) ) SS_DELAY(100) SS_TAP(X_ENTER));
    }
    break;
    case ST_MACRO_17:
    if (record->event.pressed) {
      SEND_STRING(SS_LCTL(SS_LSFT(SS_TAP(X_U)))SS_DELAY(100)  SS_TAP(X_KP_0)SS_DELAY(100)  SS_TAP(X_KP_0)SS_DELAY(100)  SS_TAP(X_KP_2)SS_DELAY(100)  SS_TAP(X_E)  SS_DELAY(100) SS_TAP(X_ENTER));
    }
    break;

    case DUAL_FUNC_0:
      if (record->tap.count > 0) {
        if (record->event.pressed) {
          register_code16(KC_BSPC);
        } else {
          unregister_code16(KC_BSPC);
        }
      } else {
        if (record->event.pressed) {
          register_code16(LCTL(KC_BSPC));
        } else {
          unregister_code16(LCTL(KC_BSPC));
        }  
      }  
      return false;
    case DUAL_FUNC_1:
      if (record->tap.count > 0) {
        if (record->event.pressed) {
          register_code16(KC_ENTER);
        } else {
          unregister_code16(KC_ENTER);
        }
      } else {
        if (record->event.pressed) {
          register_code16(LSFT(KC_ENTER));
        } else {
          unregister_code16(LSFT(KC_ENTER));
        }  
      }  
      return false;
    case DUAL_FUNC_2:
      if (record->tap.count > 0) {
        if (record->event.pressed) {
          register_code16(LCTL(KC_A));
        } else {
          unregister_code16(LCTL(KC_A));
        }
      } else {
        if (record->event.pressed) {
          register_code16(LALT(LSFT(KC_LBRC)));
        } else {
          unregister_code16(LALT(LSFT(KC_LBRC)));
        }  
      }  
      return false;
    case DUAL_FUNC_3:
      if (record->tap.count > 0) {
        if (record->event.pressed) {
          register_code16(LCTL(KC_E));
        } else {
          unregister_code16(LCTL(KC_E));
        }
      } else {
        if (record->event.pressed) {
          register_code16(LALT(LSFT(KC_RBRC)));
        } else {
          unregister_code16(LALT(LSFT(KC_RBRC)));
        }  
      }  
      return false;
    case RGB_SLD:
        if (rawhid_state.rgb_control) {
            return false;
        }
        if (record->event.pressed) {
            rgblight_mode(1);
        }
        return false;
    case HSV_0_0_0:
        if (rawhid_state.rgb_control) {
            return false;
        }
        if (record->event.pressed) {
            rgblight_mode(1);
            rgblight_sethsv(0,0,0);
        }
        return false;
    case HSV_0_137_115:
        if (rawhid_state.rgb_control) {
            return false;
        }
        if (record->event.pressed) {
            rgblight_mode(1);
            rgblight_sethsv(0,137,115);
        }
        return false;
  }
  return true;
}



// ============================================================
// INJECTED BY ORYX-CUSTOM-MOONLANDER WORKFLOW
// ============================================================
bool process_record_user_oryx(uint16_t keycode, keyrecord_t *record);
#include "custom_code.c"

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_user_custom(keycode, record)) {
        return false;
    }
    return process_record_user_oryx(keycode, record);
}
