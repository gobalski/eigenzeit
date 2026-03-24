cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(eigenzeit)

pico_sdk_init()

add_executable(eigenzeit src/main.c)

pico_enable_stdio_usb(eigenzeit 1)
pico_add_extra_outputs(eigenzeit)

target_link_libraries(eigenzeit pico_stdlib)

