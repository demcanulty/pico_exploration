# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")




# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(pico_i2s  C CXX ASM)

# initialize the Pico SDK
pico_sdk_init()

add_library(sequencer_synth INTERFACE)

target_sources(sequencer_synth INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/synth/synth.c
        ${CMAKE_CURRENT_LIST_DIR}/sound_pwm/sound_pwm.c
        ${CMAKE_CURRENT_LIST_DIR}/sound_i2s/sound_i2s.c
        ${CMAKE_CURRENT_LIST_DIR}/sequencer/sequencer.c
)

pico_generate_pio_header(sequencer_synth ${CMAKE_CURRENT_LIST_DIR}/sound_i2s/sound_i2s_16bits.pio)
pico_generate_pio_header(sequencer_synth ${CMAKE_CURRENT_LIST_DIR}/sound_i2s/sound_i2s_8bits.pio)

target_include_directories(sequencer_synth INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/synth
        ${CMAKE_CURRENT_LIST_DIR}/sequencer
        ${CMAKE_CURRENT_LIST_DIR}/sound_i2s
        ${CMAKE_CURRENT_LIST_DIR}/sound_pwm
)

target_link_libraries(sequencer_synth INTERFACE
    pico_stdlib
    hardware_pwm
    hardware_irq
    hardware_dma
    hardware_pio
)

