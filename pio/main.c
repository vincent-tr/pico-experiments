/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
// Our assembled program:
#include "hello.pio.h"

int main() {
  PIO pio = pio0;
  uint offset = pio_add_program(pio, &hello_program);
  uint sm = pio_claim_unused_sm(pio, true);

  hello_program_init(pio, sm, offset, PICO_DEFAULT_LED_PIN);

  pio_sm_set_enabled(pio, sm, true);
  int freq = 3;
  // printf("Blinking pin %d at %d Hz\n", pin, freq);
  pio->txf[sm] = clock_get_hz(clk_sys) / (2 * freq);
}
