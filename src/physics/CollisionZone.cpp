#include "CollisionZone.h"

std::inplace_vector<unsigned int, kMaxZonesPerAgent>
CollisionZone::computeZones(const Agent& agent) const {
    const auto x = agent.getPosition().x;
    const auto y = agent.getPosition().y;
    const auto w = agent.getSize().w;
    const auto h = agent.getSize().h;

    auto zoneForPoint = [&](const float px, const float py) -> unsigned int {
        const auto col = std::min(static_cast<unsigned int>(px * columns_), columns_ - 1);
        const auto row = std::min(static_cast<unsigned int>(py * rows_), rows_ - 1);
        return row * columns_ + col;
    };

    std::inplace_vector<unsigned int, kMaxZonesPerAgent> zones;
    for (const float px : {x, x + w}) {
        for (const float py : {y, y + h}) {
            if (px < 0.0f || px > 1.0f || py < 0.0f || py > 1.0f) {
                continue;
            }
            const auto zone = zoneForPoint(px, py);
            if (std::ranges::find(zones, zone) == zones.end()) {
                zones.push_back(zone);
            }
        }
    }
    return zones;
}

void CollisionZone::remove(Agent& agent) {
    auto zone_it = agents_to_zone_.find(&agent);
    remove(zone_it);
}

void CollisionZone::remove(AgentToZoneMap::iterator& it) {
    if (it != agents_to_zone_.end()) {
        for (const auto zone : it->second) {
            auto& agents = zone_to_agents_[zone];
            if (auto agent_it = std::ranges::find(agents, it->first); agent_it != agents.end()) {
                *agent_it = agents.back();
                agents.pop_back();
            }
        }
        agents_to_zone_.erase(it);
    }
}

void CollisionZone::update(Agent& agent) {
    const auto new_zones = computeZones(agent);

    if (auto zone_it = agents_to_zone_.find(&agent); zone_it != agents_to_zone_.end()) {
        if (zone_it->second == new_zones) {
            return;
        }
        remove(zone_it);
    }

    auto& agent_zones = agents_to_zone_[&agent];
    for (const auto zone : new_zones) {
        auto& agents = zone_to_agents_[zone];
        assert(agents.size() < kMaximumNumberOfAdjacentAgents && "max agents in zone reached");
        agents.push_back(&agent);
        agent_zones.push_back(zone);
    }
}

std::span<Agent* const> CollisionZone::getCollidingAgents(Agent& agent) const {
    colliding_agents_cache_.clear();

    const auto zone_it = agents_to_zone_.find(&agent);
    if (zone_it == agents_to_zone_.end()) {
        return {};
    }

    const auto ac = agent.getPosition().translate(agent.getSize().w / 2, agent.getSize().h / 2);
    const auto threshold = std::min(agent.getSize().w, agent.getSize().h);

    for (const auto zone : zone_it->second) {
        const auto agents_it = zone_to_agents_.find(zone);
        if (agents_it == zone_to_agents_.end()) {
            continue;
        }
        for (Agent* candidate : agents_it->second) {
            if (candidate == &agent || std::ranges::find(colliding_agents_cache_, candidate) !=
                                           colliding_agents_cache_.end()) {
                continue;
            }

            const auto cc = candidate->getPosition().translate(candidate->getSize().w / 2,
                                                               candidate->getSize().h / 2);
            const auto distance = (ac - cc).length();

            if (distance < threshold) {
                assert(colliding_agents_cache_.size() < kMaximumNumberOfAdjacentAgents &&
                       "candidate buffer overflow");
                colliding_agents_cache_.push_back(candidate);
            }
        }
    }

    return colliding_agents_cache_;
}
