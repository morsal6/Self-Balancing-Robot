/**
 * @file Mpu6050.cpp
 * @brief Implementatie van het STD van Mpu6050.
 */
#include "Mpu6050.hpp"

#include "Ontwerpparameters.hpp"

#include "esp_log.h"

#include <cstdint>

namespace {
const char* TAG = "Mpu6050";

constexpr uint16_t I2C_ADRES = 0x68;
constexpr uint32_t I2C_SNELHEID_HZ = 100000;  // 100 kHz, minder storingsgevoelig dan 400 kHz
constexpr int I2C_TIMEOUT_MS = 5;             // een uitlezing duurt ca. 1,6 ms
// Mislukken alle pogingen door een timeout, dan duurt leesSensor() 15 ms in
// plaats van ca. 1,6 ms; xTaskDelayUntil haalt die achterstand daarna in.
constexpr int LEESPOGINGEN = 3;

// Registers van de MPU6050
constexpr uint8_t REG_SMPLRT_DIV = 0x19;
constexpr uint8_t REG_CONFIG = 0x1A;
constexpr uint8_t REG_GYRO_CONFIG = 0x1B;
constexpr uint8_t REG_ACCEL_CONFIG = 0x1C;
constexpr uint8_t REG_ACCEL_XOUT_H = 0x3B;
constexpr uint8_t REG_PWR_MGMT_1 = 0x6B;
constexpr uint8_t REG_WHO_AM_I = 0x75;

constexpr float LSB_PER_G = 16384.0f;     // bij +-2 g
constexpr float LSB_PER_GRAD_S = 131.0f;  // bij +-250 graden/s

/// Of de vorige uitlezing gelukt is; alleen om de foutmelding één keer te loggen.
bool vorigeGelukt = true;

/// Schrijft één register van de MPU6050.
esp_err_t schrijfRegister(i2c_master_dev_handle_t apparaat, uint8_t reg, uint8_t waarde) {
  const uint8_t data[2] = {reg, waarde};
  return i2c_master_transmit(apparaat, data, sizeof(data), I2C_TIMEOUT_MS);
}

/// Zet twee bytes (big endian) om naar een int16_t.
int16_t naarInt16(const uint8_t* data) {
  return static_cast<int16_t>((data[0] << 8) | data[1]);
}
}  // namespace

Mpu6050::Mpu6050(const char* taskName, UBaseType_t taskPriority,
                 uint32_t taskStackSizeBytes, BaseType_t taskCoreNumber,
                 gpio_num_t sdaPin, gpio_num_t sclPin) {
  eventGroup = xEventGroupCreate();
  configASSERT(eventGroup != NULL);

  // I2C-bus en apparaat
  i2c_master_bus_config_t busConfig = {};
  busConfig.i2c_port = I2C_NUM_0;
  busConfig.sda_io_num = sdaPin;
  busConfig.scl_io_num = sclPin;
  busConfig.clk_source = I2C_CLK_SRC_DEFAULT;
  busConfig.glitch_ignore_cnt = 7;
  busConfig.flags.enable_internal_pullup = true;
  i2c_master_bus_handle_t bus = nullptr;
  ESP_ERROR_CHECK(i2c_new_master_bus(&busConfig, &bus));

  i2c_device_config_t apparaatConfig = {};
  apparaatConfig.dev_addr_length = I2C_ADDR_BIT_LEN_7;
  apparaatConfig.device_address = I2C_ADRES;
  apparaatConfig.scl_speed_hz = I2C_SNELHEID_HZ;
  ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &apparaatConfig, &apparaat));

  // Sensor wakker maken en instellen.
  esp_err_t err = schrijfRegister(apparaat, REG_PWR_MGMT_1, 0x01);  // klok: PLL gyro X
  if (err == ESP_OK) err = schrijfRegister(apparaat, REG_SMPLRT_DIV, 4);    // 1 kHz / 5 = 200 Hz
  if (err == ESP_OK) err = schrijfRegister(apparaat, REG_CONFIG, 0x03);     // laagdoorlaatfilter 44 Hz
  if (err == ESP_OK) err = schrijfRegister(apparaat, REG_GYRO_CONFIG, 0x00);   // +-250 graden/s
  if (err == ESP_OK) err = schrijfRegister(apparaat, REG_ACCEL_CONFIG, 0x00);  // +-2 g

  if (err == ESP_OK) {
    uint8_t whoAmI = 0;
    const uint8_t reg = REG_WHO_AM_I;
    i2c_master_transmit_receive(apparaat, &reg, 1, &whoAmI, 1, I2C_TIMEOUT_MS);
    ESP_LOGI(TAG, "MPU6050 ingesteld, WHO_AM_I = 0x%02X", whoAmI);
  } else {
    // Een niet ingestelde sensor mag geen metingen leveren: zonder apparaat
    // geeft leesSensor() alleen ongeldige metingen, en dat is een sensorfout
    // (UC-S3-05).
    ESP_LOGE(TAG, "MPU6050 instellen mislukt (%s). Controleer SDA/SCL en voeding.",
             esp_err_to_name(err));
    i2c_master_bus_rm_device(apparaat);
    apparaat = nullptr;
  }

  const BaseType_t gestart = xTaskCreatePinnedToCore(taskWrapper, taskName, taskStackSizeBytes,
                                                     this, taskPriority, &taskHandle,
                                                     taskCoreNumber);
  configASSERT(gestart == pdPASS);
}

void Mpu6050::start() {
  xEventGroupSetBits(eventGroup, eventBitStart);
}

void Mpu6050::addListener(MetingListener* listener) {
  this->listener = listener;
}

Meting Mpu6050::leesSensor() {
  Meting meting = {};  // geldig = false
  if (apparaat == nullptr) {
    return meting;
  }

  // Een losse storing (bijvoorbeeld van de motoren) is nog geen sensorfout:
  // pas als alle pogingen mislukken, is de meting ongeldig.
  uint8_t data[14];
  const uint8_t reg = REG_ACCEL_XOUT_H;
  esp_err_t err = ESP_FAIL;
  for (int poging = 0; poging < LEESPOGINGEN && err != ESP_OK; poging++) {
    err = i2c_master_transmit_receive(apparaat, &reg, 1, data, sizeof(data), I2C_TIMEOUT_MS);
  }

  meting.geldig = (err == ESP_OK);
  if (!meting.geldig && vorigeGelukt) {
    ESP_LOGE(TAG, "Uitlezen mislukt (%s)", esp_err_to_name(err));
  }
  vorigeGelukt = meting.geldig;

  if (meting.geldig) {
    meting.ax = naarInt16(&data[0]) / LSB_PER_G;
    meting.ay = naarInt16(&data[2]) / LSB_PER_G;
    meting.az = naarInt16(&data[4]) / LSB_PER_G;
    // data[6..7] is de temperatuur
    meting.gx = naarInt16(&data[8]) / LSB_PER_GRAD_S;
    meting.gy = naarInt16(&data[10]) / LSB_PER_GRAD_S - GY_OFFSET;
    meting.gz = naarInt16(&data[12]) / LSB_PER_GRAD_S;
  }
  return meting;
}

void Mpu6050::taskWrapper(void* pParam) {
  static_cast<Mpu6050*>(pParam)->main();
}

void Mpu6050::main() {
  Meting meting;
  TickType_t vorigeWektijd = 0;

  while (true) {
    switch (state) {
      case State::Uit:
        xEventGroupWaitBits(eventGroup, eventBitStart, pdTRUE, pdFALSE, portMAX_DELAY);
        vorigeWektijd = xTaskGetTickCount();  // de after(REGELPERIODE) telt vanaf hier
        state = State::Meten;
        break;

      case State::Meten:
        // after(REGELPERIODE). Meten wacht nergens anders op, dus geen software
        // timer nodig. xTaskDelayUntil wekt op vaste momenten, zodat de
        // periode ook gelijk blijft als het uitlezen wat tijd kost.
        xTaskDelayUntil(&vorigeWektijd, pdMS_TO_TICKS(REGELPERIODE));
        meting = leesSensor();
        if (listener != nullptr) {
          listener->metingGedaan(meting);
        }
        state = State::Meten;
        break;
    }
  }
}
