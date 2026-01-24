#pragma once

#include <stdio.h>

#ifdef DEBUG
#define DEBUG_LOG(format, ...) fprintf(stderr, "[DEBUG] " format "\n", __VA_ARGS__)
#else
#define DEBUG_LOG(format, ...) ((void)0)
#endif
