#include <cstdio>
#include <exception>

#include "Game.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    try {
        GameApp app;
        return app.run();
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Fatal error: %s\n", e.what());
        return 1;
    }
}
