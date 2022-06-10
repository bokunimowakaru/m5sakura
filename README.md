# ボクにもわかる さくらのモノプラットフォーム用 サンプル・プログラム集
Application Examples of Sakura's Mono Platform for M5Stack  
forked from sakura-internet/sipf-std-client_sample_m5stack  

## サンプル・プログラム集
さくらのモノプラットフォームを利用するためのM5Stack用のサンプル・プログラム集です。  


#### Example 1 ボタン送信機

![ボタン送信](/images/ex01_lcd01.gif)
![ボタン受信](/images/ex01_wstool.gif)

#### Example 2 マイコン温度送信機

![温度送信](/images/ex02_lcd01.gif)
![温度受信](/images/ex02_wstool.gif)

#### Example 3 LED制御

![LED制御送信](/images/ex03_wstool.gif)  

Valueの欄に輝度0～255を入力して M5Stackに送信すると、M5Stackに接続した RGB LED Unit のLEDの輝度が変化します。  
RGB LED Unit が無くても、M5Stack 上に表示したLCDの電球の画像がON/OFFします。

![LED制御受信](/images/ex03_lcd01.gif)
![LED制御受信](/images/ex03_lcd02.gif)

#### Example 4 温度・湿度センサ送信機

M5Stack に ENVⅡ Unit または ENV Ⅲ Unit を接続してください。温度と湿度を送信します。  

![温湿度送信](/images/ex04_lcd02.gif)
![温湿度受信](/images/ex04_wstool.gif)  

さくらのモノプラットフォームからAmbientやLINEアプリにデータを転送してみます。  

![Ambientで受信](/images/ex04_ambient.gif)
![LINEで受信](/images/ex04_line.gif)  

#### Example 5 GPS/GNSS位置情報送信機

GPS/GNSS用のアンテナ(LNA内蔵・アクティブタイプ)を接続してください。位置情報を送信します。  

![GNSS送信](/images/ex05_lcd01.gif)
![日本地図](/images/ex05_lcd02.gif)

さくらのモノプラットフォームからAmbientに位置情報を転送し、詳細な地図を表示してみます。  

![位置情報受信](/images/ex05_wstool.gif)
![詳細地図](/images/ex05_ambient.gif)

### WebSocket 確認ツール Message IoT さくらのモノプラットフォーム用
  https://git.bokunimo.com/IchigoJam/message_iot/sipf.html

## 権利情報
本サンプル・プログラムは下記からフォークしたものをベースに作成しました。  
  https://github.com/sakura-internet/sipf-std-client_sample_m5stack

元のソースのライセンスについては、フォーク元（さくらインターネット）に帰属し、改変部や追加部の権利は当方に帰属します。改変部はMITライセンスで再配布いたします。画像データについては各サンプル・プログラムのフォルダ内のREADMEが付与されているファイルを参照ください  


-------------------------------------------------------------------------------------------------------
## 以下は元のソースに含まれていた情報です。  
-------------------------------------------------------------------------------------------------------
# sipf-std-client sample for M5Stack

### はじめに

このスケッチはM5Stackから`さくらのモノプラットフォーム`を利用するためのサンプルプログラムです。  
M5Stack COREとさくらのモノプラットフォーム標準ファームウェアを書き込んだ `M5Stack対応LTEモジュール cat.M1 nRF9160（SCO-M5SNRF9160）` との組み合わせで動作します。

導入に関しては [ご利用の流れ（SCO-M5SNRF9160)](https://manual.sakura.ad.jp/cloud/iotpf-beta/getting-started/gs-scom5snrf9160-beta.html) をご参照ください。

---

### About

This sketch is sample program of `Sakura's MONOPLATFORM` for M5Stack.  
Target device is M5Stack Core & SCO-M5SNRF9160.

Please reference for installation [ご利用の流れ（SCO-M5SNRF9160)](https://manual.sakura.ad.jp/cloud/iotpf-beta/getting-started/gs-scom5snrf9160-beta.html). (in Japanese)
