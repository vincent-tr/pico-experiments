#include <iostream>
#include <array>
#include <functional>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/binary_info.h"
#include "pico/time.h"

int main() {
  bi_decl(bi_program_description("Internal temp tests"));

  stdio_init_all();

  adc_init();

  while(true) {
    sleep_ms(1000);

    adc_select_input(4);

    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    const float conversion_factor = 3.3f / (1 << 12);
    uint16_t result = adc_read();
    printf("Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor);
  }
}
