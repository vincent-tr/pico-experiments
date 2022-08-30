cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(test_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(internal-temp
  main.cc
)

# pico_generate_pio_header(pio ${CMAKE_CURRENT_LIST_DIR}/hello.pio)

pico_enable_stdio_usb(internal-temp 1)
pico_enable_stdio_uart(internal-temp 0)
pico_add_extra_outputs(internal-temp)
target_link_libraries(internal-temp pico_stdlib hardware_pio hardware_adc)
