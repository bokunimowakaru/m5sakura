/******************************************************************************
Example 02: 温度センサ送信機 for さくらモノプラットフォーム
・M5StackのCPU温度を送信します。

                                          Copyright (c) 2022 Wataru KUNINO
*******************************************************************************
【参考文献】さくらインターネット(SAKURA internet Inc.) GitHubページ
https://github.com/sakura-internet/sipf-std-client_sample_m5stack
/blob/main/sipf-std-m5stack/sipf-std-m5stack.ino
*/
/*
 * Copyright (c) 2021 Sakura Internet Inc.
 *
 * SPDX-License-Identifier: MIT
 */
/*****************************************************************************/

#include <M5Stack.h>                            // M5Stack用ライブラリ組み込み
#include "sipf_client.h"                        // さくらモノプラット用
#include "on_jpg.h"                             // 点灯した電球のJPEGデータ
#include "off_jpg.h"                            // 消灯した電球のJPEGデータ
#define PIN_LED_RGB 21                          // RGB LED 接続先IOポート番号
#define INTERVAL_ms 30000                       // 受信間隔

static uint8_t buff[256];                       // 受信データ表示用のバッファ
unsigned long time_prev = millis()-INTERVAL_ms; // CPU時刻(ms単位)の30秒前を保持
/*  起動直後に受信を実行するために30秒を減算する。起動直後なので計算結果は
    マイナスになるが、time_prevは符号なし変数なので巨大なプラス値になる。
    判定時は現在時刻から巨大time_prev値を減算する。一周して適正値になる。*/
int timeout_n = 0;                              // 通信タイムアウト回数
byte led_stat = 0;                              // LED点灯輝度(0～255)

void ledControl(byte val){                      // LED制御関数(val=輝度)
    ws2812_led(val);                            // RGB LED Unitを輝度valで点灯
    if(val){                                    // val>0のとき
        M5.Lcd.drawJpg(on_jpg,on_jpg_len,220,60);      // LCDにJPEG画像onを表示
    }else{                                      // val==0のとき
        M5.Lcd.drawJpg(off_jpg,off_jpg_len,220,60);    // LCDにJPEG画像offを表示
    }
}

void reset(){                                   // LTEモジュールのリセット
    M5.Lcd.print("Booting... ");                // 起動中の表示
    while(resetSipfModule());                   // LTEモジュールのリセット
    uint32_t fw_version;                        // バージョン保持用の変数を定義
    M5.Lcd.print("\nFwVersion... ");            // バージョン取得表示
    while(SipfGetFwVersion(&fw_version));       // FWバージョンを取得
    M5.Lcd.printf("%08X\n",fw_version);         // バージョン表示
    if(fw_version < 0x000400 && SipfSetAuthMode(0x01)){ // AuthModeモード設定
        M5.Lcd.println("Auth mode... NG");      // 設定失敗時の表示
    }
}

void setup(){                                   // 起動時に一度だけ実行する関数
    M5.Lcd.begin();                             // M5Stack用Lcdライブラリの起動
    M5.Lcd.setBrightness(31);                   // 輝度を下げる（省エネ化）
    M5.Lcd.fillScreen(BLACK);                   // LCDを消去
    ws2812_led_setup(PIN_LED_RGB);              // RGB LED 初期設定(ポート設定)
    sipf_drawTitle("Example 03 LED");           // LCDにタイトルを表示
    reset();                                    // 関数リセットを呼び出し
    sipf_drawButton(0, "OFF");                  // ボタンA(左)の描画
    sipf_drawButton(1, "RX");                   // ボタンB(中央)の描画
    sipf_drawButton(2, "ON");                   // ボタンC(右)の描画
    sipf_drawResultWindow();                    // RESULT画面の描画
    ledControl(led_stat);                          // LED消灯
    M5.Lcd.println("+++ Ready +++");            // 準備完了表示
}

void loop() {
    boolean rx = false;                         // 受信フラグ(false:OFF)
    M5.update();                                // M5Stack用IO状態の更新
    delay(1);                                   // 誤作動防止
    if(M5.BtnA.wasPressed()){                   // ボタンA(左)が押されたとき
        led_stat = 0;                           // LED輝度を0(OFF)に設定
        ledControl(led_stat);                   // LED消灯
    }
    if(M5.BtnB.wasPressed()){                   // ボタンB(中央)が押されたとき
        rx = true;                              // 受信設定
    }
    if(M5.BtnC.wasPressed()){                   // ボタンC(右)が押されたとき
        led_stat = 30;                          // LED輝度を30(0～255)に設定
        ledControl(led_stat);                   // LED点灯
    }
    if(millis() - time_prev > INTERVAL_ms){     // 30秒以上が経過した時
        rx = true;                              // 受信設定
    }
    if(rx){                                     // 受信フラグがTrueの時
        time_prev = millis();                   // 現在のマイコン時刻を保持
        sipf_drawResultWindow();                // RESULT画面の描画
        ledControl(led_stat);                   // LED制御
        M5.Lcd.printf("Requested RX data\n");   // リクエスト表示
        memset(buff, 0, sizeof(buff));          // 変数buffの内容を消去
        static SipfObjObject objs;              // 受信結果代入用の構造体
        uint64_t stm, rtm;                      // 送信時刻,受信時刻
        uint8_t remain, qty;                    // 残データ数,受信obj数
        int ret = SipfCmdRx(buff, &stm, &rtm, &remain, &qty, &objs, 1);
        if(ret > 0) {                           // 受信に成功した時
            M5.Lcd.printf("OK\nOTID:\n%s\n", buff);
            M5.Lcd.printf("Recieved: ");        // 受信結果表示
            for(int i=0; i<4; i++) M5.Lcd.printf("%02X ",*(objs.value+i));
            led_stat = *objs.value;             // 受信1バイト目をLED輝度に設定
            M5.Lcd.printf("\nLED: %d",led_stat); // LEd制御値を表示
            ledControl(led_stat);               // LED制御
        }else if(ret == 0){
            M5.Lcd.printf("RX buffer is empty.\nOK\n");
        }else{
            if(ret == -3){                      // タイムアウト発生時
                timeout_n++;                    // タイムアウト数をカウント
            }else{                              // 発生しなかったとき
                timeout_n = 0;                  // タイムアウト数を0に戻す
            }
            M5.Lcd.printf("NG: %d(%d)\n", ret, timeout_n); // 応答値を表示
            if(timeout_n >= 3){                 // 3回連続でタイムアウト
                reset();                        // 関数リセットを呼び出し
                timeout_n = 0;                  // タイムアウト数を0に戻す
            }
        }
    }
}
