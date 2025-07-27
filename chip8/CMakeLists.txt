add_library(
    chip8 
    
    src/chip8.cpp 
    include/chip8/chip8.hpp
)

# Create an option to switch between a system sdl library and a vendored SDL library
option(CHIP8_VENDORED "Use vendored libraries" OFF)

if(CHIP8_VENDORED)
    # This assumes you have added SDL as a submodule in vendored/SDL
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL3 package,
    # 2. look for the SDL3-shared component, and
    # 3. fail if the shared component cannot be found.
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

target_include_directories(
    chip8 
    
    PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    chip8 
    
    PRIVATE 
    SDL3::SDL3
)