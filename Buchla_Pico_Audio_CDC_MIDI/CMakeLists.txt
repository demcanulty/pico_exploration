
set(FAMILY rp2040)
set(BOARD pico_sdk)
set(TINYUSB_FAMILY_PROJECT_NAME_PREFIX "tinyusb_dev_")
set(TOP ${PICO_TINYUSB_PATH})

set(CMAKE_BUILD_TYPE "Debug")
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

project(Buchla_Pico_Project C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Buchla_Pico_Project 
                main.cpp 
                blink.c 
                cdc_app.c 
                uac2_app.c 
                usb_descriptors.c
                midi.c)




pico_set_program_name(Buchla_Pico_Project "Buchla_Pico_Project")
pico_set_program_version(Buchla_Pico_Project "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Buchla_Pico_Project 0)
pico_enable_stdio_usb(Buchla_Pico_Project 1)

# Add the standard library to the build
target_link_libraries(  Buchla_Pico_Project
                        pico_stdlib
                        tinyusb_board   #needed for bsp/board_api.h
                        tinyusb_device
                        pico_multicore
                        )

# Add the standard include files to the build
target_include_directories(Buchla_Pico_Project PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(Buchla_Pico_Project)

