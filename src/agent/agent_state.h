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

bool agent_state_new(Agent* const agent) __attribute__((nonnull(1)));
void agent_state_free(Agent* const agent) __attribute__((nonnull(1)));
void agent_state_free_all(void);
void agent_state_push(Agent* const agent, const AgentStateId id) __attribute__((nonnull(1)));
int agent_state_update(Agent* const agent, const Timer* timer) __attribute__((nonnull(1)));
