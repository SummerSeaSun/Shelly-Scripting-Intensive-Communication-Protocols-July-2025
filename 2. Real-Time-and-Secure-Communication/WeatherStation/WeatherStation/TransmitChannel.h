#include "Arduino.h"

class TransmitChannel {
  public:
    virtual void transmitData(float temperature, float humidity, float pressure, 
      float wind_speed, float wind_gust, float precipitation)=0;
    virtual void begin(String dev_name)=0;
    virtual void end()=0;
};

class CustomBLEChannel: public TransmitChannel {
  public:
    void transmitData(float temperature, float humidity, float pressure, 
      float wind_speed, float wind_gust, float precipitation);
    void begin(String dev_name);
    void end();
};

class BTHomeChannel: public TransmitChannel {
  public:
    void transmitData(float temperature, float humidity, float pressure, 
      float wind_speed, float wind_gust, float precipitation);
    void begin(String dev_name);
    void end();
};
