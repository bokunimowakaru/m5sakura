/******************************************************************************
モバイル網経由のファームウェア・アップデータです。

ご注意：アップデートに失敗したとしても無保証です。
※異なるデバイス用のファームウェアを誤って使用しないように注意してください。

(手順)
1. 下記から対象デバイス用の最新ファームウェアをPCなどにダウンロードする
    https://github.com/sakura-internet/sipf-std-client_nrf9160/releases
    (一例) sipf-std-client_nrf9160_v0.4.4_scm-ltem1nrf_nrf9160ns.zip
    ※注意：誤ったファームを使ってしまうと起動しなくなるので注意すること
    ※とくに「nrf9160」の文字につられないでください
2. さくらのモノプラットフォームのコントロールパネルにアクセスする
    https://secure.sakura.ad.jp/cloud/
3. モノプラットフォームの「プロジェクト」で対象のプロジェクトを選択する
4. タブ「ファイル送受信」を選択する
5. 手順1でダウンロードしたZIPフォルダ内のapp_update.binをアップロードする
6. 本プログラムをM5Stackに書き込む
7. M5Stackの右ボタンでFOTAによるファームウェアのアップデートができる

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

#include <Arduino.h>
#include <M5Stack.h>
#include <string.h>
#include "sipf_client.h"

/*
#define ENABLE_GNSS
*/

#ifndef ENABLE_GNSS
#define WIN_RESULT_LEFT   (0)
#define WIN_RESULT_TOP    (50)
#define WIN_RESULT_WIDTH  (320)
#define WIN_RESULT_HEIGHT (148)
#define WIN_RESULT_TITLE_HEIGHT (10)
#else
#define WIN_RESULT_LEFT   (0)
#define WIN_RESULT_TOP    (110)
#define WIN_RESULT_WIDTH  (320)
#define WIN_RESULT_HEIGHT (88)
#define WIN_RESULT_TITLE_HEIGHT (10)
#endif
/**
 * SIPF接続情報
 */
static uint8_t buff[256];
static uint32_t cnt_btn1;

static int resetSipfModule()
{
  digitalWrite(5, LOW);
  pinMode(5, OUTPUT);

  // Reset要求
  digitalWrite(5, HIGH);
  delay(10);
  digitalWrite(5, LOW);

  // UART初期化
  Serial2.begin(115200, SERIAL_8N1, 16, 17);

  // 起動完了メッセージ待ち
  Serial.println("### MODULE OUTPUT ###");
  int len, is_echo = 0;
  for (;;) {
    len = SipfUtilReadLine(buff, sizeof(buff), 60000); //タイムアウト60秒
    if (len < 0) {
      return -1;  //Serialのエラーかタイムアウト
    }
    if (len == 1) {
      //空行なら次の行へ
      continue;
    }
    if (len >= 13) {
      if (memcmp(buff, "*** SIPF Client", 15) == 0) {
        is_echo = 1;
      }
      //起動完了メッセージを確認
      if (memcmp(buff, "+++ Ready +++", 13) == 0) {
        Serial.println("#####################");
        break;
      }
      //接続リトライオーバー
      if (memcmp(buff, "ERR:Faild", 9) == 0) {
        Serial.println((char*)buff);
        Serial.println("#####################");
        return -1;
      }
    }
    if (is_echo) {
      Serial.printf("%s\r\n", (char*)buff);
    }
  }
  return 0;
}

static void drawTitle(void)
{
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillRect(0, 0, 320, 20, 0xfaae);
  M5.Lcd.setCursor(2, 2);
  M5.Lcd.setTextColor(TFT_BLACK, 0xfaae);
  M5.Lcd.printf("sipf-FOTA-update for M5\n");

  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  M5.Lcd.setCursor(0, 24);
}

static void drawButton(uint8_t button, uint32_t value)
{
  M5.Lcd.fillRect(35 + (95 * button), 200, 60, 40, 0xfaae);

  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(TFT_BLACK, 0xfaae);
  M5.Lcd.setCursor(40 + (95 * button), 210);

  switch (button) {
  case 0: // TX
    M5.Lcd.printf("%4d", value);
    break;
  case 1: // RX
    M5.Lcd.printf(" RX ");
    break;
  case 2:
    M5.Lcd.printf("FOTA");
    break;
  }
}

static void drawButton(uint8_t button, String S)
{
  M5.Lcd.fillRect(35 + (95 * button), 200, 60, 40, 0xfaae);
  M5.Lcd.setTextColor(TFT_BLACK, 0xfaae);
  M5.Lcd.drawCentreString(S, 65 + (95 * button), 210, 1);
}


static void setCursorResultWindow(void)
{
  M5.Lcd.setTextColor(TFT_BLACK, 0xce79);
  M5.Lcd.setCursor(0, WIN_RESULT_TOP + WIN_RESULT_TITLE_HEIGHT + 1);
}

static void drawResultWindow(void)
{
  M5.Lcd.setTextSize(1);

  M5.Lcd.fillRect(WIN_RESULT_LEFT, WIN_RESULT_TOP, WIN_RESULT_WIDTH, WIN_RESULT_TITLE_HEIGHT, 0xfaae);
  M5.Lcd.setTextColor(TFT_BLACK, 0xfaae);
  M5.Lcd.setCursor(1, WIN_RESULT_TOP + 1);
  M5.Lcd.printf("RESULT");

  M5.Lcd.fillRect(WIN_RESULT_LEFT, WIN_RESULT_TOP + WIN_RESULT_TITLE_HEIGHT, WIN_RESULT_WIDTH, WIN_RESULT_HEIGHT - WIN_RESULT_TITLE_HEIGHT, 0xce79);
  setCursorResultWindow();
}

void reset(){
  M5.Lcd.printf("Booting...");
  while (resetSipfModule() != 0) {
    M5.Lcd.printf(" FAILED\n");
    return;
  }
  M5.Lcd.printf(" OK\n");
}

void getVer(){
    uint32_t fw_version;                        // バージョン保持用の変数を定義
    M5.Lcd.print("FwVersion... ");            // バージョン取得表示
    while(SipfGetFwVersion(&fw_version)){       // バージョンを取得
	    M5.Lcd.printf(" FAILED\r\n");
	    delay(1000);
	    return;
    }
    M5.Lcd.printf("%08X\n",fw_version);         // バージョン表示
    if(fw_version < 0x000400 && SipfSetAuthMode(0x01)){ // AuthModeモード設定
        M5.Lcd.println("Auth mode... NG");      // 設定失敗時の表示
    }
}

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  M5.Power.begin();

  Serial.begin(115200);

  M5.Lcd.setBrightness(127);

  drawTitle();

  reset();
  getVer();

  drawResultWindow();
  M5.Lcd.println("+++ Ready +++");
  M5.Lcd.println("Hello, Press [Right] button to update firmware");

  cnt_btn1 = 0;

  drawButton(0, cnt_btn1);
  drawButton(1, "RESET");
  drawButton(2, "FOTA");

  SipfClientFlushReadBuff();
}

void loop() {
  // put your main code here, to run repeatedly:
  int available_len;
  /* PCとモジュールのシリアルポートを中継 */
  available_len = Serial.available();
  for (int i = 0; i < available_len; i++) {
    unsigned char b = Serial.read();
    Serial2.write(b);
  }

  available_len = Serial2.available();
  for (int i = 0; i < available_len; i++) {
    unsigned char b = Serial2.read();
    Serial.write(b);
  }

  M5.update();
  delay(1);

  /* `TX1'ボタンを押した */
  if (M5.BtnA.wasPressed()) {
    cnt_btn1++;
    drawResultWindow();
    M5.Lcd.printf("ButtonA pushed: TX(tag_id=0x01 value=%d)\n", cnt_btn1);
    memset(buff, 0, sizeof(buff));
    int ret = SipfCmdTx(0x01, OBJ_TYPE_UINT32, (uint8_t*)&cnt_btn1, 4, buff);
    if (ret == 0) {
      M5.Lcd.printf("OK\nOTID: %s\n", buff);
      drawButton(0, cnt_btn1);
    } else {
      M5.Lcd.printf("NG: %d\n", ret);
    }
  }

  /* `RX'ボタンを押した */
  if (M5.BtnB.wasPressed()) {
    drawResultWindow();
    M5.Lcd.printf("ButtonB pushed: RESET & Get Version.\n");
    reset();
    getVer();
  /*
    M5.Lcd.printf("ButtonB pushed: RX request.\n");
    memset(buff, 0, sizeof(buff));

		static SipfObjObject objs[16];
		uint64_t stm, rtm;
		uint8_t remain, qty;
    int ret = SipfCmdRx(buff, &stm, &rtm, &remain, &qty, objs, 16);
    if (ret > 0) {
      time_t t;
      struct tm *ptm;
      static char ts[128];
      M5.Lcd.printf("OTID: %s\r\n", buff);
      //User send datetime.
      t = (time_t)(stm / 1000);
      ptm = localtime(&t);
      strftime(ts, sizeof(ts),"User send datetime(UTC)    : %Y/%m/%d %H:%M:%S\r\n", ptm);
      M5.Lcd.printf(ts);
      //SIPF receive datetime.
      t = (time_t)(rtm / 1000);
      ptm = localtime(&t);
      strftime(ts, sizeof(ts),"SIPF received datetime(UTC): %Y/%m/%d %H:%M:%S\r\n", ptm);
      M5.Lcd.printf(ts);
      //remain, qty
      M5.Lcd.printf("remain=%d, qty=%d\r\n", remain, qty);
      //obj
      for (int i = 0; i < ret; i++) {
        M5.Lcd.printf("obj[%d]:tag=0x%02x, type=0x%02x, len=%d, value=", i, objs[i].tag_id, objs[i].type, objs[i].value_len);
        uint8_t *p_value = objs[i].value;
        SipfObjPrimitiveType v;
        switch (objs[i].type) {
        case OBJ_TYPE_UINT8:
          memcpy(v.b, p_value, sizeof(uint8_t));
          M5.Lcd.printf("%u\r\n", v.u8);
          break;
        case OBJ_TYPE_INT8:
          memcpy(v.b, p_value, sizeof(int8_t));
          M5.Lcd.printf("%d\r\n", v.i8);
          break;
        case OBJ_TYPE_UINT16:
          memcpy(v.b, p_value, sizeof(uint16_t));
          M5.Lcd.printf("%u\r\n", v.u16);
          break;
        case OBJ_TYPE_INT16:
          memcpy(v.b, p_value, sizeof(int16_t));
          M5.Lcd.printf("%d\r\n", v.i16);
          break;
        case OBJ_TYPE_UINT32:
          memcpy(v.b, p_value, sizeof(uint32_t));
          M5.Lcd.printf("%u\r\n", v.u32);
          break;
        case OBJ_TYPE_INT32:
          memcpy(v.b, p_value, sizeof(int32_t));
          M5.Lcd.printf("%d\r\n", v.i32);
          break;
        case OBJ_TYPE_UINT64:
          memcpy(v.b, p_value, sizeof(uint64_t));
          M5.Lcd.printf("%llu\r\n", v.u64);
          break;
        case OBJ_TYPE_INT64:
          memcpy(v.b, p_value, sizeof(int64_t));
          M5.Lcd.printf("%lld\r\n", v.i64);
          break;
        case OBJ_TYPE_FLOAT32:
          memcpy(v.b, p_value, sizeof(float));
          M5.Lcd.printf("%f\r\n", v.f);
          break;
        case OBJ_TYPE_FLOAT64:
          memcpy(v.b, p_value, sizeof(double));
          M5.Lcd.printf("%lf\r\n", v.d);
          break;
        case OBJ_TYPE_BIN:
          M5.Lcd.printf("0x");
          for (int j = 0; j < objs[i].value_len; j++) {
            M5.Lcd.printf("%02x", objs[i].value[j]);
          }
          M5.Lcd.printf("\r\n");
          break;
        case OBJ_TYPE_STR_UTF8:
          for (int j = 0; j < objs[i].value_len; j++) {
            M5.Lcd.printf("%c", objs[i].value[j]);
          }
          M5.Lcd.printf("\r\n");
          break;
        default:
          break;
        }
      }
      M5.Lcd.printf("OK\n");
    } else if (ret == 0) {
      M5.Lcd.printf("RX buffer is empty.\nOK\n");
    } else {
      M5.Lcd.printf("NG: %d\n", ret);
    }
  */
  }

  /* `FOTA'ボタンを押した */
  if (M5.BtnC.wasPressed()) {
    drawResultWindow();
    M5.Lcd.printf("ButtonC pushed: FOTA request.\n");
    char cmd[512];
    int len, ret;
    M5.Lcd.print("UNLOCK... ");
    SipfClientFlushReadBuff();
    len = sprintf(cmd, "$$UNLOCK UNLOCK\r\n");
    ret = Serial2.write((uint8_t*)cmd, len);
    for (;;) {
        ret = SipfUtilReadLine((uint8_t*)cmd, sizeof(cmd), 10000);
        M5.Lcd.println(cmd);
        if (cmd[0] == '$') continue;    // エコーバック
        if (memcmp(cmd, "NG", 2) == 0) {
            M5.Lcd.println("ERROR: UNLOCK");
            return;
        }
        if (memcmp(cmd, "OK", 2) == 0) {
            delay(1000);
            M5.Lcd.print("UPDATE... ");
            SipfClientFlushReadBuff();
            len = sprintf(cmd, "$$UPDATE UPDATE\r\n");
            ret = Serial2.write((uint8_t*)cmd, len);
            int num=0,n;
            for (;;) {
                n = 0;
                ret = SipfUtilReadLine((uint8_t*)cmd, sizeof(cmd), 10000);
                if(ret < 3) continue;
                if(memcmp(cmd, "FOTA ", 5) == 0 || memcmp(cmd, "*** ", 4) == 0) drawResultWindow();
                if(cmd[1]>='1' and cmd[1]<='9'){
                    n = 10 * (cmd[1] - '0') + (cmd[2] - '0');
                    if(cmd[2]=='0' && n != num) drawResultWindow();
                }
                if(cmd[1]==' ' and cmd[2]>='0' and cmd[1]<='9'){
                    n = cmd[2] - '0';
                }
                if(cmd[0]>='0' and cmd[0]<='9' and  cmd[1]=='9') drawResultWindow();
                char *p = strstr(cmd, "\r");
                if(p) *p = '\0';
                p = strstr(cmd, "\n");
                if(p) *p = '\0';
                if(n==0 || n != num) M5.Lcd.println(cmd);
                num = n;
                if (memcmp(cmd, "NG", 2) == 0) {
                    M5.Lcd.println("ERROR: UPDATE");
                    return;
                }
            }
        }
    }
  }
}
