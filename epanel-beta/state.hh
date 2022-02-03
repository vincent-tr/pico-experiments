#pragma once

#include <cstdint>
#include <array>
#include "common/service.hh"

namespace mylife {

  class state : public service {
  public:
    virtual ~state() = default;
    virtual void setup() override;

    void set_input(uint8_t index, bool value);
    bool get_input(uint8_t index) const;
    uint16_t get_inputs() const;
    void set_output(uint8_t index, uint8_t value);
    uint8_t get_output(uint8_t index) const;

    void reset();

  private:
    uint16_t m_inputs = 0;
    std::array<uint8_t, 16> m_outputs = {0};
  };

}
