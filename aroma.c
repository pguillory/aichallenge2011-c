#include <assert.h>
#include <string.h>
#include "map.h"
#include "holy_ground.h"
#include "threat.h"
#include "mystery.h"
#include "aroma.h"

#define AROMA_NATURAL_DISSIPATION  0.95
#define AROMA_ANT_DISSIPATION      0.2
#define AROMA_ARMY_ANT_DISSIPATION 0.9

#define AROMA_FOOD               100.0
#define AROMA_ENEMY               10.0
#define AROMA_INTRUDER           200.0
#define AROMA_ENEMY_HILL         500.0
#define AROMA_MYSTERY             10.0
#define AROMA_CONFLICT             0.0
#define AROMA_CONFLICT_FRONT      80.0
#define AROMA_CONFLICT_SIDE        0.0
#define AROMA_CONFLICT_REAR        0.0

void aroma_reset() {
    int row, col;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            aroma[row][col] = 0.0;
        }
    }
}

void aroma_spread(float aroma[MAX_ROWS][MAX_COLS]) {
    int row, col;
    int row2, col2;
    char square;
    char square2;
    int direction;
    float sum;
    int count;
    float aroma2[MAX_ROWS][MAX_COLS];

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            sum = 0;
            count = 0;
            square = map[row][col];
            if (!(square & SQUARE_WATER)) {
                sum += aroma[row][col];
                count += 1;

                for (direction = 0; direction < 4; direction++) {
                    neighbor(row, col, direction, &row2, &col2);
                    if (row == row2 && col == col2) continue;
                    square2 = map[row2][col2];
                    if (!(square2 & SQUARE_WATER)) {
                        sum += aroma[row2][col2];
                        count += 1;
                    }
                }
            }
            if (count) {
                aroma2[row][col] = sum / count;
            } else {
                aroma2[row][col] = 0.0;
            }
        }
    }
    // printf("sizeof(aroma) = %i\n", sizeof(aroma));
    // printf("sizeof(aroma2) = %i\n", sizeof(aroma2));
    // assert(sizeof(aroma) == sizeof(aroma2));
    memcpy(aroma, aroma2, sizeof(aroma2));
}

void aroma_iterate() {
    int row, col;
    int row2, col2;
    char square;
    int direction;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if ((map[row][col] & SQUARE_ANT) && (owner[row][col] == 0)) {
                aroma[row][col] *= AROMA_ANT_DISSIPATION;
                army_aroma[row][col] *= AROMA_ARMY_ANT_DISSIPATION;
            } else {
                aroma[row][col] *= AROMA_NATURAL_DISSIPATION;
                army_aroma[row][col] *= AROMA_NATURAL_DISSIPATION;
            }
        }
    }

    aroma_spread(aroma);
    aroma_spread(army_aroma);

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            square = map[row][col];
            if (square & SQUARE_FOOD) {
                aroma[row][col] += AROMA_FOOD;
            }
            if (square & SQUARE_HILL) {
                if (owner[row][col]) {
                    aroma[row][col] += AROMA_ENEMY_HILL;
                    army_aroma[row][col] += AROMA_ENEMY_HILL;
                }
            }
            aroma[row][col] += AROMA_MYSTERY * mystery[row][col] / MYSTERY_MAX;
            // if (!(square & (SQUARE_LAND | SQUARE_WATER))) {
            //     aroma[row][col] += AROMA_UNDISCOVERED;
            // }
            if (square & SQUARE_ANT) {
                if (owner[row][col]) {
                    if (holy_ground[row][col]) {
                        aroma[row][col] += AROMA_INTRUDER;
                        army_aroma[row][col] += AROMA_INTRUDER;
                    } else {
                        // aroma[row][col] += AROMA_ENEMY;
                        army_aroma[row][col] += AROMA_ENEMY;
                    }
                } else {
                    for (direction = 0; direction < 4; direction++) {
                        neighbor(row, col, direction, &row2, &col2);
                        if (enemy_could_attack[row][col] == 0 && enemy_could_attack[row2][col2] > 0) {
                            // aroma[row][col] += AROMA_CONFLICT;
                            // aroma[row2][col2] += AROMA_CONFLICT_FRONT;
                            army_aroma[row2][col2] += AROMA_CONFLICT_FRONT;
                            // neighbor(row, col, (direction + 1) % 4, &row2, &col2);
                            // aroma[row2][col2] += AROMA_CONFLICT_SIDE;
                            // neighbor(row, col, (direction + 2) % 4, &row2, &col2);
                            // aroma[row2][col2] += AROMA_CONFLICT_REAR;
                            // neighbor(row, col, (direction + 3) % 4, &row2, &col2);
                            // aroma[row2][col2] += AROMA_CONFLICT_SIDE;
                        }
                    }
                }
            }
        }
    }
}

void aroma_stabilize() {
    aroma_reset();
    int i;
    for (i = 0; i < 20; i++) {
        aroma_iterate();
    }
}

char *aroma_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    int row, col;
    char c;
    float v;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            v = aroma[row][col];
            if (v <= 0) c = '-';
            else if (v < 1) c = '0';
            else if (v < 2) c = '1';
            else if (v < 4) c = '2';
            else if (v < 8) c = '3';
            else if (v < 16) c = '4';
            else if (v < 32) c = '5';
            else if (v < 64) c = '6';
            else if (v < 128) c = '7';
            else if (v < 256) c = '8';
            else if (v < 512) c = '9';
            else if (v < 1024) c = 'a';
            else if (v < 2048) c = 'b';
            else if (v < 4096) c = 'c';
            else if (v < 8192) c = 'c';
            else if (v < 16384) c = 'c';
            else if (v < 32768) c = 'd';
            else if (v < 65536) c = 'e';
            else c = '+';

            *output++ = c;
        }
        *output++ = '\n';
    }
    *--output = '\0';
    return buffer;
}

char *army_aroma_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    int row, col;
    char c;
    float v;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            v = army_aroma[row][col];
            if (v <= 0) c = '-';
            else if (v < 1) c = '0';
            else if (v < 2) c = '1';
            else if (v < 4) c = '2';
            else if (v < 8) c = '3';
            else if (v < 16) c = '4';
            else if (v < 32) c = '5';
            else if (v < 64) c = '6';
            else if (v < 128) c = '7';
            else if (v < 256) c = '8';
            else if (v < 512) c = '9';
            else if (v < 1024) c = 'a';
            else if (v < 2048) c = 'b';
            else if (v < 4096) c = 'c';
            else if (v < 8192) c = 'c';
            else if (v < 16384) c = 'c';
            else if (v < 32768) c = 'd';
            else if (v < 65536) c = 'e';
            else c = '+';

            *output++ = c;
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
int main(int argc, char *argv[]) {
    attackradius2 = 5;
    viewradius2 = 55;
    // puts(aroma_to_string());
    // printf("%f %f %f %f\n", aroma[0][0], aroma[0][1], aroma[0][2], aroma[0][3]);

    map_load_from_string(".%*%");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    assert(aroma[0][0] == 0);
    assert(army_aroma[0][0] == 0);

    map_load_from_string("..*%");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    assert(aroma[0][0] > 0);
    assert(army_aroma[0][0] == 0);

    map_load_from_string(".%*.");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    assert(aroma[0][0] > 0);
    assert(army_aroma[0][0] == 0);

    // map_load_from_string(".%?%");
    // holy_ground_calculate();
    // threat_calculate();
    // mystery_reset();
    // aroma_stabilize();
    // assert(aroma[0][0] == 0);
    // 
    // map_load_from_string("..?%");
    // holy_ground_calculate();
    // threat_calculate();
    // mystery_reset();
    // aroma_stabilize();
    // puts(aroma_to_string());
    // assert(aroma[0][0] > 0);

    map_load_from_string(".%...%*%\n"
                         "...%...%\n"
                         "%%%%%%%%");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    // puts(aroma_to_string());

    assert(aroma[0][0] > 0);
    assert(aroma[0][1] == 0);
    assert(aroma[0][2] > aroma[0][0]);
    assert(aroma[0][3] > aroma[0][2]);
    assert(aroma[0][4] > aroma[0][3]);
    assert(aroma[0][5] == 0);

    map_load_from_string("*1");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_reset();
    aroma_iterate();
    assert(aroma[0][0] == AROMA_FOOD);
    assert(aroma[0][1] == AROMA_ENEMY_HILL);

    map_load_from_string("0...........bb............................");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_reset();
    aroma_iterate();
    assert(HOLY_GROUND_RANGE == 12);
    // printf("%f %f %f %f\n", aroma[0][11], aroma[0][12], aroma[0][13], aroma[0][14]);
    assert(aroma[0][11] == 0.0);
    assert(aroma[0][12] == AROMA_INTRUDER);
    assert(aroma[0][13] == AROMA_ENEMY);
    assert(aroma[0][14] == 0.0);
    assert(army_aroma[0][11] == 0.0);
    assert(army_aroma[0][12] == AROMA_INTRUDER);
    assert(army_aroma[0][13] == AROMA_ENEMY);
    assert(army_aroma[0][14] == 0.0);

    map_load_from_string("?.");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_reset();
    aroma_iterate();
    assert(aroma[0][0] >= AROMA_MYSTERY * MYSTERY_INITIAL / MYSTERY_MAX - 0.00001);
    assert(aroma[0][0] <= AROMA_MYSTERY * MYSTERY_INITIAL / MYSTERY_MAX + 0.00001);
    mystery[0][0] = MYSTERY_MAX;
    aroma_reset();
    aroma_iterate();
    assert(aroma[0][0] == AROMA_MYSTERY);

    map_load_from_string("..................\n"
                         "..................\n"
                         "..................\n"
                         "..................\n"
                         "..a...b...........\n"
                         "..................\n"
                         "..................\n"
                         "..................\n"
                         "..................\n"
                         "..................\n"
                         "..................");
    // puts(map_to_string());
    holy_ground_calculate();
    // puts(holy_ground_to_string());
    threat_calculate();
    mystery_reset();
    aroma_reset();
    aroma_iterate();
    // puts(aroma_to_string());
    // printf("%f %f %f %f %f\n", aroma[4][2], aroma[4][3], aroma[5][2], aroma[3][2], aroma[4][1]);
    assert(aroma[4][2] == AROMA_CONFLICT);
    assert(aroma[4][3] == AROMA_CONFLICT_FRONT);
    assert(aroma[5][2] == AROMA_CONFLICT_SIDE);
    assert(aroma[3][2] == AROMA_CONFLICT_SIDE);
    assert(aroma[4][1] == AROMA_CONFLICT_REAR);

    puts("ok");
    return 0;
}
#endif
