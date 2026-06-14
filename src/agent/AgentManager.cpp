#include "AgentManager.h"
#include "Agent.h"
#include <iterator>

void AgentManager::add(std::unique_ptr<Agent> agent) {
    agents_.push_back(std::move(agent));
}

void AgentManager::addDeferred(std::unique_ptr<Agent> agent) {
    agents_to_add_.push_back(std::move(agent));
}

void AgentManager::removeDeferred(const Agent& agent) {
    agents_to_remove_.insert(&agent);
}

void AgentManager::flushDeferred() {
    std::erase_if(agents_, [&](const auto& ptr) { return agents_to_remove_.contains(ptr.get()); });
    agents_to_remove_.clear();

    agents_.insert(agents_.end(),
                   std::make_move_iterator(agents_to_add_.begin()),
                   std::make_move_iterator(agents_to_add_.end()));
    agents_to_add_.clear();
}
