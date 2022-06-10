#!/usr/bin/env python3
# coding: utf-8
# さくらのモノプラットフォーム用のWebSocketを受信し、Ambientへ転送する
# Copyright (c) 2019-2022 Wataru KUNINO
#
# 引用
# https://github.com/bokunimowakaru/iot/blob/master/server/ws_logger_sakura.py
# https://github.com/bokunimowakaru/RaspberryPi/blob/master/network/sakura/ws_logger.py
################################################################################
# 下記のライブラリが必要です
# pip3 install websocket-client

token = '00000000-0000-0000-0000-000000000000' # さくらのモノプラットフォームのtokenを記入
ambient_chid='00000'                # ここにAmbientで取得したチャネルIDを入力
ambient_wkey='0123456789abcdef'     # ここにはライトキーを入力
ambient_interval = 29               # Ambientへの送信間隔

ambient_gnss_en = True              # AmbientへGPS/GNSS位置情報を送信
ambient_lat = 1                     # Ambient用の緯度用tag番号
ambient_lng = 2                     # Ambient用の経度用tag番号

import sys
import websocket
import urllib.request                                   # HTTP通信ライブラリを組み込む
import json                                             # JSON変換ライブラリを組み込む
import datetime

url_ws = 'wss://ws.sipf.iot.sakura.ad.jp/v0/'
url_s = 'https://ambidata.io/api/v2/channels/'+ambient_chid+'/data' # アクセス先
head_dict = {'Content-Type':'application/json'}         # ヘッダを変数head_dictへ

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
date_prev = datetime.datetime(1970,1,1)                 # 前回の送信時刻を保持

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
    body_dict = {'writeKey':ambient_wkey}
    for data in res_payload_dict:                       # 各チャネルに対して
        print('--------------------------------------')
        data_type = data.get('type')
        try:
            data_tag = int('0x' + data.get('tag'),0)
        except ValueError:
            data_tag= 0
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
        if ambient_gnss_en:
            if data_tag == ambient_lat:
                body_dict['lat'] = data_value
            if data_tag == ambient_lng:
                body_dict['lng'] = data_value
        if data_tag > 0 and data_tag <= 8:
            key = 'd' + str(data_tag)
            body_dict[key] = data_value
    print('Message  =', '"' + data_text + '"')          # 受信結果(文字列)の表示
    if int(ambient_chid) == 0 or \
        date - date_prev < datetime.timedelta(seconds=ambient_interval):
        continue
    date_prev = date

    # クラウドへの送信処理
    print("\nTo Ambient",body_dict)                     # 送信内容body_dictを表示
    post = urllib.request.Request(url_s, json.dumps(body_dict).encode(), head_dict)
                                                        # POSTリクエストデータを作成
    try:                                                # 例外処理の監視を開始
        res = urllib.request.urlopen(post)              # HTTPアクセスを実行
    except Exception as e:                              # 例外処理発生時
        print(e,url_s)                                  # エラー内容と変数url_sを表示
        continue
    res_str = res.read().decode()                       # 受信テキストを変数res_strへ
    res.close()                                         # HTTPアクセスの終了
    if len(res_str):                                    # 受信テキストがあれば
        print('Response:', res_str)                     # 変数res_strの内容を表示
    else:
        print('Done')                                   # Doneを表示

sock.close()                                            # ソケットの切断
