#pragma once

#include "../utils/timer.h"
#include "agent.h"

typedef enum {
    AGENT_STATE_IDLE,
    AGENT_STATE_RUN_LEFT,
    AGENT_STATE_RUN_RIGHT,
    AGENT_STATE_JUMP,
    AGENT_STATE_DYING,
    AGENT_STATE_DEAD,
    AGENT_STATE_COUNT,
} AgentStateId;

bool agent_state_new(Agent* const agent);
void agent_state_free(Agent* const agent);
void agent_state_free_all(void);
void agent_state_push(Agent* const agent, const AgentStateId id);
int agent_state_update(Agent* const agent, const Timer* timer);
