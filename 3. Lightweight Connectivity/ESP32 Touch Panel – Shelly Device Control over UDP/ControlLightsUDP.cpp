#include <Arduino.h>
#include <WiFi.h>
#include "ControlLightsUDP.h"

void ControlLightsUDP::timerHandler() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // Read packet into buffer
    int len = udp.read(incomingPacket, 1024);
    if (len > 0) {
      incomingPacket[len] = '\0';  // Null-terminate
    }
    Serial.print("UDP: received data:"); Serial.println(incomingPacket);
    deserializeJson(doc, incomingPacket);
    onStatusChange(doc);
  }
}


void ControlLightsUDP::begin(OnStatusChangeFunc onStatusChange) {
  this->onStatusChange = onStatusChange;

  udp.begin(LOCAL_UDP_PORT);
  IPAddress localIP = WiFi.localIP();
  IPAddress subnet = WiFi.subnetMask();

  // Calculate broadcast IP
  IPAddress broadcastIP;
  for (int i = 0; i < 4; i++) {
    broadcastIP[i] = localIP[i] | (~subnet[i]);
  }

  // Convert to String
  broadcastAddress = broadcastIP.toString();

  Serial.print("Broadcast IP: "); Serial.println(broadcastAddress);
}

void ControlLightsUDP::sendRPC(String rpcMessage, String dst) {
  Serial.print("UDP: sending data to "); Serial.println(dst); Serial.println(rpcMessage);
  // dst is an ip address
  udp.beginPacket(dst.c_str(), REMOTE_UDP_PORT);
  udp.print(rpcMessage);
  udp.endPacket();
}

void ControlLightsUDP::sendAllRPC(String rpcMessage) {
/*  if (sizeof(dst)==0) {
    dst = new String[1] {broadcastAddress};
  }
  for (int i=0; i < sizeof(dst) / sizeof(dst[0]); i++) {
    sendRPC(rpcMessage, dst[i]);
  }*/
  sendRPC(rpcMessage, broadcastAddress);
}
