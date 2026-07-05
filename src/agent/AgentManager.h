#pragma once

#include <functional>
#include <memory>
#include <unordered_set>
#include <vector>

#include "Agent.h"

class AgentManager {
  public:
    void add(std::unique_ptr<Agent> agent);
    void addDeferred(std::unique_ptr<Agent> agent);
    void removeDeferred(Agent& agent);
    void flushDeferred(std::function<void(Agent&)> deallocator = nullptr);

    auto begin() {
        return agents_.begin();
    }
    auto end() {
        return agents_.end();
    }
    auto begin() const {
        return agents_.cbegin();
    }
    auto end() const {
        return agents_.cend();
    }

  private:
    std::vector<std::unique_ptr<Agent>> agents_;
    std::vector<std::unique_ptr<Agent>> agents_to_add_;
    std::unordered_set<Agent*> agents_to_remove_;
};
