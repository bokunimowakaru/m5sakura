#!/bin/bash
################################################################################
#
# さくらのモノプラットフォーム用のWebSocketを受信し、Ambientへ転送する
#
# Copyright (c) 2016-2021 Wataru KUNINO
#
# 引用
# https://github.com/bokunimowakaru/RaspberryPi/blob/master/network/sakura/sakuraToAmbient.sh
################################################################################

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

token="xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"       # 取得したWebsocket用トークン

################################################################################
# Ambient 設定手順
# (参考文献)
#    IoTデータ可視化サービスAmbient(アンビエントデーター社) https://ambidata.io/
################################################################################
# 1. https://ambidata.io/ へアクセス
# 2. 右上の[ユーザ登録(無料)]ボタンでメールアドレス、パスワードを設定して
#    アカウントを登録
# 3. [チャネルを作る]ボタンでチャネルIDを新規作成する
# 4. 「チャネルID」を下記のAmbientChannelIdに転記する
# 5. 「ライトキー」を下記のAmbientWriteKeyのダブルコート(")内に転記する

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
