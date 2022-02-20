#include <iostream>
#include <array>
#include <functional>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/time.h"

#define GPIO_ZC 13 // green
#define GPIO_TRIAC 14 // yellow
#define GPIO_MOSFET 15 // orange

template<typename... X>
class callback_manager;

template<typename... Args>
class callback_manager<void(Args...)> final {
public:
  void add(std::function<void(Args...)> &&callback) {
    m_callbacks.emplace_back(std::move(callback));
  }

  void call(Args... args) {
    for (const auto &callback : m_callbacks) {
      callback(args...);
    }
  }

 private:
  std::vector<std::function<void(Args...)>> m_callbacks;
};

class zero_crossing_detector final {
public:
  zero_crossing_detector(uint gpio) {
    m_instance = this;

    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);

    // rising_per_sec rising per second
    gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &s_irq_handler);
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
  void irq_handler(uint32_t events) {
    auto now = get_absolute_time();
    auto elapsed = absolute_time_diff_us(m_last_event, now);
    m_last_event = now;

    switch(events) {
    case GPIO_IRQ_EDGE_RISE:
      m_period = m_zero_duration + elapsed;
      break;

    case GPIO_IRQ_EDGE_FALL:
      m_zero_duration = elapsed;
      break;
    }
  }

  static void s_irq_handler(uint gpio, uint32_t events) {
    m_instance->irq_handler(events);
  }

  // this is ugly but we cannot pass args to irq handler
  static zero_crossing_detector *m_instance;

  absolute_time_t ABSOLUTE_TIME_INITIALIZED_VAR(m_last_event, 0);
  uint32_t m_period = 0;
  uint32_t m_zero_duration = 0;

  callback_manager<void()> m_callback;
};

zero_crossing_detector *zero_crossing_detector::m_instance = nullptr;

int main() {
  bi_decl(bi_program_description("Zero crossing detector tests"));
  bi_decl(bi_1pin_with_name(GPIO_ZC, "ZC pin"));
  bi_decl(bi_1pin_with_name(PICO_DEFAULT_LED_PIN, "On-board LED"));

  stdio_init_all();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  zero_crossing_detector zc(GPIO_ZC);

  bool led_state = false;

  while(true) {
    sleep_ms(1000);

    std::cout << "period: " << zc.period_us() << " us, zero duration: " << zc.zero_duration_us() << std::endl;

    gpio_put(PICO_DEFAULT_LED_PIN, led_state ? 1 : 0);
    led_state = !led_state;
  }
}
