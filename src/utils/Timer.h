#pragma once

#include <chrono>

struct Timer {
    std::chrono::nanoseconds current_time;
    float delta_time; // seconds
};
