#include "chip8/chip8.hpp"
#include "chip8/sound.hpp"
#include "chip8/renderer.hpp"
#include "chip8/input_handler.hpp"

#include "gui/debugger.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "imgui_memory_editor.h"

#include <iostream>
#include <filesystem>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Enter ROM file path." << std::endl;
        return 1;
    }


    if (!SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO) ) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_InitSubSystem failed: %s", SDL_GetError());
        return 1;
    }

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_Window *window = SDL_CreateWindow("Re:Chip-8", 1280*main_scale, 720*main_scale, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create SDL window: %s", SDL_GetError());
        return 1;
    }

    SDL_SetWindowFullscreen(window, false);
    // TODO Needs to be tested on windows
    // Explicitly set the initial size again (some platforms need this)
    // SDL_SetWindowSize(window, 1280 * main_scale, 720 * main_scale);
    // SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);    

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create SDL renderer: %s", SDL_GetError());
        return 1;
    }

    // ImGui Setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Style
    ImGui::StyleColorsDark();
    
    // Scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    Renderer display_renderer(*renderer);
    SDL_Texture* chip8_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    SDL_SetTextureScaleMode(chip8_texture, SDL_SCALEMODE_NEAREST);
    
    InputHandler input_handler;

    Chip8 chip8;
    if (!chip8.loadRom(argv[1]) || !chip8.init())
        return 1;
    
    Debugger gui_debugger;    
        
    {    
        SoundManager sound_manager;
        Timer<FPS> fps_cap_timer;

        bool dockspace_initialized = false;

        while (chip8.is_running) {
            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            if (!dockspace_initialized) {
                using namespace ImGui;
                dockspace_initialized = true;
                ImGuiID dockspace_id = ImGui::GetMainViewport()->ID;

                ImGui::DockBuilderRemoveNode(dockspace_id); // clear previous layout
                ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_None);
                ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

                // Split the main dockspace into regions
                ImGuiID dock_main_id = dockspace_id;
                ImGuiID dock_id_left, dock_id_right, dock_id_bottom;
                dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.22f, nullptr, &dock_main_id);
                dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.35f, nullptr, &dock_main_id);
                dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.45f, nullptr, &dock_main_id);

                // --- RIGHT SIDE (split vertically for each window) ---
                ImGuiID right_top = dock_id_right;
                ImGuiID right_middle, right_bottom;
                right_middle = ImGui::DockBuilderSplitNode(right_top, ImGuiDir_Down, 0.41f, nullptr, &right_top);
                right_bottom = ImGui::DockBuilderSplitNode(right_middle, ImGuiDir_Down, 0.62f, nullptr, &right_middle);

                ImGui::DockBuilderDockWindow("Registers", right_top);
                ImGui::DockBuilderDockWindow("Program Counter", right_middle);
                ImGui::DockBuilderDockWindow("Keypad", right_bottom);

                // --- LEFT SIDE ---
                ImGuiID left_top = dock_id_left;
                ImGuiID left_middle, left_bottom;
                left_middle = ImGui::DockBuilderSplitNode(left_top, ImGuiDir_Down, 0.83f, nullptr, &left_top);
                left_bottom = ImGui::DockBuilderSplitNode(left_middle, ImGuiDir_Down, 0.71f, nullptr, &left_middle);
                
                ImGui::DockBuilderDockWindow("Special Registers", left_top);
                ImGui::DockBuilderDockWindow("Stack", left_middle);
                ImGui::DockBuilderDockWindow("Disassembly", left_bottom);

                // --- CENTER ---
                ImGui::DockBuilderDockWindow("Chip-8 Display", dock_main_id);

                // --- BOTTOM ---
                ImGui::DockBuilderDockWindow("RAM", dock_id_bottom);

                ImGui::DockBuilderFinish(dockspace_id);
            }

            // This must come AFTER the DockBuilder setup
            ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);

            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Load ROM")) {

                    } else if (ImGui::MenuItem("Reload ROM")) {

                    } else if (ImGui::MenuItem("Exit")) {
                        
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View")) {
                    ImGui::MenuItem("Reset Layout");
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            SDL_Event event;        
            while (SDL_PollEvent(&event)) {                 
                ImGui_ImplSDL3_ProcessEvent(&event);

                input_handler.handleInput(event, chip8);
            }

            // Always show the window
            if (ImGui::Begin("Chip-8 Display")) {
                ImVec2 avail_size = ImGui::GetContentRegionAvail();
                float aspect_ratio = 64.0f / 32.0f;
                ImVec2 image_size = avail_size;
                if (image_size.x / image_size.y > aspect_ratio)
                    image_size.x = image_size.y * aspect_ratio;
                else
                    image_size.y = image_size.x / aspect_ratio;

                ImGui::Image((void*)chip8_texture, image_size);
            }
            ImGui::End();

            if (!chip8.is_paused) {
                chip8.step();
                if (chip8.draw_to_screen) {
                    display_renderer.updateTexture(chip8.display, chip8_texture);
                }

                if (chip8.play_sound)
                    sound_manager.playSound();
                else
                    sound_manager.stopSound();
            }

            gui_debugger.showRegisters(chip8.V);
            gui_debugger.showTimers(chip8.PC, chip8.I, chip8.delay_timer, chip8.sound_timer);
            gui_debugger.showProgramCounter(chip8.PC, chip8.RAM, chip8);
            gui_debugger.showStack(chip8.stack);
            gui_debugger.showKeypad(chip8.keypad);
            gui_debugger.showDisassembly(chip8.PC, chip8.RAM, chip8.rom_size);

            static MemoryEditor mem_edit;
            mem_edit.HighlightMin = chip8.PC;
            mem_edit.HighlightMax = chip8.PC + 1;
            mem_edit.HighlightColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
            mem_edit.DrawWindow("RAM", chip8.RAM.data(), sizeof(chip8.RAM));

            display_renderer.clearWindow();

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
