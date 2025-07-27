#include <array>
#include <cstdint>

class Chip8 {
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

    Chip8();
    ~Chip8() = default; 
    Chip8(const Chip8&) = default;

    
};  