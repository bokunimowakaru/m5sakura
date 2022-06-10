#!/usr/bin/env python3
# coding: utf-8
# WebSocketを受信する
# Copyright (c) 2019-2022 Wataru KUNINO
#
# 引用
# https://github.com/bokunimowakaru/iot/blob/master/server/ws_logger_sakura.py
# https://github.com/bokunimowakaru/RaspberryPi/blob/master/network/sakura/ws_logger.py
################################################################################
# 下記のライブラリが必要です
# pip3 install websocket-client

import sys
import websocket
import datetime

url = 'wss://ws.sipf.iot.sakura.ad.jp/v0/'
token = '00000000-0000-0000-0000-000000000000'          # sakura.ioのtokenを記入
buf_n= 128                                              # 受信バッファ容量(バイト)
argc = len(sys.argv)                                    # 引数の数をargcへ代入
print('WebSocket Logger (usage:',sys.argv[0],'token)')  # タイトル表示

if argc == 2:                                           # 入力パラメータ数の確認
    token = sys.argv[1]                                 # トークンを設定

url += token
print('Listening,',url)                                 # URL表示
sock = websocket.create_connection(url)                 # ソケットを作成
while sock:                                             # 作成に成功したとき
    payload=sock.recv()                                 # WebSocketを取得
    str=''                                              # 表示用の文字列変数str
    for c in payload:                                   # WebSocket内
        if ord(c) >= ord(' '):                          # 表示可能文字
            str += c                                    # 文字列strへ追加
    date=datetime.datetime.today()                      # 日付を取得
    print(date.strftime('%Y/%m/%d %H:%M'), end='')      # 日付を出力
    print(', '+str)                                     # 受信データを出力
sock.close()                                            # ソケットの切断
