#include <iostream>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#include "zc.hh"

#define GPIO_ZC 13 // green
#define GPIO_TRIAC 14 // yellow
#define GPIO_MOSFET 15 // orange

int main() {
  bi_decl(bi_program_description("Zero crossing detector tests"));
  bi_decl(bi_1pin_with_name(GPIO_ZC, "ZC pin"));
  bi_decl(bi_1pin_with_name(PICO_DEFAULT_LED_PIN, "On-board LED"));

  stdio_init_all();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  zero_crossing_detector zc(GPIO_ZC);

  absolute_time_t ABSOLUTE_TIME_INITIALIZED_VAR(last_ts, 0);
  int64_t cb_period = 0;

  zc.register_callback([&]() {
    auto now = get_absolute_time();
    cb_period = absolute_time_diff_us(last_ts, now);
    last_ts = now;
  });

  bool led_state = false;

  while(true) {
    sleep_ms(1000);

    std::cout << "period: " << zc.period_us() << " us, zero duration: " << zc.zero_duration_us() << "us, cb period: " << cb_period << "us" << std::endl;

    gpio_put(PICO_DEFAULT_LED_PIN, led_state ? 1 : 0);
    led_state = !led_state;
  }
}
