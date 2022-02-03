#pragma once

#include "common/service.hh"

namespace mylife {
  class state;

  class com : public service {
  public:
    com(uint8_t address);
    virtual ~com() = default;

    virtual void setup() override;
    virtual void loop() override;

  private:
    state *m_state = nullptr;
    uint8_t m_address;
  };

}
