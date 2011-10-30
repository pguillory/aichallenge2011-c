#include <assert.h>
#include <string.h>
#include "map.h"
#include "holy_ground.h"
#include "threat.h"
#include "aroma.h"
#include "directions.h"

char direction_symbols[] = {'N', 'E', 'S', 'W'};

void directions_calculate() {
    int row, col;
    char c;
    int row2, col2;
    int direction;
    float max_aroma;
    float min_threat;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if (map[row][col] & SQUARE_LAND) {
                if (map[row][col] & SQUARE_FOOD) {
                    c = '*';
                } else if (killzone[row][col]) {
                    c = '!';
                } else {
                    c = '+';

                    min_threat = threat[row][col];
                    max_aroma = aroma[row][col];

                    for (direction = 0; direction < 4; direction++) {
                        neighbor(row, col, direction, &row2, &col2);

                        if (map[row2][col2] & SQUARE_FOOD) {
                            c = '+';
                            break;
                        }

                        if ((min_threat > threat[row2][col2]) || ((min_threat == threat[row2][col2]) && (max_aroma < aroma[row2][col2]))) {
                            min_threat = threat[row2][col2];
                            max_aroma = aroma[row2][col2];
                            c = direction_symbols[direction];
                        }
                    }
                }
            } else if (map[row][col] & SQUARE_WATER) {
                c = '-';
            } else {
                c = '?';
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
#include "holy_ground.c"
#include "threat.c"
#include "mystery.c"
#include "aroma.c"
int main(int argc, char *argv[]) {
    char *expected;
    attackradius2 = 5;
    viewradius2 = 55;
    // puts(aroma_to_string());
    // printf("%i %i %i %i\n", map[0][0], map[0][1], map[0][2], map[0][3]);

    map_load_from_string("..*%");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    // puts(aroma_to_string());
    directions_calculate();
    // puts(directions_to_string());
    assert(directions[0][0] == 'E');

    map_load_from_string(".%...%*%\n"
                         "...%...%\n"
                         "%%%%%%%%");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    directions_calculate();

    // puts(map_to_string());
    // puts(threat_to_string());
    // puts(aroma_to_string());
    // puts(directions_to_string());

    assert(directions[0][0] == 'S');
    assert(directions[0][1] == '-');
    assert(directions[0][2] == 'E');
    assert(directions[0][3] == 'E');
    assert(directions[0][4] == 'S');
    assert(directions[0][5] == '-');

    map_load_from_string(".........%\n"
                         ".........%\n"
                         ".........%\n"
                         ".........%\n"
                         "....b....%\n"
                         ".........%\n"
                         ".........%\n"
                         ".........%\n"
                         "........*%\n"
                         "%%%%%%%%%%");
    expected = "EEEEEESSS-\n"
               "SE+NNEESS-\n"
               "S+N!!!EES-\n"
               "SW!!!!!ES-\n"
               "SW!!!!!ES-\n"
               "SS!!!!!SS-\n"
               "ESS!!!ESS-\n"
               "EESSSEEE+-\n"
               "EEEEEEE+*-\n"
               "----------";
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    directions_calculate();
    // puts(directions_to_string());
    assert(strcmp(directions_to_string(), expected) == 0);

    map_load_from_string(".aa...b.....*......................\n"
                         ".aa.........*......................");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    directions_calculate();

    // puts(threat_to_string());
    // puts(aroma_to_string());
    // puts(directions_to_string());



    // aroma_iterate();

    puts("ok");
    return 0;
}
#endif
