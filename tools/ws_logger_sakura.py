#!/usr/bin/env python3
# coding: utf-8
# さくらのモノプラットフォーム用のWebSocketを受信する
# Copyright (c) 2019-2022 Wataru KUNINO
#
# 引用
# https://github.com/bokunimowakaru/iot/blob/master/server/ws_logger_sakura.py
# https://github.com/bokunimowakaru/RaspberryPi/blob/master/network/sakura/ws_logger.py
################################################################################
# 下記のライブラリが必要です
# pip3 install websocket-client

################################################################################
# さくらのモノプラットフォーム登録手順
# (参考文献) 詳細は下記を参照ください
#   さくらインターネット社 Sakura Cloud Docs さくらのモノプラットフォーム
#   https://manual.sakura.ad.jp/cloud/manual-iotpf.html
################################################################################
# 1. さくらのクラウド用コントロールパネルにアクセスしてください。
#     https://secure.sakura.ad.jp/cloud/
# 2. モノプラットフォームで「プロジェクト」を作成してください。
# 3. セキュアモバイルコネクトのメニュー「SIM」でモジュールの登録をします。
#   ※月額13円/デバイスと通信料6円/MBの費用が発生します。
#   - ICCIDとPASSCODEはモジュールの裏面に書かれています(半田付け前に要確認)
#   - SIMの状態は「有効」を選択します
#   - キャリアは「Softbank」を選択します(執筆時点SCO-M5SNRF9160のファームの場合)
#   - 接続先はモノプラットフォームを選択します
#   - プロジェクトでは、2で作成したプロジェクト名を選択してください
# 4. モノプラットフォームで「サービスアダプタ」を登録してください。
#   ※基本利用月額220円/デバイスとサービスアダプタ月額11円/デバイスが発生します
#   - プロジェクトでは、2で作成したプロジェクト名を選択してください
#   - サービスアダプタには「websocket」を選択してください
# 5. 「サービスアダプタ」の情報画面でトークンを確認してください。
#   - モノプラットフォームの「サービスアダプタ」でプロジェクト名をダブルクリック
#   - 「xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx」の形式で表示される
# 6. 下記の「token=」の部分にトークンを転記してください。

token = '00000000-0000-0000-0000-000000000000' # さくらのモノプラットフォームのtokenを記入

import sys                                              # 引数の入力に使用
import websocket                                        # WebSocketクライアント組み込み
import json                                             # JSON変換ライブラリを組み込む
import datetime                                         # 日時の取得に使用

url_ws = 'wss://ws.sipf.iot.sakura.ad.jp/v0/'           # アクセス先(さくら,WebSocket)
argc = len(sys.argv)                                    # 引数の数をargcへ代入
print('WebSocket Logger (usage:',sys.argv[0],'token)')  # タイトル表示

if argc >= 2:                                           # 入力パラメータ数の確認
    token = sys.argv[1]                                 # トークンを設定

url_ws += token                                         # トークンを連結
print('Listening,',url_ws)                              # URL表示
try:
    sock = websocket.create_connection(url_ws)          # ソケットを作成
except Exception as e:                                  # 例外処理発生時
    print(e)                                            # エラー内容を表示
    exit()                                              # プログラムの終了
print('CONNECTED')
while sock:                                             # 作成に成功したとき
    res=sock.recv()                                     # WebSocketを取得
    date=datetime.datetime.today()                      # 日付を取得
    print(date.strftime('%Y/%m/%d %H:%M'), end='')      # 日付を出力
    print(', '+res)                                     # 受信データを出力
    res_dict = json.loads(res)                          # 辞書型の変数res_dictへ
    res_type = res_dict.get('type')                     # res_dict内のtypeを取得
    if res_type == 'keepalive':                         # typeがkeepaliveのとき
        continue
    if res_type != 'object':                            # typeがobjectでないとき
        continue
    res_id = res_dict.get('device_id')                  # res_dict内のmoduleを取得
    print('from     =',res_id)
    data_time = res_dict.get('timestamp_platform_from_src')
    print('datetime =', data_time)
    res_payload_dict = res_dict['payload']              # res_dict内のpayload取得
    data_text=''                                        # 受信テキスト用の文字列
    for data in res_payload_dict:                       # 各チャネルに対して
        print('--------------------------------------')
        data_type = data.get('type')
        data_tag  = data.get('tag')
        data_type_s= 'Unknown'
        data_value = None
        # 各種OBJECT_TYPE
        # https://manual.sakura.ad.jp/cloud/iotpf/device-adapter/sipf_obj_cmd_guide.html#object-type-list-label
        if data_type[0:5] == 'float' :
            data_type_s= 'Float'
            try:
                data_value = float(data['value'])
            except ValueError:
                data_type_s= 'Unknown'
        if data_type[0:3] == 'int' or data_type[0:4] == 'uint':
            data_type_s= 'Integer'
            try:
                data_value = int(data['value'])
            except ValueError:
                data_type_s= 'Unknown'
        if data_type == 'string_utf8':
            data_type_s= 'String'
            data_text += data['value']
        if data_type == 'binary':
            data_type_s= 'String'
            data_bin = data['value']
            i=0
            data_value=[]
            while i < len(data_bin):                    # 受信16進数文字列の処理
                if i % 4 == 0:                          # リトルEndian 2バイト値
                    val = int(data_bin[i+2:i+4] + data_bin[i:i+2],16)
                    if val >= 32768:                    # 2補数の簡易処理
                        val -= 65536
                    data_value.append(val)              # 受信数値を配列変数へ
                c = chr(int(data_bin[i:i+2],16))        # 文字コードへ変換
                if ord(c) >= 16 and ord(c) < 256:       # 特殊文字ではないとき
                    data_text += c                      # 文字列へ追加
                i += 2
        print('tag      =', data_tag)
        print('type     =', data_type_s)
        print('value    =', data_value)                 # 受信結果(数値)の表示
    print('Message  =', '"' + data_text + '"')          # 受信結果(文字列)の表示
sock.close()                                            # ソケットの切断
