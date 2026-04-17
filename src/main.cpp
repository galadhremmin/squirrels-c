#include <cstdio>
#include <exception>
#include <format>
#include <stdexcept>

#include "Game.h"
#include "utils/Random.h"

int main(int argc, char* argv[]) {
    for (int i = 0; i < argc; i += 1) {
        std::string_view arg{argv[i]};
        if (arg == "--seed" && i + 1 < argc) {
            squirrel::Random::instanceFromSeed(std::stoull(argv[++i]));
        }
    }

    int return_code = 0;
    try {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            throw std::runtime_error(std::format("Failed to initialize SDL: {}", SDL_GetError()));
        }

        SDL_Log("[Random] Seed %lu\n", squirrel::Random::instance().seed());

        GameApp app{};
        return_code = app.run();

        SDL_Quit();
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Fatal error: %s\n", e.what());
        return_code = 1;
    }

    return return_code;
}
