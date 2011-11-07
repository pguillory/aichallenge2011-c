#include <assert.h>
#include <string.h>
#include "globals.h"
#include "map.h"
#include "potential_enemy.h"
#include "holy_ground.h"
#include "threat.h"
#include "mystery.h"
#include "aroma.h"
#include "army.h"
#include "moves.h"
#include "server.h"
#include "bot.h"

void dump_state() {
    // logs("map:");
    logs(map_to_string());
    // logs("holy ground:");
    // logs(holy_ground_to_string());
    // logs("threat:");
    // logs(threat_to_string());
    // // logs(mystery_to_string());
    // logs("aroma:");
    // logs(aroma_to_string());
    // logs("moves:");
    // logs(moves_to_string());
}

void bot_init() {
    map_reset();
    potential_enemy_reset();
    mystery_reset();
    aroma_reset();
    server_go();
}

void bot_begin_turn() {
    map_begin_update();
}

void bot_see_water(point p) {
    grid(update, p) |= SQUARE_WATER;
}

void bot_see_food(point p) {
    grid(update, p) |= SQUARE_FOOD;
}

void bot_see_ant(point p, int player) {
    grid(update, p) |= SQUARE_ANT;
    grid(owner, p) = player;
}

void bot_see_hill(point p, int player) {
    grid(update, p) |= SQUARE_HILL;
    grid(owner, p) = player;
}

void bot_see_dead_ant(point p, int player) {
    if (player) {
        enemy_dead_ant_count += 1;
    } else {
        friendly_dead_ant_count += 1;
    }
}

void issue_order_at(point p) {
    if (map_has_friendly_ant(p)) {
        switch (grid(moves, p)) {
            case NORTH:
                server_order(p, 'N');
                break;
            case EAST:
                server_order(p, 'E');
                break;
            case SOUTH:
                server_order(p, 'S');
                break;
            case WEST:
                server_order(p, 'W');
                break;
        }
    }
}

void bot_end_turn() {
    long int start_time;
    int map_time, potential_enemy_time, holy_ground_time, threat_time, mystery_time, aroma_time, army_time, moves_time;

    start_time = now();
    map_finish_update();
    map_time = now() - start_time;

    start_time = now();
    potential_enemy_iterate();
    potential_enemy_time = now() - start_time;

    start_time = now();
    holy_ground_calculate();
    holy_ground_time = now() - start_time;

    start_time = now();
    threat_calculate();
    threat_time = now() - start_time;

    start_time = now();
    mystery_iterate();
    mystery_time = now() - start_time;

    start_time = now();
    aroma_iterate(100);
    aroma_time = now() - start_time;

    start_time = now();
    army_calculate();
    army_time = now() - start_time;

    start_time = now();
    moves_calculate();
    moves_time = now() - start_time;

    foreach_point(issue_order_at);
    server_go();

#ifdef DEBUG
    // dump_state();
    fprintf(logfile, "turn %i, friendly %i (+%i), enemy %i..%i, times %i %i %i %i %i %i %i %i\n",
            turn,
            friendly_ant_count, food_consumed + initial_friendly_ant_count - friendly_ant_count - friendly_dead_ant_count,
            visible_enemy_ant_count, potential_enemy_ant_count,
            map_time, potential_enemy_time, holy_ground_time, threat_time, mystery_time, aroma_time, army_time, moves_time);
    fflush(logfile);
#endif
}

#ifdef UNIT_TESTS
#undef UNIT_TESTS
#include "globals.c"
#include "map.c"
#include "moves.c"
#include "server.c"
int main(int argc, char *argv[]) {

    puts("ok");
    return 0;
}
#endif
