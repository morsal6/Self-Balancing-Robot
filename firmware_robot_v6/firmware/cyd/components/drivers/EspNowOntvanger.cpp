/**
 * @file EspNowOntvanger.cpp
 * @brief Implementatie van EspNowOntvanger.
 */
#include "EspNowOntvanger.hpp"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include <cstring>

namespace {
const char* TAG = "EspNowOntvanger";

BerichtListener* ontvangListener = nullptr;

// Draait in de WiFi-taak, dus alleen doorgeven.
void ontvangCallback(const esp_now_recv_info_t* info, const uint8_t* data, int lengte) {
  (void)info;

  if (lengte != static_cast<int>(sizeof(Bericht)) || ontvangListener == nullptr) {
    return;
  }
  Bericht bericht;
  std::memcpy(&bericht, data, sizeof(bericht));
  ontvangListener->berichtOntvangen(bericht);
}
}

void EspNowOntvanger::init() {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  wifi_init_config_t wifiConfig = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifiConfig));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_channel(ESPNOW_KANAAL, WIFI_SECOND_CHAN_NONE));
  // Zonder energiebesparing mist de CYD minder berichten.
  ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

  ontvangListener = listener;
  ESP_ERROR_CHECK(esp_now_init());
  ESP_ERROR_CHECK(esp_now_register_recv_cb(ontvangCallback));
  uint8_t mac[6] = {};
  esp_wifi_get_mac(WIFI_IF_STA, mac);
  ESP_LOGI(TAG, "ESP-NOW ontvangt op kanaal %d (MAC CYD %02X:%02X:%02X:%02X:%02X:%02X)",
           ESPNOW_KANAAL, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void EspNowOntvanger::addListener(BerichtListener* listener) {
  this->listener = listener;
}
