#pragma once

#include <cassert>
#include <inplace_vector>
#include <span>
#include <unordered_map>

#include "../agent/Agent.h"

static constexpr size_t kMaximumNumberOfAdjacentAgents = 10;

class CollisionMap {
  public:
    CollisionMap(const unsigned int rows, const unsigned int columns)
        : rows_(rows), columns_(columns) {
    }

    void update(Agent& agent);
    [[nodiscard]] std::span<Agent* const> getAdjacentAgents(Agent& agent) const;

  private:
    const unsigned int rows_;
    const unsigned int columns_;

    std::unordered_map<Agent*, unsigned int> agents_to_cell_;
    std::unordered_map<unsigned int, std::inplace_vector<Agent*, kMaximumNumberOfAdjacentAgents>>
        cell_to_agents_;

    [[nodiscard]] unsigned int cellFor(const Agent& agent) const noexcept {
        return static_cast<unsigned int>(agent.getPosition().y * rows_) * columns_ +
               static_cast<unsigned int>(agent.getPosition().x * columns_);
    }
};
