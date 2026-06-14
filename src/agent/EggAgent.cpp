#include "EggAgent.h"
#include "AgentManager.h"

void EggAgent::update(const Timer& timer, AgentManager& manager) {
    if (timer.current_time >= ttl_) {
        manager.removeDeferred(*this);
    }
}
