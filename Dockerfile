FROM ghcr.io/qmk/qmk_cli:latest

# 作業ディレクトリ設定
WORKDIR /usr/src

# 環境変数
ENV QMK_VERSION=0.22.14

# 必要なリポジトリをクローンしてセットアップ
RUN git clone --depth 1 --recurse-submodules --shallow-submodules --branch ${QMK_VERSION} https://github.com/qmk/qmk_firmware.git qmk_firmware \
    && qmk setup --home qmk_firmware --yes \
    && /usr/bin/python3 -m pip install -r qmk_firmware/requirements.txt \
    && git clone --depth 1 https://github.com/Yowkees/keyball.git keyball
