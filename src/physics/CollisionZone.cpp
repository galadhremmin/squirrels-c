#include "CollisionZone.h"
#include <print>

void CollisionZone::remove(Agent& agent) {
    auto cell_it = agents_to_cell_.find(&agent);
    remove(cell_it);
}

void CollisionZone::remove(AgentToCellMap::iterator& it) {
    if (it != agents_to_cell_.end()) {
        auto& agents = cell_to_agents_[it->second];
        if (auto agent_it = std::ranges::find(agents, it->first); agent_it != agents.end()) {
            *agent_it = agents.back();
            agents.pop_back();
        }

        agents_to_cell_.erase(it);
    }
}

void CollisionZone::update(Agent& agent) {
    // Determine the agent's position within the collision grid based on their center position.
    const auto dx = agent.getPosition().x + agent.getSize().w / 2;
    const auto dy = agent.getPosition().y + agent.getSize().h / 2;

    const auto cell =
        static_cast<unsigned int>(dy * rows_) * columns_ + static_cast<unsigned int>(dx * columns_);

    if (auto cell_it = agents_to_cell_.find(&agent); cell_it != agents_to_cell_.end()) {
        if (cell_it->second == cell) {
            return;
        }

        remove(cell_it);
    }

    if (cell > rows_ * columns_) {
        std::println("{} is out of bounds", cell);
        return;
    }

    auto& agents = cell_to_agents_[cell];
    assert(agents.size() < kMaximumNumberOfAdjacentAgents && "max agents in cell reached");

    agents.push_back(&agent);
    agents_to_cell_.insert_or_assign(&agent, cell);
}

std::span<Agent* const> CollisionZone::getAdjacentAgents(Agent& agent) const {
    if (!agents_to_cell_.contains(&agent)) {
        return {};
    }

    auto agents_it = cell_to_agents_.find(agents_to_cell_.at(&agent));
    if (agents_it == cell_to_agents_.end()) {
        return {};
    }

    return agents_it->second;
}
