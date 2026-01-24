#include "paths.h"
#include <stdio.h>

void path_for_sprite(char* buffer, const size_t buffer_size, const char* sprite_name) {
    int written = snprintf(buffer, buffer_size, "%s/%s.png", SPRITES_DIR, sprite_name);
}
