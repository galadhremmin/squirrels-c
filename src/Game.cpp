#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <stdexcept>

#include "Game.h"
#include "stages/GameStage.h"
#include "utils/Timer.h"

GameApp::GameApp() {
    window_.reset(SDL_CreateWindow("Squirrels", 1024, 768, SDL_WINDOW_OPENGL));
    if (window_ == nullptr) {
        throw std::runtime_error("Failed to create window");
    }

    renderer_.reset(SDL_CreateRenderer(window_.get(), nullptr));
    if (renderer_ == nullptr) {
        throw std::runtime_error("Failed to create renderer");
    }
}

int GameApp::run() {
    GameStage world(renderer_.get(), window_.get());

    Timer timer = {
        .current_time = std::chrono::nanoseconds{SDL_GetTicksNS()},
        .delta_time = 0.0f,
    };

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_EVENT_QUIT:
                quit = true;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (e.key.scancode == SDL_SCANCODE_ESCAPE) {
                    quit = true;
                    break;
                }
                [[fallthrough]];
            default:
                world.processInput(e);
                break;
            }
        }

        auto current_time = std::chrono::nanoseconds{SDL_GetTicksNS()};
        auto time_diff = current_time - timer.current_time;
        timer.delta_time = std::chrono::duration<float>{time_diff}.count();

        world.update(timer);
        world.render();

        timer.current_time = current_time;
    }

    return 0;
}
