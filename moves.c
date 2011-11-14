#include <assert.h>
#include <string.h>
#include "map.h"
#include "potential_enemy.h"
#include "holy_ground.h"
#include "threat.h"
#include "aroma.h"
#include "army.h"
#include "moves.h"

bytegrid focus[2];
bytegrid could_occupy[2];
bytegrid max_focus;

move spiral_pattern[21] = {
    {.point = { 0,  0}, .dir = NORTH},
    {.point = {-1,  0}, .dir = NORTH},
    {.point = { 0,  1}, .dir = EAST},
    {.point = { 1,  0}, .dir = SOUTH},
    {.point = { 0, -1}, .dir = WEST},
    {.point = {-2,  0}, .dir = NORTH},
    {.point = { 0,  2}, .dir = EAST},
    {.point = { 2,  0}, .dir = SOUTH},
    {.point = { 0, -2}, .dir = WEST},
    {.point = {-1,  1}, .dir = NORTH},
    {.point = { 1,  1}, .dir = EAST},
    {.point = { 1, -1}, .dir = SOUTH},
    {.point = {-1, -1}, .dir = WEST},
    {.point = {-2,  1}, .dir = NORTH},
    {.point = {-1,  2}, .dir = EAST},
    {.point = { 1,  2}, .dir = EAST},
    {.point = { 2,  1}, .dir = SOUTH},
    {.point = { 2, -1}, .dir = SOUTH},
    {.point = { 1, -2}, .dir = WEST},
    {.point = {-1, -2}, .dir = WEST},
    {.point = {-2, -1}, .dir = NORTH},
};


void reset_at(point p) {
    int player, dir;

    if (map_has_ant(p)) {
        player = grid(owner, p) ? 1 : 0;

        grid(moves[player], p) = NORTH | EAST | SOUTH | WEST | STAY;

        grid(could_occupy[player], p) = 1;

        for (dir = 1; dir < STAY; dir *= 2) {
            grid(could_occupy[player], neighbor(p, dir)) = 1;
        }
    }
}

void prune_impossible_moves_into(point p) {
    point p2;
    int dir, player;

    if (map_has_water(p) || map_has_food(p)) {
        for (dir = 1; dir < STAY; dir *= 2) {
            p2 = neighbor(p, dir);
            if (map_has_ant(p2)) {
                player = grid(owner, p2) ? 1 : 0;
                grid(moves[player], p2) &= ~backward(dir);
            }
        }
        for (player = 0; player < 2; player++) {
            grid(could_occupy[player], p) = 0;
        }
    }
}

void prune_kamikazee_moves_into(point p) {
    if (grid(could_occupy[0], p) && grid(could_occupy[1], p)) {
        grid(could_occupy[0], p) = 0;

        grid(moves[0], p) &= ~STAY;
        grid(moves[0], neighbor(p, SOUTH)) &= ~NORTH;
        grid(moves[0], neighbor(p, WEST)) &= ~EAST;
        grid(moves[0], neighbor(p, NORTH)) &= ~SOUTH;
        grid(moves[0], neighbor(p, EAST)) &= ~WEST;
    }
}

void reset() {
    memset(moves, 0, sizeof(moves));
    memset(could_occupy, 0, sizeof(could_occupy));

    foreach_point(reset_at);
    foreach_point(prune_impossible_moves_into);
    foreach_point(prune_kamikazee_moves_into);
}




void calculate_focus_at(point p, int player) {
    // printf("calculate_focus_at(p=%i:%i) player=%i\n", p.row, p.col, player);

    static bytegrid local_moves;
    assert(sizeof(local_moves) == sizeof(moves[0]));
    memcpy(local_moves, moves[!player], sizeof(local_moves));

    int i, j;
    point p2, p3;
    int dirs[5], dir;
    move move;
    int count = 0;

    for (i = 0; i < 21; i++) {
        move = spiral_pattern[i];
        p2 = add_points(p, move.point);
        dirs[0] = STAY;
        dirs[1] = move.dir;
        dirs[2] = right(move.dir);
        dirs[3] = left(move.dir);
        dirs[4] = backward(move.dir);
        for (j = 0; j < 5; j++) {
            dir = dirs[j];
            p3 = neighbor(p2, dir);
            if (grid(local_moves, p3) & backward(dir)) {
                grid(local_moves, p3) = 0;
                grid(local_moves, neighbor(p3, NORTH)) &= ~SOUTH;
                grid(local_moves, neighbor(p3, EAST)) &= ~WEST;
                grid(local_moves, neighbor(p3, SOUTH)) &= ~NORTH;
                grid(local_moves, neighbor(p3, WEST)) &= ~EAST;
                count += 1;
                break;
            }
        }
    }

    grid(focus[player], p) = count;
}

void calculate_focus2(point p) {
    if (grid(could_occupy[0], p)) {
        calculate_focus_at(p, 0);
    } else if (grid(could_occupy[1], p)) {
        calculate_focus_at(p, 1);
    }
}

void calculate_max_focus(point p) {
    int i;
    point p2;

    if (grid(could_occupy[1], p)) {
        for (i = 0; i < 21; i++) {
            p2 = add_points(p, spiral_pattern[i].point);
            if (grid(max_focus, p2) > grid(focus[1], p)) {
                grid(max_focus, p2) = grid(focus[1], p);
            }
        }
    }
}

void adjust_max_focus(point p) {
    // (grid(holy_ground, p) == 0) &&
    // (friendly_ant_count < potential_enemy_ant_count / 10) &&
    // (grid(visible_ally_count, p) < 20)


    if ((grid(holy_ground, p) < 5) ||
        // (friendly_ant_count > potential_enemy_ant_count / 10) ||
        (grid(visible_ally_count, p) > 15)
    ) {
        grid(max_focus, p) += 1;
    }
}

void calculate_focus() {
    memset(focus, 0, sizeof(focus));
    memset(max_focus, 100, sizeof(max_focus));

    foreach_point(calculate_focus2);
    foreach_point(calculate_max_focus);
    foreach_point(adjust_max_focus);
}

void prune_outnumbered_moves_at(point p) {
    point p2;
    int dir;

    if (map_has_friendly_ant(p)) {
        for (dir = 1; dir <= STAY; dir *= 2) {
            if (grid(moves[0], p) & dir) {
                p2 = neighbor(p, dir);
                // printf("checking move %i:%i to  %i:%i: is %i > %i?\n", p.row, p.col, p2.row, p2.col, grid(focus[0], p2), grid(max_focus, p2));
                if (grid(focus[0], p2) >= grid(max_focus, p2)) {
                    grid(moves[0], p) &= ~dir;
                }
            }
        }
    }
}

void prune_outnumbered_moves() {
    foreach_point(prune_outnumbered_moves_at);
}




void cornered_ants_take_revenge_at(point p) {
    int dir;
    point p2;

    if (map_has_friendly_ant(p) && (grid(moves[0], p) == 0)) {
        grid(moves[0], p) = STAY;

        for (dir = 1; dir < STAY; dir *= 2) {
            p2 = neighbor(p, dir);
            if (map_has_land(p2) && !map_has_food(p2)) {
                grid(moves[0], p) |= dir;
            }
        }
    }
}

void cornered_ants_take_revenge() {
    foreach_point(cornered_ants_take_revenge_at);
}





int available_move_count;
float best_value;
point best_p;
char best_dir;
float worst_value;
point worst_p;
char worst_dir;

float value_at(point p) {
    if (grid(focus[0], p) < grid(max_focus, p)) {
        return grid(aroma, p) + grid(threat, p) * 10000.0;
    } else {
        return grid(aroma, p) - grid(threat, p) * 10000.0;
    }
}

float army_value_at(point p) {
    if (grid(focus[0], p) < grid(max_focus, p)) {
        return grid(army_aroma, p) + grid(threat, p) * 10000.0;
    } else {
        return grid(army_aroma, p) - grid(threat, p) * 10000.0;
    }
}

float move_value(point p, int move) {
    point p2;

    switch (move) {
        case NORTH:
        case EAST:
        case SOUTH:
        case WEST:
            p2 = neighbor(p, move);
            break;
        case STAY:
            return 0;
        default:
            assert(0);
    }

    if (grid(army, p)) {
        return army_value_at(p2) - army_value_at(p);
    } else {
        return value_at(p2) - value_at(p);
    }
}

void consider_available_moves_at(point p) {
    int dir;
    float value;

    if (map_has_friendly_ant(p)) {
        switch (grid(moves[0], p)) {
            case 0:
            case NORTH:
            case EAST:
            case SOUTH:
            case WEST:
            case STAY:
                break;
            default:
                for (dir = 1; dir <= STAY; dir *= 2) {
                    if (grid(moves[0], p) & dir) {
                        available_move_count += 1;
                        value = move_value(p, dir);

                        if ((available_move_count == 1) || (best_value < value)) {
                            best_value = value;
                            best_p = p;
                            best_dir = dir;
                        }

                        if ((available_move_count == 1) || (worst_value > value)) {
                            worst_value = value;
                            worst_p = p;
                            worst_dir = dir;
                        }
                    }
                }
                break;
        }
    }
}

void select_move(point p, int dir);

void exclude_move(point p, int dir) {
    if (grid(moves[0], p) & dir) {
        grid(moves[0], p) &= ~dir;

        switch (grid(moves[0], p)) {
            case NORTH:
            case EAST:
            case SOUTH:
            case WEST:
            case STAY:
                select_move(p, grid(moves[0], p));
                break;
        }
    }
}

void select_move(point p, int dir) {
    point p2;

    grid(moves[0], p) = dir;
    p2 = neighbor(p, dir);
    if (dir != NORTH) exclude_move(neighbor(p2, SOUTH), NORTH);
    if (dir != EAST)  exclude_move(neighbor(p2, WEST),  EAST);
    if (dir != SOUTH) exclude_move(neighbor(p2, NORTH), SOUTH);
    if (dir != WEST)  exclude_move(neighbor(p2, EAST),  WEST);
    if (dir != STAY)  exclude_move(p2, STAY);
}

void save_crushed_ant(point p) {
    point p2;

    if (map_has_friendly_ant(p) && (grid(moves[0], p) == 0)) {
        grid(moves[0], p) = STAY;
        p2 = neighbor(p, NORTH); exclude_move(p2, SOUTH); save_crushed_ant(p2);
        p2 = neighbor(p, EAST);  exclude_move(p2, WEST);  save_crushed_ant(p2);
        p2 = neighbor(p, SOUTH); exclude_move(p2, NORTH); save_crushed_ant(p2);
        p2 = neighbor(p, WEST);  exclude_move(p2, EAST);  save_crushed_ant(p2);
    }

    // if (map_has_friendly_ant(p)) {
    //     switch (grid(moves[0], p)) {
    //         case 0:
    //             grid(moves[0], p) = STAY;
    //         case STAY:
    //             p2 = neighbor(p, NORTH); exclude_move(p2, SOUTH); save_crushed_ant(p2);
    //             p2 = neighbor(p, EAST);  exclude_move(p2, WEST);  save_crushed_ant(p2);
    //             p2 = neighbor(p, SOUTH); exclude_move(p2, NORTH); save_crushed_ant(p2);
    //             p2 = neighbor(p, WEST);  exclude_move(p2, EAST);  save_crushed_ant(p2);
    //     }
    // }
}

void resolve_short_loops(point p1) {
    point p2, p3;

    if (map_has_friendly_ant(p1)) {
        if (grid(moves[0], p1) != STAY) {
            p2 = neighbor(p1, grid(moves[0], p1));
            if (map_has_friendly_ant(p2)) {
                p3 = neighbor(p2, grid(moves[0], p2));
                if (points_equal(p1, p3)) {
                    grid(moves[0], p1) = STAY;
                    grid(moves[0], p2) = STAY;
                }
            }
        }
    }
}








void moves_calculate() {
    reset();

    calculate_focus();
    prune_outnumbered_moves();
    calculate_focus();
    prune_outnumbered_moves();
    calculate_focus();
    prune_outnumbered_moves();
    calculate_focus();
    prune_outnumbered_moves();

    cornered_ants_take_revenge();

    do {
        available_move_count = 0;
        foreach_point(consider_available_moves_at);
        if (available_move_count == 0) break;
        select_move(best_p, best_dir);
        exclude_move(worst_p, worst_dir);
    } while (1);

    foreach_point(save_crushed_ant);
    foreach_point(resolve_short_loops);
}






char attack_moves_to_string_at(point p) {
    if (map_has_water(p)) {
        return '%';
    } else if (map_has_land(p)) {
        if (map_has_friendly_ant(p)) {
            switch (grid(moves[0], p)) {
                case 0:
                    return '-';
                case NORTH:
                    return 'N';
                case EAST:
                    return 'E';
                case SOUTH:
                    return 'S';
                case WEST:
                    return 'W';
                case STAY:
                    return 'X';
                default:
                    return '+';
            }
        } else if (map_has_enemy_ant(p)) {
            return 'a' + grid(owner, p);
        } else {
            return '.';
        }
    } else {
        return '?';
    }
}

char *attack_moves_to_string() {
    return point_callback_to_string(attack_moves_to_string_at);
}

char focus_to_string_at(point p) {
    if (map_has_water(p)) {
        return '%';
    } else if (grid(could_occupy[0], p)) {
        if (grid(focus[0], p) < 10) {
            return '0' + grid(focus[0], p);
        } else {
            return '+';
        }
    } else if (grid(could_occupy[1], p)) {
        if (grid(focus[1], p) < 10) {
            return '0' + grid(focus[1], p);
        } else {
            return '+';
        }
    } else {
        return '.';
    }
}

char *focus_to_string() {
    return point_callback_to_string(focus_to_string_at);
}

char max_focus_to_string_at(point p) {
    if (map_has_water(p)) {
        return '%';
    } else if (grid(could_occupy[0], p)) {
        if (grid(max_focus, p) < 10) {
            return '0' + grid(max_focus, p);
        } else {
            return '+';
        }
    } else {
        return '.';
    }
}

char *max_focus_to_string() {
    return point_callback_to_string(max_focus_to_string_at);
}

#ifdef UNIT_TESTS
#undef UNIT_TESTS
#include "globals.c"
#include "map.c"
#include "holy_ground.c"
#include "threat.c"
#include "mystery.c"
#include "aroma.c"
#include "army.c"
int main(int argc, char *argv[]) {
    attackradius2 = 5;

    // input = "..........\n"
    //         "..........\n"
    //         "..........\n"
    //         "..........\n"
    //         "..........";
    // map_load_from_string(input);
    // threat_calculate();
    // moves_calculate();
    // puts(attack_moves_to_string());
    // puts(focus_to_string());
    // assert(moves[0][1][0] == 0);
    // 
    // input = "..........\n"
    //         "a.........\n"
    //         "..........\n"
    //         "..........\n"
    //         "..........";
    // map_load_from_string(input);
    // threat_calculate();
    // moves_calculate();
    // puts(attack_moves_to_string());
    // puts(focus_to_string());
    // printf("moves[0][1][0] %i\n", moves[0][1][0]);
    // assert(moves[0][1][0] == (NORTH | SOUTH | EAST | WEST | STAY));

    map_load_from_string("...........\n"
                         ".a...b.....\n"
                         "...........\n"
                         "...........\n"
                         "...........");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    army_calculate();
    moves_calculate();
    assert((moves[0][1][1] & EAST) == 0);

    map_load_from_string("...........\n"
                         ".a...b.....\n"
                         ".a.........\n"
                         "...........\n"
                         "...........");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    army_calculate();
    moves_calculate();
    puts(focus_to_string());
    puts(max_focus_to_string());
    puts(threat_to_string());
    puts(attack_moves_to_string());
    assert(moves[0][1][1] & EAST);

    map_load_from_string("...........\n"
                         ".a...b.....\n"
                         ".a...b.....\n"
                         "...........\n"
                         "...........");
    moves_calculate();
    assert((moves[0][1][1] & EAST) == 0);

    // input = "...........\n"
    //         ".a..b......\n"
    //         "...........\n"
    //         "...........\n"
    //         "...........";
    // map_load_from_string(input);
    // threat_calculate();
    // moves_calculate();
    // puts(attack_moves_to_string());
    // assert(moves[0][1][0] == 0); //(EAST | NORTH | SOUTH | STAY));
    // 
    // input = "%.........\n"
    //         "a..b......\n"
    //         "%.........\n"
    //         "..........\n"
    //         "..........";
    // map_load_from_string(input);
    // threat_calculate();
    // moves_calculate();
    // puts(attack_moves_to_string());
    // assert(moves[0][1][0] == 0);
    // 
    // input = "a%........\n"
    //         "..........\n"
    //         "...b......\n"
    //         "..........\n"
    //         "..........";
    // map_load_from_string(input);
    // threat_calculate();
    // moves_calculate();
    // puts(attack_moves_to_string());
    // assert(moves[0][0][0] == 0);
    // 
    // input = "..........\n"
    //         "..........\n"
    //         "a.........\n"
    //         "a...b.....\n"
    //         "a.........\n"
    //         "..........\n"
    //         "..........\n"
    //         "..........";
    // map_load_from_string(input);
    // threat_calculate();

    // moves_calculate();

    puts("ok");
    return 0;
}
#endif
