#include "agent_state.h"
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_AGENT_STATE_MACHINES_CAPACITY 16
#define MAX_NEXT_STATES 10
#define NO_STATE_MACHINE_ID SIZE_MAX
#define AGENT_STATE_TRANSIENT_DURATION_NS 2000000000UL // 2 seconds in nanoseconds

typedef struct {
    bool is_cancellable_on_queue;
    bool is_cancellable;
    bool is_transient;
    uint64_t duration_ns;
    AgentStateId state_id_after_expiry;
    void (*on_enter_state)(Agent* const agent, const AgentStateId from_state_id)
        __attribute__((nonnull(1)));
} AgentState;

typedef struct {
    AgentStateId current_state_id;
    uint64_t current_state_start_time_ns;

    AgentStateId next_states[MAX_NEXT_STATES];
    size_t next_states_count;
} AgentStateMachine;

static size_t state_machine_id_get(const Agent* agent);
static bool state_expired(const AgentStateMachine* state_machine, const Timer* timer);
static void on_enter_state_idle(Agent* const agent, const AgentStateId from_state_id);
static void on_enter_state_run_left(Agent* const agent, const AgentStateId from_state_id);
static void on_enter_state_run_right(Agent* const agent, const AgentStateId from_state_id);
static void on_enter_state_jump(Agent* const agent, const AgentStateId from_state_id);
static void on_enter_state_dying(Agent* const agent, const AgentStateId from_state_id);
static void on_enter_state_dead(Agent* const agent, const AgentStateId from_state_id);

static const AgentState states[AGENT_STATE_COUNT] = {
    [AGENT_STATE_IDLE] =
        {
            .is_cancellable_on_queue = true,
            .is_cancellable = true,
            .is_transient = false,
            .duration_ns = 0,
            .state_id_after_expiry = AGENT_STATE_IDLE,
            .on_enter_state = on_enter_state_idle,
        },
    [AGENT_STATE_RUN_LEFT] =
        {
            .is_cancellable_on_queue = true,
            .is_cancellable = true,
            .is_transient = false,
            .duration_ns = 0,
            .state_id_after_expiry = AGENT_STATE_IDLE,
            .on_enter_state = on_enter_state_run_left,
        },
    [AGENT_STATE_RUN_RIGHT] =
        {
            .is_cancellable_on_queue = true,
            .is_cancellable = true,
            .is_transient = false,
            .duration_ns = 0,
            .state_id_after_expiry = AGENT_STATE_IDLE,
            .on_enter_state = on_enter_state_run_right,
        },
    [AGENT_STATE_JUMP] =
        {
            .is_cancellable_on_queue = true,
            .is_cancellable = false,
            .is_transient = true,
            .duration_ns = AGENT_STATE_TRANSIENT_DURATION_NS,
            .state_id_after_expiry = AGENT_STATE_IDLE,
            .on_enter_state = on_enter_state_jump,
        },
    [AGENT_STATE_DYING] =
        {
            .is_cancellable_on_queue = false,
            .is_cancellable = false,
            .is_transient = true,
            .duration_ns = AGENT_STATE_TRANSIENT_DURATION_NS,
            .state_id_after_expiry = AGENT_STATE_DEAD,
            .on_enter_state = on_enter_state_dying,
        },
    [AGENT_STATE_DEAD] =
        {
            .is_cancellable_on_queue = false,
            .is_cancellable = false,
            .is_transient = false,
            .duration_ns = 0,
            .state_id_after_expiry = AGENT_STATE_DEAD,
            .on_enter_state = on_enter_state_dead,
        },
};

static AgentStateMachine** agent_state_machines = NULL;
static bool agent_state_machine_at_pointer_is_reusable = false;
static size_t agent_state_machines_pointer = 0;
static size_t agent_state_machines_capacity = 0;

bool agent_state_new(Agent* const agent) {
    if (agent == NULL) {
        SDL_Log("Agent is NULL in agent_state_new");
        return false;
    }

    if (agent->state_machine_id > 0 && agent->state_machine_id != NO_STATE_MACHINE_ID) {
        SDL_Log(
            "Agent %s already has a state machine (id: %zu)", agent->name, agent->state_machine_id);
        return false;
    }

    if (agent_state_machine_at_pointer_is_reusable) {
        agent->state_machine_id = agent_state_machines_pointer + 1;
        agent_state_machine_at_pointer_is_reusable = false;
        goto find_next_available_slot;
    }

    if (agent_state_machines_pointer >= agent_state_machines_capacity) {
        const size_t previous_capacity = agent_state_machines_capacity;
        const size_t new_capacity =
            previous_capacity == 0 ? INITIAL_AGENT_STATE_MACHINES_CAPACITY : previous_capacity * 2;

        // Initially, the agent state machines array is not allocated, so we need to allocate it.
        if (agent_state_machines == NULL) {
            agent_state_machines = calloc(new_capacity, sizeof(AgentStateMachine*));

            if (agent_state_machines == NULL) {
                SDL_Log("Failed to allocate memory for agent state machines");
                return false;
            }
        } else {
            // The number of state machines have outgrown our current capacity, so we need to
            // expand the state machines array by to the new capacity.
            agent_state_machines =
                realloc(agent_state_machines, new_capacity * sizeof(AgentStateMachine*));

            if (agent_state_machines == NULL) {
                SDL_Log("Failed to reallocate memory for agent state machines");
                return false;
            }

            // Make sure to zero out the new memory, otherwise it will contain garbage.
            memset(agent_state_machines + previous_capacity,
                   0,
                   (new_capacity - previous_capacity) * sizeof(AgentStateMachine*));
        }

        agent_state_machines_capacity = new_capacity;
    }

    // Create a new state machine for the agent at the first available slot (determined by the
    // pointer).
    AgentStateMachine** const next_slot = &agent_state_machines[agent_state_machines_pointer];
    *next_slot = calloc(1, sizeof(AgentStateMachine));
    if (*next_slot == NULL) {
        SDL_Log("Failed to allocate memory for agent state machine");
        return false;
    }
    (*next_slot)->current_state_id = AGENT_STATE_IDLE;
    (*next_slot)->current_state_start_time_ns = 0;
    (*next_slot)->next_states_count = 0;

    // This is probably the most confusing part of the code, but add 1 to distinguish between agents
    // which do have a state machine and those which don't. So the ID must be subtracted by 1 to
    // get the actual index, but that is handled within the state machine code, and unbeknownst to
    // the agent code.
    agent->state_machine_id = agent_state_machines_pointer + 1;

find_next_available_slot:
    // Find the next available slot in the state machines array.
    while (agent_state_machines_pointer + 1 < agent_state_machines_capacity) {
        agent_state_machines_pointer += 1;
        if (agent_state_machines[agent_state_machines_pointer] == NULL) {
            break;
        }
    }

    return true;
}

void agent_state_free(Agent* const agent) {
    if (agent == NULL || agent->state_machine_id == 0) {
        return;
    }

    const size_t state_machine_index = agent->state_machine_id - 1;

    // Validate that the index is within bounds
    if (state_machine_index >= agent_state_machines_capacity || agent_state_machines == NULL) {
        SDL_Log("Invalid state machine index %zu for agent %s", state_machine_index, agent->name);
        agent->state_machine_id = 0;
        return;
    }

    // Check if the state machine has already been freed
    if (agent_state_machines[state_machine_index] == NULL) {
        SDL_Log("State machine for agent %s has already been freed", agent->name);
        agent->state_machine_id = 0;
        return;
    }

    if (agent_state_machine_at_pointer_is_reusable) {
        // If we're already indicating that the pointer is pointing to
        // a reusable state machine, we have no choice but to free all subsequent state machines as
        // the current implementation doesn't track all the indices of reusable state machines, only
        // the first one. This can be optimized by tracking all the indices of reusable state
        // machines.
        //
        // Note there's a special case here: we want the pointer to be the smallest index of the
        // reusable state machine, so we need to find the smallest index of the reusable state
        // machine. If the pointer is pointing to a reusable state machine at a higher index, the
        // highest index of the reusable state machine is the index of the state machine to free.
        size_t index_to_free = 0;
        size_t next_index = 0;

        if (state_machine_index < agent_state_machines_pointer) {
            index_to_free = agent_state_machines_pointer;
            next_index = state_machine_index;
        } else {
            index_to_free = state_machine_index;
            next_index = agent_state_machines_pointer;
        }

        AgentStateMachine** state_machine = &agent_state_machines[index_to_free];
        free(*state_machine);
        *state_machine = NULL;

        agent_state_machines_pointer = next_index;
    } else {
        // Otherwise, we can just mark the state machine at the pointer as reusable and we'll save
        // a few allocations in the future.
        agent_state_machines_pointer = state_machine_index;
        agent_state_machine_at_pointer_is_reusable = true;
    }

    agent->state_machine_id = 0;
}

void agent_state_free_all(void) {
    if (agent_state_machines == NULL) {
        return;
    }

    if (agent_state_machines_capacity > 0) {
        for (size_t i = agent_state_machines_capacity; i > 0; i--) {
            AgentStateMachine** state_machine = &agent_state_machines[i - 1];
            if (*state_machine != NULL) {
                free(*state_machine);
                *state_machine = NULL;
            }
        }
    }
    free(agent_state_machines);
    agent_state_machines = NULL;
    agent_state_machines_capacity = 0;
    agent_state_machines_pointer = 0;
    agent_state_machine_at_pointer_is_reusable = false;
}

void agent_state_push(Agent* const agent, const AgentStateId next_id) {
    if (agent == NULL) {
        SDL_Log("Agent is NULL in agent_state_push");
        return;
    }

    if (next_id < AGENT_STATE_IDLE || next_id >= AGENT_STATE_COUNT) {
        SDL_Log("Invalid state id: %d", next_id);
        return;
    }

    const size_t state_machine_id = state_machine_id_get(agent);
    if (state_machine_id == NO_STATE_MACHINE_ID) {
        SDL_Log("Agent %s has no state machine", agent->name);
        return;
    }

    AgentStateMachine* state_machine = agent_state_machines[state_machine_id];

    if (next_id == state_machine->current_state_id) {
        return;
    }

    if (state_machine->next_states_count > 0) {
        const AgentStateId last_pending_state_id =
            state_machine->next_states[state_machine->next_states_count - 1];
        const AgentState* last_pending_state = &states[last_pending_state_id];
        if (!last_pending_state->is_cancellable_on_queue) {
            SDL_Log("Cannot push state %d, next state %d is not cancellable",
                    next_id,
                    last_pending_state_id);
            return;
        }
    }

    if (state_machine->next_states_count < MAX_NEXT_STATES) {
        state_machine->next_states[state_machine->next_states_count] = next_id;
        state_machine->next_states_count++;
    } else {
        SDL_Log("Cannot push state %d, next states count is at max", next_id);
    }
}

int agent_state_update(Agent* const agent, const Timer* timer) {
    if (timer == NULL) {
        SDL_Log("Timer is NULL in agent_state_update");
        return -1;
    }

    const size_t state_machine_id = state_machine_id_get(agent);
    if (state_machine_id == NO_STATE_MACHINE_ID) {
        SDL_Log("Agent %s has no state machine", agent->name);
        return -1;
    }

    AgentStateMachine* state_machine = agent_state_machines[state_machine_id];
    if (state_machine == NULL) {
        SDL_Log("State machine is NULL for agent %s", agent->name);
        return -1;
    }

    const AgentStateId current_state_id = state_machine->current_state_id;
    if (current_state_id >= AGENT_STATE_COUNT) {
        SDL_Log("Invalid current state id %d for agent %s", current_state_id, agent->name);
        return -1;
    }

    const AgentState* current_state = &states[current_state_id];

    int new_state_id = -1;

    if (state_expired(state_machine, timer)) {
        new_state_id = (int)current_state->state_id_after_expiry;
    } else if (!current_state->is_transient && state_machine->next_states_count > 0) {
        new_state_id = (int)state_machine->next_states[0];
        state_machine->next_states_count -= 1;

        memmove(state_machine->next_states,
                state_machine->next_states + 1,
                state_machine->next_states_count * sizeof(AgentStateId));
        state_machine->next_states[state_machine->next_states_count] = 0;
    }

    if (new_state_id != -1 && new_state_id >= 0 && new_state_id < AGENT_STATE_COUNT) {
        state_machine->current_state_id = (AgentStateId)new_state_id;
        state_machine->current_state_start_time_ns = timer->current_time_ns;
        if (states[new_state_id].on_enter_state != NULL) {
            states[new_state_id].on_enter_state(agent, current_state_id);
        }
    }

    return new_state_id;
}

static size_t state_machine_id_get(const Agent* agent) {
    if (agent == NULL) {
        return NO_STATE_MACHINE_ID;
    }

    if (agent->state_machine_id == 0 || agent->state_machine_id == NO_STATE_MACHINE_ID) {
        return NO_STATE_MACHINE_ID;
    }

    return agent->state_machine_id - 1;
}

static bool state_expired(const AgentStateMachine* state_machine, const Timer* timer) {
    if (state_machine == NULL || timer == NULL) {
        return false;
    }

    // Validate state ID before array access
    if (state_machine->current_state_id >= AGENT_STATE_COUNT) {
        return false;
    }

    const AgentState* current_state = &states[state_machine->current_state_id];

    if (!current_state->is_transient) {
        return false;
    }

    // Handle potential timer wraparound: if current_time_ns < start_time_ns, timer has wrapped
    if (timer->current_time_ns < state_machine->current_state_start_time_ns) {
        // Timer wrapped around, consider state as expired to prevent infinite states
        return true;
    }

    return (timer->current_time_ns - state_machine->current_state_start_time_ns >=
            current_state->duration_ns);
}

static void on_enter_state_idle(Agent* const agent, const AgentStateId from_state_id) {
    SDL_Log("Entering state idle from state %d", from_state_id);
    agent->animation_state.face = SPRITE_ANIMATION_FACE_FRONT;
    agent->velocity_x = 0.0f;
    agent->velocity_y = 0.0f;
    agent->direction = 0.0f;
}

static void on_enter_state_run_left(Agent* const agent, const AgentStateId from_state_id) {
    SDL_Log("Entering state run left from state %d", from_state_id);
    agent->animation_state.face = SPRITE_ANIMATION_FACE_LEFT;
    agent->velocity_x = -50.0f;
}

static void on_enter_state_run_right(Agent* const agent, const AgentStateId from_state_id) {
    SDL_Log("Entering state run right from state %d", from_state_id);
    agent->animation_state.face = SPRITE_ANIMATION_FACE_RIGHT;
    agent->velocity_x = 50.0f;
}

static void on_enter_state_jump(Agent* const agent, const AgentStateId from_state_id) {
    SDL_Log("Entering state jump from state %d", from_state_id);
    if (from_state_id == AGENT_STATE_RUN_LEFT) {
        agent->animation_state.face = SPRITE_ANIMATION_FACE_LEFT;
    } else if (from_state_id == AGENT_STATE_RUN_RIGHT) {
        agent->animation_state.face = SPRITE_ANIMATION_FACE_RIGHT;
    } else if (from_state_id == AGENT_STATE_IDLE) {
        agent->animation_state.face = SPRITE_ANIMATION_FACE_FRONT;
    }
}

static void on_enter_state_dying(Agent* const agent, const AgentStateId from_state_id) {
    SDL_Log("Entering state dying from state %d", from_state_id);
    (void)agent;
}

static void on_enter_state_dead(Agent* const agent, const AgentStateId from_state_id) {
    SDL_Log("Entering state dead from state %d", from_state_id);
    agent->velocity_x = 0.0f;
    agent->velocity_y = 0.0f;
    agent->direction = 0.0f;
}
