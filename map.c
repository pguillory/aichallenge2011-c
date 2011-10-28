#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "map.h"

void map_reset() {
    int row, col;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            map[row][col] = 0;
            owner[row][col] = 0;
        }
    }
}

void map_begin_update() {
    int row, col;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            update[row][col] = 0;
        }
    }
}

void map_finish_update() {
    int row, col;
    int row2, col2;
    int dr, dc;
    int viewradius = ceil(sqrt(viewradius2));

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            map[row][col] &= SQUARE_LAND | SQUARE_WATER | SQUARE_FOOD | SQUARE_HILL;
        }
    }

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if (update[row][col] & SQUARE_ANT) {
                map[row][col] |= SQUARE_ANT;
            }
        }
    }

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if ((map[row][col] & SQUARE_ANT) && (owner[row][col] == 0)) {
                for (dr = -viewradius; dr <= viewradius; dr++) {
                    row2 = normalize_row(row + dr);
                    for (dc = -viewradius; dc <= viewradius; dc++) {
                        col2 = normalize_col(col + dc);
                        if (distance2(row, col, row2, col2) <= viewradius2) {
                            map[row2][col2] |= SQUARE_VISIBLE;
                        }
                    }
                }
            }
        }
    }

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if (map[row][col] & SQUARE_VISIBLE) {
                if (update[row][col] & SQUARE_WATER) {
                    map[row][col] |= SQUARE_WATER;
                } else {
                    if (!(map[row][col] & SQUARE_WATER) && !(map[row][col] & SQUARE_LAND)) {
                        map[row][col] |= SQUARE_LAND;
                    }
                }
                if (update[row][col] & SQUARE_FOOD) {
                    map[row][col] |= SQUARE_FOOD;
                } else {
                    map[row][col] &= ~SQUARE_FOOD;
                }
                if (update[row][col] & SQUARE_HILL) {
                    map[row][col] |= SQUARE_HILL;
                } else {
                    map[row][col] &= ~SQUARE_HILL;
                }
            }
        }
    }
}

//    ?   = Unknown
//    .   = Land
//    %   = Water
//    *   = Food
// [a..z] = Ant
// [0..9] = Hill
// [A..Z] = Ant on a hill

void map_load_from_string(char *input) {
    int i;
    int row, col;
    char c;

    rows = MAX_ROWS;
    cols = MAX_COLS;
    map_reset();
    rows = 0;
    cols = 0;

    for (i = 0, row = 0, col = 0; ; i++) {
        c = input[i];
        if (c == '.') {
            map[row][col] |= SQUARE_VISIBLE | SQUARE_LAND;
        }
        else if (c == '%') {
            map[row][col] |= SQUARE_VISIBLE | SQUARE_WATER;
        }
        else if (c == '*') {
            map[row][col] |= SQUARE_VISIBLE | SQUARE_LAND | SQUARE_FOOD;
        }
        else if (c >= 'a' && c <= 'z') {
            map[row][col] |= SQUARE_VISIBLE | SQUARE_LAND | SQUARE_ANT;
            owner[row][col] = c - 'a';
        }
        else if (c >= '0' && c <= '9') {;
            map[row][col] |= SQUARE_VISIBLE | SQUARE_LAND | SQUARE_HILL;
            owner[row][col] = c - '0';
        }
        else if (c >= 'A' && c <= 'Z') {
            map[row][col] |= SQUARE_VISIBLE | SQUARE_LAND | SQUARE_ANT | SQUARE_HILL;
            owner[row][col] = c - 'A';
        }

        if (c == '\n') {
            row += 1;
            if (rows <= row) rows = row + 1;
            col = 0;
        } else if (c == '\0') {
            break;
        } else {
            if (rows < 1) rows = 1;
            col += 1;
            if (cols < col) cols = col;
        }
    }
}

char *map_to_string() {
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
                } else if ((square & SQUARE_ANT) && (square & SQUARE_HILL)) {
                    *output++ = 'A' + owner[row][col];
                } else if (square & SQUARE_ANT) {
                    *output++ = 'a' + owner[row][col];
                } else if (square & SQUARE_HILL) {
                    *output++ = '0' + owner[row][col];
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
int main(int argc, char *argv[]) {
    char *input;

    input = "?.%*\naB2?";
    map_load_from_string(input);
    assert(rows == 2);
    assert(cols == 4);
    assert(strcmp(input, map_to_string()) == 0);

    // map_load_from_string(".%*%");
    // assert(rows == 1);
    // assert(cols == 4);
    // assert(aroma[0][0] == 0);
    // map_load_from_string("..*%");
    // assert(aroma[0][0] > 0);
    // 
    // map_load_from_string(".%?%");
    // assert(aroma[0][0] == 0);
    // map_load_from_string("..?%");
    // assert(aroma[0][0] > 0);
    // 
    // map_load_from_string(".%*%");
    // assert(aroma[0][0] == 0);
    // map_load_from_string(".%*.");
    // assert(aroma[0][0] > 0);
    // 
    // map_load_from_string("..*%");
    // assert(direction[0][0] == 'E');

    puts("ok");
    return 0;
}
#endif
