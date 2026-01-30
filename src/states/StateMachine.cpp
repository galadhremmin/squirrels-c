#include "StateMachine.h"

#include <SDL3/SDL_log.h>
#include <stdexcept>

StateMachine::StateMachine(StateProvider&& state_provider,
                           const int initial_state_id,
                           const std::size_t max_pending_states)
    : state_provider_(std::move(state_provider)), current_state_id_(initial_state_id),
      next_states_(max_pending_states) {
    if (state_provider_.hasState(kStateMachineInvalidStateId)) {
        throw std::invalid_argument("The state provider must not implement the invalid state.");
    }
    if (!state_provider_.hasState(kStateMachineDefaultStateId)) {
        throw std::invalid_argument("The state provider must implement the default state.");
    }
    if (!state_provider_.hasState(initial_state_id)) {
        throw std::invalid_argument("The state provider must implement the initial state.");
    }
}

int StateMachine::getCurrentStateId() const {
    return current_state_id_;
}

int StateMachine::getPreviousStateId() const {
    return previous_state_id_;
}

void StateMachine::pushState(const int next_id) {
    if (!state_provider_.hasState(next_id)) {
        SDL_Log("[ERROR] Invalid state id: %d", next_id);
        return;
    }

    if (next_id == current_state_id_) {
        return;
    }

    if (!next_states_.empty()) {
        const int last_pending_state_id = next_states_.back();
        const auto& last_def = state_provider_.getStateDefinition(last_pending_state_id);

        if (!last_def.is_terminal_state) {
            SDL_Log("Cannot push state %d, next state %d is a terminal state",
                    next_id,
                    last_pending_state_id);
            return;
        }

        if (next_states_.size() == next_states_.max_size() - 1) {
            if (!last_def.is_cancellable_on_queue) {
                SDL_Log("Cannot push state %d, next state %d is not cancellable on queue and next "
                        "states count is at max",
                        next_id,
                        last_pending_state_id);
                return;
            }
            // Drop the last pending state and replace it with the new incoming state.
            next_states_.pop_back();
        }
    }

    if (next_states_.size() < next_states_.max_size()) {
        next_states_.push_back(next_id);
    } else {
        SDL_Log("Cannot push state %d, next states count is at max", next_id);
    }
}

void StateMachine::reset() {
    current_state_id_ = kStateMachineDefaultStateId;
    current_state_start_time_ns_ = 0;
    next_states_.clear();
}

int StateMachine::update(const Timer& timer) {
    if (!state_provider_.hasState(current_state_id_)) {
        reset();
        return kStateMachineInvalidStateId;
    }

    const auto& current_def = state_provider_.getStateDefinition(current_state_id_);

    int new_state_id = kStateMachineInvalidStateId;

    if (current_def.is_transient) {
        if (timer.current_time_ns > current_state_start_time_ns_ &&
            (timer.current_time_ns - current_state_start_time_ns_) >= current_def.duration_ns) {
            new_state_id = current_def.state_id_after_expiry;
        }
    }

    if (!next_states_.empty() &&
        (new_state_id != kStateMachineInvalidStateId || !current_def.is_transient)) {
        new_state_id = next_states_.front();
        next_states_.pop_front();
    }

    if (new_state_id != kStateMachineInvalidStateId) {
        previous_state_id_ = current_state_id_;
        current_state_id_ = new_state_id;
        current_state_start_time_ns_ = timer.current_time_ns;
    }

    return new_state_id;
}
