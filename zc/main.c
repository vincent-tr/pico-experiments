#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#define GPIO_ZC 13 // green
#define GPIO_TRIAC 14 // yellow
#define GPIO_MOSFET 15 // orange

int main() {
  bi_decl(bi_program_description("Zero crossing detector tests"));
  bi_decl(bi_1pin_with_name(GPIO_ZC, "ZC pin"));

  gpio_init(GPIO_ZC);
  gpio_set_dir(GPIO_ZC, GPIO_IN);

  while(true) {

  }
}
