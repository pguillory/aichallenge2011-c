#include <math.h>
#include "map.h"
#include "holy_ground.h"
#include "threat.h"

void threat_calculate() {
    int row, col;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            threat[row][col] = 0;
            conflict[row][col] = 0;
            killzone[row][col] = 0;
        }
    }

    int rowp, colp;
    int dr, dc;
    int row2, col2;
    int direction;
    int attackradius = ceil(sqrt(attackradius2) + sqrt(2));
    int rows_scanned, cols_scanned;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if ((map[row][col] & SQUARE_ANT) && owner[row][col]) {
                for (dr = -attackradius, rows_scanned = 0; dr <= attackradius && rows_scanned < rows; dr++, rows_scanned++) {
                    row2 = normalize_row(row + dr);
                    for (dc = -attackradius, cols_scanned = 0; dc <= attackradius && cols_scanned < cols; dc++, cols_scanned++) {
                        col2 = normalize_col(col + dc);
                        if (distance2(row, col, row2, col2) <= attackradius2) {
                            killzone[row2][col2] = 1;
                        }

                        for (direction = 0; direction < 4; direction++) {
                            neighbor(row, col, direction , &rowp, &colp);
                            if (map[rowp][colp] & SQUARE_LAND) {
                                if (distance2(rowp, colp, row2, col2) <= attackradius2) {
                                    threat[row2][col2] += 1;
                                    conflict[row2][col2] = 1;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if (threat[row][col] > 0) {
                threat[row][col] += (holy_ground[row][col] ? 0 : 1);
            }
        }
    }
    
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if ((map[row][col] & SQUARE_ANT) && (owner[row][col] == 0)) {
                // printf("friendly ant at %i:%i\n", row, col);
                for (dr = -1, rows_scanned = 0; dr <= +1 && rows_scanned < rows; dr++, rows_scanned++) {
                    row2 = normalize_row(row + dr);
                    for (dc = -1, cols_scanned = 0; dc <= +1 && cols_scanned < cols; dc++, cols_scanned++) {
                        col2 = normalize_col(col + dc);
                        if (threat[row2][col2] > 0) {
                            // printf("reducing threat at %i:%i\n", row2, col2);
                            threat[row2][col2] -= 1;
                        }
                    }
                }
            }
        }
    }
}

char *threat_to_string() {
    static char chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    int row, col;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if (threat[row][col] <= 36) {
                *output++ = chars[(int) threat[row][col]];
            } else {
                *output++ = '+';
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
#include "holy_ground.c"
int main(int argc, char *argv[]) {
    char *map_string, *threat_string;
    attackradius2 = 5;
    viewradius2 = 55;

    map_string = "...b....";
    threat_string = "24555420";
    map_load_from_string(map_string);
    threat_calculate();
    puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = "........\n"
                 "........\n"
                 "........\n"
                 "...b....\n"
                 "........\n"
                 "........\n"
                 "........\n"
                 "........";

    threat_string = "00222000\n"
                    "03343300\n"
                    "23555320\n"
                    "24555420\n"
                    "23555320\n"
                    "03343300\n"
                    "00222000\n"
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

    threat_string = "00222000\n"
                    "03343300\n"
                    "23444320\n"
                    "23444320\n"
                    "22444220\n"
                    "02232200\n"
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

    threat_string = "55420245\n"
                    "55320235\n"
                    "43300033\n"
                    "22000002\n"
                    "00000000\n"
                    "22000002\n"
                    "43300033\n"
                    "55320235";
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
    threat_string = "00222000\n"
                    "03454300\n"
                    "25787520\n"
                    "36999630\n"
                    "36999630\n"
                    "25787520\n"
                    "03454300\n"
                    "00222000";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = ".........\n"
                 ".........\n"
                 ".........\n"
                 "a...b....\n"
                 ".........\n"
                 ".........\n"
                 ".........\n"
                 ".........";
    threat_string = "000222000\n"
                    "003343300\n"
                    "013555320\n"
                    "014555420\n"
                    "013555320\n"
                    "003343300\n"
                    "000222000\n"
                    "000000000";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = ".........\n"
                 ".........\n"
                 ".........\n"
                 "a...b....\n"
                 "a........\n"
                 ".........\n"
                 ".........\n"
                 ".........";
    threat_string = "000222000\n"
                    "003343300\n"
                    "013555320\n"
                    "004555420\n"
                    "003555320\n"
                    "003343300\n"
                    "000222000\n"
                    "000000000";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = ".........\n"
                 ".........\n"
                 "a........\n"
                 "a...b....\n"
                 "....b....\n"
                 ".........\n"
                 ".........\n"
                 ".........";

    threat_string = "000222000\n"
                    "003454300\n"
                    "005787520\n"
                    "016999630\n"
                    "026999630\n"
                    "025787520\n"
                    "003454300\n"
                    "000222000";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    puts("ok");
    return 0;
}
#endif
