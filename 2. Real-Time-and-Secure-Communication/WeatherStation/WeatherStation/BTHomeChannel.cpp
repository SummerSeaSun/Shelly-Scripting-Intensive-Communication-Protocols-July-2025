#include "TransmitChannel.h"
#include "BTHome.h"
#include "Arduino.h"

BTHome bthome;

void BTHomeChannel::begin(String dev_name){
  bthome.begin(dev_name, false, "", false);
}

void BTHomeChannel::transmitData(float temperature, float humidity, float pressure, 
  float wind_speed, float wind_gust, float precipitation) {
    bthome.resetMeasurement();
    bthome.addMeasurement(ID_TEMPERATURE_PRECISE, temperature);
    bthome.addMeasurement(ID_HUMIDITY_PRECISE, humidity);
    bthome.addMeasurement(ID_PRESSURE, pressure);
    bthome.addMeasurement(ID_SPD, wind_speed);
    bthome.addMeasurement(ID_SPD, wind_gust);
    bthome.addMeasurement(ID_PRECIPITATION, precipitation);
    bthome.sendPacket();
    //delay(1000);
    //bthome.stop();
}

void BTHomeChannel::end() {
  bthome.end();
}