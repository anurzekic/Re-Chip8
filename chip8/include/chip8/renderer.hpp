#pragma once

#include "types.hpp"

#include <SDL3/SDL.h>

class Renderer {
public:
    Renderer(SDL_Renderer& renderer);
    ~Renderer() = default;
    Renderer(const Renderer&) = default;

    void renderDisplay(const display_t& display);
    void updateTexture(const display_t& display, SDL_Texture* texture);
    void clearWindow();
private:
    SDL_Renderer *renderer;
    color background_color;
    color draw_color;
};
