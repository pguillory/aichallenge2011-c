#include "map.h"
#include "holy_ground.h"

void holy_ground_calculate() {
    int row, col;
    int row2, col2;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if ((map[row][col] & SQUARE_HILL) && (owner[row][col] == 0)) {
                for (row2 = 0; row2 < rows; row2++) {
                    for (col2 = 0; col2 < cols; col2++) {
                        if (distance2(row, col, row2, col2) <= viewradius2) {
                            holy_ground[row2][col2] = 1;
                        } else {
                            holy_ground[row2][col2] = 0;
                        }
                    }
                }
            }
        }
    }
}

char *holy_ground_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    int row, col;
    char square;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            square = map[row][col];
            if (square & SQUARE_LAND) {
                if (square & SQUARE_HILL) {
                    *output++ = '0' + owner[row][col];
                } else if (holy_ground[row][col]) {
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

    map_string = "0...............\n"
                 "................\n"
                 "................\n"
                 "................\n"
                 "................\n"
                 "................\n"
                 "................\n"
                 "................\n"
                 "................\n"
                 "................\n"
                 "................\n"
                 "................\n"
                 "................\n"
                 "................\n"
                 "................\n"
                 "................";

    holy_ground_string = "0,,,,,,,.,,,,,,,\n"
                         ",,,,,,,,.,,,,,,,\n"
                         ",,,,,,,,.,,,,,,,\n"
                         ",,,,,,,...,,,,,,\n"
                         ",,,,,,,...,,,,,,\n"
                         ",,,,,,.....,,,,,\n"
                         ",,,,,.......,,,,\n"
                         ",,,...........,,\n"
                         "................\n"
                         ",,,...........,,\n"
                         ",,,,,.......,,,,\n"
                         ",,,,,,.....,,,,,\n"
                         ",,,,,,,...,,,,,,\n"
                         ",,,,,,,...,,,,,,\n"
                         ",,,,,,,,.,,,,,,,\n"
                         ",,,,,,,,.,,,,,,,";

    map_load_from_string(map_string);
    holy_ground_calculate();
    // puts(holy_ground_to_string());
    assert(strcmp(holy_ground_to_string(), holy_ground_string) == 0);

    puts("ok");
    return 0;
}
#endif
