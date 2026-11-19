/**
 * @file EspNowZender.cpp
 * @brief Implementatie van EspNowZender.
 */
#include "EspNowZender.hpp"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include <cstring>

namespace {
const char* TAG = "EspNowZender";

bool vorigeAangekomen = false;

void verzondenCallback(const uint8_t* mac, esp_now_send_status_t status) {
  (void)mac;
  const bool aangekomen = (status == ESP_NOW_SEND_SUCCESS);
  if (aangekomen != vorigeAangekomen) {
    if (aangekomen) {
      ESP_LOGI(TAG, "CYD bereikbaar");
    } else {
      ESP_LOGW(TAG, "CYD niet bereikbaar (staat hij aan, klopt CYD_MAC?)");
    }
    vorigeAangekomen = aangekomen;
  }
}
}

EspNowZender::EspNowZender() {
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
}

void EspNowZender::configureer() {
  if (ESP_ERROR_CHECK_WITHOUT_ABORT(esp_now_init()) != ESP_OK) {
    return;
  }
  esp_now_register_send_cb(verzondenCallback);
  esp_now_peer_info_t peer = {};
  std::memcpy(peer.peer_addr, CYD_MAC, ESP_NOW_ETH_ALEN);
  peer.channel = ESPNOW_KANAAL;
  peer.ifidx = WIFI_IF_STA;
  peer.encrypt = false;
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_now_add_peer(&peer));
}

void EspNowZender::verstuur(Bericht bericht) {
  // Mislukt het, dan is dit bericht weg; de volgende komt na 50 ms.
  esp_now_send(CYD_MAC, reinterpret_cast<const uint8_t*>(&bericht), sizeof(bericht));
}
