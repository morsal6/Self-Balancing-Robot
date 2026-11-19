/**
 * @file TryReadQueue.hpp
 * @brief tryReadQueue() uit de FreeRTOS-reader.
 */
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

/// @brief Leest een item als het er is en houdt het eventbit gelijk met de queue.
template <typename T>
inline bool tryReadQueue(QueueHandle_t queue, T& item,
                         EventGroupHandle_t eventGroup,
                         EventBits_t eventBitQueue) {
  const bool succes = (xQueueReceive(queue, &item, 0) == pdPASS);

  if (uxQueueMessagesWaiting(queue) > 0) {
    xEventGroupSetBits(eventGroup, eventBitQueue);
  } else {
    xEventGroupClearBits(eventGroup, eventBitQueue);
    if (uxQueueMessagesWaiting(queue) > 0) {
      xEventGroupSetBits(eventGroup, eventBitQueue);
    }
  }
  return succes;
}
