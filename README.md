# WebRTC Native Client Momo Custom

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

## WebRTC Native Client Momo Custom について

WebRTC Native Client Momo Custom は libwebrtc を利用しブラウザなしで様々な環境で動作する WebRTC ネイティブクライアント [Momo](https://github.com/shiguredo/momo) に対して **汎用的ではない特殊な機能を追加したカスタム版です**

[Momo](https://github.com/shiguredo/momo) のカスタムにおける参考のために公開しています。積極的なメンテナンスは行いません。VERSIONファイルとCHANGESでオリジナルのMomoとの世代差を確認した上でご利用ください

WebRTC による映像音声通信のみであればカスタム元の [Momo](https://github.com/shiguredo/momo) をご利用ください。ドキュメントを含めた公開と積極的な最新版追従を行っています。

### カスタム版について

カスタムの差分がわかりやすいようにブランチで管理しています。現在は下記のブランチを提供しています

- [custom/base](https://github.com/tnoho/custom-momo) **(現在のブランチ)**
  - カスタムのベースとしたオリジナルの [Momo](https://github.com/shiguredo/momo) の develop スナップショットからメンテを行えないドキュメントとビルドシステムを削除したものです
- [custom/robot-arm](https://github.com/tnoho/custom-momo/tree/custom/robot-arm)
  - [WebRTCをブラウザ外で使ってブラウザでできることを増やしてみませんか?](https://tnoho.booth.pm/items/1572872) で記載したロボットアームの制御コードのブランチです
  - WebRTC の DataChannel を経由した USBシリアル と pigpio 経由のサーボ制御に対応しています
  - [custom/base-old](https://github.com/tnoho/custom-momo/tree/custom/base-old) と[このように比較することで](https://github.com/tnoho/custom-momo/compare/custom/base-old...custom/robot-arm)改変箇所がわかるようになっています
  - **現在ではオリジナルの [Momo](https://github.com/shiguredo/momo) で DataChannel を経由した USBシリアル に対応しました。安定性とメンテナンスの観点から、そちらのご利用をお勧めします。**
- [custom/base-old](https://github.com/tnoho/custom-momo/tree/custom/base-old)
  - [custom/robot-arm](https://github.com/tnoho/custom-momo/tree/custom/robot-arm) の改造のベースとした時のオリジナルの [Momo](https://github.com/shiguredo/momo) の develop スナップショットです。比較用に残しています

### ハードウェアエンコーダへの対応

- Raspberry Pi の GPU に積まれている H.264 ハードウェアエンコーダー機能を利用することが可能です
- macOS に積まれている [VideoToolbox](https://developer.apple.com/documentation/videotoolbox) の H.264 ハードウェアエンコーダー機能を利用することが可能です
- Jetson Nano に搭載されている H.264 ハードウェアエンコーダー機能を利用することで H.264 を 4K@30 での配信が可能です。

### データチャネル経由でのシリアルの読み書き

Momo はデータチャネルを利用しシリアルに直接読み書きが可能です。信頼性より低遅延を優先したい場合の利用を想定しています。

## バイナリ提供について

Momo Custom はバイナリでの提供は実施していません
映像音声通信のみであればカスタム元の [Momo](https://github.com/shiguredo/momo) をご利用になれます

## 動作環境

- Raspbian Buster ARMv7
    - Raspberry Pi 4 B で動作
    - Raspberry Pi 3 B/B+ で動作
- Raspbian Buster ARMv6
    - Raspberry Pi Zero W/WH で動作

## ビルドする

ビルドシステムは提供していません
映像音声通信のみであればカスタム元の [Momo](https://github.com/shiguredo/momo) でお試しください

## ライセンス

Apache License 2.0

```
Copyright 2018-2020, Shiguredo Inc, tnoho and melpon and kdxu

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

