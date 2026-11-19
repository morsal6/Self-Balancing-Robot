/**
 * @file EspNowOntvanger.hpp
 * @brief Ontvangt berichten van de S3 via ESP-NOW.
 */
#pragma once

#include "Bericht.hpp"
#include "BerichtListener.hpp"

class EspNowOntvanger {
private:
  BerichtListener* listener = nullptr;

public:
  void init();

  void addListener(BerichtListener* listener);
};
