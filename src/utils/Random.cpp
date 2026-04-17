#include "Random.h"

#include <cassert>

namespace squirrel {

namespace {
Random* s_instance = nullptr;
}

Random::Random(uint64_t seed) : seed_(seed), engine_(seed_) {
}

Random& Random::instance() {
    if (!s_instance) {
        s_instance = new Random(std::random_device{}());
    }
    return *s_instance;
}

Random& Random::instanceFromSeed(uint64_t seed) {
    assert(!s_instance && "instanceFromSeed must be called before instance()");
    s_instance = new Random(seed);
    return *s_instance;
}

uint64_t Random::seed() const {
    return seed_;
}

bool Random::coinFlip() {
    return dist_bool_(engine_);
}

float Random::range(float min, float max) {
    return std::uniform_real_distribution<float>{min, max}(engine_);
}

} // namespace squirrel
