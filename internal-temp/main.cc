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
  adc_select_input(4);
  adc_set_temp_sensor_enabled(true);


  while(true) {
    sleep_ms(1000);

    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    const float conversion_factor = 3.3f / (1 << 12);
    uint16_t result = adc_read();
    float adc_voltage = result * conversion_factor;
    // T = 27 - (ADC_Voltage - 0.706)/0.001721
    auto temp = 27 - (adc_voltage - 0.706) / 0.001721;
    printf("Raw value: 0x%03x, voltage: %f V, temp: %f C\n", result, adc_voltage, temp);
  }
}
