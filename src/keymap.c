#include QMK_KEYBOARD_H
#include "quantum.h"


// Tap Dance
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP, // Send two single taps
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

// Tap dance enums
enum {
    J_CTL,
    L_CTL,
};

td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) {
        if (state->interrupted) return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed) return TD_DOUBLE_HOLD;
        else return TD_DOUBLE_TAP;
    }

    if (state->count == 3) {
        if (state->interrupted || !state->pressed) return TD_TRIPLE_TAP;
        else return TD_TRIPLE_HOLD;
    } else return TD_UNKNOWN;
}

static td_tap_t jtap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

static td_tap_t ltap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

bool is_double_hold_active = false;
bool is_double_hold_l_active = false;

void j_finished(tap_dance_state_t *state, void *user_data) {
    jtap_state.state = cur_dance(state);
    switch (jtap_state.state) {
        case TD_SINGLE_TAP:
            register_code(KC_J);
            break;
        case TD_SINGLE_HOLD:
            register_code(KC_LCTL);
            break;
        case TD_DOUBLE_TAP:
            register_code(KC_ESC);
            break;
        case TD_DOUBLE_HOLD:
            is_double_hold_active = true;
            break;
        case TD_DOUBLE_SINGLE_TAP:
            tap_code(KC_J);
            tap_code(KC_J);
            break;
        default:
            break;
    }
}

void l_finished(tap_dance_state_t *state, void *user_data) {
    ltap_state.state = cur_dance(state);
    switch (ltap_state.state) {
        case TD_SINGLE_TAP:
            register_code(KC_L);
            break;
        case TD_SINGLE_HOLD:
            register_code(KC_LALT);
            break;
        case TD_DOUBLE_TAP:
            register_code(KC_MINS);
            break;
        case TD_DOUBLE_HOLD:
            is_double_hold_l_active = true;
            break;
        case TD_DOUBLE_SINGLE_TAP:
            tap_code(KC_L);
            tap_code(KC_L);
            break;
        default:
            break;
    }
}

void j_reset(tap_dance_state_t *state, void *user_data) {
    switch (jtap_state.state) {
        case TD_SINGLE_TAP:
            unregister_code(KC_J);
            break;
        case TD_SINGLE_HOLD:
            unregister_code(KC_LCTL);
            break;
        case TD_DOUBLE_TAP:
            unregister_code(KC_ESC);
            break;
        case TD_DOUBLE_HOLD:
            is_double_hold_active = false;
            break;
        default:
            break;
    }
    jtap_state.state = TD_NONE;
}

void l_reset(tap_dance_state_t *state, void *user_data) {
    switch (ltap_state.state) {
        case TD_SINGLE_TAP:
            unregister_code(KC_L);
            break;
        case TD_SINGLE_HOLD:
            unregister_code(KC_LALT);
            break;
        case TD_DOUBLE_TAP:
            unregister_code(KC_MINS);
            break;
        case TD_DOUBLE_HOLD:
            is_double_hold_l_active = false;
            break;
        default:
            break;
    }
    ltap_state.state = TD_NONE;
}

void matrix_scan_user(void) {
    if (is_double_hold_active) {
        tap_code(KC_J);
        wait_ms(TAPPING_TERM / 3);
    }
    if (is_double_hold_l_active) {
        tap_code(KC_L);
        wait_ms(TAPPING_TERM / 3);
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [J_CTL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, j_finished, j_reset),
    [L_CTL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, l_finished, l_reset)
};



// 独自キーの作成
enum custom_keycodes {
    CMDSHIFT4 = SAFE_RANGE,
};



// デフォルトキーを上書きする
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static bool comm_registered = false;
    static bool dot_registered  = false;
    uint8_t mod_state = get_mods();

    switch (keycode) {
        case CMDSHIFT4:
            if (record->event.pressed) {
                register_code(KC_LGUI);   // MacのCommandキー(Left GUI)
                register_code(KC_LSFT);  // Shiftキー
                register_code(KC_4);     // '4'キー
                unregister_code(KC_4);
                unregister_code(KC_LSFT);
                unregister_code(KC_LGUI);
            }
            return false; // すでに独自処理をしたので false を返す
        case KC_COMM: // 「,」キーが押された/離された時
            if (record->event.pressed) {
                // Shift が押されているかチェック
                if (mod_state & MOD_MASK_SHIFT) {
                    del_mods(MOD_MASK_SHIFT);
                    register_code16(LSFT(KC_1));
                    comm_registered = true;
                    set_mods(mod_state);
                    return false;
                }
            } else { // 離したとき
                if (comm_registered) {
                    unregister_code16(LSFT(KC_1));
                    comm_registered = false;
                    return false;
                }
            }
            // 通常処理に任せる場合は true
            return true;

        case KC_DOT: // 「.」キーが押された/離された時
            if (record->event.pressed) {
                if (mod_state & MOD_MASK_SHIFT) {
                    del_mods(MOD_MASK_SHIFT);
                    register_code16(LSFT(KC_SLSH));
                    dot_registered = true;
                    set_mods(mod_state);
                    return false;
                }
            } else {
                if (dot_registered) {
                    unregister_code16(LSFT(KC_SLSH));
                    dot_registered = false;
                    return false;
                }
            }
            return true;
    }
    return true;
}



// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default
  [0] = LAYOUT_universal(
    KC_Q           , KC_W           , KC_E          , KC_R           , KC_T          ,                              KC_Y         , KC_U        , LT(3,KC_I)   , KC_O        , KC_P          ,
    LSFT_T(KC_A)   , LALT_T(KC_S)   , LGUI_T(KC_D)  , LCTL_T(KC_F)   , KC_G          ,                              KC_H         , TD(J_CTL)   , RGUI_T(KC_K) , TD(L_CTL)   , RSFT_T(KC_ENT),
    KC_Z           , KC_X           , KC_C          , KC_V           , KC_B          ,                              KC_N         , KC_M        , KC_COMM      , KC_DOT      , KC_BSPC       ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , LSFT_T(KC_SPC), LT(2,KC_TAB), LT(3,KC_LNG2), LT(1,KC_LNG1), KC_NO       , KC_NO        , KC_NO       , KC_NO
  ),

  [1] = LAYOUT_universal(
    S(KC_MINS)     , S(KC_2)        , S(KC_3)       , S(KC_4)        , S(KC_5)       ,                              S(KC_6)      , S(KC_7), S(KC_BSLS), KC_BSLS   , S(KC_GRV),
    LSFT_T(KC_MINS), LALT_T(KC_SLSH), LGUI_T(KC_GRV), LCTL_T(KC_QUOT), KC_SCLN       ,                              S(KC_9)      , KC_LBRC, S(KC_COMM), S(KC_LBRC), KC_ENT   ,
    S(KC_EQL)      , S(KC_8)        , KC_EQL        , S(KC_QUOT)     , S(KC_SCLN)    ,                              S(KC_0)      , KC_RBRC, S(KC_DOT) , S(KC_RBRC), KC_BSPC  ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , LSFT_T(KC_SPC), LT(2,KC_TAB), LT(1,KC_LNG2), LT(2,KC_LNG1), KC_NO  , KC_NO     , KC_NO     , KC_NO
  ),

  [2] = LAYOUT_universal(
    S(KC_MINS)     , S(KC_2)        , S(KC_3)       , S(KC_4)        , S(KC_5)       ,                              S(KC_6)      , KC_7        , KC_8        , KC_9        , KC_0          ,
    LSFT_T(KC_MINS), LALT_T(KC_SLSH), LGUI_T(KC_GRV), LCTL_T(KC_QUOT), KC_SCLN       ,                              S(KC_9)      , RCTL_T(KC_4), RGUI_T(KC_5), RALT_T(KC_6), RSFT_T(KC_ENT),
    S(KC_EQL)      , S(KC_8)        , KC_EQL        , S(KC_QUOT)     , S(KC_SCLN)    ,                              S(KC_0)      , KC_1        , KC_2        , KC_3        , KC_BSPC       ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , LSFT_T(KC_SPC), LT(2,KC_TAB), LT(1,KC_LNG2), LT(2,KC_LNG1), KC_NO       , KC_NO       , KC_NO       , KC_NO
  ),

  [3] = LAYOUT_universal(
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , KC_NO         ,                              KC_NO        , KC_NO       , KC_NO       , KC_NO       , CMDSHIFT4     ,
    KC_LSFT        , KC_LALT        , KC_LGUI       , KC_LCTL        , KC_NO         ,                              KC_LEFT      , KC_DOWN     , KC_UP       , KC_RGHT     , KC_ENT        ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , KC_NO         ,                              KC_NO        , KC_PGDN     , KC_PGUP     , KC_NO       , KC_BSPC       ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , LSFT_T(KC_SPC), LT(2,KC_TAB), LT(3,KC_LNG2), LT(1,KC_LNG1), KC_NO       , KC_NO       , KC_NO       , KC_NO
  ),

  [4] = LAYOUT_universal(
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , KC_NO         ,                              KC_NO        , KC_NO       , KC_NO       , KC_NO       , KC_NO         ,
    KC_LSFT        , KC_LALT        , KC_LGUI       , KC_LCTL        , KC_NO         ,                              KC_BTN4      , KC_BTN1     , KC_BTN3     , KC_BTN2     , KC_NO         ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , KC_NO         ,                              KC_NO        , KC_NO       , KC_NO       , KC_NO       , KC_NO         ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , LSFT_T(KC_SPC), LT(2,KC_TAB), LT(3,KC_LNG2), LT(1,KC_LNG1), KC_NO       , KC_NO       , KC_NO       , KC_NO
  ),
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t highest_layer = get_highest_layer(state);

    switch (highest_layer) {
        case 1:
            tap_code(KC_LNG2);
            break;
        case 2:
            tap_code(KC_LNG2);
            break;
        case 3:
            set_auto_mouse_enable(false);
            keyball_set_scroll_mode(true);
            break;
        default:
            set_auto_mouse_enable(true);
            keyball_set_scroll_mode(false);
            break;
    }
    return state;
}

void keyboard_post_init_user(void) {
    layer_state_set_user(layer_state);
}

#ifdef OLED_ENABLE
#    include "lib/oledkit/oledkit.h"

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    keyball_oled_render_layerinfo();
}
#endif
