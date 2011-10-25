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
            // aroma[row][col] = 0;
        }
    }
}

void map_start_turn() {
    int row, col;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            map[row][col] &= SQUARE_SEEN_MASK | SQUARE_WATER_MASK | SQUARE_HILL_MASK;
            // if (map[row][col] & SQUARE_WATER_MASK) {
            //     aroma[row][col] = -999;
            // } else {
            //     aroma[row][col] = 0;
            // }
        }
    }
}

void calculate_sight() {
    int row, col;
    int row2, col2;
    int dr, dc;
    int viewradius = ceil(sqrt(viewradius2));

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if ((map[row][col] & SQUARE_ANT_MASK) && (owner[row][col] == 0)) {
                for (dr = -viewradius; dr <= viewradius; dr++) {
                    row2 = normalize_row(row + dr);
                    for (dc = -viewradius; dc <= viewradius; dc++) {
                        col2 = normalize_col(col + dc);
                        if (distance2(row, col, row2, col2) <= viewradius2) {
                            map[row2][col2] |= SQUARE_SEEN_MASK | SQUARE_VISIBLE_MASK;
                        }
                    }
                }
            }
        }
    }
}

void map_end_turn() {
    calculate_sight();
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
            map[row][col] |= SQUARE_SEEN_MASK;
        }
        else if (c == '%') {
            map[row][col] |= SQUARE_SEEN_MASK | SQUARE_WATER_MASK;
        }
        else if (c == '*') {
            map[row][col] |= SQUARE_SEEN_MASK | SQUARE_FOOD_MASK;
        }
        else if (c >= 'a' && c <= 'z') {
            map[row][col] |= SQUARE_SEEN_MASK | SQUARE_ANT_MASK;
            owner[row][col] = c - 'a';
        }
        else if (c >= '0' && c <= '9') {;
            map[row][col] |= SQUARE_SEEN_MASK | SQUARE_HILL_MASK;
            owner[row][col] = c - '0';
        }
        else if (c >= 'A' && c <= 'Z') {
            map[row][col] |= SQUARE_SEEN_MASK | SQUARE_ANT_MASK | SQUARE_HILL_MASK;
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

    calculate_sight();
}

char *map_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    int row, col;
    char square;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            square = map[row][col];
            if ((square & SQUARE_SEEN_MASK) == 0) {
                *output++ = '?';
            } else if (square & SQUARE_WATER_MASK) {
                *output++ = '%';
            } else if (square & SQUARE_FOOD_MASK) {
                *output++ = '*';
            } else if ((square & SQUARE_ANT_MASK) && (square & SQUARE_HILL_MASK)) {
                *output++ = 'A' + owner[row][col];
            } else if (square & SQUARE_ANT_MASK) {
                *output++ = 'a' + owner[row][col];
            } else if (square & SQUARE_HILL_MASK) {
                *output++ = '0' + owner[row][col];
            } else {
                *output++ = '.';
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
    char input[MAX_ROWS * MAX_COLS + MAX_COLS];

    strcpy(input, "?.%*\naB2?");
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
    // 
    // map_load_from_string(".%...%*%\n"
    //                      "...%...%\n"
    //                      "%%%%%%%%");
    // assert(rows == 3);
    // assert(cols == 8);
    // // puts_aroma();
    // // puts_direction();
    // assert(aroma[0][0] > 0);
    // assert(aroma[0][1] == 0);
    // assert(aroma[0][2] > aroma[0][0]);
    // assert(aroma[0][3] > aroma[0][2]);
    // assert(aroma[0][4] > aroma[0][3]);
    // assert(aroma[0][5] == 0);
    // 
    // puts(map_to_string());
    // puts("");
    // puts(map_directions_to_string());
    // assert(direction[0][0] == 'S');
    // assert(direction[0][1] == '.');
    // assert(direction[0][2] == 'E');
    // assert(direction[0][3] == 'E');
    // assert(direction[0][4] == 'S');
    // assert(direction[0][5] == '.');

    puts("ok");
    return 0;
}
#endif
