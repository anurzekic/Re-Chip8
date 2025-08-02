#include "chip8/chip8.hpp"

#include <iostream>
#include <filesystem>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Enter ROM file path." << std::endl;
        return 1;
    }

    std::filesystem::path rom_path(argv[1]);
    if (!std::filesystem::exists(rom_path) || !std::filesystem::is_regular_file(rom_path)) {
        std::cout << "File " << argv[1] << " does not exist." << std::endl;
        return 2;
    } else {
        std::cout << "Starting " << argv[1] << std::endl;
    }

    Chip8 chip8(rom_path);

    chip8.run();
    
    return 0;
}
