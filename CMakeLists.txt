cmake_minimum_required(VERSION 4.0)

project(chip8_emulator VERSION 1.0)

add_subdirectory(chip8)

add_executable(chip8_emulator main.cpp)

target_link_libraries(
    chip8_emulator 
    PRIVATE 
    chip8
)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_11)

# add compiler warning flags just when building this project via
# the BUILD_INTERFACE genex
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(
    compiler_flags 
    INTERFACE 
    "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>" 
    "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)
