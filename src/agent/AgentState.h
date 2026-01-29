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

struct AgentStateMachine {
    AgentStateId current_state_id = AGENT_STATE_IDLE;
    uint64_t current_state_start_time_ns = 0;
    std::deque<AgentStateId> next_states;
};

void agent_state_init(Agent* agent);
void agent_state_push(Agent* agent, AgentStateId id);
int agent_state_update(Agent* agent, const Timer* timer);
