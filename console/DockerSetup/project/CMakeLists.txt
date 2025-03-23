cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(my_project C CXX ASM)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()


# rest of your project
add_executable(my_program
    main.c
)
#add custom linker
pico_set_linker_script(my_program ${CMAKE_CURRENT_LIST_DIR}/memmap_custom.ld)


# Optimize for smallest size
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os ")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -mcpu=cortex-m0plus -mthumb")




pico_enable_stdio_uart(my_program 0)
pico_enable_stdio_usb(my_program 0)

#set(CMAKE_VERBOSE_MAKEFILE ON)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(my_program 
						pico_stdlib
						hardware_spi
						hardware_gpio
						hardware_pwm
						pico_time
						pico_platform
						pico_multicore)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(my_program)