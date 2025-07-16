#include <WiFi.h>
#include <Arduino.h>
#include <Wifi.h>

void connectToWiFi(char *ssid, char *password) {
  Serial.println("");
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.print("Connected! IP Address: ");
  Serial.println(WiFi.localIP());
}
