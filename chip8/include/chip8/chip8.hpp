#pragma once

#include "chip8/timer.hpp"
#include "chip8/defines.h"

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <SDL3/SDL.h>

class Chip8 {
public:
    Chip8(const std::filesystem::path& rom_path);
    ~Chip8() = default; 
    Chip8(const Chip8&) = default;

    void init();
    void run();

    void fetchInstruction();

private:
    std::array<uint8_t, 4096> RAM;
    std::array<uint8_t, 16> registers;
    
    // Special registers
    uint16_t I;
    uint8_t delay_timer;
    uint8_t sound_timer;

    // Program counter
    uint16_t PC;
    // Stack pointer
    uint8_t SP;

    std::array<uint16_t, 16> stack;

    std::array<bool, 16> keypad;

    SDL_Window *window;
    SDL_Renderer *renderer;

    Timer<FPS> fps_cap_timer;

    struct draw_color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    draw_color draw_color;

    std::ifstream rom;

    void showRamContent() const;
};  
