/**
 * @file keymap.c
 * @brief カスタムキーマップの実装
 *
 * - 独自キーコード: CMDSHIFT4
 * - キーの上書き: SHIFT + , => !, SHIFT + . => ?, CTRL + H => BS に変更
 * - コンボキー: j+k => ESC (JK_ESC)
 * - レイヤー切り替え時の挙動: レイヤー1,2 になったら英数入力に切り替える
 * - マウスレイヤーは3秒で自動でオフになるがキーが押されたらオフになるまでの時間を短くする
 *
 */

#include QMK_KEYBOARD_H
#include "quantum.h"


/*********************************************************************
 * 独自キーコード
 *********************************************************************/
#define CMDSHIFT4 G(S(KC_4))  /**< Mac の Command + Shift + 4 */


/*********************************************************************
 * キーが押された時の挙動の定義(デフォルトの挙動を書き換える)
 *********************************************************************/
/**
 * @brief キー入力を処理するフック関数
 *
 * 独自キーコードや特定条件下での振る舞いを上書きする。
 * @return true で通常通り処理続行、false で以降の処理をスキップ
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // マウスレイヤーでキーが押下されたらオートマウスレイヤーのアクティブ時間を短くする
    if (record->event.pressed && layer_state_is(AUTO_MOUSE_DEFAULT_LAYER)) {
        set_auto_mouse_timeout(TAPPING_TERM);
    }

    return true;
}


/*********************************************************************
 * キーの上書き
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
 * @brief Ctrl + h => Backspace
 *
 * @details
 * - `MOD_MASK_CTRL`: Ctrl を押しながら
 * - `KC_H`: 'h' (物理キー)を入力すると
 * - `KC_BSPC`: Backspace キーコードを出力
 */
const key_override_t backspace_override = ko_make_basic(
    MOD_MASK_CTRL,
    KC_H,
    KC_BSPC
);

/**
 * @brief 定義した key_override_t をまとめた配列
 */
const key_override_t *overrides_list[] = {
    &exclamation_override,
    &question_override,
    &backspace_override,
    NULL  // 終端を示すためNULLを入れる
};

// QMK が参照する key_overrides に代入
const key_override_t **key_overrides = overrides_list;


/*********************************************************************
 * コンボキーの定義
 *********************************************************************/
/**
 * @enum combos
 * @brief コンボキー用のインデックス
 */
enum combos {
    JK_ESC,  /**< j + k => ESC */
};


/**
 * @brief コンボに使用するキー列
 */
const uint16_t PROGMEM jk_combo[] = {RCTL_T(KC_J), RGUI_T(KC_K), COMBO_END};


/**
 * @brief コンボをまとめて定義する配列
 */
combo_t key_combos[] = {
  [JK_ESC] = COMBO(jk_combo, KC_ESC),
};


/*********************************************************************
 * キーマップの定義
 *********************************************************************/
/**
 * @brief 各レイヤーのキーマップ
 *
 * clang-format off
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default
  [0] = LAYOUT_universal(
    KC_Q           , KC_W           , KC_E          , KC_R           , KC_T          ,                              KC_Y         , KC_U        , LT(3,KC_I)   , KC_O        , KC_P          ,
    LSFT_T(KC_A)   , LALT_T(KC_S)   , LGUI_T(KC_D)  , LCTL_T(KC_F)   , KC_G          ,                              KC_H         , RCTL_T(KC_J), RGUI_T(KC_K) , RALT_T(KC_L), RSFT_T(KC_ENT),
    KC_Z           , KC_X           , KC_C          , KC_V           , KC_B          ,                              KC_N         , KC_M        , KC_COMM      , KC_DOT      , KC_MINS       ,
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
    KC_LSFT        , KC_LALT        , KC_LGUI       , KC_LCTL        , KC_NO         ,                              KC_NO        , KC_BTN1     , KC_BTN3     , KC_BTN2     , KC_NO         ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , KC_NO         ,                              KC_NO        , KC_NO       , KC_NO       , KC_NO       , KC_NO         ,
    KC_NO          , KC_NO          , KC_NO         , KC_NO          , LSFT_T(KC_SPC), LT(2,KC_TAB), LT(3,KC_LNG2), LT(1,KC_LNG1), KC_NO       , KC_NO       , KC_NO       , KC_NO
  ),
};
/** clang-format on */


/*********************************************************************
 * レイヤー切り替えた時の挙動
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
            // デフォルトレイヤーに戻ったらオートマウスの設定を元に戻す。AUTO_MOUSE_TIMEは最大値が1秒なので3倍にする
            set_auto_mouse_timeout(AUTO_MOUSE_TIME * 3);
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
}
