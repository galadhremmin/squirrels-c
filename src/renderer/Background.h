#pragma once

#include <SDL3/SDL_pixels.h>
#include <string>

namespace squirrel {

struct Background {
    std::string ground_texture_name;
    std::string sky_texture_name;
    std::string trees_texture_name;
    SDL_Color sky_color;
    float sky_offset_x;
};

} // namespace squirrel
