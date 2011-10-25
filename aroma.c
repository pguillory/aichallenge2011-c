#include <assert.h>
#include <string.h>
#include "map.h"
#include "aroma.h"

void aroma_reset() {
    int row, col;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            aroma[row][col] = 0.0;
        }
    }
}

void aroma_iterate() {
    int row, col;
    char square;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            square = map[row][col];
            if (square & SQUARE_FOOD_MASK) {
                aroma[row][col] += 100.0;
            }
            if (square & SQUARE_ANT_MASK) {
                aroma[row][col] *= 0.0;
            }
            if (square & SQUARE_HILL_MASK) {
                if (owner[row][col]) {
                    aroma[row][col] += 50.0;
                }
            }
            if (!(square & SQUARE_VISIBLE_MASK)) {
                aroma[row][col] += 5.0;
            }
            if (!(square & SQUARE_SEEN_MASK)) {
                aroma[row][col] += 50.0;
            }
        }
    }

    static float aroma2[MAX_ROWS][MAX_COLS];
    int row2, col2;
    char square2;
    float sum;
    int count;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            sum = 0;
            count = 0;
            square = map[row][col];
            if (!(square & SQUARE_WATER_MASK)) {
                sum += aroma[row][col];
                count += 1;

                row2 = normalize_row(row - 1);
                col2 = col;
                square2 = map[row2][col2];
                if (!(square2 & SQUARE_WATER_MASK)) {
                    sum += aroma[row2][col2];
                    count += 1;
                }

                row2 = normalize_row(row + 1);
                col2 = col;
                square2 = map[row2][col2];
                if (!(square2 & SQUARE_WATER_MASK)) {
                    sum += aroma[row2][col2];
                    count += 1;
                }

                row2 = row;
                col2 = normalize_col(col + 1);
                square2 = map[row2][col2];
                if (!(square2 & SQUARE_WATER_MASK)) {
                    sum += aroma[row2][col2];
                    count += 1;
                }

                row2 = row;
                col2 = normalize_col(col - 1);
                square2 = map[row2][col2];
                if (!(square2 & SQUARE_WATER_MASK)) {
                    sum += aroma[row2][col2];
                    count += 1;
                }
            }
            // printf("%i:%i sum=%0.2f count=%i\n", row, col, sum, count);
            if (count) {
                aroma2[row][col] = sum / count * 0.9;
            } else {
                aroma2[row][col] = 0.0;
            }
        }
    }

    assert(sizeof(aroma) == sizeof(aroma2));
    memcpy(aroma, aroma2, sizeof(aroma));
}

void aroma_stabilize() {
    aroma_reset();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
    aroma_iterate();
}
// void calculate_smell() {
//     int row, col;
//     char square;
//     for (row = 0; row < rows; row++) {
//         for (col = 0; col < cols; col++) {
//             square = map[row][col];
//             if (square & SQUARE_ANT_MASK) {
//                 if (owner[row][col]) {
//                     smell(row, col, -1, 0.9);
//                 } else {
//                     smell(row, col, -1, 0.9);
//                     see(row, col);
//                 }
//             }
//             if (square & SQUARE_HILL_MASK) {
//                 if (owner[row][col]) {
//                     smell(row, col, 1, 0.9);
//                 // } else {
//                 //     smell(row, col, -1, 0.5);
//                 }
//             }
//             if (square & SQUARE_FOOD_MASK) {
//                 smell(row, col, 2, 0.8);
//             }
//             if ((square & SQUARE_SEEN_MASK) == 0) {
//                 smell(row, col, 1, 0.9);
//             }
//         }
//     }
// }

// char *aroma_to_chart() {
//     static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
//     char *output = buffer;
//     int row, col;
// 
//     for (row = 0; row < rows; row++) {
//         for (col = 0; col < cols; col++) {
//             printf("| %0.2f ", aroma[row][col]);
//         }
//         printf("|\n");
//     }
// }

char *aroma_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    int row, col;
    char c;
    float v;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            v = aroma[row][col];
            if (v <= 0) {
                c = '-';
            } else if (v < 1) {
                c = '0';
            } else if (v < 2) {
                c = '1';
            } else if (v < 4) {
                c = '2';
            } else if (v < 8) {
                c = '3';
            } else if (v < 16) {
                c = '4';
            } else if (v < 32) {
                c = '5';
            } else if (v < 64) {
                c = '6';
            } else if (v < 128) {
                c = '7';
            } else if (v < 256) {
                c = '8';
            } else if (v < 512) {
                c = '9';
            } else {
                c = '+';
            }

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
int main(int argc, char *argv[]) {
    // puts(aroma_to_string());
    // printf("%i %i %i %i\n", map[0][0], map[0][1], map[0][2], map[0][3]);

    map_load_from_string(".%*%");
    aroma_stabilize();
    assert(aroma[0][0] == 0);

    map_load_from_string("..*%");
    aroma_stabilize();
    assert(aroma[0][0] > 0);

    map_load_from_string(".%?%");
    aroma_stabilize();
    assert(aroma[0][0] == 0);

    map_load_from_string("..?%");
    aroma_stabilize();
    assert(aroma[0][0] > 0);

    // map_load_from_string(".%*%");
    // assert(aroma[0][0] == 0);
    // map_load_from_string(".%*.");
    // assert(aroma[0][0] > 0);
    // 
    // map_load_from_string("..*%");
    // assert(direction[0][0] == 'E');

    map_load_from_string(".%...%*%\n"
                         "...%...%\n"
                         "%%%%%%%%");
    // puts(map_to_string());
    aroma_stabilize();
    // puts(aroma_to_string());
    // aroma_reset();
    // printf("%f %f %f %f %f %f %f\n", aroma[0][0], aroma[0][1], aroma[0][2], aroma[0][3], aroma[0][4], aroma[0][5], aroma[0][6]);
    // aroma_iterate();

    assert(aroma[0][0] > 0);
    assert(aroma[0][1] == 0);
    assert(aroma[0][2] > aroma[0][0]);
    assert(aroma[0][3] > aroma[0][2]);
    assert(aroma[0][4] > aroma[0][3]);
    assert(aroma[0][5] == 0);
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
