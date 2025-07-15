#include "TransmitChannel.h"
#include "CustomBLE.h"
#include "Arduino.h"

CustomBLE custom_ble;

void CustomBLEChannel::begin(String dev_name){
  custom_ble.begin(dev_name, false, "dd469421e5f4089a1418ea24ba37c61bdd469421e5f4089a1418ea24ba37c61b");
}

void CustomBLEChannel::transmitData(float temperature, float humidity, float pressure, 
  float wind_speed, float wind_gust, float precipitation) {
    custom_ble.resetMeasurement();
    custom_ble.addMeasurement(ID_TEMPERATURE, temperature);
    custom_ble.addMeasurement(ID_HUMIDITY, humidity);
    custom_ble.addMeasurement(ID_PRESSURE, pressure);
    custom_ble.addMeasurement(ID_WIND_SPEED, wind_speed);
    custom_ble.addMeasurement(ID_WIND_GUST, wind_gust);
    custom_ble.addMeasurement(ID_PRECIPITATION, precipitation);
    custom_ble.sendPacket();
    //delay(1000);
    //custom_ble.stop();
}

void CustomBLEChannel::end() {
  custom_ble.end();
}