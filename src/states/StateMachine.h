#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>

#include "../utils/Timer.h"
#include "StateProvider.h"

constexpr int kStateMachineInvalidStateId = -1;
constexpr int kStateMachineDefaultStateId = 0;

class StateMachine {
  public:
    explicit StateMachine(StateProvider&& state_provider,
                          const int initial_state_id = 0,
                          const std::size_t max_pending_states = 10);
    ~StateMachine() = default;

    StateMachine(const StateMachine&) = delete;
    StateMachine& operator=(const StateMachine&) = delete;
    StateMachine(StateMachine&&) noexcept = delete;
    StateMachine& operator=(StateMachine&&) noexcept = delete;

    int getCurrentStateId() const;
    int getPreviousStateId() const;

    void pushState(const int next_id);
    int update(const Timer& timer);
    void reset();

  private:
    StateProvider state_provider_;
    int current_state_id_{kStateMachineDefaultStateId};
    int previous_state_id_{kStateMachineDefaultStateId};
    uint64_t current_state_start_time_ns_ = 0;
    std::deque<int> next_states_;
};
