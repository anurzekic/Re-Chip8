#include "chip8/renderer.hpp"

#include "config.hpp"

Renderer::Renderer(SDL_Renderer& renderer) : renderer(&renderer) {
    background_color.r = 0;
    background_color.g = 0;
    background_color.b = 0;
    background_color.a = 100;

    draw_color.r = 255;
    draw_color.g = 255;
    draw_color.b = 255;
    draw_color.a = 100;
}

// TODO Either make one function or a function that calls either one of these methods depending if the current display is empty (clear)
void Renderer::renderDisplay(const display_t& display) {
    SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, background_color.a);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);

    for (int y = 0; y < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            if (display[y][x]) {
                SDL_FRect pixel{
                    static_cast<float>(x) * SCALE,
                    static_cast<float>(y) * SCALE,
                    SCALE, SCALE
                };
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void Renderer::clearWindow() {
    SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, background_color.a);
    SDL_RenderClear(renderer);
}
