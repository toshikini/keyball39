## Costmized qmk firmware for Keyball39

This is a costmized qmk firmware for Keyball39. The original firmware is [Yowkees/keyball](https://github.com/Yowkees/keyball). The original firmware is based on the qmk firmware. I made some changes to the original firmware to make it more suitable for me.

### Changes

 * - PR_TGL key toggles trackball speed
 * - Key overrides: Shift+, => ! / Shift+. => ? / Ctrl+H => Backspace
 * - Combo keys: j+k => ESC, .+, => -
 * - Layers 1 and 2 automatically switch to English (KC_LNG2)
 * - Mouse layer (layer 3) auto-disables after 3s, time shortened by key presses
 * - CMDSHIFT4 macro for Mac screenshots (Command+Shift+4)


## How to Build

This repository provides two methods for building the project: directly on the host OS or using Docker.

### Building on the Host OS

#### Prerequisites

If you are using macOS, install the following tools beforehand:

```bash
brew install qmk/qmk/qmk
brew tap osx-cross/avrbrew
brew install avr-gcc arm-none-eabi-gcc
```

Next, clone the `qmk_firmware` repository:

```bash
git clone https://github.com/qmk/qmk_firmware.git --depth 1 --recurse-submodules --shallow-submodules -b 0.22.14 qmk
```

Then, install the required Python libraries. Note that the path may vary depending on your environment and version:

```bash
/opt/homebrew/Cellar/qmk/1.1.6/libexec/bin/python -m pip install -r qmk/requirements.txt
```

#### Steps

Once the prerequisites are set up, clone or fork this repository, modify the source code under the `src` directory, and run the following command:

```bash
make
```

That's it! The generated hex file will be created in the `build` directory.

---

### Building with Docker

#### Prerequisites

Make sure the following tools are installed:

- Docker
- Docker Compose

#### Steps

Clone or fork this repository, modify the source code under the `src` directory, and run the following commands:

```bash
make build
make up
```

That's it! The generated hex file will be created in the `build` directory.

---

For more details on the build process, refer to the `Makefile`.


