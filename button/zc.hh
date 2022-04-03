#pragma once

#include "pico/stdlib.h"

#include "callback.hh"

class zero_crossing_detector final {
public:
  zero_crossing_detector(uint gpio)
   : m_gpio(gpio) {
    m_instance = this;

    alarm_pool_init_default();

    gpio_init(m_gpio);
    gpio_set_dir(m_gpio, GPIO_IN);

    // rising_per_sec rising per second
    gpio_set_irq_enabled_with_callback(m_gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &s_irq_handler);
  }

  uint32_t period_us() const {
    return m_period;
  }

  uint32_t zero_duration_us() const {
    return m_zero_duration;
  }

  void register_callback(std::function<void()> &&callback) {
    m_callback.add(std::move(callback));
  }

private:
  void irq_handler(uint gpio, uint32_t events) {
    if (gpio != m_gpio) {
      return;
    }
    
    auto now = get_absolute_time();
    auto elapsed = absolute_time_diff_us(m_last_event, now);
    m_last_event = now;

    switch(events) {
    case GPIO_IRQ_EDGE_RISE:
      m_period = m_zero_duration + elapsed;

      // middle of zero
      add_alarm_in_us(m_zero_duration / 2, s_alarm_trigger, this, true);
      break;

    case GPIO_IRQ_EDGE_FALL:
      m_zero_duration = elapsed;
      break;
    }
  }

  void alarm_trigger() {
    m_callback.call();
  }

  static void s_irq_handler(uint gpio, uint32_t events) {
    m_instance->irq_handler(gpio, events);
  }

  static int64_t s_alarm_trigger(alarm_id_t id, void *user_data) {
    auto instance = reinterpret_cast<zero_crossing_detector *>(user_data);
    instance->alarm_trigger();
    return 0;
  }

  // this is ugly but we cannot pass args to irq handler
  static zero_crossing_detector *m_instance;

  uint m_gpio;
  absolute_time_t ABSOLUTE_TIME_INITIALIZED_VAR(m_last_event, 0);
  uint32_t m_period = 0;
  uint32_t m_zero_duration = 0;

  callback_manager<void()> m_callback;
};
