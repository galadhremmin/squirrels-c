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

    void update(Agent& agent) {
        const auto cell = cellFor(agent);

        if (auto cell_it = agents_to_cell_.find(&agent); cell_it != agents_to_cell_.end()) {
            if (cell_it->second == cell)
                return;

            auto& agents = cell_to_agents_[cell_it->second];
            if (auto agent_it = std::ranges::find(agents, &agent); agent_it != agents.end()) {
                *agent_it = agents.back();
                agents.pop_back();
            }
        }

        auto& agents = cell_to_agents_[cell];
        assert(agents.size() < kMaximumNumberOfAdjacentAgents && "max agents in cell reached");

        agents.push_back(&agent);
        agents_to_cell_.insert_or_assign(&agent, cell);
    }

    [[nodiscard]] std::span<Agent* const> getAdjacentAgents(Agent& agent) const {
        if (!agents_to_cell_.contains(&agent))
            return {};

        auto agents_it = cell_to_agents_.find(agents_to_cell_.at(&agent));
        if (agents_it == cell_to_agents_.end())
            return {};

        const auto& bucket = agents_it->second;
        return std::span<Agent* const>(bucket.data(), bucket.size());
    }

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
