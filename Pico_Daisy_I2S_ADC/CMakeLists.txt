
set(FAMILY rp2040)
set(BOARD pico_sdk)
set(TINYUSB_FAMILY_PROJECT_NAME_PREFIX "tinyusb_dev_")
set(TOP ${PICO_TINYUSB_PATH})

set(CMAKE_BUILD_TYPE "Debug")
set( CMAKE_VERBOSE_MAKEFILE on )
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

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

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
# if(WIN32)
#     set(USERHOME $ENV{USERPROFILE})
# else()
#     set(USERHOME $ENV{HOME})
# endif()
# set(sdkVersion 2.1.0)
# set(toolchainVersion 13_3_Rel1)
# set(picotoolVersion 2.1.0)
# set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# if (EXISTS ${picoVscode})
#     include(${picoVscode})
# endif()
# ====================================================================================


set(PICO_BOARD pico2 CACHE STRING "Board type")

set(PICO_CXX_ENABLE_EXCEPTIONS 1)


# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Pico_project C CXX ASM)



# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()


add_library(audio_i2s INTERFACE)

target_include_directories(audio_i2s INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/midi
        ${CMAKE_CURRENT_LIST_DIR}/Daisy_LGPL
        ${CMAKE_CURRENT_LIST_DIR}/Daisy_source
        ${CMAKE_CURRENT_LIST_DIR}/Daisy_source/Utility
        ${CMAKE_CURRENT_LIST_DIR}/sound_i2s
        ${CMAKE_CURRENT_LIST_DIR}/usb
        ${CMAKE_CURRENT_LIST_DIR}/utils

)

pico_generate_pio_header(audio_i2s ${CMAKE_CURRENT_LIST_DIR}/sound_i2s/sound_i2s_16bits.pio)
pico_generate_pio_header(audio_i2s ${CMAKE_CURRENT_LIST_DIR}/sound_i2s/sound_i2s_8bits.pio)


# Add executable. Default name is the project name, version 0.1

add_executable(Pico_project 
                        main.cpp
                        adc.cpp
                        audio_process.cpp
                        midi/midi.cpp
                        sound_i2s/sound_i2s.c
                        usb/cdc_app.c 
                        #usb/uac2_app.c
                        usb/usb_descriptors.c
                        utils/string_help.cpp

                        Daisy_LGPL/Filters/allpass.cpp
                        Daisy_LGPL/Filters/tone.cpp
                        Daisy_LGPL/Filters/comb.cpp
                        Daisy_LGPL/Filters/biquad.cpp
                        Daisy_LGPL/Filters/nlfilt.cpp
                        Daisy_LGPL/Filters/atone.cpp
                        Daisy_LGPL/Filters/moogladder.cpp
                        Daisy_LGPL/Filters/mode.cpp
                        Daisy_LGPL/PhysicalModeling/pluck.cpp
                        Daisy_LGPL/Dynamics/balance.cpp
                        Daisy_LGPL/Dynamics/compressor.cpp
                        Daisy_LGPL/Synthesis/blosc.cpp
                        Daisy_LGPL/Effects/reverbsc.cpp
                        Daisy_LGPL/Effects/fold.cpp
                        Daisy_LGPL/Effects/bitcrush.cpp
                        Daisy_LGPL/Control/line.cpp
                        Daisy_LGPL/Utility/jitter.cpp
                        Daisy_LGPL/Utility/port.cpp
                        Daisy_source/Filters/soap.cpp
                        Daisy_source/Filters/svf.cpp
                        Daisy_source/PhysicalModeling/drip.cpp
                        Daisy_source/PhysicalModeling/modalvoice.cpp
                        Daisy_source/PhysicalModeling/KarplusString.cpp
                        Daisy_source/PhysicalModeling/stringvoice.cpp
                        Daisy_source/PhysicalModeling/resonator.cpp
                        Daisy_source/Dynamics/limiter.cpp
                        Daisy_source/Dynamics/crossfade.cpp
                        Daisy_source/Drums/analogbassdrum.cpp
                        Daisy_source/Drums/synthsnaredrum.cpp
                        Daisy_source/Drums/analogsnaredrum.cpp
                        Daisy_source/Drums/synthbassdrum.cpp
                        Daisy_source/Drums/hihat.cpp
                        Daisy_source/Sampling/granularplayer.cpp
                        Daisy_source/Synthesis/fm2.cpp
                        Daisy_source/Synthesis/variablesawosc.cpp
                        Daisy_source/Synthesis/variableshapeosc.cpp
                        Daisy_source/Synthesis/vosim.cpp
                        Daisy_source/Synthesis/formantosc.cpp
                        Daisy_source/Synthesis/oscillator.cpp
                        Daisy_source/Synthesis/oscillatorbank.cpp
                        Daisy_source/Synthesis/zoscillator.cpp
                        Daisy_source/Noise/particle.cpp
                        Daisy_source/Noise/grainlet.cpp
                        Daisy_source/Noise/clockednoise.cpp
                        Daisy_source/Effects/flanger.cpp
                        Daisy_source/Effects/phaser.cpp
                        Daisy_source/Effects/decimator.cpp
                        Daisy_source/Effects/autowah.cpp
                        Daisy_source/Effects/sampleratereducer.cpp
                        Daisy_source/Effects/overdrive.cpp
                        Daisy_source/Effects/tremolo.cpp
                        Daisy_source/Effects/chorus.cpp
                        Daisy_source/Effects/wavefolder.cpp
                        Daisy_source/Control/adenv.cpp
                        Daisy_source/Control/phasor.cpp
                        Daisy_source/Control/adsr.cpp
                        Daisy_source/Utility/dcblock.cpp
                        Daisy_source/Utility/metro.cpp
                        )

pico_set_program_name(Pico_project "Pico_project")
pico_set_program_version(Pico_project "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Pico_project 0)
pico_enable_stdio_usb(Pico_project 1)



# Add the standard library to the build
target_link_libraries(  Pico_project        #THIS PROJECT
                        audio_i2s           #declared above, this creates the pio.h files
                        hardware_adc
                        hardware_clocks
                        hardware_dma
                        hardware_irq
                        hardware_pio
                        hardware_pwm
                        hardware_sync
                        hardware_vreg
                        pico_bootrom
                        pico_multicore
                        pico_stdlib
                        pico_sync
                        tinyusb_board   #needed for bsp/board_api.h
                        tinyusb_device
                        )

# Add the standard include files to the build
target_include_directories(Pico_project PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(Pico_project)


target_compile_definitions(${PROJECT_NAME} PRIVATE
        # This is where you define the audio output.
        # Make sure to define one of the two lines only.
        # USE_AUDIO_PWM=1
        USE_AUDIO_I2S=1
        )

