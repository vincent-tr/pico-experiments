#include <iostream>
#include <array>
#include <functional>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#define GPIO_ZC 13 // green
#define GPIO_TRIAC 14 // yellow
#define GPIO_MOSFET 15 // orange

class sampling final {
  static constexpr std::size_t size = 20;
public:
  void add(int64_t value) {
    m_current = (m_current+1) % size;
    m_values[m_current] = value;
  }

  int64_t average() const {
    int64_t sum = 0;
    for (const auto &value : m_values) {
      sum += value;
    }

    return sum / size;
  }

private:
  std::size_t m_current;
  std::array<int64_t, size> m_values = {0};
};

static volatile uint32_t rising_per_sec;
static absolute_time_t last_ts;
static sampling sampling_low;
static sampling sampling_hi;

static void gpio_cb(uint gpio, uint32_t events) {
  auto now = get_absolute_time();
  auto elapsed = absolute_time_diff_us(last_ts, now);
  last_ts = now;

  if (events == GPIO_IRQ_EDGE_RISE) {
    ++rising_per_sec;

    sampling_low.add(elapsed);
  } else {
    sampling_hi.add(elapsed);
  }
}

int main() {
  bi_decl(bi_program_description("Zero crossing detector tests"));
  bi_decl(bi_1pin_with_name(GPIO_ZC, "ZC pin"));
  bi_decl(bi_1pin_with_name(PICO_DEFAULT_LED_PIN, "On-board LED"));

  stdio_init_all();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  gpio_init(GPIO_ZC);
  gpio_set_dir(GPIO_ZC, GPIO_IN);

  // rising_per_sec rising per second
  gpio_set_irq_enabled_with_callback(GPIO_ZC, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_cb);

  bool led_state = false;

  while(true) {
    sleep_ms(1000);

    auto value = rising_per_sec;
    rising_per_sec = 0;

    std::cout << "rising_per_sec: " << value << " Hz, low: " << sampling_low.average() << " us, hi: " << sampling_hi.average() << std::endl;

    gpio_put(PICO_DEFAULT_LED_PIN, led_state ? 1 : 0);
    led_state = !led_state;
  }
}
