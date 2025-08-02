#include "chip8/chip8.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include <type_traits>

Chip8::Chip8() {
    draw_color.r = 120;
    draw_color.g = 140;
    draw_color.b = 90;
    draw_color.a = 100;

    RAM.fill(0);
    keypad.fill(0);
}

void Chip8::init() {
    window = SDL_CreateWindow("Chip-8 Emulator", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
    SDL_RenderClear(renderer);
    
    const std::array<uint8_t, 80> font = {        
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    memcpy(RAM.data(), font.data(), sizeof(font));
}

void Chip8::run() {
    init();

    bool game_is_still_running = true;
    size_t insturctions_per_frame = INSTRUCTION_PER_SECOND / FPS; 
    
    auto t_start = std::chrono::high_resolution_clock::now();
    while (game_is_still_running) 
    {
        SDL_Event event;        
        while (SDL_PollEvent(&event)) 
        {  
            SDL_RenderPresent(renderer);

            // poll until all events are handled!
            // decide what to do with this event.
            /* user has pressed a key? */
            if (event.type == SDL_EVENT_KEY_DOWN) 
            {
                SDL_Log("Wow, you just pressed the %s key!", SDL_GetKeyName(event.key.key));
                if (event.key.key == SDLK_ESCAPE) 
                {
                    game_is_still_running = false;
                }
            }
        }

        for (size_t i = 0; i <  insturctions_per_frame; i++)
        {
            
        }
        
        fps_cap_timer.sleep();
        // SDL_RenderClear(renderer);
    // update game state, draw the current frame
    
    }
}
