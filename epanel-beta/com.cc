#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "common/service.hh"
#include "common/application.hh"
#include "common/logger.hh"
#include "state.hh"
#include "com.hh"

namespace mylife {
  static auto logger_name = "com";

  static constexpr uint sda_pin = 4; // PICO_DEFAULT_I2C_SDA_PIN;
  static constexpr uint scl_pin = 5; // PICO_DEFAULT_I2C_SCL_PIN;
  static constexpr uint baudrate = 50000; // 50 kHz

  static constexpr uint8_t reg_check = 1;
  static constexpr uint8_t reg_reset = 2;
  static constexpr uint8_t reg_inputs = 3;
  static constexpr uint8_t reg_outputs = 4;

  static constexpr uint16_t magic = 0x4242;

  com::com(uint8_t address)
   : m_address(address) {
  }

  void com::setup() {
    m_state = static_cast<state *>(application::instance()->get_service("state"));

    gpio_init(sda_pin);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);

    gpio_init(scl_pin);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(scl_pin);

    i2c_init(i2c0, baudrate);
    i2c_set_slave_mode(i2c0, true, m_address);
  }

  static uint16_t i2c_read() {
    uint16_t value;
    i2c_read_raw_blocking(i2c0, reinterpret_cast<uint8_t *>(&value), sizeof(value));
    return value;
  }

  static void i2c_write(uint16_t value) {
    i2c_write_raw_blocking(i2c0, reinterpret_cast<const uint8_t *>(&value), sizeof(value));
  }

  void com::loop() {
    if (i2c_get_read_available(i2c0) == 0) {
      return;
    }

    uint8_t reg;
    i2c_read_raw_blocking(i2c0, &reg, 1);
    DEBUG << "got request " << static_cast<int>(reg);

    switch (reg) {
      case reg_check:
        i2c_write(magic);
        break;

      case reg_reset:
        i2c_read();
        m_state->reset();
        break;

      case reg_inputs:
        i2c_write(m_state->get_inputs());
        break;

      case reg_outputs: {
        auto word = i2c_read();
        auto index = (uint8_t)(word >> 8);
        auto value = (uint8_t)(word & 0x00ff);
        m_state->set_output(index, value);
        break;
      }

      default:
        ERROR << "got unknown request" << static_cast<int>(reg);
        break;
    }
  }
}
