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
#define INTERVAL_ms 30000                       // 送信間隔
static float TEMP_ADJ = -25.0;                  // 温度値の補正用
static uint8_t buff[256];                       // 受信データ表示用のバッファ
unsigned long time_prev = millis();             // マイコン時刻(ms単位)を保持

void setup(){                                   // 起動時に一度だけ実行する関数
    M5.Lcd.begin();                             // M5Stack用Lcdライブラリの起動
    M5.Lcd.setBrightness(31);                   // 輝度を下げる（省エネ化）
    M5.Lcd.fillScreen(BLACK);                   // LCDを消去
    sipf_drawTitle("Example 02 Temperature");   // LCDにタイトルを表示
    M5.Lcd.print("Booting... ");                // 起動中の表示
    while(resetSipfModule());                   // LTEモジュールのリセット
    uint32_t fw_version;                        // バージョン保持用の変数を定義
    M5.Lcd.print("\nFwVersion... ");            // バージョン取得表示
    while(SipfGetFwVersion(&fw_version));       // FWバージョンを取得
    M5.Lcd.printf("%08X\n",fw_version);         // バージョン表示
    if(fw_version < 0x000400 && SipfSetAuthMode(0x01)){ // AuthModeモード設定
        M5.Lcd.println("Auth mode... NG");      // 設定失敗時の表示
    }
    sipf_drawButton(0, "-0.5");                 // ボタンA(左)の描画
    sipf_drawButton(1, "SEND");                 // ボタンB(中央)の描画
    sipf_drawButton(2, "+0.5");                 // ボタンC(右)の描画
    sipf_drawResultWindow();                    // RESULT画面の描画
    M5.Lcd.println("+++ Ready +++");            // 準備完了表示
}

void loop() {
    boolean tx = false;                         // 送信フラグ(false:OFF)
    float temp = temperatureRead() + TEMP_ADJ;  // マイコンの温度値を取得
    if(millis()%3000 == 0){                     // 3秒に1回の処理
        M5.Lcd.printf("%.1f, ",temp);           // 温度を表示
    }
    M5.update();                                // M5Stack用IO状態の更新
    delay(1);                                   // 誤作動防止
    if(M5.BtnA.wasPressed()){                   // ボタンA(左)が押されたとき
        TEMP_ADJ -= 0.5;                        // 温度補正値から0.5を減算
        M5.Lcd.printf("%.1f(%.1f), ",temp,TEMP_ADJ); // 温度と補正値を表示
    }
    if(M5.BtnB.wasPressed()){                   // ボタンB(中央)が押されたとき
        tx = true;                              // 送信設定
    }
    if(M5.BtnC.wasPressed()){                   // ボタンC(右)が押されたとき
        TEMP_ADJ += 0.5;                        // 温度補正値に0.5を加算
        M5.Lcd.printf("%.1f(%.1f), ",temp,TEMP_ADJ); // 温度と補正値を表示
    }
    if(millis() - time_prev > INTERVAL_ms){     // 30秒以上が経過した時
        tx = true;                              // 送信設定
    }
    if(tx){
        time_prev = millis();                   // 現在のマイコン時刻を保持
        sipf_drawResultWindow();                // RESULT画面の描画
        byte tag_id = 0x01;                     // Tag ID を 0x01に設定
        M5.Lcd.printf("TX(tag_id=0x%02X temp=%f)\n", tag_id, temp);
        memset(buff, 0, sizeof(buff));          // 変数buffの内容を消去
        int ret = SipfCmdTx(tag_id, OBJ_TYPE_FLOAT32, (uint8_t*)&temp, 4, buff);
        if (ret == 0) {                         // 送信に成功した時
            M5.Lcd.printf("OK\nOTID: %s\n", buff); // 送信結果を表示
        }else{                                  // 送信に失敗した時
            M5.Lcd.printf("NG: %d\n", ret);     // 応答値を表示
        }
    }
}
