#include "Arduino.h"
#include "LoRaWan_APP.h"

using namespace std;

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

#define LORA_BUFFER_SIZE 100

class LoRa {
  public:
    void begin(String dev_name = "TheWeatherStation", bool enable_encryption=false, String encryption_key="");
    void end();
    void setDeviceName(String dev_name = "");
    void resetMeasurement();
    void sendPacket();
    void addMeasurement(uint8_t sensor_id, uint32_t value);
    void addMeasurement(uint8_t sensor_id, float value);
    void tick();

    bool encryption_enabled = false;
  private:
    vector<uint8_t> buildPacket();
    static void encrypt(const unsigned char* input, uint8_t input_size, const uint8_t* key, uint8_t key_size, unsigned char* output);

    uint8_t *encryption_key;
    uint8_t encryption_key_size;
    String dev_name;
    bool no_measurement = true;
    uint32_t sensors_data[sizeof(SENSORS_METADATA) / sizeof(SensorMetadata)];
};
