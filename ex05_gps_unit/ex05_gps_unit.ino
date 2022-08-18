/******************************************************************************
Example 04: GPS(GNSS)位置情報送信機 for さくらモノプラットフォーム
・約30秒ごとに位置情報を送信します。
・左ボタンで日本地図表示に切り替わります。
・右ボタンで通常表示に切り替わります。

                                          Copyright (c) 2022 Wataru KUNINO
*******************************************************************************
★ご注意★・屋外での視覚用にLCDの★輝度を最大に設定★してあります。
          ・電源を入れてから位置情報が得られるまで数分以上を要します。
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
#include "japan_jpg.h"                          // 日本地図のJPEGデータ
#include "lib_TinyGPS.h"                        // GPS通信用ライブラリ
#define TITLE "Example 05 GNSS/GPS"             // タイトル
#define INTERVAL_ms 30000                       // 送信間隔
RTC_DATA_ATTR int mode = 0;                     // 0:標準 1:日本地図表示
/*******************************************************************************
 GPS位置と地図画像の位置座標対応表
 *******************************************************************************
 ・位置情報と画像ファイルの位置座標との対応表です。
 ・測定点の近い座標を追加することで、より精度の高い表示が可能になります。
 ・(近郊の地図や海外など)地図画像を変更する場合は、書き換えが必要です。

 要素数：JpMAP_N
 書式：{経度,緯度,X座標,Y座標}
*******************************************************************************/

#define JpMAP_N 7                               // 位置座標対応表の件数
const float japan[JpMAP_N][4]={
    {129.87, 32.76,  21, 194},
    {135.00, 34.65, 104, 178},
    {139.74, 35.66, 176, 173},
    {139.15, 37.83, 171, 133},
    {141.92, 45.50, 231,  3},
    {127.65, 26.20, 198, 234},
    {128.30, 26.83, 214, 220}
};                                              // 位置座標対応データ

TinyGPS gps;                                    // GPSライブラリのインスタンス

static uint8_t otid[33];                        // 送信時のOTID保持用バッファ
unsigned long time_prev = millis();             // マイコン時刻(ms単位)を保持
unsigned long time_metric = millis();           // 同上、メータ表示用
int timeout_n = 0;                              // 通信タイムアウト回数
float lat =  35.+40./60.+37.8311/3600.;         // 緯度の初期値
float lon = 139.+44./60.+51.5282/3600.;         // 経度の初期値
float alt = 0.;                                 // 高度の初期値

void disp(bool cls){                            // 画面描画
    switch(mode){                               // 画面モードに応じた処理
        case 0:                                 // mode=0のとき：
            if(cls){
                M5.Lcd.fillScreen(BLACK);       // 表示内容を消去
                sipf_drawTitle(TITLE);          // タイトルを表示
            }
            sipf_drawButton(0, "Map");          // ボタンA(左)の描画
            sipf_drawButton(1, "SEND");         // ボタンB(中央)の描画
            sipf_drawButton(2, "Text");         // ボタンC(右)の描画
            sipf_drawResultWindow();            // RESULT画面の描画
            break;
        case 1:                                 // mode=1のとき
            M5.Lcd.drawJpg(japan_jpg,japan_jpg_len); // LCDに日本地図を表示
            disp_point(lat,lon);                // 緯度経度を位置に点を描画
            M5.Lcd.setTextColor(WHITE);         // 文字色を白(背景なし)に設定
            M5.Lcd.setCursor(0, 0);
            break;
    }
}

void disp_point(float lat, float lon){          // 緯度経度の位置に点を描画する
    float min[2]={999,999};                     // 検索結果の保持用(最小値)
    int ind[2]={0,0};                           // 検索結果の保持用(配列番号)
    for(int i = 0; i < JpMAP_N; i++){           // 位置座標対応表を検索
        float d0 = lon - japan[i][0];           // 経度の差をd0に
        float d1 = lat - japan[i][1];           // 緯度の差をd1に
        float d = sqrt(pow(d0,2)+pow(d1,2));    // d0とd1から距離を計算
        if(min[0] < d && d < min[1]){           // 過去の結果の2位よりも近い
            min[1] = d;                         // 暫定2位として距離を更新
            ind[1] = i;                         // 暫定2位として配列番号を更新
        }else if(d < min[0]){                   // 過去の結果の1位よりも近い
            min[1] = min[0];                    // 現1位の距離を2位に更新
            ind[1] = ind[0];                    // 現1位の配列番号を2位に更新
            min[0] = d;                         // 暫定1位として距離を更新
            ind[0] = i;                         // 暫定1位として配列番号を更新
        }
    }                                           // 全位置座標対応表の繰り返し
    int x=(int)(
        (japan[ind[1]][2]-japan[ind[0]][2])
        *((lon-japan[ind[0]][0])/(japan[ind[1]][0]-japan[ind[0]][0]))
        +japan[ind[0]][2]
    );                                          // 1位と2位の結果からX座標を計算
    int y=(int)(
        (japan[ind[1]][3]-japan[ind[0]][3])
        *((lat-japan[ind[0]][1])/(japan[ind[1]][1]-japan[ind[0]][1]))
        +japan[ind[0]][3]
    );                                          // 1位と2位の結果からY座標を計算
    if(x>=0 && x<320 && y>=0 && y<240){         // 計算結果が表示領域内のとき
        M5.Lcd.fillCircle(x,y,3,RED);           // 赤色の丸印を描画
        M5.Lcd.drawCircle(x,y,4,WHITE);         // 白色の縁取りを描画
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
    M5.Lcd.setBrightness(255);                  // LCDを最大輝度に設定
    M5.Lcd.fillScreen(BLACK);                   // LCDを消去
    sipf_drawTitle(TITLE);                      // LCDにタイトルを表示
    reset();                                    // 関数リセットを呼び出し
    M5.Lcd.printf("Enable GNSS ");              // 起動中表示
    setupGps();                                 // GPS初期化
    disp(false);                                // 画面描画
    M5.Lcd.println("+++ Ready +++");            // 準備完了表示
}

void loop() {
    GnssLocation lc;                            // 位置情報保持用の構造体変数
    boolean tx = false;                         // 送信フラグ(false:OFF)
    if(millis() - time_metric > 1000){
        time_metric = millis();
        lc.fixed = getGpsPos(gps,&lc.latitude,&lc.longitude,&lc.altitude);
        if(lc.fixed){
            lat = lc.latitude;
            lon = lc.longitude;
            alt = lc.altitude;
            if(mode==0){
                sipf_drawGnssLocation(&lc);
            }else{
                disp_point(lat, lon);
            }
        }
    }
    M5.update();                                // M5Stack用IO状態の更新
    delay(1);                                   // 誤作動防止
    if(M5.BtnA.wasPressed()){                   // ボタンA(左)が押されたとき
        mode = 1;
        disp(false);                            // 画面描画
    }
    if(M5.BtnB.wasPressed()){                   // ボタンB(中央)が押されたとき
        tx = true;                              // 送信設定
    }
    if(M5.BtnC.wasPressed()){                   // ボタンC(右)が押されたとき
        mode = 0;
        disp(true);                             // 画面描画(画面消去を伴う)
    }
    if(millis() - time_prev > INTERVAL_ms){     // 30秒以上が経過した時
        tx = true;                              // 送信設定
    }
    if(tx){                                     // 送信フラグがTrueの時
        time_prev = millis();                   // 現在のマイコン時刻を保持
        disp(false);                            // 画面描画
        M5.Lcd.printf("TX(tag_id=0x01 lat=%f)\n", lat);
        SipfCmdTx1(1, OBJ_TYPE_FLOAT32, (uint8_t*)&lat, 4);
        M5.Lcd.printf("TX(tag_id=0x02 lon=%f)\n", lon);
        SipfCmdTx2(2, OBJ_TYPE_FLOAT32, (uint8_t*)&lon, 4);
        M5.Lcd.printf("TX(tag_id=0x03 alt=%f)... ", alt);
        memset(otid, 0, sizeof(otid));          // 変数otidの内容を消去
        int ret = SipfCmdTx3(3, OBJ_TYPE_FLOAT32, (uint8_t*)&alt, 4, otid);
        if (ret == 0) {                         // 送信に成功した時
            M5.Lcd.printf("OK\nOTID: %s\n", otid); // 送信結果を表示
        }else{                                  // 送信に失敗した時
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
