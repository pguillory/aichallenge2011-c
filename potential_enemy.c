#include <assert.h>
#include <string.h>
#include "map.h"
#include "potential_enemy.h"

unsigned char potential_enemy2[MAX_ROWS][MAX_COLS];

void potential_enemy_reset_at(point p) {
    int value;
    if (map_has_water(p)) {
        value = 0;
    } else if (map_is_visible(p)) {
        value = map_has_enemy_ant(p) ? 1 : 0;
    } else {
        value = 1;
    }

    grid(potential_enemy, p) = value;
    potential_enemy_ant_count += value;
}

void potential_enemy_reset() {
    potential_enemy_ant_count = 0;
    foreach_point(potential_enemy_reset_at);
}

void propagate_potential_enemies_at(point p) {
    int value;
    int dir;
    point p2;

    if (map_has_water(p)) {
        value = 0;
    } else if (map_is_visible(p)) {
        value = map_has_enemy_ant(p) ? 1 : 0;
    } else {
        value = 0;
        for (dir = 1; dir <= STAY; dir *= 2) {
            p2 = neighbor(p, dir);
            if (grid(potential_enemy, p2)) {
                value = 1;
                break;
            }
        }
    }

    grid(potential_enemy2, p) = value;
    potential_enemy_ant_count += value;
}

void potential_enemy_iterate() {
    potential_enemy_ant_count = 0;
    foreach_point(propagate_potential_enemies_at);

    assert(sizeof(potential_enemy) == sizeof(potential_enemy2));
    memcpy(potential_enemy, potential_enemy2, sizeof(potential_enemy));
}

char *potential_enemy_to_string() {
    static char buffer[MAX_ROWS * (MAX_COLS + 1)];
    char *output = buffer;
    point p;
    // char square;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            if (map_has_water(p)) {
                *output++ = '%';
            } else {
                *output++ = '0' + grid(potential_enemy, p);
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
int main(int argc, char *argv[]) {
    char *input;

    // rows = 10;
    // cols = 10;
    // map_reset();

    input = "%%%%%%%%\n"
            "%.....?%\n"
            "%......%\n"
            "%......%\n"
            "%......%\n"
            "%%%%%%%%";
    map_load_from_string(input);
    potential_enemy_reset();

    map_begin_update();
    map_finish_update();

    // printf("potential_enemy_ant_count: %i\n", potential_enemy_ant_count);

    assert(potential_enemy_ant_count == 1);
    potential_enemy_iterate();
    assert(potential_enemy_ant_count == 3);
    potential_enemy_iterate();
    assert(potential_enemy_ant_count == 6);
    potential_enemy_iterate();
    assert(potential_enemy_ant_count == 10);
    potential_enemy_iterate();
    assert(potential_enemy_ant_count == 14);
    potential_enemy_iterate();
    assert(potential_enemy_ant_count == 18);
    potential_enemy_iterate();
    assert(potential_enemy_ant_count == 21);
    potential_enemy_iterate();
    assert(potential_enemy_ant_count == 23);
    potential_enemy_iterate();
    assert(potential_enemy_ant_count == 24);
    potential_enemy_iterate();
    assert(potential_enemy_ant_count == 24);

    map[1][1] |= SQUARE_VISIBLE;
    potential_enemy_iterate();
    puts(potential_enemy_to_string());
    assert(potential_enemy_ant_count == 23);

    // map_begin_update();
    // map_update_ant((point){0, 0}, 1);
    // grid(update, p) |= SQUARE_ANT;
    // grid(owner, p) = 1;
    // map_finish_update();
    // 
    // potential_enemy_iterate();
    // printf("potential_enemy_ant_count: %i\n", potential_enemy_ant_count);
    // puts(potential_enemy_to_string());
    // assert(potential_enemy_ant_count == 5);

    puts("ok");
    return 0;
}
#endif
