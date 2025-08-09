add_library(
    chip8 
    
    src/chip8.cpp 
    src/input_handler.cpp
    src/sound.cpp
    src/renderer.cpp

    include/chip8/chip8.hpp
    include/chip8/timer.hpp
    include/chip8/input_handler.hpp
    include/chip8/sound.hpp
    include/chip8/renderer.hpp
    include/chip8/types.hpp
)

option(CHIP8_VENDORED "Use vendored libraries" OFF)

if(CHIP8_VENDORED)
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
else()
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

target_include_directories(
    chip8 
    PUBLIC 
    "${PROJECT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    chip8 
    PRIVATE 
    SDL3::SDL3
    imgui  # imgui target is available from top-level
)
