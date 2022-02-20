#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#include "zc.hh"

#define GPIO_ZC 13 // green
#define GPIO_TRIAC 14 // yellow
#define GPIO_MOSFET 15 // orange

// from esphome
/// Time in microseconds the gate should be held high
/// 10µs should be long enough for most triacs
/// For reference: BT136 datasheet says 2µs nominal (page 7)
/// However other factors like gate driver propagation time
/// are also considered and a really low value is not important
/// See also: https://github.com/esphome/issues/issues/1632
static const uint32_t GATE_ENABLE_TIME = 50;

static int64_t end_triac(alarm_id_t id, void *user_data) {
  gpio_put(GPIO_TRIAC, 0);
  return 0;
}

static int64_t trigger_triac(alarm_id_t id, void *user_data) {
  gpio_put(GPIO_TRIAC, 1);
  add_alarm_in_us(GATE_ENABLE_TIME, &end_triac, nullptr, true);
  return 0;
}

int main() {
  bi_decl(bi_program_description("Triac tests"));
  bi_decl(bi_1pin_with_name(GPIO_TRIAC, "Triac pin"));
  bi_decl(bi_1pin_with_name(PICO_DEFAULT_LED_PIN, "On-board LED"));

  stdio_init_all();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  gpio_init(GPIO_TRIAC);
  gpio_set_dir(GPIO_TRIAC, GPIO_OUT);

  zero_crossing_detector zc(GPIO_ZC);

  int power = 50; // /255

  zc.register_callback([&]() {
    // gpio_put(GPIO_TRIAC, 0);

    auto delay_us = zc.period_us() * (255 - power) / 255;
    add_alarm_in_us(delay_us, &trigger_triac, nullptr, true);
  });

  bool led_state = false;

  while(true) {
    sleep_ms(1000);

    gpio_put(PICO_DEFAULT_LED_PIN, led_state ? 1 : 0);
    led_state = !led_state;
    power = led_state ? 50 : 150;
  }
}
