#include "Arduino.h"

struct SensorMetadata {
    String name;
    uint8_t bytes_count;
    float factor;
    String unit;
};

const SensorMetadata SENSORS_METADATA[] = {
    {"temperature", 2, 0.01, "C"},
    {"humidity", 2, 0.01, "%"},
    {"pressure", 3, 0.01, "hPa"},
    {"wind_speed", 2, 0.01, "m/s"},
    {"wind_gust", 2, 0.01, "m/s"},
    {"precipitation", 2, 0.1, "mm/h"},
};

#define ID_TEMPERATURE 0x00
#define ID_HUMIDITY 0x01
#define ID_PRESSURE 0x02
#define ID_WIND_SPEED 0x03
#define ID_WIND_GUST 0x04
#define ID_PRECIPITATION 0x05

class CustomBLE {
  public:
    String encryption_key;

    void begin(String dev_name = "TheWeatherStation", bool enable_encryption=false, String encryption_key="");
    void end();
    void setDeviceName(String dev_name = "");
    void buildPaket();
    void start(uint32_t duration = 0);
    void stop();
    bool isAdvertising();
    void resetMeasurement();
    void sendPacket(uint32_t delay_ms = 1500);
    void addMeasurement(uint8_t sensor_id, uint32_t value);
    void addMeasurement(uint8_t sensor_id, float value);

  private:
    static void encrypt(const unsigned char* input, const char* key, unsigned char* output);

    bool encryption_enabled = false;
    String dev_name;
    bool no_measurement = true;
    uint32_t sensors_data[sizeof(SENSORS_METADATA) / sizeof(SensorMetadata)];
};
