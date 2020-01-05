# WebRTC Native Client Momo Custom

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

## WebRTC Native Client Momo Custom について

WebRTC Native Client Momo Custom は libwebrtc を利用しブラウザなしで様々な環境で動作する WebRTC ネイティブクライアント [Momo](https://github.com/shiguredo/momo) に対して **汎用的ではない特殊な機能を追加したカスタム版です**

[Momo](https://github.com/shiguredo/momo) のカスタムにおける参考のために公開しています。積極的なメンテナンスは行いません。VERSIONファイルとCHANGESでオリジナルのMomoとの世代差を確認した上でご利用ください

WebRTC による映像音声通信のみであればカスタム元の [Momo](https://github.com/shiguredo/momo) をご利用ください。ドキュメントを含めた公開と積極的な最新版追従を行っています。

### カスタム版について

カスタムの差分がわかりやすいようにブランチで管理しています。現在は下記のブランチを提供しています

- base
  - カスタムのベースとしたオリジナルの [Momo](https://github.com/shiguredo/momo) の develop スナップショットからメンテを行えないドキュメントとビルドシステムを削除したものです

### ハードウェアエンコーダへの対応

- Raspberry Pi の GPU に積まれている H.264 ハードウェアエンコーダー機能を利用することが可能です
- macOS に積まれている [VideoToolbox](https://developer.apple.com/documentation/videotoolbox) の H.264 ハードウェアエンコーダー機能を利用することが可能です
- Jetson Nano に搭載されている H.264 ハードウェアエンコーダー機能を利用することで H.264 を 4K@30 での配信が可能です。

## バイナリ提供について

Momo Custom はバイナリでの提供は実施していません
映像音声通信のみであればカスタム元の [Momo](https://github.com/shiguredo/momo) をご利用になれます

## 動作環境

- Raspbian Buster ARMv7
    - Raspberry Pi 4 B で動作
    - Raspberry Pi 3 B/B+ で動作
- Raspbian Buster ARMv6
    - Raspberry Pi Zero W/WH で動作
- Ubuntu 18.04 x86_64
- Ubuntu 18.04 ARMv8 Jetson Nano
    - [NVIDIA Jetson Nano](https://www.nvidia.com/ja-jp/autonomous-machines/embedded-systems/jetson-nano/)

Momo を使ってみたい人は [USE.md](doc/USE.md) をお読みください。

## ビルドする

ビルドシステムは提供していません
映像音声通信のみであればカスタム元の [Momo](https://github.com/shiguredo/momo) でお試しください

## ライセンス

Apache License 2.0

```
Copyright 2018-2019, Shiguredo Inc, tnoho and melpon and kdxu

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

## Momo についての電子書籍

Momo のコントリビュータである @tnoho が書いた Momo のノウハウが沢山詰まった本が販売されています。

[WebRTCをブラウザ外で使ってブラウザでできることを増やしてみませんか?\(電子版\) \- でんでんらぼ \- BOOTH](https://tnoho.booth.pm/items/1572872)


## サポートについて

バグ報告は GitHub Issues へお願いします。

### バグ報告

https://github.com/tnoho/custom-momo/issues

