#include "CollisionMap.h"

void CollisionMap::update(Agent& agent) {
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

std::span<Agent* const> CollisionMap::getAdjacentAgents(Agent& agent) const {
    if (!agents_to_cell_.contains(&agent))
        return {};

    auto agents_it = cell_to_agents_.find(agents_to_cell_.at(&agent));
    if (agents_it == cell_to_agents_.end())
        return {};

    const auto& bucket = agents_it->second;
    return std::span<Agent* const>(bucket.data(), bucket.size());
}
