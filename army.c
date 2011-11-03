#include <assert.h>
#include <string.h>
#include "map.h"
#include "aroma.h"
#include "army.h"

void army_calculate() {
    point p, p2, d;
    int rows_scanned, cols_scanned;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            army[p.row][p.col] = 0;
        }
    }

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            if (army_aroma[p.row][p.col] == 0.0) continue;

            if (friendly_ant_exists_at(p)) {
                for (d.row = -1, rows_scanned = 0; d.row <= 1 && rows_scanned < rows; d.row++, rows_scanned++) {
                    for (d.col = -1, cols_scanned = 0; d.col <= 1 && cols_scanned < cols; d.col++, cols_scanned++) {
                        p2 = add_points(p, d);
                        if (points_equal(p, p2)) continue;
                        if (friendly_ant_exists_at(p2)) {
                            army[p.row][p.col] += 1;
                        }
                    }
                }
            }
        }
    }

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            if (army[p.row][p.col] < 2) continue;

            if (friendly_ant_exists_at(p)) {
                for (d.row = -1, rows_scanned = 0; d.row <= 1 && rows_scanned < rows; d.row++, rows_scanned++) {
                    for (d.col = -1, cols_scanned = 0; d.col <= 1 && cols_scanned < cols; d.col++, cols_scanned++) {
                        p2 = add_points(p, d);
                        if (points_equal(p, p2)) continue;
                        if (friendly_ant_exists_at(p2)) {
                            army[p2.row][p2.col] = 2;
                        }
                    }
                }
            }
        }
    }

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            if (army[p.row][p.col] < 2) {
                army[p.row][p.col] = 0;
            } else {
                army[p.row][p.col] = 1;
            }
        }
    }
}

char *army_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    point p;
    char square;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            square = map[p.row][p.col];
            if (square & SQUARE_LAND) {
                if (square & SQUARE_FOOD) {
                    *output++ = '*';
                } else if (square & SQUARE_ANT) {
                    // *output++ = '0' + army[p.row][p.col];
                    if (army[p.row][p.col]) {
                        *output++ = 'A' + owner[p.row][p.col];
                    } else {
                        *output++ = 'a' + owner[p.row][p.col];
                    }
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
#include "aroma.c"
int main(int argc, char *argv[]) {
    char *input, *expected;

    input = "..aa..b.....*.............\n"
            "...aa.b............a......\n"
            "...........a..........a...\n"
            "....a.......a.........a...\n"
            ".................aa...a...\n"
            "..........................\n"
            "....a..a..................\n"
            "....aa.a..................\n"
            "..........................\n"
            "..........................";
    expected = "..AA..b.....*.............\n"
               "...AA.b............a......\n"
               "...........a..........A...\n"
               "....a.......a.........A...\n"
               ".................aa...A...\n"
               "..........................\n"
               "....A..a..................\n"
               "....AA.a..................\n"
               "..........................\n"
               "..........................";
    map_load_from_string(input);
    aroma_stabilize();
    army_calculate();
    // puts(map_to_string());
    // puts(army_to_string());
    assert(strcmp(army_to_string(), expected) == 0);

    puts("ok");
    return 0;
}
#endif
