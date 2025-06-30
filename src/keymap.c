/**
 * @file keymap.c
 * @brief Custom QMK keymap:
 * - PR_TGL key toggles trackball speed
 * - Key overrides: Shift+, => ! / Shift+. => ? / Ctrl+H => Backspace / Ctrl+D => Delete
 * - Combo keys
 */

#include QMK_KEYBOARD_H
#include "quantum.h"
#include "os_detection.h"


/*********************************************************************
 * Macros
 *********************************************************************/
#define CMDSHIFT4 G(S(KC_4))  /** Mac の Command + Shift + 4 */


/*********************************************************************
 * Trackball speed control
 *********************************************************************/
/**
 * @brief トラックボールの速度を変更するためのトグル関数
 */
void precision_toggle(bool pressed) {
    if (!pressed) {
        return;
    }

    // 低速モードと通常モードのCPI値
    const uint16_t down_cpi = 3;
    const uint16_t nomal_cpi = 10;

    // 現在のCPIに基づいて切り替え
    uint16_t current_cpi = keyball_get_cpi();
    keyball_set_cpi(current_cpi == nomal_cpi ? down_cpi : nomal_cpi);
}


/*********************************************************************
 * Key Press Handling
 *********************************************************************/

/**
 * @enum original_keycodes
 * @brief 独自キーコード用のインデックス
 */
enum original_keycodes {
    PR_TGL = SAFE_RANGE,  /** トラックボールの速度を変更するためのトグル */
    JIS_TGL = SAFE_RANGE + 1,  /** JIS配列キーボードの場合のトグル */
    KATAKANA = SAFE_RANGE + 2,
    CTLSPC = SAFE_RANGE + 3,
};


/**
 * @brief JIS配列キーボードかどうかのフラグ
 */
bool is_jis = false;

/**
 * @brief OSを検出する関数
 */
uint32_t keyboard_callback(uint32_t delay, void *cb_arg) {
    switch(detected_host_os()) {
        case OS_WINDOWS:
            is_jis = true;
        default:
            is_jis = false;
    }
    return 0;
}


/**
 * @brief キー入力を処理するフック関数
 *
 * 独自キーコードや特定条件下での振る舞いを上書きする。
 * @return true で通常通り処理続行、false で以降の処理をスキップ
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static bool backspace_registered  = false;
    static bool delete_registered  = false;
    uint8_t mod_state = get_mods();

    switch(keycode) {
        case PR_TGL:
            precision_toggle(record->event.pressed);
            return false;
        case KC_H: // Ctrl + H で Backspace
            if (record->event.pressed) {
                if (mod_state & MOD_MASK_CTRL) {
                    del_mods(MOD_MASK_CTRL);
                    register_code(KC_BSPC);
                    backspace_registered = true;
                    set_mods(mod_state);
                    return false;
                }
            } else {
                if (backspace_registered) {
                    unregister_code(KC_BSPC);
                    backspace_registered = false;
                    return false;
                }
            }
            return true;
        case KC_D: // Ctrl + D で Delete
            if (record->event.pressed) {
                if (mod_state & MOD_MASK_CTRL) {
                    del_mods(MOD_MASK_CTRL);
                    register_code(KC_DEL);
                    delete_registered = true;
                    set_mods(mod_state);
                    return false;
                }
            } else {
                if (delete_registered) {
                    unregister_code(KC_DEL);
                    delete_registered = false;
                    return false;
                }
            }
            return true;
        case JIS_TGL:
            if (record->event.pressed) {
                is_jis = !is_jis;
            }
            return false;
        case KATAKANA:
            if (record->event.pressed && is_jis) {
                register_code(KC_F7);
                unregister_code(KC_F7);
                return false;
            } else if (record->event.pressed) {
                register_code(KC_LCTL);
                register_code(KC_K);
                unregister_code(KC_K);
                unregister_code(KC_LCTL);
                return false;
            }
            break;
        case KC_AT:
            if (record->event.pressed && is_jis) {
                register_code(KC_LBRC);
                unregister_code(KC_LBRC);
                return false;
            }
            break;
        case KC_HASH:
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_3);
                unregister_code(KC_3);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
        case KC_DLR:
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_4);
                unregister_code(KC_4);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
        case KC_PERC:
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_5);
                unregister_code(KC_5);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
        case KC_CIRC:
            if (record->event.pressed && is_jis) {
                register_code(KC_EQL);
                unregister_code(KC_EQL);
                return false;
            }
            break;
        case KC_AMPR:
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_6);
                unregister_code(KC_6);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
        case KC_PIPE:
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_INT3);
                unregister_code(KC_INT3);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
        case KC_BSLS:
            if (record->event.pressed && is_jis) {
                register_code(KC_INT1);
                unregister_code(KC_INT1);
                return false;
            }
            break;
        case KC_TILD:
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_EQL);
                unregister_code(KC_EQL);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
        case KC_PLUS:
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_SCLN);
                unregister_code(KC_SCLN);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
        case KC_ASTR:
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_QUOT);
                unregister_code(KC_QUOT);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
        case LCTL_T(KC_QUOT):
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_7);
                unregister_code(KC_7);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
        case KC_EQL:
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_MINS);
                unregister_code(KC_MINS);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
        case KC_DQUO:
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_2);
                unregister_code(KC_2);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
        case KC_COLN:
            if (record->event.pressed && is_jis) {
                register_code(KC_QUOT);
                unregister_code(KC_QUOT);
                return false;
            }
            break;
        case LGUI_T(KC_GRV):
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_LBRC);
                unregister_code(KC_LBRC);
                unregister_code(KC_LSFT);
                return false;
            }
        case KC_RPRN:
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_9);
                unregister_code(KC_9);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
        case KC_LPRN:
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_8);
                unregister_code(KC_8);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
        case KC_LBRC:
            if (record->event.pressed && is_jis) {
                register_code(KC_RBRC);
                unregister_code(KC_RBRC);
                return false;
            }
            break;
        case KC_RBRC:
            if (record->event.pressed && is_jis) {
                register_code(KC_NUHS);
                unregister_code(KC_NUHS);
                return false;
            }
            break;
        case KC_LCBR:
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_RBRC);
                unregister_code(KC_RBRC);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
        case KC_RCBR:
            if (record->event.pressed && is_jis) {
                register_code(KC_LSFT);
                register_code(KC_NUHS);
                unregister_code(KC_NUHS);
                unregister_code(KC_LSFT);
                return false;
            }
            break;
         default:
            break;
    }

    return true;
}


/*********************************************************************
 * Key Overrides
 *********************************************************************/
/**
 * @brief Shift + , => '!'
 *
 * @details
 * - `MOD_MASK_SHIFT`: Shift を押しながら
 * - `KC_COMMA`: ',' (物理キー)を入力すると
 * - `KC_EXLM`: '!' (Shift+数字キーや記号キーのエイリアス)を出力
 */
const key_override_t exclamation_override = ko_make_basic(
    MOD_MASK_SHIFT,
    KC_COMMA,
    KC_EXLM
);

/**
 * @brief Shift + . => '?'
 *
 * @details
 * - `MOD_MASK_SHIFT`: Shift を押しながら
 * - `KC_DOT`: '.' (物理キー)を入力すると
 * - `KC_QUES`: '?' (Shift + / のエイリアス)を出力
 */
const key_override_t question_override = ko_make_basic(
    MOD_MASK_SHIFT,
    KC_DOT,
    KC_QUES
);

/**
 * @brief 定義した key_override_t をまとめた配列
 */
const key_override_t *overrides_list[] = {
    &exclamation_override,
    &question_override,
    NULL  // 終端を示すためNULLを入れる
};

// QMK が参照する key_overrides に代入
const key_override_t **key_overrides = overrides_list;


/*********************************************************************
 * Combos
 *********************************************************************/
/**
 * @enum combos
 * @brief コンボキー用のインデックス
 */
enum combos {
    CO_ESC,
    CO_TAB,
    CO_SHIFTTAB,
    CO_BTN1,
    CO_BTN2,
    CO_BTN3,
};


/**
 * @brief コンボに使用するキー列
 */
const uint16_t PROGMEM ui_combo[] = {KC_U, KC_I, COMBO_END};
const uint16_t PROGMEM dotcom_combo[] = {KC_COMM, KC_DOT, COMBO_END};
const uint16_t PROGMEM mdot_combo[] = {KC_M, KC_DOT, COMBO_END};
const uint16_t PROGMEM mcom_combo[] = {KC_COMM, KC_M, COMBO_END};
const uint16_t PROGMEM er_combo[] = {KC_E, KC_R, COMBO_END};
const uint16_t PROGMEM we_combo[] = {KC_E, KC_W, COMBO_END};


/**
 * @brief コンボをまとめて定義する配列
 */
combo_t key_combos[] = {
  [CO_ESC] = COMBO(ui_combo, KC_ESC),
  [CO_BTN1] = COMBO(mcom_combo, KC_BTN1),
  [CO_BTN2] = COMBO(dotcom_combo, KC_BTN2),
  [CO_BTN3] = COMBO(mdot_combo, KC_BTN3),
  [CO_TAB] = COMBO(er_combo, KC_TAB),
  [CO_SHIFTTAB] = COMBO(we_combo, LSFT(KC_TAB)),
};


/*********************************************************************
 * Keymaps
 *********************************************************************/
/**
 * @brief 各レイヤーのキーマップ
 *
 * clang-format off
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // Layer 0: Default Layer
  [0] = LAYOUT_universal(
    KC_Q           , KC_W           , KC_E          , KC_R           , KC_T          ,                              KC_Y         , KC_U        , KC_I           , KC_O        , KC_P          ,
    LSFT_T(KC_A)   , LALT_T(KC_S)   , LGUI_T(KC_D)  , LCTL_T(KC_F)   , KC_G          ,                              KC_H         , RCTL_T(KC_J), RGUI_T(KC_K)   , RALT_T(KC_L), RSFT_T(KC_ENT),
    LT(1,KC_Z)     , KC_X           , KC_C          , KC_V           , KC_B          ,                              KC_N         , KC_M        , KC_COMM        , KC_DOT      , KC_BSPC,
    KC_NO          , KC_BTN1        , KC_BTN2       , LT(2,KC_SPC)   , LT(2,KC_SPC)  , LT(2,KC_SPC), LT(2,KC_SPC), LT(2,KC_SPC)  , KC_NO       , KC_NO          , KC_NO       , KC_NO
  ),

  // Layer 1: Symbols Layer
  [1] = LAYOUT_universal(
    KC_UNDS        , KC_AT          , KC_HASH       , KC_DLR         , KC_PERC       ,                              KC_CIRC      , KC_AMPR, KC_PIPE   , KC_BSLS   , KC_TILD  ,
    KC_MINS        , KC_PLUS        , KC_DQUO       , KC_QUOT        , KC_COLN       ,                              KC_LPRN      , KC_LBRC, KC_LABK   , KC_LCBR   , KC_ENT   ,
    KC_EQL         , KC_ASTR        , KC_SLSH       , KC_GRV         , KC_SCLN       ,                              KC_RPRN      , KC_RBRC, KC_RABK   , KC_RCBR   , KC_BSPC   ,
    KC_NO          , KC_BTN1        , KC_BTN2       , LT(2,KC_SPC)   , LT(2,KC_SPC)  , LT(2,KC_SPC), LT(2,KC_SPC), LT(2,KC_SPC)  , KC_NO       , KC_NO          , KC_NO       , KC_NO
  ),

  // Layer 2: Function/Number Layer
  [2] = LAYOUT_universal(
    KC_1           , KC_2           , KC_3          , KC_4           , KC_5          ,                              KC_6         , KC_7        , KC_8        , KC_9        , KC_0          ,
    LSFT_T(CW_TOGG), KC_LALT        , KC_LGUI       , KC_LCTL        , KC_NO         ,                              KC_LEFT      , KC_DOWN     , KC_UP       , KC_RGHT     , KC_ENT       ,
    JIS_TGL        , KC_NO          , KC_BTN4       , KC_BTN5        , KC_NO         ,                              KC_HOME      , KC_PGDN     , KC_PGUP     , KC_END      , KATAKANA      ,
    KC_NO          , KC_BTN1        , KC_BTN2       , LT(2,KC_SPC)   , LT(2,KC_SPC)  , LT(2,KC_SPC), LT(2,KC_SPC), LT(2,KC_SPC)  , KC_NO       , KC_NO          , KC_NO       , KC_NO
  ),
};
/** clang-format on */


/*********************************************************************
 * Layer Change hook
 *********************************************************************/
/**
 * @brief レイヤーの状態を更新するフック関数
 */
layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t highest_layer = get_highest_layer(state);

    switch (highest_layer) {
        case 2:
            keyball_set_scroll_mode(true);
            break;
        default:
            keyball_set_scroll_mode(false);
            break;
    }
    return state;
}


/**
 * @brief キーボード初期化後に呼ばれるユーザーフック関数
 */
void keyboard_post_init_user(void) {
    layer_state_set_user(layer_state);
    precision_toggle(true);  // トラックボールの速度を初期化
    defer_exec(100, keyboard_callback, NULL);  // OSを検出
}
