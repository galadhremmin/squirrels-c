#pragma once

#include <SDL3/SDL.h>

namespace squirrel {

struct Texture {
    SDL_Texture* texture;
    float width;
    float height;
};

} // namespace squirrel
