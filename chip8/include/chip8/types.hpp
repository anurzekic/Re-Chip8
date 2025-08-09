#pragma once

#include "config.hpp"

#include <array>
#include <cstdint>

// Color struct
struct color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

// Display type alias (2D array of bool)
using display_t = std::array<std::array<bool, WINDOW_WIDTH>, WINDOW_HEIGHT>;
