#include "chip8/chip8.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

int main(int argc, char **argv) {
    std::filesystem::path rom_path(argv[1]);
    
    std::ifstream rom(rom_path.string());
    Chip8 chip8;

    chip8.run();
    
    return 0;
}
