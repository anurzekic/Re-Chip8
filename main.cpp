#include "chip8/chip8.hpp"

#include <iostream>
#include <filesystem>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Enter ROM file path." << std::endl;
        return 1;
    }

    Chip8 chip8;
    if (!chip8.loadRom(argv[1]) || !chip8.init())
        return 1;

    chip8.run();
    
    return 0;
}
