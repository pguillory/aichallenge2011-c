#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "map.h"

void map_reset() {
    point p;
    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            map[p.row][p.col] = 0;
            owner[p.row][p.col] = 0;
        }
    }
}

void map_begin_update() {
    point p;
    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            update[p.row][p.col] = 0;
        }
    }
}

void map_foreach(void (*f)(point)) {
    point p;
    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            f(p);
        }
    }
}

void map_finish_update() {
    point p, p2, d;
    int viewradius = ceil(sqrt(viewradius2));

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            map[p.row][p.col] &= SQUARE_LAND | SQUARE_WATER | SQUARE_FOOD | SQUARE_HILL;
        }
    }

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            if (update[p.row][p.col] & SQUARE_ANT) {
                map[p.row][p.col] |= SQUARE_ANT;
            }
        }
    }

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            if ((map[p.row][p.col] & SQUARE_ANT) && (owner[p.row][p.col] == 0)) {
                for (d.row = -viewradius; d.row <= viewradius; d.row++) {
                    for (d.col = -viewradius; d.col <= viewradius; d.col++) {
                        p2 = add_points(p, d);
                        if (distance2(p, p2) <= viewradius2) {
                            map[p2.row][p2.col] |= SQUARE_VISIBLE;
                        }
                    }
                }
            }
        }
    }

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            if (map[p.row][p.col] & SQUARE_VISIBLE) {
                if (update[p.row][p.col] & SQUARE_WATER) {
                    map[p.row][p.col] |= SQUARE_WATER;
                } else {
                    if (!(map[p.row][p.col] & SQUARE_WATER) && !(map[p.row][p.col] & SQUARE_LAND)) {
                        map[p.row][p.col] |= SQUARE_LAND;
                    }
                }
                if (update[p.row][p.col] & SQUARE_FOOD) {
                    map[p.row][p.col] |= SQUARE_FOOD;
                } else {
                    map[p.row][p.col] &= ~SQUARE_FOOD;
                }
                if (update[p.row][p.col] & SQUARE_HILL) {
                    map[p.row][p.col] |= SQUARE_HILL;
                } else {
                    map[p.row][p.col] &= ~SQUARE_HILL;
                }
            }
        }
    }
}

void map_give(point p, unsigned char mask) {
    map[p.row][p.col] |= mask;
}

void map_take(point p, unsigned char mask) {
    map[p.row][p.col] &= ~mask;
}
void map_set_owner(point p, int player) {
    owner[p.row][p.col] = player;
}

int friendly_ant_exists_at(point p) {
    return ((map[p.row][p.col] & SQUARE_ANT) && (owner[p.row][p.col] == 0));
}

int enemy_ant_exists_at(point p) {
    return ((map[p.row][p.col] & SQUARE_ANT) && (owner[p.row][p.col] > 0));
}

void map_load_from_string(char *input) {
    rows = MAX_ROWS;
    cols = MAX_COLS;
    map_reset();
    rows = 0;
    cols = 0;

    int i = 0;
    point p = {0, 0};
    char c;

    for (i = 0; ; i++) {
        c = input[i];
        if (c == '.') {
            map_give(p, SQUARE_VISIBLE | SQUARE_LAND);
        }
        else if (c == '%') {
            map_give(p, SQUARE_VISIBLE | SQUARE_WATER);
        }
        else if (c == '*') {
            map_give(p, SQUARE_VISIBLE | SQUARE_LAND | SQUARE_FOOD);
        }
        else if (c >= 'a' && c <= 'z') {
            map_give(p, SQUARE_VISIBLE | SQUARE_LAND | SQUARE_ANT);
            map_set_owner(p, c - 'a');
        }
        else if (c >= '0' && c <= '9') {;
            map_give(p, SQUARE_VISIBLE | SQUARE_LAND | SQUARE_HILL);
            map_set_owner(p, c - '0');
        }
        else if (c >= 'A' && c <= 'Z') {
            map_give(p, SQUARE_VISIBLE | SQUARE_LAND | SQUARE_ANT | SQUARE_HILL);
            map_set_owner(p, c - 'A');
        }

        if (c == '\n') {
            p.row += 1;
            if (rows <= p.row) rows = p.row + 1;
            p.col = 0;
        } else if (c == '\0') {
            break;
        } else {
            if (rows < 1) rows = 1;
            p.col += 1;
            if (cols < p.col) cols = p.col;
        }
    }
}

char *map_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    point p;
    char square;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            square = map[p.row][p.col];
            if (square & SQUARE_LAND) {
                if (square & SQUARE_FOOD) {
                    *output++ = '*';
                } else if ((square & SQUARE_ANT) && (square & SQUARE_HILL)) {
                    *output++ = 'A' + owner[p.row][p.col];
                } else if (square & SQUARE_ANT) {
                    *output++ = 'a' + owner[p.row][p.col];
                } else if (square & SQUARE_HILL) {
                    *output++ = '0' + owner[p.row][p.col];
                } else {
                    *output++ = '.';
                }
            } else if (square & SQUARE_WATER) {
                *output++ = '%';
            } else {
                *output++ = '?';
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
int main(int argc, char *argv[]) {
    char *input;

    input = "?.%*\naB2?";
    map_load_from_string(input);
    assert(rows == 2);
    assert(cols == 4);
    assert(strcmp(input, map_to_string()) == 0);

    // map_load_from_string(".%*%");
    // assert(rows == 1);
    // assert(cols == 4);
    // assert(aroma[0][0] == 0);
    // map_load_from_string("..*%");
    // assert(aroma[0][0] > 0);
    // 
    // map_load_from_string(".%?%");
    // assert(aroma[0][0] == 0);
    // map_load_from_string("..?%");
    // assert(aroma[0][0] > 0);
    // 
    // map_load_from_string(".%*%");
    // assert(aroma[0][0] == 0);
    // map_load_from_string(".%*.");
    // assert(aroma[0][0] > 0);
    // 
    // map_load_from_string("..*%");
    // assert(direction[0][0] == 'E');

    puts("ok");
    return 0;
}
#endif
