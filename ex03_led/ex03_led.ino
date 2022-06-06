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
#define INTERVAL_ms 30000                       // 受信間隔
static uint8_t buff[256];                       // 受信データ表示用のバッファ
unsigned long time_prev = millis();             // マイコン時刻(ms単位)を保持
boolean led_stat = false;

void ledControl(boolean on){                    // LED制御関数
    if(on){                                     // on=trueのとき
        M5.Lcd.drawJpg(on_jpg,on_jpg_len,220,60);      // LCDにJPEG画像onを表示
    }else{                                      // on=falseのとき
        M5.Lcd.drawJpg(off_jpg,off_jpg_len,220,60);    // LCDにJPEG画像offを表示
    }
}

void setup(){                                   // 起動時に一度だけ実行する関数
    M5.Lcd.begin();                             // M5Stack用Lcdライブラリの起動
    M5.Lcd.setBrightness(31);                   // 輝度を下げる（省エネ化）
    M5.Lcd.fillScreen(BLACK);                   // LCDを消去
    sipf_drawTitle("Example 03 LED");          // LCDにタイトルを表示
    M5.Lcd.print("Booting... ");                // 起動中の表示
    if(resetSipfModule()){                      // LTEモジュールのリセット
        M5.Lcd.println("NG");                   // リセットの失敗時
        return;                                 // 再起動
    }
    uint32_t fw_version;                        // バージョン保持用の変数を定義
    SipfGetFwVersion(&fw_version);              // バージョンを取得
    M5.Lcd.printf("\nFwVer.%08X\n",fw_version); // バージョン表示
    if(fw_version < 0x000400 && SipfSetAuthMode(0x01)){ // AuthModeモード設定
        M5.Lcd.println("Auth mode... NG");      // 設定失敗時の表示
        return;                                 // 再起動
    }
    sipf_drawButton(0, "OFF");                  // ボタンA(左)の描画
    sipf_drawButton(1, "RX");                   // ボタンB(中央)の描画
    sipf_drawButton(2, "ON");                   // ボタンC(右)の描画
    sipf_drawResultWindow();                    // RESULT画面の描画
    ledControl(led_stat);                          // LED消灯
    M5.Lcd.println("+++ Ready +++");            // 準備完了表示
    SipfClientFlushReadBuff();                  // LTEシリアル受信バッファ消去
}

void loop() {
    boolean rx = false;                         // 受信フラグ(false:OFF)
    M5.update();                                // M5Stack用IO状態の更新
    delay(1);                                   // 誤作動防止
    if(M5.BtnA.wasPressed()){                   // ボタンA(左)が押されたとき
        led_stat = false;
        ledControl(false);                      // LED消灯
    }
    if(M5.BtnB.wasPressed()){                   // ボタンB(中央)が押されたとき
        led_stat = true;
        rx = true;                              // 受信設定
    }
    if(M5.BtnC.wasPressed()){                   // ボタンC(右)が押されたとき
        ledControl(true);                       // LED点灯
    }
    if(millis() - time_prev > INTERVAL_ms){     // 30秒以上が経過した時
        rx = true;                              // 受信設定
    }
    if(rx){
        time_prev = millis();                   // 現在のマイコン時刻を保持
        sipf_drawResultWindow();                // RESULT画面の描画
        ledControl(led_stat);                          // LED消灯
        //M5.Lcd.printf("TX(tag_id=0x%02X temp=%f)\n", tag_id, temp);
        memset(buff, 0, sizeof(buff));          // 変数buffの内容を消去
        static SipfObjObject objs;
        uint64_t stm, rtm;
        uint8_t remain, qty;
        int ret = SipfCmdRx(buff, &stm, &rtm, &remain, &qty, &objs, 1);
        if(ret > 0) {                           // 受信に成功した時
            M5.Lcd.printf("OK\nOTID: %s\n", buff); // 送信結果を表示
            uint8_t *p_value = objs.value;
            SipfObjPrimitiveType v;
            if(objs.type == OBJ_TYPE_UINT32){
                memcpy(v.b, p_value, sizeof(uint32_t));
                M5.Lcd.printf("Recieved: %u\r\n", v.u32);
                led_stat = (boolean)v.u32;
                ledControl(led_stat);                          // LED消灯
            }
        }else if(ret == 0){
            M5.Lcd.printf("RX buffer is empty.\nOK\n");
        }else{
            M5.Lcd.printf("NG: %d\n", ret);
        }
    }
}
