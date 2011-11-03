#include <assert.h>
#include <string.h>
#include "map.h"
#include "holy_ground.h"
#include "army.h"

void army_calculate() {
    int row, col;
    int row2, col2;
    int dr, dc;
    int rows_scanned, cols_scanned;
    int enemy_visible = 0;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            army[row][col] = 0;

            if ((map[row][col] & SQUARE_ANT) && (owner[row][col] > 0)) {
                enemy_visible = 1;
            } else if ((map[row][col] & SQUARE_HILL) && (owner[row][col] > 0)) {
                enemy_visible = 1;
            }
        }
    }

    if (enemy_visible == 0) {
        return;
    }

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            // if (holy_ground[row][col]) continue;

            if ((map[row][col] & SQUARE_ANT) && (owner[row][col] == 0)) {
                for (dr = -1, rows_scanned = 0; dr <= +1 && rows_scanned < rows; dr++, rows_scanned++) {
                    row2 = normalize_row(row + dr);
                    for (dc = -1, cols_scanned = 0; dc <= +1 && cols_scanned < cols; dc++, cols_scanned++) {
                        col2 = normalize_col(col + dc);
                        if (row == row2 && col == col2) continue;
                        if ((map[row2][col2] & SQUARE_ANT) && (owner[row][col] == 0)) {
                            army[row][col] += 1;
                        }
                    }
                }
            }
        }
    }
}

char *army_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    int row, col;
    char square;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            square = map[row][col];
            if (square & SQUARE_LAND) {
                if (square & SQUARE_FOOD) {
                    *output++ = '*';
                } else if (square & SQUARE_ANT) {
                    if (army[row][col]) {
                        *output++ = 'A' + owner[row][col];
                    } else {
                        *output++ = 'a' + owner[row][col];
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
int main(int argc, char *argv[]) {
    char *input, *expected;

    input = "..aa..b.....*.............\n"
            "...aa.b............a......\n"
            "...........a..............\n"
            "....a.......a.............\n"
            "..........................\n"
            ".......................1..\n"
            "..........................";
    expected = "..AA..b.....*.............\n"
               "...AA.b............a......\n"
               "...........A..............\n"
               "....a.......A.............\n"
               "..........................\n"
               "..........................\n"
               "..........................";
    map_load_from_string(input);
    army_calculate();
    // puts(map_to_string());
    // puts(army_to_string());
    assert(strcmp(army_to_string(), expected) == 0);

    puts("ok");
    return 0;
}
#endif
