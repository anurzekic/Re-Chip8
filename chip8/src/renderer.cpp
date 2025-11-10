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

void Renderer::updateTexture(const display_t& display, SDL_Texture* texture) {
    uint32_t pixels[WINDOW_WIDTH * WINDOW_HEIGHT];
    int index = 0;
    for (int y = 0; y < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            pixels[index] = display[y][x] ? 0xFFFFFFFF : 0x00000000;
            index++;
        }
    }
    SDL_UpdateTexture(texture, NULL, pixels, WINDOW_WIDTH * sizeof(uint32_t));
}

void Renderer::clearWindow() {
    SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, background_color.a);
    SDL_RenderClear(renderer);
}
