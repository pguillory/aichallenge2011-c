#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "map.h"

void map_give(point p, unsigned char mask) {
    map[p.row][p.col] |= mask;
}

void map_give_visible(point p) {
    map_give(p, SQUARE_VISIBLE);
}

void map_take(point p, unsigned char mask) {
    map[p.row][p.col] &= ~mask;
}

void map_set_owner(point p, int player) {
    owner[p.row][p.col] = player;
}

int map_is_visible(point p) {
    return (map[p.row][p.col] & SQUARE_VISIBLE);
}

int map_has_land(point p) {
    return (map[p.row][p.col] & SQUARE_LAND);
}

int map_has_water(point p) {
    return (map[p.row][p.col] & SQUARE_WATER);
}

int map_has_food(point p) {
    return (map[p.row][p.col] & SQUARE_FOOD);
}

int map_has_ant(point p) {
    return (map[p.row][p.col] & SQUARE_ANT);
}

int map_has_hill(point p) {
    return (map[p.row][p.col] & SQUARE_HILL);
}

int map_is_friendly(point p) {
    return (owner[p.row][p.col] == 0);
}

int map_is_enemy(point p) {
    return (owner[p.row][p.col] > 0);
}

int map_has_friendly_ant(point p) {
    return (map_has_ant(p) && map_is_friendly(p));
}

int map_has_enemy_ant(point p) {
    return (map_has_ant(p) && map_is_enemy(p));
}

int map_has_friendly_hill(point p) {
    return (map_has_hill(p) && map_is_friendly(p));
}

int map_has_enemy_hill(point p) {
    return (map_has_hill(p) && map_is_enemy(p));
}

void reset_map_at_point(point p) {
    map[p.row][p.col] = 0;
    owner[p.row][p.col] = 0;
}

void map_reset() {
    foreach_point(reset_map_at_point);
}

void reset_update_at_point(point p) {
    update[p.row][p.col] = 0;
}

void map_begin_update() {
    foreach_point(reset_update_at_point);
}

void carry_over_persistent_bits(point p) {
    map[p.row][p.col] &= SQUARE_LAND | SQUARE_WATER | SQUARE_FOOD | SQUARE_HILL;
}

void accept_ant_updates(point p) {
    if (update[p.row][p.col] & SQUARE_ANT) {
        map[p.row][p.col] |= SQUARE_ANT;
    }
}

void calculate_visibility(point p) {
    if (map_has_friendly_ant(p)) {
        foreach_point_within_radius2(p, viewradius2, map_give_visible);
    }
}

void accept_other_updates(point p) {
    if (map_is_visible(p)) {
        if (update[p.row][p.col] & SQUARE_WATER) {
            map_give(p, SQUARE_WATER);
        } else {
            if (!map_has_water(p) && !map_has_land(p)) {
                map_give(p, SQUARE_LAND);
            }
        }
        if (update[p.row][p.col] & SQUARE_FOOD) {
            map_give(p, SQUARE_FOOD);
        } else {
            map_take(p, SQUARE_FOOD);
        }
        if (update[p.row][p.col] & SQUARE_HILL) {
            map_give(p, SQUARE_HILL);
        } else {
            map_take(p, SQUARE_HILL);
        }
    }
}

void map_finish_update() {
    foreach_point(carry_over_persistent_bits);

    foreach_point(accept_ant_updates);

    foreach_point(calculate_visibility);

    foreach_point(accept_other_updates);
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
