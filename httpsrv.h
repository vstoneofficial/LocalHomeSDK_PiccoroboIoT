#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>

#define LOCAL_HOME_SERVER_PORT 3388
#define REPORT_STATE_ENDPOINT_URL "https://<project-id>.firebaseapp.com/updateState"
// SHA1のフィンガープリント(「XX XX ... XX XX」といった形式になる)
#define FINGERPRINT "<fingerprint>"

class LocalHomeServer {
    public:
        ESP8266WebServer *server;
        void begin();
        void task();
        void reportState();
};

class Status {
    public:
        bool on;
        bool isRunning;
        bool isPaused;
};

LocalHomeServer localHomeSrv;
Status status;

/*
 Httpサーバ開始
*/
void LocalHomeServer::begin() {
    server = new ESP8266WebServer(LOCAL_HOME_SERVER_PORT);
    server->on("/", HTTP_POST,  [](){
        StaticJsonDocument<200> doc;
        JsonObject object = doc.as<JsonObject>();
        String json = localHomeSrv.server->arg("plain");
        deserializeJson(doc, json);
        Serial.println(json);

        // 変更があった項目のみ送られてくるので、存在するものだけ状態に反映する
        if(doc.containsKey("on")) {
            status.on = doc["on"];
        }
        if(doc.containsKey("isRunning")) {
            status.isRunning = doc["isRunning"];
        }
        if(doc.containsKey("isPaused")) {
            status.isPaused = doc["isPaused"];
        }
        Serial.printf("on:%d, isRunning:%d, isPaused:%d\n", status.on, status.isRunning, status.isPaused);

        localHomeSrv.reportState();

        localHomeSrv.server->send(200);
    });
    server->begin();
    Serial.printf("Http Server at port %d\n", LOCAL_HOME_SERVER_PORT);

    status.on = false;
    status.isRunning = false;
    status.isPaused = false;
}

/*
 待受け処理
*/
void LocalHomeServer::task() {
    server->handleClient();
}

/*
 クラウドへ状態を送信
*/
void LocalHomeServer::reportState() {
    StaticJsonDocument<200> doc;
    doc["on"] = status.on;
    doc["isRunning"] = status.isRunning;
    doc["isPaused"] = status.isPaused;
    String payload = "";
    serializeJson(doc, payload);
    Serial.println(payload);

    HTTPClient http;
    http.begin(REPORT_STATE_ENDPOINT_URL, FINGERPRINT);
    http.addHeader("Content-Type", "application/json");
    int respCode = http.POST(payload);
    if(0 < respCode) {
        Serial.printf("report state response code : %d\n", respCode);
    } else {
        Serial.printf("report state failed, error: %s\n", http.errorToString(respCode).c_str());
    }

    http.end();
}
