/**
 * @file TouchListener.hpp
 * @brief Interface voor wie wil weten dat het scherm aangeraakt is.
 */
#pragma once

class TouchListener {
public:
  virtual void aangeraakt() = 0;

  virtual ~TouchListener() = default;
};
