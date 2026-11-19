/**
 * @file TouchListener.hpp
 * @brief Interface voor wie wil weten dat het scherm aangeraakt is.
 */
#pragma once

/**
 * @class TouchListener
 * @brief Interface voor wie wil weten dat het scherm aangeraakt is.
 */
class TouchListener {
public:
  /// @brief Wordt aangeroepen bij het begin van een aanraking.
  virtual void aangeraakt() = 0;

  virtual ~TouchListener() = default;
};
