ESP32を使った、JJYシミュレータの実験（いまさら！？）


#何故いまさらJJYシミュレータなの？

神奈川県のF市からS町に引っ越ししたら・・・・・・・
東北の電波時計から遠くなったり、周りが結構低かったり、住宅街だったり
時計の場所が悪かったり、その他の理由（結局原因不明）で、電波時計の受信ができなくなった。
iPhoneのアプリやPCのエミュレータも試したけど、うまくいったりいかなかったりと不安定なので、
安定して、電波時計（JJY 40KHｚ）のような電波を出すものを、マイコンで作ってみることにした。

開発はArduino IDEでおこなった。マイコンとしてESP32-WROOM等、WiFi環境があること等が必須。


#プログラムのアルゴリズムは？

(1) ESP32をWiFiに接続し、NTPサーバーに接続して、日本時間を取得。

(2) 取得データ等をJJYのフォーマットに合うように60個の配列に入れる。

(3) G5ポートをLED-PWDの出力とする。PWMは、搬送波の40kHzで使用する。マーク、0（零）、１をJJYの規定時間で流す。

(4) PWMを60個目まで流す処理が終われば、再度NTPサーバーから、時間の情報を取得。

(5) (2)へ戻り処理を続ける　　以下ループ


プログラムの(1)とBCD変換、PWM出力については、Windows11のCopilot(プレビュー版)で生成、試験したものを加工した。


#未実装等

(1) うるう秒の処理。

(2) 15分、45分でのフォーマットがその他の分とは異なるが、同じものを使用している。

(3) バグはあると思います。気がついたら修正


#参考URL

標準電波の出し方について

https://jjy.nict.go.jp/jjy/trans/index.html


電波時計の製作

http://elm-chan.org/works/jjyclock/report.html


ESP32でにせJJY その(1) 電波時計を合わせろ

https://zenn.dev/taroh/articles/cad31d2a04db25




#メモ1：PCへのUSBシリアル・ドライバインストール

PC本体とESP32 WROOMを接続し、問題なく接続できればいいが、接続できない場合は、
ESP32 WROOMに実装されている「CP210x USB to UART Bridge VCP Drivers」をインストールし、PCとの接続ができるようにする。



#メモ2：Arduino IDEの設定

基本設定の追加のボードマネジャーのURLに以下を記入

https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json


ボード名は、ESP32 WROOM-DA Module　と設定する。

