
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
set(PICO_BOARD pico CACHE STRING "Board type")


# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
# We also need PICO EXTRAS
include(pico_extras_import.cmake)


project(usb_sound_card C CXX ASM)


#PROJECT(usb_sound_card)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(usb_sound_card
        usb_sound_card.c
        )

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(${PROJECT_NAME} pico_cyw43_arch_none)
endif()

target_compile_definitions(usb_sound_card PUBLIC
        AUDIO_FREQ_MAX=48000

        # ours are zero based, so say so
        PICO_USBDEV_USE_ZERO_BASED_INTERFACES=1

        # need large descriptor
        PICO_USBDEV_MAX_DESCRIPTOR_SIZE=256


        PICO_USBDEV_ISOCHRONOUS_BUFFER_STRIDE_TYPE=1
        PICO_USBDEV_ENABLE_DEBUG_TRAgCE

        PICO_AUDIO_I2S_MONO_OUTPUT=0
        PICO_AUDIO_I2S_MONO_INPUT=0
)

target_link_libraries(usb_sound_card 
                        PRIVATE 
                        pico_stdlib 
                        usb_device 
                        pico_audio_i2s 
                        pico_multicore
                        pico_stdio_uart)


pico_set_program_name(${PROJECT_NAME} ${PROJECT_NAME})

pico_add_extra_outputs(${PROJECT_NAME})

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

