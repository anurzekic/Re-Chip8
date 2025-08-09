#include "chip8/input_handler.hpp"

#include <SDL3/SDL.h>

InputHandler::InputHandler() {
    key_bindings = {
        {SDL_SCANCODE_1, 0x1}, {SDL_SCANCODE_2, 0x2}, {SDL_SCANCODE_3, 0x3}, {SDL_SCANCODE_4, 0xC},
        {SDL_SCANCODE_Q, 0x4}, {SDL_SCANCODE_W, 0x5}, {SDL_SCANCODE_E, 0x6}, {SDL_SCANCODE_R, 0xD},
        {SDL_SCANCODE_A, 0x7}, {SDL_SCANCODE_S, 0x8}, {SDL_SCANCODE_D, 0x9}, {SDL_SCANCODE_F, 0xE},
        {SDL_SCANCODE_Z, 0xA}, {SDL_SCANCODE_X, 0x0}, {SDL_SCANCODE_C, 0xB}, {SDL_SCANCODE_V, 0xF}
    };
}

InputHandler::~InputHandler() {}

void InputHandler::handleInput(const SDL_Event& event, Chip8& chip8) {
    const SDL_Scancode& key = event.key.scancode;
    const Uint32& event_type = event.type;

    switch (event_type)
    {
    case SDL_EVENT_KEY_DOWN:
        switch (key)
        {
        case SDL_SCANCODE_ESCAPE:
            chip8.is_running = false;
            break;
        case SDL_SCANCODE_SPACE:
            chip8.is_paused = !chip8.is_paused; // Toggle pause state
            break;
        default:
            if (checkValidKeyPressed(key))
                chip8.keypad[key_bindings[key]] = 1;
            break;
        }
        break;
    case SDL_EVENT_KEY_UP:
        if (checkValidKeyPressed(key))
            chip8.keypad[key_bindings[key]] = 0;
        break;
    default:
        break;
    }
}

bool InputHandler::checkValidKeyPressed(const SDL_Scancode& key) const {
    return key_bindings.find(key) != key_bindings.end();
}