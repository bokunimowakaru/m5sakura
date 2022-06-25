# ボクにもわかる さくらのモノプラットフォーム用 サンプル・プログラム集
Application Examples of Sakura's Mono Platform for M5Stack  
forked from sakura-internet/sipf-std-client_sample_m5stack  

## サンプル・プログラム集
さくらのモノプラットフォームを利用するためのM5Stack用のサンプル・プログラム集です。  
LTEモジュール(GNSSつき) cat.M1 nRF9160 (SCO-M5SNRF9160) をM5Stackに接続し、モバイル通信網にセンサ情報を送信したり、M5Stackに接続したLEDを制御してみます。  

![GPS(GNSS)位置情報送信](/images/ex05_gnss.gif)


### Example 1 ボタン送信機

![ボタン送信](/images/ex01_lcd01.gif)
![ボタン受信](/images/ex01_wstool.gif)

### Example 2 マイコン温度送信機

![温度送信](/images/ex02_lcd01.gif)
![温度受信](/images/ex02_wstool.gif)

### Example 3 LED制御

![LED制御送信](/images/ex03_wstool.gif)  

Valueの欄に輝度0～255を入力して M5Stackに送信すると、M5Stackに接続した RGB LED Unit のLEDの輝度が変化します。  
RGB LED Unit が無くても、M5Stack 上に表示したLCDの電球の画像がON/OFFします。

![LED制御受信](/images/ex03_lcd01.gif)
![LED制御受信](/images/ex03_lcd02.gif)

### Example 4 温度・湿度センサ送信機

M5Stack に ENVⅡ Unit または ENV Ⅲ Unit を接続してください。温度と湿度を送信します。  

![温湿度送信](/images/ex04_lcd02.gif)
![温湿度受信](/images/ex04_wstool.gif)  

さくらのモノプラットフォームからAmbientやLINEアプリにデータを転送してみます。  

![Ambientで受信](/images/ex04_ambient.gif)
![LINEで受信](/images/ex04_line.gif)  

### Example 5 GPS(GNSS)位置情報送信機

GPS(GNSS)用のアンテナ(LNA内蔵・アクティブタイプ)を接続してください。位置情報を送信します。  

![GPS(GNSS)送信](/images/ex05_lcd01.gif)
![日本地図](/images/ex05_lcd02.gif)

さくらのモノプラットフォームからAmbientに位置情報を転送し、詳細な地図を表示してみます。  

![位置情報受信](/images/ex05_wstool.gif)
![詳細地図](/images/ex05_ambient.gif)

### WebSocket 確認ツール Message IoT さくらのモノプラットフォーム用
  [https://git.bokunimo.com/IchigoJam/message_iot/sipf.html](https://git.bokunimo.com/IchigoJam/message_iot/sipf.html)

### ファームウェア・アップデータ [sipf-FOTA-update.ino](/sipf-FOTA-update/sipf-FOTA-update.ino)  

M5Stackに書き込み、M5Stackの表面の右ボタンを押すと、さくらのモノプラットフォーム上のファームウェアをアップデートすることが来ます。  
進捗状況も表示できるようにしました。  
予めファームウェアをさくらのモノプラットフォームに保存しておく必要があります。使用方法は、ソースコードを参照してください。  

**ご注意**：アップデートに失敗したとしても無保証です。  
**異なるデバイス用のファームウェアを誤って使用しないように注意してください。**  

-------------------------------------------------------------------------------------------------------

## 権利情報

本サンプル・プログラムは下記からフォークしたものをベースに作成しました。  
  [https://github.com/sakura-internet/sipf-std-client_sample_m5stack](https://github.com/sakura-internet/sipf-std-client_sample_m5stack)

元のソースのライセンスについては、フォーク元（さくらインターネット）に帰属し、改変部や追加部の権利は当方に帰属します。改変部はMITライセンスで再配布いたします。画像データについては各サンプル・プログラムのフォルダ内のREADMEが付与されているファイルを参照ください  

### 元のソースのREADME

[元ソース(さくらインターネット社)のREADME](/README_sakura.md)

### 改変部と追加部の権利

ライセンスについては各ソースリストならびに各フォルダ内のファイルに記載の通りです。  
使用・変更・配布は可能ですが、権利表示を残してください。  
また、提供情報や配布ソフトによって生じたいかなる被害についても，一切，補償いたしません。  

[ライセンス](/LICENSE)

----------------------------------------------------------------
# git.bokunimo.com GitHub Pages site
[http://git.bokunimo.com/](http://git.bokunimo.com/)  
----------------------------------------------------------------

by 国野 亘 Wataru KUNINO <https://bokunimo.net/>
