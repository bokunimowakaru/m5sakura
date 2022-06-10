# ボクにもわかる さくらのモノプラットフォーム用 サンプル・プログラム集
Application Examples of Sakura's Mono Platform for M5Stack  
forked from sakura-internet/sipf-std-client_sample_m5stack  

## サンプル・プログラム集
さくらのモノプラットフォームを利用するためのM5Stack用のサンプル・プログラム集です。  


### WebSocket 確認ツール Message IoT さくらのモノプラットフォーム用
  https://git.bokunimo.com/IchigoJam/message_iot/sipf.html

![ボタン受信](/images/ex01_wstool.gif) ![温度受信](/images/ex02_wstool.gif) ![LED制御](/images/ex03_wstool.gif)  

## 権利情報
本サンプル・プログラムは下記からフォークしたものをベースに作成しました。  
  https://github.com/sakura-internet/sipf-std-client_sample_m5stack

元のソースのライセンスについては、フォーム元（さくらインターネット）に帰属し、改変部の権利は当方に帰属します（MITライセンスで再配布）。  


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
