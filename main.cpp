#include "chip8/chip8.hpp"
#include "chip8/sound.hpp"
#include "chip8/renderer.hpp"
#include "chip8/input_handler.hpp"

#include <iostream>
#include <filesystem>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Enter ROM file path." << std::endl;
        return 1;
    }


    if (!SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO) ) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_InitSubSystem failed: %s", SDL_GetError());
        return false;
    }
    SDL_Window *window = SDL_CreateWindow("Re:Chip-8", WINDOW_WIDTH * SCALE, WINDOW_HEIGHT * SCALE, 0);;
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create SDL window: %s", SDL_GetError());
        return false;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create SDL renderer: %s", SDL_GetError());
        return false;
    }


    Renderer display_renderer(*renderer);
    InputHandler input_handler;

    Chip8 chip8;
    if (!chip8.loadRom(argv[1]) || !chip8.init())
        return 1;
    
    {    
        SoundManager sound_manager;
        Timer<FPS> fps_cap_timer;

        while (chip8.is_running) {
            SDL_Event event;        
            while (SDL_PollEvent(&event)) {  
                input_handler.handleInput(event, chip8);
            }

            if (!chip8.is_paused) {
                chip8.run();
                if (chip8.draw_to_screen) {
                    display_renderer.renderDisplay(chip8.display);
                    chip8.draw_to_screen = false;
                }

                if (chip8.play_sound) {
                    sound_manager.playSound();
                } else {
                    sound_manager.stopSound();
                }
            }
            fps_cap_timer.sleep();
        }
    }

    SDL_Log("Exiting...");

    // Clean up
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
