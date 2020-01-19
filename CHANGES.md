# 変更履歴

- UPDATE
    - 下位互換がある変更
- ADD
    - 下位互換がある追加
- CHANGE
    - 下位互換のない変更
- FIX
    - バグ修正


## Momo Custom の変更履歴ではありません

世代差が確認できるよう、オリジナルの [Momo](https://github.com/shiguredo/momo) の CHANGES を残しています

## develop

- [UPDATE] libwebrtc を M80.3987@{#2} に上げる
    - libwebrtc のハッシュは fba51dc69b97f6f170d9c325a38e05ddd69c8b28
    - @melpon
- [UPDATE] Momo 2020.1 にバージョンを上げる
    - バージョン番号を <リリース年>.<その年のリリース回数> に変更
    - @voluntas
- [UPDATE] Boost 1.72.0 にアップデートする
    - @voluntas
- [UPDATE] --video-device を Linux 全般で有効にする
    - V4L2 capturer を使うようにした
    - @shino
- [UPDATE] Jetson Nano 用のライブラリを NVIDIA L4T 32.3.1 に上げる
    - [L4T \| NVIDIA Developer](https://developer.nvidia.com/embedded/linux-tegra)
    - @melpon
- [UPDATE] 音声系オプションの --disable-residual-echo-detector を追加する
    - @melpon
- [ADD] データチャネルを利用したシリアルポートへの読み書き機能を追加する
    - --serial を指定することでデータチャネル経由でのシリアル読み書きが可能になる
    - test と ayame モードでのみ利用可能
    - @tnoho
- [ADD] 自由に解像度の値を指定できるようにする
    - `--resolution 640x480` のように指定できるようになりました
    - この機能が有効になるのは、カメラに依存するため動作保証はありません
    - @melpon
- [ADD] Sora モード利用時のシグナリング接続情報に enviroment / libwebrtc / sora_client を追加する
    - Jetson Nano の場合
        - `"environment": "[aarch64] Ubuntu 18.04.3 LTS (nvidia-l4t-core 32.2.1-20190812212815)"`
        - `"libwebrtc": "Shiguredo-Build M80.3987@{#2} (80.3987.2.1 15b26e4)"`
        - `"sora_client": "WebRTC Native Client Momo 2020.1 (f6b69e77)"`
    - macOS の場合
        - `"environment": "[x86_64] macOS Version 10.15.2 (Build 19C57)"`
        - `"libwebrtc": "Shiguredo-Build M80.3987@{#2} (80.3987.2.1 15b26e4)"`
        - `"sora_client": "WebRTC Native Client Momo 2020.1 (f6b69e77)"`
    - Ubuntu 18.04 x86_64 の場合
        - `"environment": "[x86_64] Ubuntu 18.04.3 LTS"`
        - `"libwebrtc": "Shiguredo-Build M80.3987@{#2} (80.3987.2.1 15b26e4)"`
        - `"sora_client": "WebRTC Native Client Momo 2020.1 (f6b69e77)"`
    - @melpon
- [ADD] Ayame モード利用時のシグナリング接続情報に enviroment / libwebrtc / ayameClient を追加する
    - Sora 時の sora_client が ayameClient に変わります
    - @melpon
- [CHANGE] momo --help の英語化
    - @shino @msnoigrs
- [CHANGE] <package>.edit の機能とドキュメントを削除
    - @melpon
- [CHANGE] armv6 で SDL を使えなくする
    - @melpon
- [FIX] --no-video を指定しているにもかかわらずカメラを一瞬だけ掴むのを修正する
    - @melpon @mganeko
- [FIX] SDL が有効でない時に SDL 関連のオプションを指定するとエラーにする
    - @melpon
- [FIX] macOS のビルドで Python 2.7 必須を外す
    - @melpon
- [FIX] Ayame モードで WebSocket が閉じられた際に再接続処理に進まない箇所を修正
    - @Hexa
- [FIX] Ayame モードで シグナリングで bye を受信した際処理として、各 close 処理を追加する
    - @Hexa
- [FIX] Ayame モードで 再接続処理の 1 回目を、5 秒後からすぐに実行されるように変更する
    - @Hexa

## 19.12.1

- [UPDATE] libwebrtc を時前ビルドしないようにする
    - https://github.com/shiguredo-webrtc-build/webrtc-build を利用する
    - @melpon
- [FIX] momo + ayame モードで再接続時に delay してしまう問題を解決
    - @kdxu

## 19.12.0

- [UPDATE] libwebrtc M79 コミットポジションを 5 にする
    - libwebrtc のハッシュは b484ec0082948ae086c2ba4142b4d2bf8bc4dd4b
    - @voluntas
- [UPDATE] json を 3.7.3 に上げる
    - @voluntas
- [ADD] sora モード利用時の --role に sendrecv | sendonly | recvonly を指定できるようにする
    - @melpon
- [FIX] QVGA の指定を 320x240 にする
    - @melpon @Bugfire
- [FIX] ayame モードで再接続時に segmentation fault が起こる場合があるのを修正する
   - ただし、互いに接続を確立するまでping-pongを送らない/ping timeoutで再接続するまで数秒かかることがある」ので、再接続によって受信側が数秒待つ必要が出てくる可能性がある
   - 上記の問題はこの修正では未解決
   - @kdxu
- [FIX] OpenH264 を明示的にビルドしないようにする
    - @melpon

## 19.11.1

- [ADD] Raspberry Pi 4 での動作を確認
    - @voluntas @Hexa
- [UPDATE] libwebrtc M79 コミットポジションを 3 にする
    - libwebrtc のハッシュは 2958d0d691526c60f755eaa364abcdbcda6adc39
    - @voluntas
- [UPDATE] libwebrtc M79 コミットポジションを 2 にする
    - libwebrtc のハッシュは 8e36cc906e5e1c16486e60e62acbf79c1c691879
    - @voluntas
- [UPDATE] Ayame で isExistUser フラグが accept 時に返却されなかった場合 2 回 peer connection を生成する
- [ADD] SDL を利用した音声と映像の受信可能にする `--use-sdl` を追加する
    - [Simple DirectMedia Layer](https://www.libsdl.org/)
    - @tnoho
- [ADD] SDL を Sora のマルチストリームに対応する `--multistream` を追加する
    - @tnoho
- [ADD] SDL を Sora のスポットライトに対応する `--spotlight` を追加する
    - @tnoho
- [ADD] SDL 利用時に Jetson Nano では H.264 ハードウェアデコーダを利用するようにする
    - @tnoho
- [ADD] SDL 利用時に自分のカメラ映像を表示する `--show-me` を追加する
    - @tnoho
- [ADD] SDL 利用時に映像を表示するウインドウの幅を `--window-width` と `--window-height` で指定可能にする
    - @tnoho
- [ADD] SDL 利用時に映像を表示するウインドウをフルスクリーンにする `--fullscreen` を追加する
    - f を押すと全画面、もう一度 f を押すと戻る
    - @tnoho
- [ADD] sora 利用時に `--role upstream` または `--role downstream` を指定できるようにする
    - @melpon
- [CHANGE] ayame の `accept` 時に返却される `isExistUser` フラグによって offer を送るかどうかを決めるよう変更する
    - @kdxu
- [FIX] C++14 にする
    - @melpon
- [FIX] USE_H264が定義されない場合でも--video-codecが使えるように修正する
    - @msnoigrs

## 19.11.0

- [UPDATE] json を 3.7.1 に上げる
    - @voluntas
- [UPDATE] GitHub Actions の macOS ビルドを macos-latest に変更する
    - @voluntas
- [UPDATE] libwebrtc M78 コミットポジションを 8 にする
    - libwebrtc のハッシュは 0b2302e5e0418b6716fbc0b3927874fd3a842caf
    - @voluntas
- [ADD] GitHub Actions のデイリービルドに ROS を追加する
    - @voluntas
- [ADD] GitHub Actions のビルドに Jetson Nano と macOS を追加する
    - @voluntas
- [ADD] Jetson Nano で 4K@30 出すためのドキュメントを追加
    - @tnoho @voluntas
- [ADD] macOS 用に --video-device オプションを追加
    - @hakobera
- [FIX] GitHub Actions のビルドがディスク容量不足でエラーになっていたのを修正する
    - @hakobera
- [FIX] ayame の client id を指定していない場合のランダム生成がうまくいっていなかったので修正する
    - @kdxu
- [FIX] ROS バージョンが正常にビルドできていなかったのを修正する
    - @melpon

## 19.09.2

- [UPDATE] libwebrtc M78 コミットポジションを 5 にする
    - libwebrtc のハッシュは dfa0b46737036e347acbd3b47f0f58ff6c8350c8
    - @voluntas
- [FIX] iceServers が json プロパティかつ array の場合のみ ice_servers_ にセットするよう修正する
    - @kdxu

## 19.09.1

- [ADD] Jetson Nano のハードウェアエンコーダを利用する機能を実装
    - @tnoho
- [ADD] Jetson Nano のビルドを追加
    - @melpon
- [ADD] CI を CircleCI から GitHub Actions へ切り替える
    - macOS の時間制限が OSS の場合はないため Weekly build から Daily build のみにきりかえる
    - @hakobera
- [ADD] .clang-format の追加
    - @melpon
- [UPDATE] libwebrtc M78 コミットポジションを 3 にする
    - libwebrtc のハッシュは 68c715dc01cd8cd0ad2726453e7376b5f353fcd1
    - @voluntas
- [UPDATE] コマンドオプションをできるだけ共通化する
    - @melpon
- [UPDATE] Raspberry Pi のビルド OS を Ubuntu 16.04 から 18.04 に上げる
    - @melpon

## 19.09.0

- [ADD] --disable-echo-cancellation オプションを追加
    - @melpon
- [ADD] --disable-auto-gain-control オプションを追加
    - @melpon
- [ADD] --disable-noise-suppression オプションを追加
    - @melpon
- [ADD] --disable-highpass-filter オプションを追加
    - @melpon
- [ADD] --disable-typing-detection オプションを追加
    - @melpon
- [UPDATE] Boost 1.71.0 にアップデートする
    - @voluntas
- [UPDATE] libwebrtc M78 コミットポジションを 0 にする
    - libwebrtc のハッシュは 5b728cca77c46ed47ae589acba676485a957070b
    - @tnoho
- [UPDATE] libwebrtc M77 コミットポジションを 10 にする
    - libwebrtc のハッシュは ad73985e75684cb4ac4dadb9d3d86ad0d66612a0
    - @voluntas
- [FIX] Track を複数の PeerConnection で共有するよう修正
    - @tnoho
- [FIX] --no-audio 設定時にも capturer をチェックしていたので修正
    - @tnoho
- [FIX] PeerConnectionObserver の解放がなかったため修正
    - @tnoho

## 19.08.1

- [ADD] Raspberry Pi 用に `--video-device` オプションを追加
    - @melpon
- [UPDATE] sora の metadata オプションを公開する
    - @melpon

## 19.08.0

- [UPDATE] nlohmann/json を v3.7.0 にアップデートする
    - @melpon
- [UPDATE] Raspbian Buster に対応
    - @voluntas
- [UPDATE] libwebrtc M77 コミットポジションを 6 にする
    - libwebrtc のハッシュは 71e2db7296a26c6d9b18269668d74b764a320680
    - @voluntas
- [UPDATE] libwebrtc M77 コミットポジションを 3 にする
    - libwebrtc のハッシュは 3d8e627cb5893714a66082544d562cbf4a561515
    - @kdxu @voluntas
- [UPDATE] libwebrtc M76 コミットポジションを 3 にする
    - libwebrtc のハッシュは 9863f3d246e2da7a2e1f42bbc5757f6af5ec5682
    - @voluntas
- [UPDATE] I420 の時にもハードウェアでリサイズする
    - @tnoho
- [ADD] Raspberry Pi 向けに --use-native オプションを追加しました
    - USB カメラ用で MJPEG をハードウェアデコードします
    - @tnoho
- [ADD] Raspberry Pi 向けに --force-i420 オプションを追加しました
    - Raspberry Pi 専用カメラ用で MJPEG を使えないため HD 以上の解像度でも MJPEG にせず強制的に I420 でキャプチャーする
    - @tnoho
- [ADD] Ayame のサブコマンドに --signaling-key を追加する
    - @kdxu @tnoho
- [ADD] Ayame 利用時に iceServers の払い出しに対応する
    - 独自の STUN/TURN が利用可能になる
    - @kdxu @tnoho
- [CHANGE] Ayame のサブコマンドで client id を optional に指定できるように修正する
    - @kdxu
- [CHANGE] ./momo p2p を ./momo test に変更する
    - @melpon
- [FIX] Ayame の candidate 交換の際の JSON スキーマが間違っていたのを修正する
    - @kdxu
- [FIX] Ayame の sdp 交換の際の type が answer 固定になっていたのを修正する
    - @kdxu
- [FIX] Ayame で peer connection 生成後に createOffer して send する実装が漏れていたので追加する
    - @kdxu
- [FIX] Ayame で momo を起動したあとに映像を受信できない場合が発生するのバグを修正する
    - @kdxu
- [FIX] Raspberry Pi でハードウェアエンコーダを利用した際に再接続できなくなることがある問題の修正
    - @tnoho
- [FIX] libwebrtc M77 で作成した armv6 バイナリがクラッシュしてしまう問題の対策
    - @tnoho
- [FIX] macOS 版 Momo で VideoToolbox 利用時の解像度変更時に落ちる問題の修正
    - @hakobera
- [FIX] macOS 版がビルドは成功するが動作させようとするとセグメンテーションフォルトする問題の修正
    - @hakobera
- [FIX] Raspberry Pi でハードウェアエンコーダを利用した際にGPUのメモリを食いつぶしてしまう問題の修正
    - @tnoho

## 19.07.0

- [UPDATE] Raspberry Pi の H.264 を MMAL を利用したハードウェアエンコードに変更する
    - 720p 30fps や 1080p 20fps を可能にする
    - @tnoho
- [UPDATE] libwebrtc を M75 に上げる
    - libwebrtc のハッシュは 159c16f3ceea1d02d08d51fc83d843019d773ec6
    - @tnoho
- [UPDATE] libwebrtc を M76 に上げる
    - libwebrtc のハッシュは d91cdbd2dd2969889a1affce28c89b8c0f8bcdb7
    - @kdxu
- [UPDATE] Unified Plan に対応する
    - @tnoho
- [UPDATE] no-audio 時に AudioDevice を無効化するよう変更
    - @tnoho
- [UPDATE] CLI11 を v1.8.0 にアップデートする
    - @melpon
- [UPDATE] JSON v3.6.1 にアップデートする
    - @melpon
- [UPDATE] macOS のビルドドキュメントを独立させる
    - @voluntas
- [UPDATE] doc/CACHE.md を削除
    - make PACKAGE.clean にてビルドキャッシュの削除が可能になったため
    - @melpon
- [UPDATE] audio/video の共通オプションを sora のオプションに移動する
    - Momo 側ではコーデックやビットレートは指定できない
    - p2p の場合は HTML で sdp を切り替えている
    - --audio-codec
    - --audio-bitrate
    - --video-codec
    - --video-bitrate
    - @melpon
- [UPDATE] WebRTC Signaling Server Ayame 19.07.0 に追従する
    - @kdxu
- [ADD] WebRTC Signaling Server Ayame に対応しました
    - https://github.com/OpenAyame/ayame
    - @kdxu
- [ADD] Circle CI で Linux 版を毎日 22:00 に自動ビルドする
    - @voluntas
- [ADD] Circle CI で macOS 版を毎週日曜日 22:00 に自動ビルドする
    - @voluntas
- [FIX] macOS でデバイスがつかめなくなっていたのを修正する
    - ただし --fixed-resolution 必須
    - @tnoho
- [FIX] ROS 対応がビルドできなくなっていたのを修正する
    - @tnoho

## 19.02.0

- [UPDATE] webrtc.js / p2p.html のリファクタリング
- [UPDATE] Momo の前段にリバースプロキシ等を設置して https でアクセス可能にした場合でも、wss で接続できるように webrtc.js を変更する
- [CHANGE] ビルド時のターゲットとオプション、パッケージの作成先を変更する
- [UPDATE] STUN サーバの URL の指定を url から urls に変更する
- [FIX] カメラがない環境で起動させるとセグフォが起きるのを修正する
- [FIX] ARM ROS 版で H.264 配信の場合はハードウェアエンコーダを使用するように修正する
- [CHANGE] ROS Audio に対応する
    - 別ノードから送られてきたオーディオを使用するように変更
- [UPDATE] 利用している libwebrtc のライブラリを M73 にする

## 19.01.0

- [ADD] Ubuntu 16.04 ARMv7 ROS 対応

## 18.12.0

- [ADD] ROS 対応
- [CHANGE] ビルド時のターゲットと、パッケージのファイル名を変更する
- [CHANGE] p2p モードの切断後に自動的に window が閉じられないようにする
- [UPDATE] Boost 1.69.0 にアップデートする
- [UPDATE] CLI11 v1.6.2 にアップデートする
- [UPDATE] JSON v3.4.0 にアップデートする
- [CHANGE] x86_64, armv8 の場合は H264 を指定できないようにする

## 18.10.2

- [ADD] 解像度を固定するオプションを追加する
- [FIX] WebSocket の分かれているべきフレームがくっついていたのを修正した

## 18.10.1

- [UPDATE] 利用している libwebrtc のライブラリを M71 にする
- [FIX] --metadata を Sora のみのオプションにする
- [FIX] P2P のオプションに --document-root を追加する
- [FIX] P2P モードで Web サーバが立ち上がった場合カレントディレクトリを晒さないようにする
- [FIX] --auido-bitrate を指定した場合に、--auido-bitrate に指定したビットレートがビデオのビットレートとして扱われる問題を修正

## 18.10.1-rc0

- [UPDATE] websocketpp と civietweb を Boost.beast に置き換える

## 18.10.0

**18.10.0-rc4 から変更なし**

## 18.10.0-rc4

- [ADD] 4K の配信にに対応する
    - armv6, armv7 にも対応はしているが、現時点で Raspberry Pi で配信はマシンパワー不足のためできない

## 18.10.0-rc3

- [FIX] バージョン情報を MOMO_VERSION に指定したら momo のバイナリの --version も反映するようにする
- [CHANGE] --metadata の引数は JSON のみを指定できるようにする

## 18.10.0-rc2

- [CHANGE] libwebrtc が 4K に対応していないため解像度指定から 4K を削除する
    - 将来的に対応していく予定
    - https://github.com/shiguredo/momo/issues/21
- [FIX] P2P モードのサンプルで映像を有効にした場合、音声が正常に流れない問題を修正

## 18.10.0-rc1

- [UPDATE] ビルド時に `LDFLAGS += -s` 渡してバイナリサイズを圧縮する仕組みを追加
- [ADD] WebRTC が依存しているライブラリのライセンスを追加
- [ADD] Websocketpp が依存しているライブラリのライセンスを追加
- [FIX] P2P モードで画面を開いているときに終了するとセグフォが起きるのを修正

## 18.10.0-rc0

**初 リリース**

- Momo を Apache License 2.0 でオープンソース化
- libwebrtc M70 対応
- Ubuntu 18.04 x86_64 対応
    - Ubuntu 18.04 x86_64 向けのバイナリの提供
- Ubuntu 16.04 ARMv8 対応
    - Ubuntu 16.04 ARMv8 向けのバイナリの提供
    - PINE64 の Rock64 対応
- Raspberry Pi 3 B/B+ 対応
    - Raspberry Pi 3 B/B+ 向け ARMv7 バイナリの提供
    - Raspberry Pi 3 B/B+ 向け H.264 HWA 対応
- Raspberry Pi Zero W/WH 対応
    - Raspberry Pi Zero W/WH 向け ARMv6 バイナリの提供
    - Raspberry Pi Zero W/WH 向け H.264 HWA 対応
- 解像度指定オプション
    - QVGA、VGA、HD、FHD、4K
- フレームレート指定オプション
    - 1-60
- 優先オプション
    - この機能は実験的機能です
    - フレームレートか解像度のどちらを優先するか指定可能
- ビデオを利用しないオプション
- オーディオを利用しないオプション
- ビデオコーデック指定オプション
- オーディオコーデック指定オプション
- デーモン化オプション
    - Systemd の利用をおすすめします
- ログレベル
- P2P 機能
- WebRTC SFU Sora 18.04.12 対応
