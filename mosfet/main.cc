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

int main() {
  bi_decl(bi_program_description("Mosfet tests"));
  bi_decl(bi_1pin_with_name(GPIO_MOSFET, "Mosfet pin"));
  bi_decl(bi_1pin_with_name(PICO_DEFAULT_LED_PIN, "On-board LED"));

  stdio_init_all();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  gpio_init(GPIO_MOSFET);
  gpio_set_dir(GPIO_MOSFET, GPIO_OUT);

  bool led_state = false;

  while(true) {
    sleep_ms(1000);

    gpio_put(PICO_DEFAULT_LED_PIN, led_state ? 1 : 0);
    gpio_put(GPIO_MOSFET, led_state ? 1 : 0);
    led_state = !led_state;
  }
}
