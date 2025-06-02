cmake_minimum_required(VERSION 3.12)

# rest of your project
add_executable(buchla_midi_audio
    main.c
    usb_descriptors.c
    usb_microphone.c
    blink.c
)

target_include_directories(buchla_midi_audio PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(buchla_midi_audio PRIVATE 
                       tinyusb_device 
                       tinyusb_board 
                       pico_analog_microphone)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(buchla_midi_audio)
