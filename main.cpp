#include "chip8/chip8.hpp"
#include "chip8/sound.hpp"
#include "chip8/renderer.hpp"
#include "chip8/input_handler.hpp"

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "imgui_club/imgui_memory_editor.h"

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

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_Window *window = SDL_CreateWindow("Re:Chip-8", 1280*main_scale, 720*main_scale, SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create SDL window: %s", SDL_GetError());
        return false;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create SDL renderer: %s", SDL_GetError());
        return false;
    }

    // ImGui Setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    // Style
    ImGui::StyleColorsDark();
    
    // Scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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
                ImGui_ImplSDL3_ProcessEvent(&event);

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
            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            ImGui::ShowDemoWindow(&show_demo_window);
            
            static MemoryEditor mem_edit;
            mem_edit.DrawWindow("RAM", chip8.RAM.data(), sizeof(chip8.RAM));

            ImGui::Render();
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
            SDL_RenderPresent(renderer);

            fps_cap_timer.sleep();
        }
    }

    SDL_Log("Exiting...");

    // Clean up
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
