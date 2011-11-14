#include <assert.h>
#include <string.h>
#include "map.h"
#include "holy_ground.h"

bytegrid holy_ground2;

void reset_at_point(point p) {
    grid(holy_ground, p) = map_has_friendly_hill(p) ? 0 : MAX_HOLY_GROUND;
}

void spread_at_point(point p) {
    int dir;
    point p2;

    if (map_has_land(p)) {
        grid(holy_ground2, p) = grid(holy_ground, p);
        for (dir = 1; dir < STAY; dir *= 2) {
            p2 = neighbor(p, dir);
            if (grid(holy_ground, p) > grid(holy_ground, p2)) {
                grid(holy_ground2, p) = grid(holy_ground, p2) + 1;
                break;
            }
        }
    } else {
        grid(holy_ground2, p) = MAX_HOLY_GROUND;
    }
}

void holy_ground_calculate() {
    int i;
    
    foreach_point(reset_at_point);

    for (i = 0; i < 20; i++) {
        foreach_point(spread_at_point);

        assert(sizeof(holy_ground) == sizeof(holy_ground2));
        memcpy(holy_ground, holy_ground2, sizeof(holy_ground));
    }
}

char holy_ground_to_string_at(point p) {
    if (map_has_land(p)) {
        if (grid(holy_ground, p) < 10) {
            return '0' + grid(holy_ground, p);
        } else if (grid(holy_ground, p) < 16) {
            return 'a' + grid(holy_ground, p) - 10;
        } else {
            return '.';
        }
    } else if (map_has_water(p)) {
        return '%';
    } else {
        return '?';
    }
}

char *holy_ground_to_string() {
    return point_callback_to_string(holy_ground_to_string_at);
}


#ifdef UNIT_TESTS
#undef UNIT_TESTS
#include "globals.c"
#include "map.c"
int main(int argc, char *argv[]) {
    char *map_string, *holy_ground_string;
    viewradius2 = 55;

    map_string = "0.....................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................";

    holy_ground_string = "0123456789aba987654321\n"
                         "123456789abcba98765432\n"
                         "23456789abcdcba9876543\n"
                         "3456789abcdedcba987654\n"
                         "456789abcdefedcba98765\n"
                         "56789abcdef.fedcba9876\n"
                         "6789abcdef...fedcba987\n"
                         "789abcdef.....fedcba98\n"
                         "6789abcdef...fedcba987\n"
                         "56789abcdef.fedcba9876\n"
                         "456789abcdefedcba98765\n"
                         "3456789abcdedcba987654\n"
                         "23456789abcdcba9876543\n"
                         "123456789abcba98765432";

    map_load_from_string(map_string);
    holy_ground_calculate();
    // puts(holy_ground_to_string());
    assert(strcmp(holy_ground_to_string(), holy_ground_string) == 0);

    map_string = "0.....................%\n"
                 "..%%%%%%%%............%\n"
                 "..%...................%\n"
                 "..%...................%\n"
                 "..%...................%\n"
                 "..%...................%\n"
                 "..%...................%\n"
                 "..%...................%\n"
                 "......................%\n"
                 "......................%\n"
                 "......................%\n"
                 "%%%%%%%%%%%%%%%%%%%%%%%";

    holy_ground_string = "0123456789abcdef......%\n"
                         "12%%%%%%%%bcdef.......%\n"
                         "23%....fedcdef........%\n"
                         "34%.....fedef.........%\n"
                         "45%f.....fef..........%\n"
                         "56%ef.....f...........%\n"
                         "67%def................%\n"
                         "78%cdef...............%\n"
                         "89abcdef..............%\n"
                         "9abcdef...............%\n"
                         "abcdef................%\n"
                         "%%%%%%%%%%%%%%%%%%%%%%%";

    map_load_from_string(map_string);
    holy_ground_calculate();
    // puts(holy_ground_to_string());
    assert(strcmp(holy_ground_to_string(), holy_ground_string) == 0);

    puts("ok");
    return 0;
}
#endif
