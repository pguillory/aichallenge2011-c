#include <assert.h>
#include <string.h>
#include "map.h"
#include "holy_ground.h"

unsigned char holy_ground2[MAX_ROWS][MAX_COLS];

void reset_holy_ground_at_point(point p) {
    grid(holy_ground, p) = map_has_friendly_hill(p);
}

void spread_holy_ground_at_point(point p) {
    int dir;
    point p2;
    if (map_has_land(p)) {
        grid(holy_ground2, p) = grid(holy_ground, p);
        for (dir = 1; dir < STAY; dir *= 2) {
            p2 = neighbor(p, dir);
            grid(holy_ground2, p) |= grid(holy_ground, p2);
        }
    } else {
        grid(holy_ground2, p) = 0;
    }
}

void holy_ground_calculate() {
    int i;
    
    foreach_point(reset_holy_ground_at_point);

    for (i = 0; i < HOLY_GROUND_RANGE; i++) {
        foreach_point(spread_holy_ground_at_point);

        assert(sizeof(holy_ground) == sizeof(holy_ground2));
        memcpy(holy_ground, holy_ground2, sizeof(holy_ground));
    }
}

char *holy_ground_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    point p;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            if (map_has_land(p)) {
                if (map_has_hill(p)) {
                    *output++ = '0' + grid(owner, p);
                } else if (grid(holy_ground, p)) {
                    *output++ = ',';
                } else {
                    *output++ = '.';
                }
            } else if (map_has_water(p)) {
                *output++ = '%';
            } else {
                *output++ = '?';
            }
        }
        *output++ = '\n';
    }
    *--output = '\0';
    return buffer;
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
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................\n"
                 "......................";

    holy_ground_string = "0,,,,,,,,,,,,,,,,,,,,,\n"
                         ",,,,,,,,,,,,,,,,,,,,,,\n"
                         ",,,,,,,,,,,.,,,,,,,,,,\n"
                         ",,,,,,,,,,...,,,,,,,,,\n"
                         ",,,,,,,,,.....,,,,,,,,\n"
                         ",,,,,,,,.......,,,,,,,\n"
                         ",,,,,,,.........,,,,,,\n"
                         ",,,,,,...........,,,,,\n"
                         ",,,,,.............,,,,\n"
                         ",,,,...............,,,\n"
                         ",,,.................,,\n"
                         ",,...................,\n"
                         ",,,.................,,\n"
                         ",,,,...............,,,\n"
                         ",,,,,.............,,,,\n"
                         ",,,,,,...........,,,,,\n"
                         ",,,,,,,.........,,,,,,\n"
                         ",,,,,,,,.......,,,,,,,\n"
                         ",,,,,,,,,.....,,,,,,,,\n"
                         ",,,,,,,,,,...,,,,,,,,,\n"
                         ",,,,,,,,,,,.,,,,,,,,,,\n"
                         ",,,,,,,,,,,,,,,,,,,,,,";

    map_load_from_string(map_string);
    holy_ground_calculate();
    // puts(holy_ground_to_string());
    assert(strcmp(holy_ground_to_string(), holy_ground_string) == 0);

    map_string = "0.........................\n"
                 "..%%%%%%..................\n"
                 "..%.......................\n"
                 "..%.......................\n"
                 "..%.......................\n"
                 "..%.......................\n"
                 "..%.......................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................\n"
                 "..........................";

    holy_ground_string = "0,,,,,,,,,,,,.,,,,,,,,,,,,\n"
                         ",,%%%%%%,,,,...,,,,,,,,,,,\n"
                         ",,%...,,,,,.....,,,,,,,,,,\n"
                         ",,%....,,,.......,,,,,,,,,\n"
                         ",,%.....,.........,,,,,,,,\n"
                         ",,%,...............,,,,,,,\n"
                         ",,%,,...............,,,,,,\n"
                         ",,,,,,...............,,,,,\n"
                         ",,,,,.................,,,,\n"
                         ",,,,...................,,,\n"
                         ",,,.....................,,\n"
                         ",,.......................,\n"
                         ",.........................\n"
                         "..........................\n"
                         ",.........................\n"
                         ",,.......................,\n"
                         ",,,.....................,,\n"
                         ",,,,...................,,,\n"
                         ",,,,,.................,,,,\n"
                         ",,,,,,...............,,,,,\n"
                         ",,,,,,,.............,,,,,,\n"
                         ",,,,,,,,...........,,,,,,,\n"
                         ",,,,,,,,,.........,,,,,,,,\n"
                         ",,,,,,,,,,.......,,,,,,,,,\n"
                         ",,,,,,,,,,,.....,,,,,,,,,,\n"
                         ",,,,,,,,,,,,...,,,,,,,,,,,";

    map_load_from_string(map_string);
    holy_ground_calculate();
    // puts(holy_ground_to_string());
    assert(strcmp(holy_ground_to_string(), holy_ground_string) == 0);

    puts("ok");
    return 0;
}
#endif
