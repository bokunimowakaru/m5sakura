# ボクにもわかる さくらのモノプラットフォーム用 サンプル・プログラム集
Application Examples of Sakura's Mono Platform for M5Stack Core2 
forked from sakura-internet/sipf-std-client_sample_m5stack Core2 

## サンプル・プログラム集 for Core2
さくらのモノプラットフォームを利用するためのM5Stack用のサンプル・プログラム集です。  
LTEモジュール(GNSSつき) cat.M1 nRF9160 (SCO-M5SNRF9160) をM5Stackに接続し、モバイル通信網にセンサ情報を送信したり、M5Stackに接続したLEDを制御してみます。  

## I2C用 Grove Port A 端子との干渉について

SCO-M5SNRF9160のリセットにGPIO33を使用します。
このため、Grove Port A 端子(GPIO32, GPIO33)は使用できません。  
環境センサ ENVⅢ ユニットは、GPIO25, GPIO26に接続してください。  
RGB LED ユニットは、GPIO25に接続してください。  

	■ RGB LED ユニット
	|Grove  |Port   (ケーブル色)              |
	+-------+---------------------------------+
	|GPIO26 |N.C.   (M5Stack=白、Grove=黄)    |
	|GPIO25 |IN/OUT (M5Stack=黄、Grove=白)    |
	|5V     |5V     (赤)                      |
	|GND    |GND    (黒)                      |
	
	■ ENVⅢ ユニット
	|Grove  |Port   (ケーブル色)              |
	+-------+---------------------------------+
	|GPIO26 |GPIO26 SCL (M5Stack=白、Grove=黄)|
	|GPIO25 |GPIO25 SDA (M5Stack=黄、Grove=白)|
	|5V     |5V         (赤)                  |
	|GND    |GND        (黒)                  |

## ご注意 [重要な注意点]
本フォルダは M5Stack Core2 用です。
SCO-M5SNRF9160 は、M5Stack Basic 以外には対応していません。  
筆者（国野亘）も、各信号ピンの結線状態やIO状態、電圧などを確認していません。端子配列の違いで SCO-M5SNRF9160 に負担をかけているかもしれません。  
ハードウェアの違いなどによって不具合が生じるリスクが M5Stack Core よりも高いかもしれないので、その点を良くご理解の上、ご利用ください。  
例えば、ハードウェアの発火や異常発熱、故障、消費電流の増大、リチウムイオンバッテリの劣化、過放電などが生じるかもしれません。  

## Core2 対応方法

### フォルダ名.ino ファイルの変更箇所

	#include <M5Core2.h>
	
	M5.begin();
	
	shtSetup(25,26);

### sipf-std-m5stack.ino ファイルの変更箇所

	digitalWrite(33, LOW);
	pinMode(33, OUTPUT);

	// Reset要求
	digitalWrite(33, HIGH);
	delay(10);
	digitalWrite(33, LOW);

	// UART初期化
	Serial2.begin(115200, SERIAL_8N1, 13, 14);

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
