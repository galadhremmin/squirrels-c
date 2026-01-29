#include "AgentState.h"

#include <SDL3/SDL.h>

#include <array>
#include <cstddef>
#include <cstdint>

#include "Agent.h"

namespace {

constexpr std::size_t kMaxNextStates = 10;
constexpr std::uint64_t kTransientDurationNs = 2000000000ULL; // 2 seconds

struct AgentStateDef {
    bool is_cancellable_on_queue;
    bool is_terminal_state;
    bool is_transient;
    std::uint64_t duration_ns;
    AgentStateId state_id_after_expiry;
    void (*on_enter_state)(Agent* agent, AgentStateId from_state_id);
};

static void on_enter_state_idle(Agent* agent, AgentStateId from_state_id);
static void on_enter_state_run_left(Agent* agent, AgentStateId from_state_id);
static void on_enter_state_run_right(Agent* agent, AgentStateId from_state_id);
static void on_enter_state_jump(Agent* agent, AgentStateId from_state_id);
static void on_enter_state_dying(Agent* agent, AgentStateId from_state_id);
static void on_enter_state_dead(Agent* agent, AgentStateId from_state_id);

static constexpr std::array<AgentStateDef, AGENT_STATE_COUNT> kStates = {{
    // AGENT_STATE_IDLE
    {true, true, false, 0, AGENT_STATE_IDLE, on_enter_state_idle},
    // AGENT_STATE_RUN_LEFT
    {true, true, false, 0, AGENT_STATE_IDLE, on_enter_state_run_left},
    // AGENT_STATE_RUN_RIGHT
    {true, true, false, 0, AGENT_STATE_IDLE, on_enter_state_run_right},
    // AGENT_STATE_JUMP
    {true, false, true, kTransientDurationNs, AGENT_STATE_IDLE, on_enter_state_jump},
    // AGENT_STATE_DYING
    {false, false, true, kTransientDurationNs, AGENT_STATE_DEAD, on_enter_state_dying},
    // AGENT_STATE_DEAD
    {false, false, false, 0, AGENT_STATE_DEAD, on_enter_state_dead},
}};

static bool state_expired(const AgentStateMachine& sm, const Timer& timer) {
    const auto current_state_id = sm.current_state_id;
    if (current_state_id < AGENT_STATE_IDLE || current_state_id >= AGENT_STATE_COUNT) {
        return false;
    }

    const auto& def = kStates[static_cast<std::size_t>(current_state_id)];
    if (!def.is_transient) {
        return false;
    }

    if (timer.current_time_ns < sm.current_state_start_time_ns) {
        return true;
    }

    return (timer.current_time_ns - sm.current_state_start_time_ns) >= def.duration_ns;
}

} // namespace

void agent_state_init(Agent* agent) {
    if (agent == nullptr) {
        SDL_Log("Agent is NULL in agent_state_init");
        return;
    }
    agent->state_machine_.current_state_id = AGENT_STATE_IDLE;
    agent->state_machine_.current_state_start_time_ns = 0;
    agent->state_machine_.next_states.clear();
}

void agent_state_push(Agent* agent, const AgentStateId next_id) {
    if (agent == nullptr) {
        SDL_Log("Agent is NULL in agent_state_push");
        return;
    }

    if (next_id < AGENT_STATE_IDLE || next_id >= AGENT_STATE_COUNT) {
        SDL_Log("Invalid state id: %d", next_id);
        return;
    }

    AgentStateMachine& sm = agent->state_machine_;

    if (next_id == sm.current_state_id) {
        return;
    }

    if (!sm.next_states.empty()) {
        const AgentStateId last_pending_state_id = sm.next_states.back();
        const auto& last_def = kStates[static_cast<std::size_t>(last_pending_state_id)];

        if (!last_def.is_terminal_state) {
            SDL_Log("Cannot push state %d, next state %d is a terminal state",
                    next_id,
                    last_pending_state_id);
            return;
        }

        if (sm.next_states.size() == kMaxNextStates - 1) {
            if (!last_def.is_cancellable_on_queue) {
                SDL_Log("Cannot push state %d, next state %d is not cancellable on queue and next "
                        "states count is at max",
                        next_id,
                        last_pending_state_id);
                return;
            }
            // Drop the last pending state and replace it with the new incoming state.
            sm.next_states.pop_back();
        }
    }

    if (sm.next_states.size() < kMaxNextStates) {
        sm.next_states.push_back(next_id);
    } else {
        SDL_Log("Cannot push state %d, next states count is at max", next_id);
    }
}

int agent_state_update(Agent* agent, const Timer* timer) {
    if (timer == nullptr) {
        SDL_Log("Timer is NULL in agent_state_update");
        return -1;
    }
    if (agent == nullptr) {
        SDL_Log("Agent is NULL in agent_state_update");
        return -1;
    }

    AgentStateMachine& sm = agent->state_machine_;
    const AgentStateId current_state_id = sm.current_state_id;
    if (current_state_id < AGENT_STATE_IDLE || current_state_id >= AGENT_STATE_COUNT) {
        SDL_Log("Invalid current state id %d for agent %s", current_state_id, agent->name_.c_str());
        return -1;
    }

    const auto& current_def = kStates[static_cast<std::size_t>(current_state_id)];

    int new_state_id = -1;

    if (state_expired(sm, *timer)) {
        new_state_id = static_cast<int>(current_def.state_id_after_expiry);
    } else if (!current_def.is_transient && !sm.next_states.empty()) {
        new_state_id = static_cast<int>(sm.next_states.front());
        sm.next_states.pop_front();
    }

    if (new_state_id >= 0 && new_state_id < AGENT_STATE_COUNT) {
        sm.current_state_id = static_cast<AgentStateId>(new_state_id);
        sm.current_state_start_time_ns = timer->current_time_ns;
        const auto& new_def = kStates[static_cast<std::size_t>(new_state_id)];
        if (new_def.on_enter_state != nullptr) {
            new_def.on_enter_state(agent, current_state_id);
        }
    }

    return new_state_id;
}

namespace {

static void on_enter_state_idle(Agent* agent, const AgentStateId from_state_id) {
    SDL_Log("Entering state idle from state %d", from_state_id);
    agent->animation_state_.face = SPRITE_ANIMATION_FACE_FRONT;
    agent->velocity_x_ = 0.0f;
    agent->velocity_y_ = 0.0f;
    agent->direction_ = 0.0f;
}

static void on_enter_state_run_left(Agent* agent, const AgentStateId from_state_id) {
    SDL_Log("Entering state run left from state %d", from_state_id);
    agent->animation_state_.face = SPRITE_ANIMATION_FACE_LEFT;
    agent->velocity_x_ = -50.0f;
    (void)from_state_id;
}

static void on_enter_state_run_right(Agent* agent, const AgentStateId from_state_id) {
    SDL_Log("Entering state run right from state %d", from_state_id);
    agent->animation_state_.face = SPRITE_ANIMATION_FACE_RIGHT;
    agent->velocity_x_ = 50.0f;
    (void)from_state_id;
}

static void on_enter_state_jump(Agent* agent, const AgentStateId from_state_id) {
    SDL_Log("Entering state jump from state %d", from_state_id);
    if (from_state_id == AGENT_STATE_RUN_LEFT) {
        agent->animation_state_.face = SPRITE_ANIMATION_FACE_LEFT;
    } else if (from_state_id == AGENT_STATE_RUN_RIGHT) {
        agent->animation_state_.face = SPRITE_ANIMATION_FACE_RIGHT;
    } else if (from_state_id == AGENT_STATE_IDLE) {
        agent->animation_state_.face = SPRITE_ANIMATION_FACE_FRONT;
    }
}

static void on_enter_state_dying(Agent* agent, const AgentStateId from_state_id) {
    SDL_Log("Entering state dying from state %d", from_state_id);
    (void)agent;
    (void)from_state_id;
}

static void on_enter_state_dead(Agent* agent, const AgentStateId from_state_id) {
    SDL_Log("Entering state dead from state %d", from_state_id);
    agent->velocity_x_ = 0.0f;
    agent->velocity_y_ = 0.0f;
    agent->direction_ = 0.0f;
    (void)from_state_id;
}

} // namespace
