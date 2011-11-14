#include "map.h"
#include "holy_ground.h"
#include "mystery.h"

void reset_mystery_at(point p) {
    if (map_is_visible(p)) {
        grid(mystery, p) = 0;
    } else {
        grid(mystery, p) = MYSTERY_INITIAL;
    }
}

void mystery_reset() {
    foreach_point(reset_mystery_at);
}

void iterate_mystery_at(point p) {
    if (map_is_visible(p)) {
        grid(mystery, p) = 0;
    } else {
        if ((grid(holy_ground, p) > 15) && (grid(mystery, p) < MYSTERY_MAX)) {
            grid(mystery, p) += 1;
        }
    }
}

void mystery_iterate() {
    foreach_point(iterate_mystery_at);
}

#ifdef UNIT_TESTS
#undef UNIT_TESTS
#include "globals.c"
#include "map.c"
int main(int argc, char *argv[]) {
    puts("ok");
    return 0;
}
#endif
