#include "map.h"
#include "mystery.h"

void mystery_reset() {
    int row, col;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if (map[row][col] & SQUARE_VISIBLE) {
                mystery[row][col] = 0;
            } else {
                mystery[row][col] = MYSTERY_INITIAL;
            }
        }
    }
}

void mystery_iterate() {
    int row, col;
    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if (map[row][col] & SQUARE_VISIBLE) {
                mystery[row][col] = 0;
            } else {
                if (mystery[row][col] < MYSTERY_MAX) {
                    mystery[row][col] += 1;
                }
            }
        }
    }
}

#ifdef UNIT_TESTS
#undef UNIT_TESTS
#include "globals.c"
int main(int argc, char *argv[]) {
    puts("ok");
    return 0;
}
#endif
