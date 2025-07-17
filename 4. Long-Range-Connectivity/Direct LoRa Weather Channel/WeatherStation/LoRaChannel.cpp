#include "TransmitChannel.h"
#include "LoRa.h"
#include "Arduino.h"

LoRa lora;

void LoRaChannel::begin(String dev_name){
  lora.begin(dev_name, false, "dd469421e5f4089a1418ea24ba37c61bdd469421e5f4089a1418ea24ba37c61b");
}

void LoRaChannel::transmitData(float temperature, float humidity, float pressure, 
  float wind_speed, float wind_gust, float precipitation) {
    lora.resetMeasurement();
    lora.addMeasurement(ID_TEMPERATURE, temperature);
    lora.addMeasurement(ID_HUMIDITY, humidity);
    lora.addMeasurement(ID_PRESSURE, pressure);
    lora.addMeasurement(ID_WIND_SPEED, wind_speed);
    lora.addMeasurement(ID_WIND_GUST, wind_gust);
    lora.addMeasurement(ID_PRECIPITATION, precipitation);
    lora.sendPacket();
}

void LoRaChannel::end() {
  lora.end();
}

void LoRaChannel::tick() {
  lora.tick();
}

void LoRaChannel::setEncryption(bool encrypt) {
  lora.encryption_enabled = encrypt;
}