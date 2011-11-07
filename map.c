#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "map.h"

void map_give(point p, unsigned char mask) {
    grid(map, p) |= mask;
}

void map_take(point p, unsigned char mask) {
    grid(map, p) &= ~mask;
}

void map_set_owner(point p, int player) {
    grid(owner, p) = player;
}

int map_is_visible(point p) {
    return (grid(map, p) & SQUARE_VISIBLE);
}

int map_has_land(point p) {
    return (grid(map, p) & SQUARE_LAND);
}

int map_has_water(point p) {
    return (grid(map, p) & SQUARE_WATER);
}

int map_has_food(point p) {
    return (grid(map, p) & SQUARE_FOOD);
}

int map_has_ant(point p) {
    return (grid(map, p) & SQUARE_ANT);
}

int map_has_hill(point p) {
    return (grid(map, p) & SQUARE_HILL);
}

int map_is_friendly(point p) {
    return (grid(owner, p) == 0);
}

int map_is_enemy(point p) {
    return (grid(owner, p) > 0);
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
    grid(map, p) = 0;
    grid(owner, p) = 0;
}

void map_reset() {
    foreach_point(reset_map_at_point);
    food_consumed = 0;
    friendly_dead_ant_count = 0;
    enemy_dead_ant_count = 0;
}

void map_begin_update_at(point p) {
    grid(update, p) = 0;
    grid(visible_ally_count, p) = 0;
}

void map_begin_update() {
    foreach_point(map_begin_update_at);
}

void carry_over_persistent_bits(point p) {
    grid(map, p) &= SQUARE_LAND | SQUARE_WATER | SQUARE_FOOD | SQUARE_HILL;
}

void accept_ant_updates_at(point p) {
    if (grid(update, p) & SQUARE_ANT) {
        grid(map, p) |= SQUARE_ANT;
        if (map_is_friendly(p)) {
            friendly_ant_count += 1;
        } else {
            visible_enemy_ant_count += 1;
        }
    }
}

void accept_ant_updates() {
    friendly_ant_count = 0;
    visible_enemy_ant_count = 0;
    foreach_point(accept_ant_updates_at);
    if (turn == 1) {
        initial_friendly_ant_count = friendly_ant_count;
    }
}


void give_visible_at(point p) {
    map_give(p, SQUARE_VISIBLE);
    grid(visible_ally_count, p) += 1;
}

void calculate_visibility(point p) {
    if (map_has_friendly_ant(p)) {
        foreach_point_within_radius2(p, viewradius2, give_visible_at);
    }
}

void accept_other_updates_at(point p) {
    int dir;
    point p2;

    if (map_is_visible(p)) {
        if (grid(update, p) & SQUARE_WATER) {
            map_give(p, SQUARE_WATER);
        } else {
            if (!map_has_water(p) && !map_has_land(p)) {
                map_give(p, SQUARE_LAND);
            }
        }
        if (grid(update, p) & SQUARE_FOOD) {
            map_give(p, SQUARE_FOOD);
        } else {
            if (map_has_food(p)) {
                for (dir = 1; dir < STAY; dir *= 2) {
                    p2 = neighbor(p, dir);
                    if (map_has_friendly_ant(p2)) {
                        food_consumed += 1;
                        break;
                    }
                }
                map_take(p, SQUARE_FOOD);
            }
        }
        if (grid(update, p) & SQUARE_HILL) {
            map_give(p, SQUARE_HILL);
        } else {
            map_take(p, SQUARE_HILL);
        }
    }
}

void accept_other_updates() {
    foreach_point(accept_other_updates_at);
}

void map_finish_update() {
    foreach_point(carry_over_persistent_bits);

    accept_ant_updates();

    foreach_point(calculate_visibility);

    accept_other_updates();
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
            square = grid(map, p);
            if (square & SQUARE_LAND) {
                if (square & SQUARE_FOOD) {
                    *output++ = '*';
                } else if ((square & SQUARE_ANT) && (square & SQUARE_HILL)) {
                    *output++ = 'A' + grid(owner, p);
                } else if (square & SQUARE_ANT) {
                    *output++ = 'a' + grid(owner, p);
                } else if (square & SQUARE_HILL) {
                    *output++ = '0' + grid(owner, p);
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
