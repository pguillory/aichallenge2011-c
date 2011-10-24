#define __GAME_C

#include <assert.h>
#include "game.h"

int abs(int x) {
    return (x >= 0) ? x : -x;
}

int distance2(int row1, int col1, int row2, int col2) {
    int dr, dc;
    int abs1, abs2;

    abs1 = abs(row1 - row2);
    abs2 = rows - abs(row1 - row2);

    dr = (abs1 < abs2) ? abs1 : abs2;

    abs1 = abs(col1 - col2);
    abs2 = cols - abs(col1 - col2);

    dc = (abs1 < abs2) ? abs1 : abs2;

    return dr * dr + dc * dc;
}

void bot_init() {

}

void bot_turn() {
    
}

#ifdef UNIT_TESTS
int main(int argc, char *argv[]) {
    rows = 10;
    cols = 10;

    assert(distance2(0, 0, -1, -1) == 2);
    assert(distance2(0, 0,  0,  0) == 0);
    assert(distance2(0, 0,  0,  1) == 1);
    assert(distance2(0, 0,  0,  2) == 4);
    assert(distance2(0, 0,  2,  2) == 8);
    assert(distance2(0, 0,  9,  9) == 2);

    puts("ok");
    return 0;
}
#endif
