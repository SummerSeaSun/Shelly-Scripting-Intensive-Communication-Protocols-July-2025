#include <cstddef>
#include <cstdint>
#include <mbedtls/aes.h>
#include "Arduino.h"
#include "NimBLEDevice.h"
#include "CustomBLE.h"
#include "Arduino.h"

#define INPUT_LENGTH 16

using namespace std;

static BLEAdvertising *pAdvertising;

const NimBLEUUID uuid("a3e87a59-fbd2-40a5-9450-a6b1a8d67059");

void CustomBLE::begin(String dev_name, bool enable_encryption, String encryption_key) {
  BLEDevice::init("");
  pAdvertising = BLEDevice::getAdvertising();
  setDeviceName(dev_name);
  resetMeasurement();

  this->encryption_enabled = enable_encryption;
  this->encryption_key = encryption_key;
}

void CustomBLE::end() {
  stop();
  resetMeasurement();

  Serial.println("before deinit");
  BLEDevice::deinit(true);
  Serial.println("after deinit");
}

void CustomBLE::setDeviceName(String dev_name) {
  if (!dev_name.isEmpty())
    this->dev_name = dev_name;
}

void CustomBLE::resetMeasurement() {
  std::fill_n(this->sensors_data, sizeof(this->sensors_data) / sizeof(this->sensors_data[0]), 0);
  this->no_measurement = false;
}

void CustomBLE::stop() {
  pAdvertising->stop();
}

void CustomBLE::start(uint32_t duration) {
  pAdvertising->start(duration);
}

bool CustomBLE::isAdvertising() {
  return pAdvertising->isAdvertising();
}

void encrypt(const unsigned char* input, const char* key, unsigned char* output)
{
	unsigned char iv[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	mbedtls_aes_context aes;
	mbedtls_aes_init(&aes);
	mbedtls_aes_setkey_enc(&aes, (const unsigned char*)key, strlen(key) * 8);
	mbedtls_aes_crypt_cfb8(&aes, MBEDTLS_AES_ENCRYPT, 13, iv, (const unsigned char*)input, output);
	mbedtls_aes_free(&aes);
}

void CustomBLE::buildPaket() {
  // Create the BLE Device
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();

  //std::string serviceData = "";
  vector<uint8_t> serviceData;

  //serviceData += static_cast<uint8_t>(this->pid & 0xFF);
  //serviceData += static_cast<uint8_t>(this->pid >> 8);
  //this->pid++;

  for (int i = 0; i < sizeof(this->sensors_data) / sizeof(this->sensors_data[0]); i++) {
      uint8_t *bytes = (uint8_t*)&(this->sensors_data[i]);
      for (int j = 0; j < SENSORS_METADATA[i].bytes_count; j++) {
        serviceData.push_back(bytes[j]);
      }
  };

  if (encryption_enabled) {
    unsigned char serviceDataArr[serviceData.size()];
    copy(serviceData.begin(),serviceData.end(),serviceDataArr);
    unsigned char cipherOutput[serviceData.size()];
    encrypt(serviceDataArr, encryption_key.c_str(), cipherOutput);
    serviceData = vector<uint8_t>(cipherOutput, cipherOutput + serviceData.size());
    //Serial.println(serviceData.size());
  }

  oAdvertisementData.setServiceData(uuid, serviceData);
  pAdvertising->setAdvertisementData(oAdvertisementData);

  //fill the local name into oScanResponseData
  if (!this->dev_name.isEmpty()) {
    int dn_length = this->dev_name.length() + 1;
    if (dn_length > 28) dn_length = 28;//BLE_ADVERT_MAX_LEN - FLAG = 31 - 3
    oScanResponseData.setName(this->dev_name.substring(0, dn_length - 1).c_str());
  }
  pAdvertising->setScanResponseData(oScanResponseData);

  pAdvertising->setConnectableMode(0);
}

void CustomBLE::sendPacket(uint32_t delay_ms)
{
  this->buildPaket();
  if (!this->isAdvertising()) this->start();
  delay(delay_ms);
  this->resetMeasurement();
}

void CustomBLE::addMeasurement(uint8_t sensor_id, uint32_t value) {
  uint8_t size = SENSORS_METADATA[sensor_id].bytes_count;
  float factor = SENSORS_METADATA[sensor_id].factor;

  uint32_t mask = 0;
  for (int i=0; i < size * 8; i++) {
    mask = (mask << 1) | 1;
  }
  uint32_t value2 = static_cast<uint32_t>(value / factor);

  this->sensors_data[sensor_id] = value2 & mask;
}

void CustomBLE::addMeasurement(uint8_t sensor_id, float value) {
  uint8_t size = SENSORS_METADATA[sensor_id].bytes_count;
  float factor = SENSORS_METADATA[sensor_id].factor;

  uint32_t mask = 0;
  for (int i=0; i < size * 8; i++) {
    mask = (mask << 1) | 1;
  }
  uint32_t value2 = static_cast<uint32_t>(value / factor);

  this->sensors_data[sensor_id] = value2 & mask;
}
