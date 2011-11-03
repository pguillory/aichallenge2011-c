#include <assert.h>
#include <string.h>
#include "map.h"
#include "holy_ground.h"

void holy_ground_calculate() {
    static unsigned char holy_ground2[MAX_ROWS][MAX_COLS];
    int i;
    point p, p2;
    int dir;
    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            holy_ground[p.row][p.col] = ((map[p.row][p.col] & SQUARE_HILL) && (owner[p.row][p.col] == 0));
        }
    }

    for (i = 0; i < HOLY_GROUND_RANGE; i++) {
        for (p.row = 0; p.row < rows; p.row++) {
            for (p.col = 0; p.col < cols; p.col++) {
                if (map[p.row][p.col] & SQUARE_LAND) {
                    holy_ground2[p.row][p.col] = holy_ground[p.row][p.col];
                    for (dir = 0; dir < 4; dir++) {
                        p2 = neighbor(p, dir);
                        holy_ground2[p.row][p.col] |= holy_ground[p2.row][p2.col];
                    }
                } else {
                    holy_ground2[p.row][p.col] = 0;
                }
            }
        }

        assert(sizeof(holy_ground) == sizeof(holy_ground2));
        memcpy(holy_ground, holy_ground2, sizeof(holy_ground));
    }
}

char *holy_ground_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    point p;
    char square;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            square = map[p.row][p.col];
            if (square & SQUARE_LAND) {
                if (square & SQUARE_HILL) {
                    *output++ = '0' + owner[p.row][p.col];
                } else if (holy_ground[p.row][p.col]) {
                    *output++ = ',';
                } else {
                    *output++ = '.';
                }
            } else if (square & SQUARE_WATER) {
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
