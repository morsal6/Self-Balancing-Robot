/**
 * @file TouchListener.hpp
 * @brief Interface voor objecten die aanrakingen van het Touchscreen ontvangen.
 */
#pragma once

/**
 * @class TouchListener
 * @brief Listener-interface van het Touchscreen.
 */
class TouchListener {
public:
  /**
   * @brief Wordt aangeroepen als het scherm wordt aangeraakt.
   */
  virtual void aangeraakt() = 0;

  virtual ~TouchListener() = default;
};
