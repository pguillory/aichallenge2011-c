#include <assert.h>
#include <string.h>
#include "map.h"
#include "aroma.h"
#include "directions.h"

char direction_symbols[] = {'N', 'S', 'E', 'W'};

void directions_calculate() {
    int row, col;
    char c;
    float max_smell;
    float options[4];
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if (!(map[row][col] & SQUARE_SEEN_MASK)) {
                c = '?';
            } else if (map[row][col] & SQUARE_WATER_MASK) {
                c = '-';
            } else if (map[row][col] & SQUARE_FOOD_MASK) {
                c = '*';
            } else {
                c = '+';

                options[NORTH] = aroma[normalize_row(row - 1)][col];
                options[SOUTH] = aroma[normalize_row(row + 1)][col];
                options[EAST] = aroma[row][normalize_col(col + 1)];
                options[WEST] = aroma[row][normalize_col(col - 1)];

                max_smell = aroma[row][col];

                if (max_smell < options[NORTH]) {
                    max_smell = options[NORTH];
                    c = direction_symbols[NORTH];
                }

                if (max_smell < options[SOUTH]) {
                    max_smell = options[SOUTH];
                    c = direction_symbols[SOUTH];
                }

                if (max_smell < options[EAST]) {
                    max_smell = options[EAST];
                    c = direction_symbols[EAST];
                }

                if (max_smell < options[WEST]) {
                    max_smell = options[WEST];
                    c = direction_symbols[WEST];
                }
            }

            directions[row][col] = c;
        }
    }
}

char *directions_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    int row, col;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            // switch (directions[row][col]) {
            //     case 'N':
            //         *output++ = '^';
            //         break;
            //     case 'S':
            //         *output++ = 'v';
            //         break;
            //     case 'E':
            //         *output++ = '>';
            //         break;
            //     case 'W':
            //         *output++ = '<';
            //         break;
            //     default:
            //         *output++ = directions[row][col];
            //         break;
            // }
            *output++ = directions[row][col];
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
    // puts(aroma_to_string());
    // printf("%i %i %i %i\n", map[0][0], map[0][1], map[0][2], map[0][3]);

    // map_load_from_string(".%*%");
    // aroma_stabilize();
    // assert(aroma[0][0] == 0);
    // 
    // map_load_from_string("..*%");
    // aroma_stabilize();
    // assert(aroma[0][0] > 0);
    // 
    // map_load_from_string(".%?%");
    // aroma_stabilize();
    // assert(aroma[0][0] == 0);
    // 
    // map_load_from_string("..?%");
    // aroma_stabilize();
    // assert(aroma[0][0] > 0);

    // map_load_from_string(".%*%");
    // aroma_stabilize();
    // assert(aroma[0][0] == 0);
    // map_load_from_string(".%*.");
    // assert(aroma[0][0] > 0);
    // 
    map_load_from_string("..*%");
    // puts(map_to_string());
    aroma_stabilize();
    // puts(aroma_to_string());
    directions_calculate();
    // puts(directions_to_string());
    assert(directions[0][0] == 'E');

    map_load_from_string(".%...%*%\n"
                         "...%...%\n"
                         "%%%%%%%%");
    puts(map_to_string());
    aroma_stabilize();
    puts(aroma_to_string());
    directions_calculate();
    puts(directions_to_string());

    // printf("%f %f %f %f %f %f %f\n", aroma[0][0], aroma[0][1], aroma[0][2], aroma[0][3], aroma[0][4], aroma[0][5], aroma[0][6]);
    // aroma_iterate();

    assert(directions[0][0] == 'S');
    assert(directions[0][1] == '-');
    assert(directions[0][2] == 'E');
    assert(directions[0][3] == 'E');
    assert(directions[0][4] == 'S');
    assert(directions[0][5] == '-');

    puts("ok");
    return 0;
}
#endif
