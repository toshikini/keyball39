/**
 * @file keymap.c
 * @brief Custom QMK keymap:
 * - PR_TGL key toggles trackball speed
 * - Key overrides: Shift+, => ! / Shift+. => ? / Ctrl+H => Backspace / Ctrl+D => Delete
 * - Combo keys: j+k => ESC, .+, => -
 * - Layers 1 and 2 automatically switch to English (KC_LNG2)
 * - Mouse layer (layer 3) auto-disables after 4s, time shortened by key presses
 * - CMDSHIFT4 macro for Mac screenshots (Command+Shift+4)
 */

#include QMK_KEYBOARD_H
#include "quantum.h"


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
};


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

    // マウスレイヤーで特定のキーが押下されたらオートマウスレイヤーのアクティブ時間を短くする
    if (record->event.pressed && layer_state_is(AUTO_MOUSE_DEFAULT_LAYER)) {
        switch (keycode) {
            case KC_BTN1:
            case KC_BTN3:
            case KC_BTN2:
            case LT(3, KC_NO):
            case LT(3, KC_LNG2):
            case LT(1, KC_LNG1):
                set_auto_mouse_timeout(TAPPING_TERM);
                break;
            default:
                break;
        }
    }

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
    JK_ESC,  /** j + k => ESC */
    DOTCOM_MINS,  /** . + , => - */
};


/**
 * @brief コンボに使用するキー列
 */
const uint16_t PROGMEM jk_combo[] = {RCTL_T(KC_J), RGUI_T(KC_K), COMBO_END};
const uint16_t PROGMEM dotcom_combo[] = {KC_DOT, KC_COMMA, COMBO_END};


/**
 * @brief コンボをまとめて定義する配列
 */
combo_t key_combos[] = {
  [JK_ESC] = COMBO(jk_combo, KC_ESC),
  [DOTCOM_MINS] = COMBO(dotcom_combo, KC_MINS),
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
    KC_Q           , KC_W           , KC_E          , KC_R           , KC_T          ,                              KC_Y         , KC_U        , LT(3,KC_I)   , KC_O        , KC_P          ,
    LSFT_T(KC_A)   , LALT_T(KC_S)   , LGUI_T(KC_D)  , LCTL_T(KC_F)   , KC_G          ,                              KC_H         , RCTL_T(KC_J), RGUI_T(KC_K) , RALT_T(KC_L), RSFT_T(KC_ENT),
    KC_Z           , KC_X           , KC_C          , KC_V           , KC_B          ,                              KC_N         , KC_M        , KC_COMM      , KC_DOT      , KC_BSPC       ,
    KC_NO          , KC_NO          , KC_NO         , LT(5,KC_NO)    , LSFT_T(KC_SPC), LT(2,KC_TAB), LT(3,KC_LNG2), LT(1,KC_LNG1), KC_NO       , KC_NO        , KC_NO       , KC_NO
  ),

  // Layer 1: Symbols Layer
  [1] = LAYOUT_universal(
    S(KC_MINS)     , S(KC_2)        , S(KC_3)       , S(KC_4)        , S(KC_5)       ,                              S(KC_6)      , S(KC_7), S(KC_BSLS), KC_BSLS   , S(KC_GRV),
    LSFT_T(KC_MINS), LALT_T(KC_SLSH), LGUI_T(KC_GRV), LCTL_T(KC_QUOT), KC_SCLN       ,                              S(KC_9)      , KC_LBRC, S(KC_COMM), S(KC_LBRC), KC_ENT   ,
    S(KC_EQL)      , S(KC_8)        , KC_EQL        , S(KC_QUOT)     , S(KC_SCLN)    ,                              S(KC_0)      , KC_RBRC, S(KC_DOT) , S(KC_RBRC), KC_BSPC  ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , LSFT_T(KC_SPC), LT(2,KC_TAB), LT(1,KC_LNG2), LT(2,KC_LNG1), KC_NO  , KC_NO     , KC_NO     , KC_NO
  ),

  // Layer 2: Number Layer
  [2] = LAYOUT_universal(
    S(KC_MINS)     , S(KC_2)        , S(KC_3)       , S(KC_4)        , S(KC_5)       ,                              S(KC_6)      , KC_7        , KC_8        , KC_9        , KC_0          ,
    LSFT_T(KC_MINS), LALT_T(KC_SLSH), LGUI_T(KC_GRV), LCTL_T(KC_QUOT), KC_SCLN       ,                              S(KC_9)      , RCTL_T(KC_4), RGUI_T(KC_5), RALT_T(KC_6), RSFT_T(KC_ENT),
    S(KC_EQL)      , S(KC_8)        , KC_EQL        , S(KC_QUOT)     , S(KC_SCLN)    ,                              S(KC_0)      , KC_1        , KC_2        , KC_3        , KC_BSPC       ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , LSFT_T(KC_SPC), LT(2,KC_TAB), LT(1,KC_LNG2), LT(2,KC_LNG1), KC_NO       , KC_NO       , KC_NO       , KC_NO
  ),

  // Layer 3: Function Layer
  [3] = LAYOUT_universal(
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , KC_NO         ,                              KC_NO        , KC_PGDN     , KC_NO       , KC_PGUP     , CMDSHIFT4     ,
    KC_LSFT        , KC_LALT        , KC_LGUI       , KC_LCTL        , KC_NO         ,                              KC_LEFT      , KC_DOWN     , KC_UP       , KC_RGHT     , KC_ENT        ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , KC_NO         ,                              KC_NO        , KC_NO       , KC_NO       , KC_NO       , KC_BSPC       ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , LSFT_T(KC_SPC), LT(2,KC_TAB), LT(3,KC_LNG2), LT(1,KC_LNG1), KC_NO       , KC_NO       , KC_NO       , KC_NO
  ),

  // Layer 4: Mouse Layer
  [4] = LAYOUT_universal(
    KBC_RST        , KC_NO          , KC_NO         , KC_NO          , KC_NO         ,                              KC_NO        , KC_NO       , LT(3,KC_NO) , KC_NO       , KC_NO         ,
    KC_LSFT        , KC_LALT        , KC_LGUI       , KC_LCTL        , KC_NO         ,                              KC_NO        , KC_BTN1     , KC_BTN3     , KC_BTN2     , PR_TGL        ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , S(KC_SCLN)    ,                              KC_NO        , KC_NO       , KC_NO       , KC_NO       , KC_NO         ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , LSFT_T(KC_SPC), LT(2,KC_TAB), LT(3,KC_LNG2), LT(1,KC_LNG1), KC_NO       , KC_NO       , KC_NO       , KC_NO
  ),

  // Layer 5: Manual Mouse Layer
  [5] = LAYOUT_universal(
    KBC_RST        , KC_NO          , KC_NO         , KC_NO          , KC_NO         ,                              KC_NO        , KC_NO       , LT(3,KC_NO) , KC_NO       , KC_NO         ,
    KC_LSFT        , KC_LALT        , KC_LGUI       , KC_LCTL        , KC_NO         ,                              KC_NO        , KC_BTN1     , KC_BTN3     , KC_BTN2     , PR_TGL        ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , S(KC_SCLN)    ,                              KC_NO        , KC_NO       , KC_NO       , KC_NO       , KC_NO         ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , LSFT_T(KC_SPC), LT(2,KC_TAB), LT(3,KC_LNG2), LT(1,KC_LNG1), KC_NO       , KC_NO       , KC_NO       , KC_NO
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
        case 1:
            tap_code(KC_LNG2);  // レイヤー1になったら英数入力に切り替える
            break;
        case 2:
            tap_code(KC_LNG2);  // レイヤー2になったら英数入力に切り替える
            break;
        case 3:
            set_auto_mouse_enable(false);
            keyball_set_scroll_mode(true);
            break;
        default:
            // デフォルトレイヤーに戻ったらオートマウスの設定を元に戻す。AUTO_MOUSE_TIMEは最大値が1秒なので4倍にする
            set_auto_mouse_timeout(AUTO_MOUSE_TIME * 4);
            set_auto_mouse_enable(true);
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
}
