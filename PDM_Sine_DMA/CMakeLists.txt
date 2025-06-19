# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

set(CMAKE_C_FLAGS_RELEASE "-Ofast -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")

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
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(PDM_Sine_Output C CXX ASM)



# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()


add_library(pdm_sine_gen INTERFACE)

target_include_directories( pdm_sine_gen INTERFACE
                            
                            ${CMAKE_CURRENT_LIST_DIR}
)

pico_generate_pio_header(pdm_sine_gen ${CMAKE_CURRENT_LIST_DIR}/pdm_output.pio)


# Add executable. Default name is the project name, version 0.1

add_executable(PDM_Sine_Output main.c )

pico_set_program_name(PDM_Sine_Output "PDM_Sine_Output")
pico_set_program_version(PDM_Sine_Output "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(PDM_Sine_Output 0)
pico_enable_stdio_usb(PDM_Sine_Output 1)

# Add the standard library to the build
target_link_libraries(PDM_Sine_Output
                         pico_stdio_usb
                        tinyusb_board   #needed for bsp/board_api.h
                        #tinyusb_device
                        pico_stdlib
                        hardware_pio
                        pdm_sine_gen
                        pico_multicore
                        hardware_clocks
                        hardware_dma
                        )

# Add the standard include files to the build
target_include_directories(PDM_Sine_Output PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(PDM_Sine_Output)

