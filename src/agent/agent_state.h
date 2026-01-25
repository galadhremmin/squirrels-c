#pragma once

// Design: docs/AGENT_STATE_MACHINE.md

#include "agent.h"

typedef struct {
    const char* name;
    void* (*transition_to)(const char* const next_state_name, Agent* const agent)
        __attribute__((nonnull(1, 2)));
} AgentState;

