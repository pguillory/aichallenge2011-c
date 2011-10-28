#include <math.h>
#include "map.h"
#include "threat.h"

void threat_calculate() {
    int row, col;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            threat[row][col] = 0;
            support[row][col] = 0;
        }
    }

    int rowp, colp;
    int dr, dc;
    int row2, col2;
    int direction;
    int attackradius;
    attackradius = ceil(sqrt(attackradius2) + sqrt(2));
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if ((map[row][col] & SQUARE_ANT) && owner[row][col]) {
                for (dr = -attackradius; dr <= attackradius; dr++) {
                    row2 = normalize_row(row + dr);
                    for (dc = -attackradius; dc <= attackradius; dc++) {
                        col2 = normalize_col(col + dc);
                        for (direction = 0; direction < 4; direction++) {
                            neighbor(row, col, direction , &rowp, &colp);
                            if (map[rowp][colp] & SQUARE_LAND) {
                                if (distance2(rowp, colp, row2, col2) <= attackradius2) {
                                    threat[row2][col2] += 1;
                                    break;
                                }
                            }
                        }
                    }
                }

                support[row][col] += 1;
                for (direction = 0; direction < 4; direction++) {
                    neighbor(row, col, direction , &rowp, &colp);
                    support[rowp][colp] += 1;
                }
            }
        }
    }
}

char *threat_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    int row, col;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            *output++ = '0' + threat[row][col];
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
    char *map_string, *threat_string;
    attackradius2 = 5;
    viewradius2 = 55;

    map_string = "........\n"
                 "........\n"
                 "........\n"
                 "...b....\n"
                 "........\n"
                 "........\n"
                 "........\n"
                 "........";

    threat_string = "00111000\n"
                    "01111100\n"
                    "11111110\n"
                    "11111110\n"
                    "11111110\n"
                    "01111100\n"
                    "00111000\n"
                    "00000000";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = "........\n"
                 "........\n"
                 "........\n"
                 "...b....\n"
                 "...%....\n"
                 "........\n"
                 "........\n"
                 "........";

    threat_string = "00111000\n"
                    "01111100\n"
                    "11111110\n"
                    "11111110\n"
                    "11111110\n"
                    "01111100\n"
                    "00000000\n"
                    "00000000";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = "b.......\n"
                 "........\n"
                 "........\n"
                 "........\n"
                 "........\n"
                 "........\n"
                 "........\n"
                 "........";

    threat_string = "11110111\n"
                    "11110111\n"
                    "11100011\n"
                    "11000001\n"
                    "00000000\n"
                    "11000001\n"
                    "11100011\n"
                    "11110111";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = "........\n"
                 "........\n"
                 "........\n"
                 "...b....\n"
                 "...b....\n"
                 "........\n"
                 "........\n"
                 "........";

    threat_string = "00111000\n"
                    "01222100\n"
                    "12222210\n"
                    "22222220\n"
                    "22222220\n"
                    "12222210\n"
                    "01222100\n"
                    "00111000";
    map_load_from_string(map_string);
    threat_calculate();
    assert(strcmp(threat_to_string(), threat_string) == 0);

    // puts(threat_to_string());

    puts("ok");
    return 0;
}
#endif
