cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(test_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(shell
  main.cc
)

pico_enable_stdio_usb(shell 1)
pico_enable_stdio_uart(shell 0)
pico_add_extra_outputs(shell)
target_link_libraries(shell pico_stdlib)