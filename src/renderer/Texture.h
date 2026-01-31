#pragma once

#include <SDL3/SDL.h>
#include <string>

namespace squirrel {
typedef struct {
    SDL_Texture* texture;
    float width;
    float height;
} Texture;
} // namespace squirrel
