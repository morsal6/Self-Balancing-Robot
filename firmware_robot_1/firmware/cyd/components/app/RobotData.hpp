/**
 * @file RobotData.hpp
 * @brief Entity met het laatst ontvangen geldige bericht.
 */
#pragma once

#include "Bericht.hpp"

/**
 * @class RobotData
 * @brief Laatst ontvangen geldige bericht en of dat nog actueel is.
 *
 * Wordt alleen gebruikt binnen de taak van TonenControl (ook
 * CommVerliesControl draait in die taak), dus er is geen mutex nodig.
 */
class RobotData {
private:
  Bericht laatsteBericht = {};
  bool actueel = false;

public:
  /**
   * @brief Bewaart een geldig bericht en markeert de gegevens als actueel.
   * @param bericht het ontvangen bericht
   */
  void schrijf(Bericht bericht);

  /**
   * @brief Markeert de gegevens als niet meer actueel (UC-CYD-03).
   */
  void markeerVerouderd();
};
