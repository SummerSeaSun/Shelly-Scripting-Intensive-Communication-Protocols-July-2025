#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include "Config.h"
#include "Wifi.h"
#include "TransmitChannel.h"

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

unsigned long last_time;

CustomBLEChannel customBLEChannel;
BTHomeChannel bthomeChannel;
TransmitChannel *wireless_channel = &customBLEChannel;

WebServer server(80);

StaticJsonDocument<1000> jsonDocument;
char buffer[1000];

float temperature;
float humidity;
float pressure;
float wind_speed;
float wind_gust;
float precipitation;
String transmit_protocol = "custom_ble";

void setup_routing() {     
  server.on("/data", HTTP_GET, getData);     
  server.on("/protocol", HTTP_POST, handleProtocolChange);    
  server.on("/data", HTTP_PATCH, handlePatch);    

  server.begin();
}
 
void create_json(char *tag, float value, char *unit) {  
  jsonDocument.clear();  

  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);
}
 
void add_json_object(char *tag, float value, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["type"] = tag;
  obj["value"] = value;
  obj["unit"] = unit; 
}

void getData() {
  jsonDocument.clear();

  add_json_object("temperature", temperature, "°C");
  add_json_object("humidity", humidity, "%");
  add_json_object("pressure", pressure, "hPa");
  add_json_object("wind_speed", wind_speed, "m/s");
  add_json_object("wind_gust", wind_gust, "m/s");
  add_json_object("precipitation", precipitation, "mm");
  
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}

void handleProtocolChange() {
  String body = server.arg("plain");
  Serial.println(body);
  deserializeJson(jsonDocument, body);

  if (jsonDocument["transmit_protocol"].is<String>()) {
    String proto = jsonDocument["transmit_protocol"];
    Serial.println("Changing transmit protocol to: " + proto);
    transmit_protocol = proto;
    if (proto == "custom_ble") {
      wireless_channel->end();
      wireless_channel = &customBLEChannel;
      wireless_channel->begin(DEVICE_NAME);
    } else if (proto == "bthome") {
      wireless_channel->end();
      wireless_channel = &bthomeChannel;
      wireless_channel->begin(DEVICE_NAME);
    }
  }
  
  server.send(200, "application/json", "{}");
  displaySensors();
  transmit_data();
}

void handlePatch() {
  if (server.hasArg("plain") == false) {
  }
  String body = server.arg("plain");
  Serial.println(body);
  deserializeJson(jsonDocument, body);

  if (jsonDocument["temperature"].is<float>())
    temperature = jsonDocument["temperature"];
  if (jsonDocument["humidity"].is<float>())
    humidity = jsonDocument["humidity"];
  if (jsonDocument["pressure"].is<float>())
    pressure = jsonDocument["pressure"];
  if (jsonDocument["wind_speed"].is<float>())
    wind_speed = jsonDocument["wind_speed"];
  if (jsonDocument["wind_gust"].is<float>())
    wind_gust = jsonDocument["wind_gust"];
  if (jsonDocument["precipitation"].is<float>())
    precipitation = jsonDocument["precipitation"];
  
  server.send(200, "application/json", "{}");
  transmit_data();
  displaySensors();
}

void transmit_data() {
  Serial.println("Transmitting data...");
  wireless_channel->transmitData(temperature, humidity, pressure, wind_speed, wind_gust, precipitation);
}

void setup() {
  Serial.begin(115200);
  connectToWiFi(SSID, PWD);
  setup_routing();
  wireless_channel->begin(DEVICE_NAME);
  last_time = millis();

  tft.init();
  tft.setRotation(1);

  displaySensors();
}

void displaySensors() {
  tft.fillScreen(TFT_BLUE);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_YELLOW, TFT_BLUE);
//  tft.print("Proto: ");tft.println(transmit_protocol);
  tft.print("Temp: ");tft.println(temperature);
  tft.print("Humidity: ");tft.println(humidity);
  tft.print("Pressure: ");tft.println(pressure);
  tft.print("Wind speed: ");tft.println(wind_speed);
  tft.print("Wind gust: ");tft.println(wind_gust);
  tft.print("Precipitation: ");tft.println(precipitation);
}

void loop() {
  server.handleClient();
  unsigned long current_time = millis();
  if (current_time - last_time > BEACON_PERIOD) {
    last_time = current_time;
    transmit_data();
  }
}
