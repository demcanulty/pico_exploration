
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

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Pico_gen C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()



add_library(audio_i2s INTERFACE)

target_include_directories(audio_i2s INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/sound_i2s
        ${CMAKE_CURRENT_LIST_DIR}/gverb
        ${CMAKE_CURRENT_LIST_DIR}/usb
        ${CMAKE_CURRENT_LIST_DIR}/midi
        ${CMAKE_CURRENT_LIST_DIR}/blink

)

pico_generate_pio_header(audio_i2s ${CMAKE_CURRENT_LIST_DIR}/sound_i2s/sound_i2s_16bits.pio)
pico_generate_pio_header(audio_i2s ${CMAKE_CURRENT_LIST_DIR}/sound_i2s/sound_i2s_8bits.pio)


# Add executable. Default name is the project name, version 0.1

add_executable(Pico_gen main.c 
                        audio_process.c
                        blink/blink.c
                        usb/cdc_app.c 
                        usb/uac2_app.c
                        usb/usb_descriptors.c
                        midi/midi.c
                        gverb/gverb.c
                        gverb/gverbdsp.c
                        sound_i2s/sound_i2s.c
                        
                        )

pico_set_program_name(Pico_gen "Pico_gen")
pico_set_program_version(Pico_gen "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Pico_gen 0)
pico_enable_stdio_usb(Pico_gen 1)


# Add the standard library to the build
target_link_libraries(  Pico_gen        #THIS PROJECT
                        pico_stdlib
                        tinyusb_board   #needed for bsp/board_api.h
                        tinyusb_device
                        pico_multicore
                        hardware_clocks
                        hardware_pio
                        hardware_pwm
                        hardware_irq
                        hardware_dma
                        audio_i2s       #declared above, this creates the pio.h files
                        hardware_vreg
                        )

# Add the standard include files to the build
target_include_directories(Pico_gen PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(Pico_gen)


target_compile_definitions(${PROJECT_NAME} PRIVATE
        # This is where you define the audio output.
        # Make sure to define one of the two lines only.
        # USE_AUDIO_PWM=1
        USE_AUDIO_I2S=1
        )

