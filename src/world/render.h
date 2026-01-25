#pragma once

#include "../utils/timer.h"
#include "world.h"

void world_render_init(World* const world);
void world_render(World* const world, const Timer* timer);
