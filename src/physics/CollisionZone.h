#pragma once

#include <cassert>
#include <inplace_vector>
#include <span>
#include <unordered_map>

#include "../agent/Agent.h"

static constexpr size_t kMaximumNumberOfAdjacentAgents = 10;
static constexpr size_t kMaxCellsPerAgent = 4;

typedef typename std::unordered_map<unsigned int,
                                    std::inplace_vector<Agent*, kMaximumNumberOfAdjacentAgents>>
    CellToAgentsMap;
typedef typename std::unordered_map<Agent*, std::inplace_vector<unsigned int, kMaxCellsPerAgent>>
    AgentToCellMap;

class CollisionZone {
  public:
    CollisionZone(const unsigned int rows, const unsigned int columns)
        : rows_(rows), columns_(columns) {
    }

    void remove(Agent& agent);
    void update(Agent& agent);
    [[nodiscard]] std::span<Agent* const> getCollidingAgents(Agent& agent) const;

  private:
    void remove(AgentToCellMap::iterator& it);
    [[nodiscard]] std::inplace_vector<unsigned int, kMaxCellsPerAgent>
    computeCells(const Agent& agent) const;

    const unsigned int rows_;
    const unsigned int columns_;

    AgentToCellMap agents_to_cell_;
    CellToAgentsMap cell_to_agents_;

    mutable std::inplace_vector<Agent*, kMaximumNumberOfAdjacentAgents> colliding_agents_;
};
