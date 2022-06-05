/******************************************************************************
Example 01: ボタン送信機 for さくらモノプラットフォーム
・本体上面のボタンを押すと、ボタンに応じたメッセージを送信します。

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
static uint8_t buff[256];                       // 受信データ表示用のバッファ

void setup(){                                   // 起動時に一度だけ実行する関数
    M5.Lcd.begin();                             // M5Stack用Lcdライブラリの起動
    M5.Lcd.setBrightness(31);                   // 輝度を下げる（省エネ化）
    M5.Lcd.fillScreen(BLACK);                   // LCDを消去
    sipf_drawTitle("Example 01 BTN");           // LCDにタイトルを表示
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
    sipf_drawButton(2, "ON");                   // ボタンC(右)の描画
    sipf_drawResultWindow();                    // RESULT画面の描画
    M5.Lcd.println("+++ Ready +++");            // 準備完了表示
    SipfClientFlushReadBuff();                  // LTEシリアル受信バッファ消去
}

void loop() {
    boolean tx = false;                         // 送信フラグ(false:OFF)
    byte tag_id = 0x01;                         // Tag ID を 0x01に設定
    uint32_t value;                             // 送信値
    M5.update();                                // M5Stack用IO状態の更新
    delay(1);                                   // 誤作動防止
    if(M5.BtnA.wasPressed()){                   // ボタンA(左)が押されたとき
        tx =true;                               // 送信設定
        value = 0;                              // 送信値を0に設定
    }
    if(M5.BtnC.wasPressed()){                   // ボタンC(右)が押されたとき
        tx =true;                               // 送信設定
        value = 1;                              // 送信値を1に設定
    }
    if(tx){
        sipf_drawResultWindow();                // RESULT画面の描画
        M5.Lcd.printf("TX(tag_id=0x%02X value=%d)\n", tag_id, value);
        memset(buff, 0, sizeof(buff));          // 変数buffの内容を消去
        int ret = SipfCmdTx(0x01, OBJ_TYPE_UINT32, (uint8_t*)&value, 4, buff);
        if (ret == 0) {                         // 送信に成功した時
            M5.Lcd.printf("OK\nOTID: %s\n", buff); // 送信結果を表示
        }else{                                  // 送信に失敗した時
            M5.Lcd.printf("NG: %d\n", ret);     // 応答値を表示
        }
    }
}
