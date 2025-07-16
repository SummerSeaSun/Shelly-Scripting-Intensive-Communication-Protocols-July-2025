#include <WiFi.h>
#include <ArduinoJson.h>

#define REMOTE_UDP_PORT 2222
#define LOCAL_UDP_PORT 2223

class ControlLightsUDP {
  public:
    using OnStatusChangeFunc = std::function<void(StaticJsonDocument<200>)>;
    void begin(OnStatusChangeFunc onStatusChange);
    void sendRPC(String rpcMessage, String dst);
    //void sendAllRPC(String rpcMessage, String dst[]={});
    void sendAllRPC(String rpcMessage);
    void timerHandler();
  private:
    OnStatusChangeFunc onStatusChange;
    WiFiUDP udp;
    StaticJsonDocument<200> doc;
    String broadcastAddress;
    char incomingPacket[1024];
};