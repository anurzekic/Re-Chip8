#include "chip8/chip8.hpp"

Chip8::Chip8() {
    window_width = 640;
    window_height = 320;

    draw_color.r = 120;
    draw_color.g = 140;
    draw_color.b = 90;
    draw_color.a = 100;
}

void Chip8::init() {
    window = SDL_CreateWindow("Chip-8 Emulator", window_width, window_height, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
    SDL_RenderClear(renderer);
}

void Chip8::run() {
    init();
    bool game_is_still_running = true;

    while (game_is_still_running) 
    {
        SDL_Event event;        
        while (SDL_PollEvent(&event)) 
        {  
            // TODO Where should this go
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
        SDL_RenderClear(renderer);
    // update game state, draw the current frame
    }
}
