#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <memory>

class GameApp {
  public:
    GameApp();

    int run();

  private:
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window_{nullptr, SDL_DestroyWindow};
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer_{nullptr, SDL_DestroyRenderer};
};
