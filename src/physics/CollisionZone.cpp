#include "CollisionZone.h"

std::inplace_vector<unsigned int, kMaxCellsPerAgent>
CollisionZone::computeCells(const Agent& agent) const {
    const auto x = agent.getPosition().x;
    const auto y = agent.getPosition().y;
    const auto w = agent.getSize().w;
    const auto h = agent.getSize().h;

    auto cellForPoint = [&](const float px, const float py) -> unsigned int {
        const auto col = std::min(static_cast<unsigned int>(px * columns_), columns_ - 1);
        const auto row = std::min(static_cast<unsigned int>(py * rows_), rows_ - 1);
        return row * columns_ + col;
    };

    std::inplace_vector<unsigned int, kMaxCellsPerAgent> cells;
    for (const float px : {x, x + w}) {
        for (const float py : {y, y + h}) {
            if (px < 0.0f || px > 1.0f || py < 0.0f || py > 1.0f) {
                continue;
            }
            const auto cell = cellForPoint(px, py);
            if (std::ranges::find(cells, cell) == cells.end()) {
                cells.push_back(cell);
            }
        }
    }
    return cells;
}

void CollisionZone::remove(Agent& agent) {
    auto cell_it = agents_to_cell_.find(&agent);
    remove(cell_it);
}

void CollisionZone::remove(AgentToCellMap::iterator& it) {
    if (it != agents_to_cell_.end()) {
        for (const auto cell : it->second) {
            auto& agents = cell_to_agents_[cell];
            if (auto agent_it = std::ranges::find(agents, it->first); agent_it != agents.end()) {
                *agent_it = agents.back();
                agents.pop_back();
            }
        }
        agents_to_cell_.erase(it);
    }
}

void CollisionZone::update(Agent& agent) {
    const auto new_cells = computeCells(agent);

    if (auto cell_it = agents_to_cell_.find(&agent); cell_it != agents_to_cell_.end()) {
        if (cell_it->second == new_cells) {
            return;
        }
        remove(cell_it);
    }

    auto& agent_cells = agents_to_cell_[&agent];
    for (const auto cell : new_cells) {
        auto& agents = cell_to_agents_[cell];
        assert(agents.size() < kMaximumNumberOfAdjacentAgents && "max agents in cell reached");
        agents.push_back(&agent);
        agent_cells.push_back(cell);
    }
}

std::span<Agent* const> CollisionZone::getCollidingAgents(Agent& agent) const {
    colliding_agents_.clear();

    const auto cell_it = agents_to_cell_.find(&agent);
    if (cell_it == agents_to_cell_.end()) {
        return {};
    }

    const auto ac = agent.getPosition().translate(agent.getSize().w / 2, agent.getSize().h / 2);
    const auto threshold = std::min(agent.getSize().w, agent.getSize().h);

    for (const auto cell : cell_it->second) {
        const auto agents_it = cell_to_agents_.find(cell);
        if (agents_it == cell_to_agents_.end()) {
            continue;
        }
        for (Agent* candidate : agents_it->second) {
            if (candidate == &agent ||
                std::ranges::find(colliding_agents_, candidate) != colliding_agents_.end()) {
                continue;
            }

            const auto cc = candidate->getPosition().translate(candidate->getSize().w / 2, candidate->getSize().h / 2);
            const auto distance = (ac - cc).length();

            if (distance < threshold) {
                assert(colliding_agents_.size() < kMaximumNumberOfAdjacentAgents &&
                       "candidate buffer overflow");
                colliding_agents_.push_back(candidate);
            }
        }
    }

    return colliding_agents_;
}
