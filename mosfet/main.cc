#include <iostream>
#include <array>
#include <functional>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/time.h"

#include "zc.hh"

#define GPIO_ZC 13 // green
#define GPIO_TRIAC 14 // yellow
#define GPIO_MOSFET 15 // orange

static int64_t mosfet_end(alarm_id_t id, void *user_data) {
  gpio_put(GPIO_MOSFET, 0);
  return 0;
}

int main() {
  bi_decl(bi_program_description("Mosfet tests"));
  bi_decl(bi_1pin_with_name(GPIO_MOSFET, "Mosfet pin"));
  bi_decl(bi_1pin_with_name(PICO_DEFAULT_LED_PIN, "On-board LED"));

  stdio_init_all();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  gpio_init(GPIO_MOSFET);
  gpio_set_dir(GPIO_MOSFET, GPIO_OUT);


  zero_crossing_detector zc(GPIO_ZC);

  int power = 50; // /255

  zc.register_callback([&]() {
    gpio_put(GPIO_MOSFET, 1);

    auto delay_us = zc.period_us() * (255 - power) / 255;
    add_alarm_in_us(delay_us, &mosfet_end, nullptr, true);
  });

  bool led_state = false;

  while(true) {
    sleep_ms(1000);

    gpio_put(PICO_DEFAULT_LED_PIN, led_state ? 1 : 0);
    led_state = !led_state;
    power = led_state ? 50 : 150;
  }
}
