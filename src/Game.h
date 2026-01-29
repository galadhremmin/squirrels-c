#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <memory>

class GameApp {
  public:
    GameApp();

    int run();

  private:
    std::shared_ptr<SDL_Window> window_;
    std::shared_ptr<SDL_Renderer> renderer_;
};
