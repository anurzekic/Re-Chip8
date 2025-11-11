add_library(
    gui 
    
    src/debugger.cpp 
    include/gui/debugger.hpp
)

target_include_directories(
    gui
    PUBLIC 
    "${PROJECT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    gui
    PRIVATE 
    imgui  # imgui target is available from top-level
    chip8
)
