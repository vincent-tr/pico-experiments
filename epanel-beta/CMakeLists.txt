cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
add_subdirectory(../../pico_i2c_slave/i2c_slave i2c_slave)

project(test_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(epanel-beta
  common/application.cc
  common/logger.cc
  common/status_blink.cc
  common/shell.cc
  state.cc
  com.cc
  main.cc
)

pico_enable_stdio_usb(epanel-beta 1)
pico_enable_stdio_uart(epanel-beta 0)
pico_add_extra_outputs(epanel-beta)
target_link_libraries(epanel-beta pico_stdlib hardware_i2c i2c_slave)