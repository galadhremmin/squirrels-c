#include <stdlib.h>

#include "agent.h"

Agent* agent_new(const char* name) {
    Agent* agent = (Agent*)calloc(1, sizeof(Agent));
    if (agent == NULL) {
        return NULL;
    }

    agent->name = name;
    return agent;
}

void agent_free(Agent** agent) {
    if (*agent == NULL) {
        return;
    }

    free(*agent);
    *agent = NULL;
}
