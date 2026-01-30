#pragma once

#include <cstdint>
#include <deque>

#include "../utils/Timer.h"

struct Agent;

typedef enum {
    AGENT_STATE_IDLE,
    AGENT_STATE_RUN_LEFT,
    AGENT_STATE_RUN_RIGHT,
    AGENT_STATE_JUMP,
    AGENT_STATE_DYING,
    AGENT_STATE_DEAD,
    AGENT_STATE_COUNT,
} AgentStateId;
