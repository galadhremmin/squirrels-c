#pragma once

#include <cstdint>
#include <random>

namespace squirrel {

class Random {
  public:
    static Random& instance();
    static Random& instanceFromSeed(uint64_t);

    uint64_t seed() const;
    bool coinFlip();
    float range(float min, float max);

  private:
    Random(uint64_t seed);
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

    uint64_t seed_;
    std::mt19937_64 engine_;
    std::bernoulli_distribution dist_bool_{0.5};
};

} // namespace squirrel
