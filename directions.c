#include <assert.h>
#include <string.h>
#include "map.h"
#include "holy_ground.h"
#include "threat.h"
#include "aroma.h"
#include "army.h"
#include "directions.h"

int available_move_count;
float best_value;
point best_p;
char best_dir;
float worst_value;
point worst_p;
char worst_dir;

float value_at(point p) {
    return aroma[p.row][p.col] - threat[p.row][p.col] * 1000.0;
}

float army_value_at(point p) {
    return army_aroma[p.row][p.col] - threat[p.row][p.col] * 1000.0;
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

    if (army[p.row][p.col]) {
        return army_value_at(p2) - army_value_at(p);
    } else {
        return value_at(p2) - value_at(p);
    }
}

void reset_directions_at(point p) {
    int dir;
    point p2;

    directions[p.row][p.col] = 0;

    if (map_has_friendly_ant(p)) {
        for (dir = 1; dir <= STAY; dir *= 2) {
            p2 = neighbor(p, dir);
            if (map_has_land(p2) && !map_has_food(p2) && !enemy_could_occupy[p2.row][p2.col]) {
                directions[p.row][p.col] |= dir;
            }
        }
    }
}

void consider_available_moves_at(point p) {
    int dir;
    float value;

    if (map_has_friendly_ant(p)) {
        switch (directions[p.row][p.col]) {
            case NORTH:
            case EAST:
            case SOUTH:
            case WEST:
            case STAY:
                break;
            default:
                for (dir = 1; dir <= STAY; dir *= 2) {
                    if (directions[p.row][p.col] & dir) {
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
    if (directions[p.row][p.col] & dir) {
        directions[p.row][p.col] &= ~dir;

        switch (directions[p.row][p.col]) {
            case NORTH:
            case EAST:
            case SOUTH:
            case WEST:
            case STAY:
                select_move(p, directions[p.row][p.col]);
                break;
        }
    }
}

void select_move(point p, int dir) {
    point p2;

    directions[p.row][p.col] = dir;
    p2 = neighbor(p, dir);
    if (dir != NORTH) exclude_move(neighbor(p2, SOUTH), NORTH);
    if (dir != EAST)  exclude_move(neighbor(p2, WEST),  EAST);
    if (dir != SOUTH) exclude_move(neighbor(p2, NORTH), SOUTH);
    if (dir != WEST)  exclude_move(neighbor(p2, EAST),  WEST);
    if (dir != STAY)  exclude_move(p2, STAY);
}

void save_crushed_ant(point p) {
    point p2;

    if (map_has_friendly_ant(p) && (directions[p.row][p.col] == 0)) {
        directions[p.row][p.col] = STAY;
        p2 = neighbor(p, NORTH); exclude_move(p2, SOUTH); save_crushed_ant(p2);
        p2 = neighbor(p, EAST);  exclude_move(p2, WEST);  save_crushed_ant(p2);
        p2 = neighbor(p, SOUTH); exclude_move(p2, NORTH); save_crushed_ant(p2);
        p2 = neighbor(p, WEST);  exclude_move(p2, EAST);  save_crushed_ant(p2);
    }
}

void resolve_short_loops(point p1) {
    point p2, p3;

    if (map_has_friendly_ant(p1)) {
        if (directions[p1.row][p1.col] != STAY) {
            p2 = neighbor(p1, directions[p1.row][p1.col]);
            if (map_has_friendly_ant(p2)) {
                p3 = neighbor(p2, directions[p2.row][p2.col]);
                if (points_equal(p1, p3)) {
                    directions[p1.row][p1.col] = STAY;
                    directions[p2.row][p2.col] = STAY;
                }
            }
        }
    }
}

void directions_calculate() {
    foreach_point(reset_directions_at);

    do {
        available_move_count = 0;
        foreach_point(consider_available_moves_at);
        if (available_move_count == 0) break;
        select_move(best_p, best_dir);
        exclude_move(worst_p, worst_dir);
    } while (1);

    foreach_point(save_crushed_ant);
    foreach_point(resolve_short_loops);

    // do {
    //     // find any move
    //     for (p.row = 0; p.row < rows; p.row++) {
    //         for (p.col = 0; p.col < cols; p.col++) {
    //             if (available_moves[p.row][p.col]) {
    //                 for (dir = 1; dir <= STAY; dir *= 2) {
    //                     if (available_moves[p.row][p.col] & dir) {
    //                         best_p = p;
    //                         best_dir = dir;
    //                         best_value = move_value(p, dir);
    //                         goto moves_available;
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // 
    //     goto no_moves_available;

// moves_available:

        // // find best move
        // for (p.row = 0; p.row < rows; p.row++) {
        //     for (p.col = 0; p.col < cols; p.col++) {
        //         for (dir = 1; dir <= STAY; dir *= 2) {
        //             if (available_moves[p.row][p.col] & dir) {
        //                 value = move_value(p, dir);
        //                 if (best_value < value) {
        //                     best_p = p;
        //                     best_dir = dir;
        //                     best_value = value;
        //                 }
        //             }
        //         }
        //     }
        // }

    //     select_move(best_p, best_dir);
    // } while (1);

// no_moves_available: {}
}


char *directions_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    point p;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            switch (directions[p.row][p.col]) {
                case NORTH:
                    *output++ = 'N';
                    break;
                case EAST:
                    *output++ = 'E';
                    break;
                case SOUTH:
                    *output++ = 'S';
                    break;
                case WEST:
                    *output++ = 'W';
                    break;
                case STAY:
                    *output++ = 'X';
                    break;
                default:
                    *output++ = '.';
                    break;
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
#include "holy_ground.c"
#include "threat.c"
#include "mystery.c"
#include "aroma.c"
#include "army.c"
int main(int argc, char *argv[]) {
    char *input, *expected;
    attackradius2 = 5;
    viewradius2 = 55;
    // puts(aroma_to_string());
    // printf("%i %i %i %i\n", map[0][0], map[0][1], map[0][2], map[0][3]);

    map_load_from_string("a.*%");
    // puts(map_to_string());
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    // puts(aroma_to_string());
    army_calculate();
    directions_calculate();
    // puts(directions_to_string());
    assert(directions[0][0] == EAST);

    input = "a%...%*%\n"
            "...%...%\n"
            "%%%%%%%%";
    map_load_from_string(input);
    // puts(map_to_string());
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    // puts(aroma_to_string());
    army_calculate();
    directions_calculate();
    // puts(directions_to_string());
    
    assert(directions[0][0] == SOUTH);

    // map_load_from_string(".........%\n"
    //                      ".........%\n"
    //                      ".........%\n"
    //                      ".........%\n"
    //                      "....b....%\n"
    //                      ".........%\n"
    //                      ".........%\n"
    //                      ".........%\n"
    //                      "........*%\n"
    //                      "%%%%%%%%%%");
    // expected = "EEEEEESSS-\n"
    //            "SE+NNEESS-\n"
    //            "S+N!!!EES-\n"
    //            "SW!!!!!ES-\n"
    //            "SW!!!!!ES-\n"
    //            "SS!!!!!SS-\n"
    //            "ESS!!!ESS-\n"
    //            "EESSSEEE+-\n"
    //            "EEEEEEE+*-\n"
    //            "----------";
    // holy_ground_calculate();
    // threat_calculate();
    // mystery_reset();
    // aroma_stabilize();
    // directions_calculate();
    // // puts(directions_to_string());
    // assert(strcmp(directions_to_string(), expected) == 0);
    
    input = ".aa...b.....*......................\n"
            ".aa.........*......................";
    map_load_from_string(input);
    // puts(map_to_string());
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    // puts(aroma_to_string());
    army_calculate();
    directions_calculate();
    // puts(directions_to_string());
    assert(directions[0][1] == EAST);
    assert(directions[0][2] == EAST);
    assert(directions[1][1] == EAST);
    assert(directions[1][2] == EAST);
    
    // input = "..a...b.....*......................\n"
    //         "......b.....*......................";
    // expected = "..X................................\n"
    //            "...................................";
    // map_load_from_string(input);
    // // puts(map_to_string());
    // holy_ground_calculate();
    // threat_calculate();
    // mystery_reset();
    // aroma_stabilize();
    // // puts(aroma_to_string());
    // army_calculate();
    // directions_calculate();
    // // puts(directions_to_string());
    // assert(strcmp(directions_to_string(), expected) == 0);

    // input = "%%%%%%%%%%%%%%%%%%%\n"
    //         "%aaaaaaaaaaaaaaaa.%\n"
    //         "%%%%%%%%%%%%%%%%%%%";
    input =    "%%%%%%%%%%%%%%%%%%%\n"
               "%b%%%%%%%%%%%%%%%%%\n"
               "%b..aaaaaaaaaaaaa.%\n"
               "%b%%%%%%%%%%%%%%%%%\n"
               "%%%%%%%%%%%%%%%%%%%";
    expected = "...................\n"
               "...................\n"
               "....XXXXXXXXXXXXX..\n"
               "...................\n"
               "...................";
    map_load_from_string(input);
    // puts(map_to_string());
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    // aroma_reset();
    // aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate();
    // aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate();
    // aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate();
    // printf("%2f %2f %2f %2f\n", army_aroma[0][0], army_aroma[0][1], army_aroma[0][2], army_aroma[0][3]);
    // printf("%2f %2f %2f %2f\n", army_aroma[1][0], army_aroma[1][1], army_aroma[1][2], army_aroma[1][3]);
    // printf("%2f %2f %2f %2f\n", army_aroma[2][0], army_aroma[2][1], army_aroma[2][2], army_aroma[2][3]);
    // puts(aroma_to_string());
    // puts(army_aroma_to_string());
    army_calculate();
    directions_calculate();
    // puts(directions_to_string());
    assert(strcmp(directions_to_string(), expected) == 0);

    input =    "aa.\n"
               "...";
    expected = "XX.\n"
               "...";
    map_load_from_string(input);
    // puts(map_to_string());
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_reset();
    aroma[0][0] = 1.0;
    aroma[0][1] = 2.0;
    // aroma_stabilize();
    // puts(aroma_to_string());
    army_calculate();
    directions_calculate();
    puts(directions_to_string());
    assert(strcmp(directions_to_string(), expected) == 0);

    puts("ok");
    return 0;
}
#endif
