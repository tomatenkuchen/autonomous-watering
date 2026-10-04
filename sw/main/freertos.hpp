/**
 * @file freertos.hpp
 * @brief FreeRTOS wrapper functions
 */

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/idf_additions.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <chrono>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>

namespace freertos {

inline auto delay(std::chrono::milliseconds duration) -> void {
  vTaskDelay(pdMS_TO_TICKS(duration.count()));
}

template <typename T> class Queue {
public:
  Queue(std::size_t queue_length)
      : queue_handle(xQueueCreate(queue_length, sizeof(T))) {}

  ~Queue() {
    if (queue_handle) {
      vQueueDelete(queue_handle);
    }
  }

  auto send(T const &item, std::chrono::milliseconds timeout) -> bool {
    return xQueueSend(queue_handle, item, pdMS_TO_TICKS(timeout.count())) ==
           pdTRUE;
  }

  auto receive(std::chrono::milliseconds timeout) -> std::optional<T> {
    T item;
    if (xQueueReceive(queue_handle, &item, pdMS_TO_TICKS(timeout.count())) ==
        pdTRUE) {
      return item;
    }
    return std::nullopt;
  }

private:
  QueueHandle_t queue_handle;
};

class Task {
public:
  Task(std::string const &name, std::size_t stack_size, std::uint8_t priority,
       std::function<void()> handler, std::uint32_t core_id = tskNO_AFFINITY)
      : handler(handler) {
    BaseType_t result =
        xTaskCreatePinnedToCore(task_function, name.c_str(), stack_size, this,
                                priority, &task_handle, core_id);
    if (result != pdPASS) {
      task_handle = nullptr;
    }
  }
  ~Task() {
    if (task_handle) {
      vTaskDelete(task_handle);
    }
  }

  auto is_valid() const -> bool { return task_handle != nullptr; }

private:
  TaskHandle_t task_handle = nullptr;
  std::function<void()> handler;

  static void task_function(void *parameters) {
    Task *task = static_cast<Task *>(parameters);
    if (task && task->handler) {
      task->handler();
    }
    vTaskDelete(nullptr);
  }
};

class Semaphore {
public:
  Semaphore() { mutex = xSemaphoreCreateMutex(); }
  ~Semaphore() { vSemaphoreDelete(mutex); }

  SemaphoreHandle_t mutex;
};

class Lock {
public:
  Lock(SemaphoreHandle_t mutex, std::chrono::milliseconds timeout)
      : mutex(mutex) {
    if (!mutex) {
      ESP_LOGE("Lock", "Mutex handle is null");
    } else {
      xSemaphoreTake(mutex, pdMS_TO_TICKS(timeout.count()));
    }
  }

  ~Lock() { xSemaphoreGive(mutex); }

private:
  SemaphoreHandle_t mutex;
};

class EventGroup {
public:
  EventGroup() { event_group = xEventGroupCreate(); }
  ~EventGroup() { vEventGroupDelete(event_group); }

  auto set_bits(EventBits_t bits) -> void {
    xEventGroupSetBits(event_group, bits);
  }

  auto clear_bits(EventBits_t bits) -> void {
    xEventGroupClearBits(event_group, bits);
  }

  auto wait_bits(EventBits_t bits, bool clear_on_exit, bool wait_for_all,
                 std::chrono::milliseconds timeout) -> EventBits_t {
    return xEventGroupWaitBits(event_group, bits, clear_on_exit, wait_for_all,
                               pdMS_TO_TICKS(timeout.count()));
  }

private:
  EventGroupHandle_t event_group;
};

} // namespace freertos
