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

    // Handle input events
    void handleEvents(SDL_Event &event);

    void handleInput(const SDL_Event& event, Chip8& chip8);

    bool checkKeyPressed(uint8_t key) const;

    // Set key bindings
    void setKeyBinding(SDL_Scancode scancode, uint8_t key);

    bool checkValidKeyPressed(const SDL_Scancode& scancode) const;

private:
    std::map<SDL_Scancode, uint8_t> key_bindings; // Key bindings
};
