#include <WiFiUdp.h>
#include <Arduino.h>

#define DISCOVERY_PACKET "HelloLocalHomeSDK"    // DiscoveryPacketで設定した文字列
#define DEVICEID "deviceid123"                  // otherDeviceIdsで設定しているものと一致するDeviceID

class LocalHomeUDP {
    public:
        WiFiUDP Udp;
        unsigned int localUdpPort = 3311;

        char incomingPacket[255];

        /*
         UDP通信開始
        */
        void begin() {
            Udp.begin(localUdpPort);
            Serial.printf("Now listening at UDP port %d\n", localUdpPort);
        }

        /*
         UDP通信処理
        */
        void task(){
            int packetSize = Udp.parsePacket();
            if (!packetSize)  return;
            
            int len = Udp.read(incomingPacket, 255);
            Serial.printf("UDP packet contents: %s\n", incomingPacket);
            if (len > 0) {
                incomingPacket[len] = 0;
            }

            // 文字列部分(NULL終端まで)をコピーする
            // 最後にNULL終端を入れるので、コピー先の配列はコピー元文字列＋1の要素数にする
            int messLen = strlen(incomingPacket) + 1;
            char mess[messLen];
            strncpy(mess, incomingPacket, messLen - 1);
            mess[messLen - 1] = 0;
            Serial.println(mess);

            // DiscoveryPacketと異なる場合は以降処理しない
            if(strcmp(mess, DISCOVERY_PACKET) != 0) {
                Serial.printf("The received message is not '%s'\n", DISCOVERY_PACKET);
                return;
            }
            Serial.println("The received message is ok");

            // DeviceIDを返す
            Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
            Udp.write(DEVICEID);
            Udp.endPacket();
        }
};
