/**
 * @file TryReadQueue.hpp
 * @brief Hulpfunctie tryReadQueue uit "Design Like a Robot! (FreeRTOS)".
 *
 * Wordt in de STD's gebruikt als guard, bijvoorbeeld:
 * eventBitMeting [tryReadQueue(metingQueue, meting, eventGroup, eventBitMeting)]
 */
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

/**
 * @brief Leest niet-blokkerend een bericht uit een queue en trekt daarna het
 *        eventbit gelijk met de werkelijke inhoud van de queue.
 *
 * Staat er na het lezen nog iets in de queue, dan blijft het eventbit staan,
 * zodat het volgende bericht bij de volgende wait wordt opgepikt.
 *
 * @tparam T type van een element in de queue
 * @param queue de queue waaruit gelezen wordt
 * @param item hierin komt het gelezen element
 * @param eventGroup event group van de lezende taak
 * @param eventBitQueue eventbit dat bij deze queue hoort
 * @return true als er een element gelezen is
 */
template <typename T>
inline bool tryReadQueue(QueueHandle_t queue, T& item,
                         EventGroupHandle_t eventGroup,
                         EventBits_t eventBitQueue) {
  // Niet-blokkerend lezen (timeout 0 ticks).
  const bool succes = (xQueueReceive(queue, &item, 0) == pdPASS);

  // Eventbit gelijktrekken met de werkelijke queue-inhoud.
  if (uxQueueMessagesWaiting(queue) > 0) {
    xEventGroupSetBits(eventGroup, eventBitQueue);    // nog iets: aan
  } else {
    xEventGroupClearBits(eventGroup, eventBitQueue);  // leeg: uit
    if (uxQueueMessagesWaiting(queue) > 0) {          // schrijver tussendoor?
      xEventGroupSetBits(eventGroup, eventBitQueue);
    }
  }
  return succes;
}
