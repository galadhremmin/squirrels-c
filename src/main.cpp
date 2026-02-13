#include <cstdio>
#include <exception>
#include <format>
#include <stdexcept>

#include "Game.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    int return_code = 0;
    try {

        if (! SDL_Init(SDL_INIT_VIDEO)) {
            throw std::runtime_error(std::format("Failed to initialize SDL: {}", SDL_GetError()));
        }
        
        GameApp app{};
        return_code = app.run();

        SDL_Quit();
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Fatal error: %s\n", e.what());
        return_code = 1;
    }

    return return_code;
}
