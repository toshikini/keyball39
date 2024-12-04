# 定数定義
BUILD_OUTPUT           := build

QMK_FIRMWARE_REPO      := https://github.com/qmk/qmk_firmware.git
QMK_FIRMWARE_VERSION   := 0.22.14
QMK_FIRMWARE_DIR       := $(BUILD_OUTPUT)/qmk_firmware

KEYBALL_FIRMWARE_REPO  := https://github.com/Yowkees/keyball.git
KEYBALL_FIRMWARE_DIR   := $(BUILD_OUTPUT)/keyball
KEYBALL_FIRMWARE_SRC   := $(KEYBALL_FIRMWARE_DIR)/qmk_firmware/keyboards/keyball
KEYBALL39_KEYMAP_DIR   := $(KEYBALL_FIRMWARE_SRC)/keyball39/keymaps


# タスク一覧
.PHONY: all clone_qmk clone_keyball build_keyball39 clean build up rm

# ローカル環境を使う場合
all: $(BUILD_OUTPUT)

clone_qmk:
	if [ ! -d "$(QMK_FIRMWARE_DIR)" ]; then \
		git clone --depth 1 --recurse-submodules --shallow-submodules --branch $(QMK_FIRMWARE_VERSION) $(QMK_FIRMWARE_REPO) $(QMK_FIRMWARE_DIR); \
	fi

clone_keyball:
	if [ ! -d "$(KEYBALL_FIRMWARE_DIR)" ]; then \
		git clone --depth 1 $(KEYBALL_FIRMWARE_REPO) $(KEYBALL_FIRMWARE_DIR); \
	fi

prepare: clone_qmk clone_keyball
	mkdir -p $(KEYBALL39_KEYMAP_DIR)
	cp -r src $(KEYBALL39_KEYMAP_DIR)
	cp -r $(KEYBALL_FIRMWARE_SRC) $(QMK_FIRMWARE_DIR)/keyboards/

$(BUILD_OUTPUT): prepare
	cd $(QMK_FIRMWARE_DIR) && make keyball/keyball39:src
	mv $(QMK_FIRMWARE_DIR)/*.hex $(BUILD_OUTPUT)

clean:
	rm -rf build/*


# Dockerを使う場合
build:
	docker-compose build

up:
	docker-compose up

rm:
	docker-compose rm -f
