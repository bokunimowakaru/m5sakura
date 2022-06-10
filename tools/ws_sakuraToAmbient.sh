#!/bin/bash
################################################################################
#
# さくらのモノプラットフォーム用のWebSocketを受信し、Ambientへ転送する
#
# http://blogs.yahoo.co.jp/bokunimowakaru/55708303.html
# 
# Copyright (c) 2016-2021 Wataru KUNINO
#
# 引用
# https://github.com/bokunimowakaru/RaspberryPi/blob/master/network/sakura/sakuraToAmbient.sh
################################################################################

# まず、さくらのIoT Platformβへアクセスし、「連携サービス」に「WebSocket」を
# 追加してください。
#
# https://secure.sakura.ad.jp/iot/
#
# 「token」が発行されるので、下記の「token=」の部分に記述してください。
#
token="xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"       # Sakura IoT トークン

#
# 次に、Ambientへアクセスし、ユーザ登録を行ってください。
# 既にユーザ登録済みの方は、チャネルIDを追加してください。
#
# https://ambidata.io/
#
# 「チャネルID」と「ライトキー」を、下記に記述してください。

AmbientChannelId=100                                # Ambient チャネルID
AmbientWriteKey="0123456789abcdef"                  # Ambient ライトキー(16桁)

DEVICE="sakra_1"                                    # 識別名
INTERVAL=30                                         # 送信間隔(30秒以上を推奨)

while true; do
    DATA=`curl --max-time $INTERVAL -s -N \
    -H 'Sec-WebSocket-Version: 13' \
    -H "Sec-WebSocket-Key: $(head -c 16 /dev/urandom | base64)" \
    -H "Connection: Upgrade" \
    -H "Upgrade: websocket" \
    https://ws.sipf.iot.sakura.ad.jp/v0/${token}`
    if [ -n "$DATA" ]; then
        DATE=`echo $DATA\
        |tr ',' '\n'\
        |grep \"timestamp_platform_from_src\"\
        |head -1\
        |cut -c32-50\
        |tr "-" "/"\
        |tr "T" " "`
        DATE=`date --date "${DATE} UTC" "+%Y/%m/%d %H:%M:%S"` # UTCをJST(等)へ変換
        VALUE=`echo $DATA\
        |tr '{' '\n'\
        |grep "\"tag\"\:\"01\""\
        |head -1\
        |tr ',' '\n'\
        |grep \"value\"\
        |head -1\
        |cut -c9-\
        |tr -d '}'`
        if [ -n "$VALUE" ]; then
            echo ${DATE}", "${DEVICE}", "${VALUE}
            JSON="{\"writeKey\":\"${AmbientWriteKey}\",\"d1\":\"${VALUE}\"}"
        #   echo $JSON
            curl -s ambidata.io/api/v2/channels/${AmbientChannelId}/data\
                 -X POST -H "Content-Type: application/json" -d ${JSON} # データ送信
        fi
    fi
done

exit
####
1234567890123456789012345678901234567890123456789012345678901234567890123456789
"timestamp_platform_from_src":"2022-06-10T03:55:48.001Z"
                               |                 |
                              32                50

"type":"float32","tag":"01","value":0}

1234567890
"value":0}
