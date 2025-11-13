#pragma once

#include "chip8/chip8.hpp"

#include <array>
#include <map>
#include <SDL3/SDL.h>

class InputHandler {
public:
    InputHandler();
    ~InputHandler();
    InputHandler(const InputHandler&) = default;

    void handleInput(const SDL_Event& event, Chip8& chip8);
    void setKeyBinding(SDL_Scancode scancode, uint8_t key); // TODO Implement
    bool checkValidKeyPressed(const SDL_Scancode& scancode) const;
    
private:
    std::map<SDL_Scancode, uint8_t> key_bindings;
};
