cmake_minimum_required(VERSION 4.0)

project(chip8_emulator VERSION 1.0)

add_subdirectory(chip8)

add_executable(chip8_emulator main.cpp)

# target_include_directories(
#     chip8_emulator 
#     PRIVATE 
#     "${PROJECT_BINARY_DIR}/include"
#     "${PROJECT_BINARY_DIR}/src"
# )

target_link_libraries(
    chip8_emulator 
    
    PRIVATE 
    chip8
)
