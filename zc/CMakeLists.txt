cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(test_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(zc
  main.cc
  zc.cc
)

# pico_generate_pio_header(pio ${CMAKE_CURRENT_LIST_DIR}/hello.pio)

pico_enable_stdio_usb(zc 1)
pico_enable_stdio_uart(zc 0)
pico_add_extra_outputs(zc)
target_link_libraries(zc pico_stdlib hardware_pio)
