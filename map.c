#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "globals.h"
#include "map.h"

#define SQUARE_SEEN_MASK     1
#define SQUARE_VISIBLE_MASK  2
#define SQUARE_WATER_MASK    4
#define SQUARE_FOOD_MASK     8
#define SQUARE_ANT_MASK      16
#define SQUARE_HILL_MASK     32


char map[MAX_ROWS][MAX_COLS];
char owner[MAX_ROWS][MAX_COLS];
float aroma[MAX_ROWS][MAX_COLS];
char available[MAX_ROWS][MAX_COLS];
char direction[MAX_ROWS][MAX_COLS];

char direction_symbols[] = {'N', 'S', 'E', 'W'};

struct Whiff {
    int row;
    int col;
    float magnitude;
    float dropoff;
};
struct Whiff whiffs[MAX_ROWS * MAX_COLS];
struct Whiff *next_whiff;

void enqueue_smell(int row, int col, float magnitude, float dropoff) {
    assert(row >= 0 && row < rows);
    assert(col >= 0 && col < cols);
    assert(dropoff >= 0.0 && dropoff <= 1.0);

    if (available[row][col]) {
        // printf("enqueue_smell(%i, %i, %0.2f, %0.2f)\n", row, col, magnitude, dropoff);
        available[row][col] = 0;

        next_whiff->row = row;
        next_whiff->col = col;
        next_whiff->magnitude = magnitude;
        next_whiff->dropoff = dropoff;

        next_whiff++;
    }
}

void dequeue_smell(int row, int col, float magnitude, float dropoff) {
    assert(row >= 0 && row < rows);
    assert(col >= 0 && col < cols);
    assert(dropoff >= 0.0 && dropoff <= 1.0);

    aroma[row][col] += magnitude;

    magnitude *= dropoff;

    enqueue_smell(normalize_row(row - 1), col, magnitude, dropoff);
    enqueue_smell(normalize_row(row + 1), col, magnitude, dropoff);
    enqueue_smell(row, normalize_col(col - 1), magnitude, dropoff);
    enqueue_smell(row, normalize_col(col + 1), magnitude, dropoff);
}

void dequeue_smells() {
    struct Whiff *whiff = whiffs;
    while (whiff < next_whiff) {
        dequeue_smell(whiff->row, whiff->col, whiff->magnitude, whiff->dropoff);
        whiff++;
    }
}

void reset_availability() {
    int row, col;
    char square;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {

            // #define SQUARE_CONDUCTS_SMELL (SQUARE_SEEN_MASK & !SQUARE_WATER_MASK)
            square = map[row][col];
            available[row][col] = ((square & SQUARE_SEEN_MASK) && !(square & SQUARE_WATER_MASK));
            // printf("%i:%i is %s, %s, %s\n", row, col, square & SQUARE_SEEN_MASK ? "seen" : "unseen", square & SQUARE_WATER_MASK ? "water" : "land", available[row][col] ? "available" : "unavailable");

            // available[row][col] = ((map[row][col] & SQUARE_CONDUCTS_SMELL) == SQUARE_CONDUCTS_SMELL);
            // if (((map[row][col] & SQUARE_CONDUCTS_SMELL) == SQUARE_CONDUCTS_SMELL)) {
            //     printf("%i:%i is available\n", row, col);
            // } else {
            //     printf("%i:%i is UN-available\n", row, col);
            // }
        }
    }
}

void smell(int row, int col, float magnitude, float dropoff) {
    assert(row >= 0 && row < rows);
    assert(col >= 0 && col < cols);
    assert(dropoff >= 0.0 && dropoff <= 1.0);

    reset_availability();
    available[row][col] = 1;

    next_whiff = whiffs;

    enqueue_smell(row, col, magnitude, dropoff);

    dequeue_smells();
}

void map_blank() {
    int row, col;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            map[row][col] = 0;
            owner[row][col] = 0;
            aroma[row][col] = 0;
        }
    }
}

void map_new_turn() {
    int row, col;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            map[row][col] &= SQUARE_SEEN_MASK | SQUARE_WATER_MASK | SQUARE_HILL_MASK;
            if (map[row][col] & SQUARE_WATER_MASK) {
                aroma[row][col] = -999;
            } else {
                aroma[row][col] = 0;
            }
        }
    }
}

void see(int row, int col) {
    int row2, col2;
    int viewradius, dr, dc;

    viewradius = sqrt(viewradius2);
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

void calculate_smell() {
    int row, col;
    char square;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            square = map[row][col];
            if (square & SQUARE_ANT_MASK) {
                if (owner[row][col]) {
                    smell(row, col, -1, 0.9);
                } else {
                    smell(row, col, -1, 0.9);
                    see(row, col);
                }
            }
            if (square & SQUARE_HILL_MASK) {
                if (owner[row][col]) {
                    smell(row, col, 1, 0.9);
                // } else {
                //     smell(row, col, -1, 0.5);
                }
            }
            if (square & SQUARE_FOOD_MASK) {
                smell(row, col, 2, 0.8);
            }
            if ((square & SQUARE_SEEN_MASK) == 0) {
                smell(row, col, 1, 0.9);
            }
        }
    }
}

void calculate_sight() {
    int row, col;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if ((map[row][col] & SQUARE_ANT_MASK) && (owner[row][col] == 0)) {
                see(row, col);
            }
        }
    }
}

void calculate_direction() {
    int row, col;
    float max_smell;
    float options[4];
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if (!(map[row][col] & SQUARE_SEEN_MASK)) {
                direction[row][col] = '?';
                continue;
            } else if (map[row][col] & SQUARE_WATER_MASK) {
                direction[row][col] = '.';
                continue;
            } else {
                direction[row][col] = '*';
            }

            options[NORTH] = aroma[normalize_row(row - 1)][col];
            options[SOUTH] = aroma[normalize_row(row + 1)][col];
            options[EAST] = aroma[row][normalize_col(col + 1)];
            options[WEST] = aroma[row][normalize_col(col - 1)];

            max_smell = aroma[row][col];

            if (max_smell < options[NORTH]) {
                max_smell = options[NORTH];
                direction[row][col] = direction_symbols[NORTH];
            }

            if (max_smell < options[SOUTH]) {
                max_smell = options[SOUTH];
                direction[row][col] = direction_symbols[SOUTH];
            }

            if (max_smell < options[EAST]) {
                max_smell = options[EAST];
                direction[row][col] = direction_symbols[EAST];
            }

            if (max_smell < options[WEST]) {
                max_smell = options[WEST];
                direction[row][col] = direction_symbols[WEST];
            }
        }
    }
}

void map_recalculate() {
    calculate_sight();
    calculate_smell();
    calculate_direction();
}

void map_discover_land(int row, int col) {
    map[row][col] |= SQUARE_SEEN_MASK;
}

void map_discover_water(int row, int col) {
    map[row][col] |= SQUARE_SEEN_MASK | SQUARE_WATER_MASK;
}

void map_discover_food(int row, int col) {
    map[row][col] |= SQUARE_SEEN_MASK | SQUARE_FOOD_MASK;
}

void map_discover_ant(int row, int col, int player) {
    map[row][col] |= SQUARE_SEEN_MASK | SQUARE_ANT_MASK;
    owner[row][col] = player;
}

void map_discover_hill(int row, int col, int player) {
    map[row][col] |= SQUARE_SEEN_MASK | SQUARE_HILL_MASK;
    owner[row][col] = player;
}

// int max(int a, int b) {
//     return (a > b) ? a : b;
// }

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
    map_blank();
    rows = 0;
    cols = 0;

    for (i = 0, row = 0, col = 0; ; i++) {
        c = input[i];
        if (c == '.') {
            map_discover_land(row, col);
        }
        else if (c == '%') {
            map_discover_water(row, col);
        }
        else if (c == '*') {
            map_discover_food(row, col);
        }
        else if (c >= 'a' && c <= 'z') {
            map_discover_ant(row, col, c - 'a');
        }
        else if (c >= '0' && c <= '9') {;
            map_discover_hill(row, col, c - '0');
        }
        else if (c >= 'A' && c <= 'Z') {
            map_discover_ant(row, col, c - 'A');
            map_discover_hill(row, col, c - 'A');
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

    map_recalculate();
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

// void puts_aroma() {
//     int row, col;
// 
//     for (row = 0; row < rows; row++) {
//         for (col = 0; col < cols; col++) {
//             printf("| %0.2f ", aroma[row][col]);
//         }
//         printf("|\n");
//     }
// }

char *map_aroma_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    int row, col;
    char c;
    int v;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            v = aroma[row][col] * 1;
            if (v < 0) {
                c = '-';
            } else if (v >= 10) {
                c = '+';
            } else {
                c = (char) v + '0';
            }
            // if (c < '0' || c > '9') {
            // }
            *output++ = c;
        }
        *output++ = '\n';
    }
    *--output = '\0';
    return buffer;
}

char *map_directions_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    int row, col;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            switch (direction[row][col]) {
                case 'N':
                    *output++ = '^';
                    break;
                case 'S':
                    *output++ = 'v';
                    break;
                case 'E':
                    *output++ = '>';
                    break;
                case 'W':
                    *output++ = '<';
                    break;
                default:
                    *output++ = direction[row][col];
                    break;
            }
        }
        *output++ = '\n';
    }
    *--output = '\0';
    return buffer;
}

void issue_orders() {
    int row, col;
    char c;

    map_recalculate();

    logs(map_to_string());
    logs(map_aroma_to_string());
    logs(map_directions_to_string());
    // puts_aroma2();
    // puts_direction();
    // puts_aroma();

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if ((map[row][col] & SQUARE_ANT_MASK) && (owner[row][col] == 0)) {
                c = direction[row][col];
                if (c == 'N' || c == 'S' || c == 'E' || c == 'W') {
                    printf("o %i %i %c\n", row, col, direction[row][col]);
                }
            }
        }
    }
}


#ifdef UNIT_TESTS
#undef UNIT_TESTS
#include "globals.c"
int main(int argc, char *argv[]) {
    char input[MAX_ROWS * MAX_COLS + MAX_COLS];

    strcpy(input, "?.%*\naB2?");
    map_load_from_string(input);
    assert(strcmp(input, map_to_string()) == 0);

    map_load_from_string(".%*%");
    assert(rows == 1);
    assert(cols == 4);
    assert(aroma[0][0] == 0);
    map_load_from_string("..*%");
    assert(aroma[0][0] > 0);

    map_load_from_string(".%?%");
    assert(aroma[0][0] == 0);
    map_load_from_string("..?%");
    assert(aroma[0][0] > 0);

    map_load_from_string(".%*%");
    assert(aroma[0][0] == 0);
    map_load_from_string(".%*.");
    assert(aroma[0][0] > 0);

    map_load_from_string("..*%");
    assert(direction[0][0] == 'E');

    map_load_from_string(".%...%*%\n"
                         "...%...%\n"
                         "%%%%%%%%");
    assert(rows == 3);
    assert(cols == 8);
    // puts_aroma();
    // puts_direction();
    assert(aroma[0][0] > 0);
    assert(aroma[0][1] == 0);
    assert(aroma[0][2] > aroma[0][0]);
    assert(aroma[0][3] > aroma[0][2]);
    assert(aroma[0][4] > aroma[0][3]);
    assert(aroma[0][5] == 0);

    puts(map_to_string());
    puts("");
    puts(map_directions_to_string());
    assert(direction[0][0] == 'S');
    assert(direction[0][1] == '.');
    assert(direction[0][2] == 'E');
    assert(direction[0][3] == 'E');
    assert(direction[0][4] == 'S');
    assert(direction[0][5] == '.');

    puts("ok");
    return 0;
}
#endif
