#include <vs-rc202.h>
#include <ESP8266WiFi.h>
#include "udp.h"
#include "httpsrv.h"

#define SSID "********"
#define PASSWORD "********"

LocalHomeUDP udp;

void setup() {
    Serial.begin(115200);
    // V-duino(VS-RC202)のライブラリを使用するための初期化処理
    initLib();
    
    // Wifi接続
    Serial.printf("Connecting to %s ", SSID);
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println(" connected");
    Serial.printf("IP : %s\n", WiFi.localIP().toString().c_str());

    // UDP通信開始
    udp.begin();
    // Httpサーバ開始
    localHomeSrv.begin();
}

void loop() {
    // UDP通信処理
    udp.task();
    // Httpサーバ待受け処理
    localHomeSrv.task();
}
