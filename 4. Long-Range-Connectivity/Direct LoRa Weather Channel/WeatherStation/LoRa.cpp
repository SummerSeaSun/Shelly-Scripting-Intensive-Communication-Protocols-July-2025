#include <cstring>
#include "Arduino.h"
#include "mbedtls/aes.h"
#include "NimBLEDevice.h"
#include "LoRaWan_APP.h"
#include "LoRa.h"
#include "Config.h"

using namespace std;

uint8_t txpacket[LORA_BUFFER_SIZE];
uint8_t rxpacket[LORA_BUFFER_SIZE];

double txNumber;

bool lora_idle=true;

static RadioEvents_t RadioEvents;
void OnTxDone( void );
void OnTxTimeout( void );

uint8_t key_bytes[32];

void HexToBytes(String hex) {
  int idx = 0;
  for (unsigned int i = 0; i < hex.length(); i += 2) {
    String byteString = hex.substring(i, i + 2);
    uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
    key_bytes[idx++] = byte;
  }
}

void LoRa::begin(String dev_name, bool enable_encryption, String encryption_key) {
  Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);
  
  txNumber=0;

  RadioEvents.TxDone = OnTxDone;
  RadioEvents.TxTimeout = OnTxTimeout;

  Radio.Init( &RadioEvents );
  Radio.SetChannel( RF_FREQUENCY );
  Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                  LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                  LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                  true, 0, 0, LORA_IQ_INVERSION_ON, 3000 ); 
  setDeviceName(dev_name);
  resetMeasurement();

  this->encryption_enabled = enable_encryption;
  HexToBytes(encryption_key);
  this->encryption_key = key_bytes;
  this->encryption_key_size = encryption_key.length() / 2;
}

void LoRa::end() {
  resetMeasurement();
}

void LoRa::setDeviceName(String dev_name) {
  if (!dev_name.isEmpty())
    this->dev_name = dev_name;
}

void LoRa::resetMeasurement() {
  std::fill_n(this->sensors_data, sizeof(this->sensors_data) / sizeof(this->sensors_data[0]), 0);
  this->no_measurement = false;
}

void LoRa::encrypt(const unsigned char* input, uint8_t input_size, const uint8_t* key, uint8_t key_size, unsigned char* output)
{
	unsigned char iv[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	mbedtls_aes_context aes;
	mbedtls_aes_init(&aes);
	//mbedtls_aes_setkey_enc(&aes, (const unsigned char*)key, (int)key_size * 8);
  mbedtls_aes_setkey_enc(&aes, key, (int)key_size * 8);
	mbedtls_aes_crypt_cfb8(&aes, MBEDTLS_AES_ENCRYPT, input_size, iv, (const unsigned char*)input, output);
  //mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*)input, output);

	mbedtls_aes_free(&aes);
}

vector<uint8_t> LoRa::buildPacket() {
  vector<uint8_t> data;

  for (int i = 0; i < sizeof(this->sensors_data) / sizeof(this->sensors_data[0]); i++) {
      uint8_t *bytes = (uint8_t*)&(this->sensors_data[i]);
      for (int j = 0; j < SENSORS_METADATA[i].bytes_count; j++) {
        data.push_back(bytes[j]);
      }
  };

  if (encryption_enabled) {
    unsigned char dataArr[data.size()];
    copy(data.begin(),data.end(),dataArr);
    unsigned char cipherOutput[data.size()];
    //unsigned char dataArr[16];
    //copy(data.begin(),data.end(),dataArr);
    //unsigned char cipherOutput[16];

    LoRa::encrypt(dataArr, data.size(), encryption_key, encryption_key_size, cipherOutput);
    data = vector<uint8_t>(cipherOutput, cipherOutput + data.size());
    //data = vector<uint8_t>(cipherOutput, cipherOutput + 16);
    //Serial.println(data.size());
  }
  return data;
}

void LoRa::sendPacket()
{
  // send only when lora is idle
  if (lora_idle) {
    vector<uint8_t> data = this->buildPacket();
    copy(data.begin(), data.end(), txpacket);
    Radio.Send( (uint8_t *)txpacket, data.size() ); //send the package out
    this->resetMeasurement();
  }
}

void LoRa::tick() {
  Radio.IrqProcess();
}

void LoRa::addMeasurement(uint8_t sensor_id, uint32_t value) {
  uint8_t size = SENSORS_METADATA[sensor_id].bytes_count;
  float factor = SENSORS_METADATA[sensor_id].factor;

  uint32_t mask = 0;
  for (int i=0; i < size * 8; i++) {
    mask = (mask << 1) | 1;
  }
  uint32_t value2 = static_cast<uint32_t>(value / factor);

  this->sensors_data[sensor_id] = value2 & mask;
}

void LoRa::addMeasurement(uint8_t sensor_id, float value) {
  uint8_t size = SENSORS_METADATA[sensor_id].bytes_count;
  float factor = SENSORS_METADATA[sensor_id].factor;

  uint32_t mask = 0;
  for (int i=0; i < size * 8; i++) {
    mask = (mask << 1) | 1;
  }
  uint32_t value2 = static_cast<uint32_t>(value / factor);

  this->sensors_data[sensor_id] = value2 & mask;
}

void OnTxDone( void )
{
	Serial.println("TX done......");
	lora_idle = true;
}

void OnTxTimeout( void )
{
    Radio.Sleep( );
    Serial.println("TX Timeout......");
    lora_idle = true;
}