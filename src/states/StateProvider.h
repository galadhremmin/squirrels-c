#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <unordered_map>

#include "../utils/Timer.h"

struct StateDefinition {
    bool is_cancellable_on_queue;
    bool is_terminal_state;
    bool is_transient = false;
    std::uint64_t duration_ns = 0;
    int state_id_after_expiry = -1;
};

class StateProvider {
  public:
    explicit StateProvider(std::unordered_map<int, StateDefinition>&& states);
    ~StateProvider() = default;

    StateProvider(const StateProvider&) = delete;
    StateProvider& operator=(const StateProvider&) = delete;
    StateProvider(StateProvider&&) noexcept = default;
    StateProvider& operator=(StateProvider&&) noexcept = default;

    bool hasState(const int state_id) const;
    const StateDefinition& getStateDefinition(const int state_id) const;

  private:
    std::unordered_map<int, StateDefinition> states_;
};
