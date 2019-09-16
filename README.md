# WebRTC Robot Arm

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

## WebRTC Robot Arm について

WebRTC Robot Arm は WebRTC Native Client Momo を DataChannel に対応させロボットの遠隔操作を可能にしたものです。

WebRTC Robot Arm では使っていない WebRTC Native Client Momo のファイルが多くありますのでご留意ください。

## WebRTC Native Client Momo について

WebRTC Native Client Momo は libwebrtc を利用しブラウザなしで様々な環境で動作する WebRTC ネイティブクライアントです。

Raspberry Pi 環境では Raspberry Pi の GPU に積まれている H.264 ハードウェアエンコーダー機能を利用することが可能です。

## OpenMomo プロジェクトについて

OpenMomo は WebRTC Native Client Momo をオープンソースとして公開し、
継続的に開発を行うことで、ブラウザやスマートフォン以外からの WebRTC 利用を知ってもらうプロジェクトです。

詳細については下記をご確認ください。

[OpenMomo プロジェクト](https://gist.github.com/voluntas/51c67d0d8ce7af9f24655cee4d7dd253)

## 動作環境

- Raspbian Buster ARMv7
    - Raspberry Pi 3 B/B+ で動作
- Raspbian Buster ARMv6
    - Raspberry Pi Zero W/WH で動作

## 使ってみる

Momo を使ってみたい人は [USE.md](doc/USE.md) をお読みください。

## ビルドする

Linux 版 Momo のビルドに挑戦したい人は [BUILD_LINUX.md](doc/BUILD_LINUX.md) をお読みください。

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

## H.264 のライセンス費用について

- Raspberry Pi のハードウェアエンコーダのライセンス費用は Raspberry Pi の価格に含まれています
    - https://www.raspberrypi.org/forums/viewtopic.php?t=200855
