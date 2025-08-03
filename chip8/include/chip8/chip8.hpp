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
    ~Chip8();

    Chip8(const Chip8&) = default;

    void init();
    void run();

private:
    using display_t = std::array<std::array<bool, WINDOW_WIDTH>, WINDOW_HEIGHT>;

    std::array<uint8_t, 4096> RAM;
    // Registers
    std::array<uint8_t, 16> V;
    
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

    display_t display;
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

    void clearWindow();

    std::string get_memory_region_label(std::size_t address) const;
    void showRamContent() const;
    
    void executeInstruction(uint16_t instruction);

    // Standard Chip-8 Instructions
    void instr_00E0();
    void instr_00EE(); 

    void instr_0nnn(uint16_t nnn);
    void instr_1nnn(uint16_t nnn);
    void instr_2nnn(uint16_t nnn);
    
    void instr_3xkk(uint8_t x, uint8_t kk);
    void instr_4xkk(uint8_t x, uint8_t kk);

    void instr_5xy0(uint8_t x, uint8_t y);

    void instr_6xkk(uint8_t x, uint8_t kk);
    void instr_7xkk(uint8_t x, uint8_t kk);

    void instr_8xy0(uint8_t x, uint8_t y);
    void instr_8xy1(uint8_t x, uint8_t y);
    void instr_8xy2(uint8_t x, uint8_t y);
    void instr_8xy3(uint8_t x, uint8_t y);
    void instr_8xy4(uint8_t x, uint8_t y);
    void instr_8xy5(uint8_t x, uint8_t y);
    void instr_8xy6(uint8_t x, uint8_t y);
    void instr_8xy7(uint8_t x, uint8_t y);

    void instr_8xyE(uint8_t x, uint8_t y);

    void instr_9xy0(uint8_t x, uint8_t y);

    void instr_Annn(uint16_t nnn);
    void instr_Bnnn(uint16_t nnn);

    void instr_Cxkk(uint8_t x, uint8_t kk);

    void instr_Dxyn(uint8_t x, uint8_t y, uint8_t n);

    void instr_Ex9E(uint8_t x);
    void instr_ExA1(uint8_t x);

    void instr_Fx07(uint8_t x);
    void instr_Fx0A(uint8_t x);
    void instr_Fx15(uint8_t x);
    void instr_Fx18(uint8_t x);
    void instr_Fx1E(uint8_t x);
    void instr_Fx29(uint8_t x);
    void instr_Fx33(uint8_t x);
    void instr_Fx55(uint8_t x);
    void instr_Fx65(uint8_t x);

    // TODO Add Other Chip-8 Variants Instructions
    
};  
