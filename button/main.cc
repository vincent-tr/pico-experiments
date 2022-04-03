#include <iostream>
#include <array>
#include <functional>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/time.h"

#include "zc.hh"

#define GPIO_ZC 13 // green
#define GPIO_BUTTON 14 // yellow
#define GPIO_MOSFET 15 // orange

static int64_t check_button(alarm_id_t id, void *user_data) {
  bool &value = *(reinterpret_cast<bool *>(user_data));
  value = !gpio_get(GPIO_BUTTON); // level is reversed (like ZC detector to be high at ZC)

  return 0;
}

int main() {
  bi_decl(bi_program_description("Button tests"));
  bi_decl(bi_1pin_with_name(GPIO_ZC, "ZC pin"));
  bi_decl(bi_1pin_with_name(GPIO_BUTTON, "Button pin"));
  bi_decl(bi_1pin_with_name(PICO_DEFAULT_LED_PIN, "On-board LED"));

  stdio_init_all();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  gpio_init(GPIO_BUTTON);
  gpio_set_dir(GPIO_BUTTON, GPIO_IN);

  // signal reboot
  for (int i=0; i<5; ++i) {
    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    sleep_ms(100);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    sleep_ms(100);
  }

  zero_crossing_detector zc(GPIO_ZC);

  bool value = false;

  zc.register_callback([&]() {
    auto period = zc.period_us();
    if (period > 0) {
      // check at half-period (should be higher level possible)
      add_alarm_in_us(period / 2, &check_button, &value, true);
    }
  });

  while(true) {
    sleep_ms(10);

    gpio_put(PICO_DEFAULT_LED_PIN, value ? 1 : 0);
  }
}
