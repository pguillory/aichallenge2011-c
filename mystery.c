#include "map.h"
#include "holy_ground.h"
#include "mystery.h"

void mystery_reset() {
    point p;
    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            if (map[p.row][p.col] & SQUARE_VISIBLE) {
                mystery[p.row][p.col] = 0;
            } else {
                mystery[p.row][p.col] = MYSTERY_INITIAL;
            }
        }
    }
}

void mystery_iterate() {
    point p;
    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            if (map[p.row][p.col] & SQUARE_VISIBLE) {
                mystery[p.row][p.col] = 0;
            } else {
                if ((holy_ground[p.row][p.col] == 0) && (mystery[p.row][p.col] < MYSTERY_MAX)) {
                    mystery[p.row][p.col] += 1;
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
