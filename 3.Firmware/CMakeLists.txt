cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

project(Pico-Dashboard)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(Pico-Dashboard
    main.c
)

target_link_libraries(Pico-Dashboard
    pico_stdlib
)

# create map/bin/hex file etc.
pico_add_extra_outputs(Pico-Dashboard)

# add url via pico_set_program_url
example_auto_set_url(Pico-Dashboard)