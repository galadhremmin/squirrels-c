#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <stdexcept>

#include "Game.h"
#include "utils/Timer.h"
#include "world/World.h"

GameApp::GameApp() : window_{}, renderer_{} {

    SDL_Window* window = SDL_CreateWindow("Squirrels", 1024, 768, SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        throw std::runtime_error("Failed to create window");
    }
    window_ = std::shared_ptr<SDL_Window>(window, [](SDL_Window* w) {
        if (w)
            SDL_DestroyWindow(w);
    });

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        throw std::runtime_error("Failed to create renderer");
    }
    renderer_ = std::shared_ptr<SDL_Renderer>(renderer, [](SDL_Renderer* r) {
        if (r)
            SDL_DestroyRenderer(r);
    });
}

int GameApp::run() {
    World world(renderer_);

    Timer timer = {
        .current_time_ns = SDL_GetTicksNS(),
        .delta_time = 0,
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

        uint64_t current_time_ns = SDL_GetTicksNS();
        uint64_t time_diff = current_time_ns - timer.current_time_ns;
        timer.delta_time = (float)time_diff / 1000000000.0f;

        world.update(timer);
        world.render(timer);

        timer.current_time_ns = current_time_ns;
    }

    return 0;
}
