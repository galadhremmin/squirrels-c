#pragma once

#include <cstdint>

enum class AgentStateId : uint8_t {
    Idle,
    Running,
    Airborne,
    Dying,
    Dead,
};
