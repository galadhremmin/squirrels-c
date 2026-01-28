#pragma once

#include <stdint.h>

typedef struct {
    uint64_t current_time_ns;
    float delta_time;
} Timer;
