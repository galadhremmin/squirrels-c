#include "StateProvider.h"

StateProvider::StateProvider(std::unordered_map<int, StateDefinition>&& states)
    : states_(std::move(states)) {
}

bool StateProvider::hasState(const int state_id) const {
    return states_.contains(state_id);
}

const StateDefinition& StateProvider::getStateDefinition(const int state_id) const {
    return states_.at(state_id);
}