#include "Paths.h"
#include <stdio.h>

size_t path_for_sprite(char* buffer, const size_t buffer_size, const char* sprite_name) {
    int written = snprintf(buffer, buffer_size, "%s/%s.png", SPRITES_DIR, sprite_name);
    // If it is negative, there was an error while writing the string. Return a 0 representing an
    // empty string.
    return written < 0 ? 0 : (size_t)written;
}
