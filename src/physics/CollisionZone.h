#pragma once

#include <cassert>
#include <inplace_vector>
#include <span>
#include <unordered_map>

#include "../agent/Agent.h"

static constexpr size_t kMaximumNumberOfAdjacentAgents = 10;
static constexpr size_t kMaxZonesPerAgent = 4;

typedef typename std::unordered_map<unsigned int,
                                    std::inplace_vector<Agent*, kMaximumNumberOfAdjacentAgents>>
    ZoneToAgentsMap;
typedef typename std::unordered_map<Agent*, std::inplace_vector<unsigned int, kMaxZonesPerAgent>>
    AgentToZoneMap;

class CollisionZone {
  public:
    CollisionZone(const unsigned int rows, const unsigned int columns)
        : rows_(rows), columns_(columns) {
    }

    /**
     * Removes the specified agent from any and all collision zones. Call this method on an agent
     * which is no longer in the scene.
     */
    void remove(Agent& agent);

    /**
     * Updates the collision zone for the specified agent. This must be called before the
     * `getCollidingAgents` method is called to ensure non-stale results.
     */
    void update(Agent& agent);

    /**
     * Returns the agents which are colliding with the specified agent. The agent itself isn't
     * included. This method isn't thread-safe and shouldn't be used in any concurrency scenario.
     */
    [[nodiscard]] std::span<Agent* const> getCollidingAgents(Agent& agent) const;

  private:
    /**
     * Convenience method for removing the specified agent from any and all zones. Operates on the
     * iterator to avoid unnecessary lookups.
     */
    void remove(AgentToZoneMap::iterator& it);

    /**
     * Computes the zones which the specified agent is occupying. An agent can straddle
     * multiple zones, either by being larger than a single zone, or because it's simply moving
     * between zones.
     */
    [[nodiscard]] std::inplace_vector<unsigned int, kMaxZonesPerAgent>
    computeZones(const Agent& agent) const;

    const unsigned int rows_;
    const unsigned int columns_;

    AgentToZoneMap agents_to_zone_;
    ZoneToAgentsMap zone_to_agents_;

    // reserve a memory block for storing the colliding agents from the last `getCollidingAgents`
    // call. The motivation for using an `inplace_vector` and allocating the cache within the class
    // is to avoid heap allocations every frame. The downside is that this implementation is
    // single-threaded.
    mutable std::inplace_vector<Agent*, kMaximumNumberOfAdjacentAgents> colliding_agents_cache_;
};
