#!/bin/bash

set -e

NUM_CORES=$(nproc) # 論理コア数を取得

BUILD_DIR="/usr/src/build"
QMK_DIR="/usr/src/qmk_firmware"
KEYBALL_DIR="/usr/src/keyball"
KEYBALL_SRC="${KEYBALL_DIR}/qmk_firmware/keyboards/keyball"
KEYBALL_TARGET="${QMK_DIR}/keyboards"
KEYBALL_KEYMAP_DIR="${KEYBALL_SRC}/keyball39/keymaps"
KEYBALL_C_FILE="${KEYBALL_SRC}/lib/keyball/keyball.c"

# BLの定義をマクロに変更しないとコンパイルエラーが発生するので対応
update_keymap_definition() {
  sed -i 's/^static const char BL =.*$/#define BL \x27\xB0\x27/' "${KEYBALL_C_FILE}"
}

prepare() {
  ln -sf /usr/src/src "${KEYBALL_KEYMAP_DIR}/"
  ln -sf "${KEYBALL_SRC}" "${KEYBALL_TARGET}/"
}

build() {
  cd "${QMK_DIR}" && qmk compile -j ${NUM_CORES} -kb keyball/keyball39 -km src
  mv "${QMK_DIR}"/*.hex "${BUILD_DIR}/"
}

main() {
  update_keymap_definition
  prepare
  build
}

main
