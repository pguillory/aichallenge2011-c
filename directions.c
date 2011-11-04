#include <assert.h>
#include <string.h>
#include "map.h"
#include "holy_ground.h"
#include "threat.h"
#include "aroma.h"
#include "army.h"
#include "directions.h"

static char available_moves[MAX_ROWS][MAX_COLS];
point best_p;
char best_dir;
float best_value;
point worst_p;
char worst_dir;
float worst_value;

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

void reset_at(point p) {
    point p2;
    int dir;

    directions[p.row][p.col] = 0;
    available_moves[p.row][p.col] = 0;

    if (map_has_friendly_ant(p)) {
        for (dir = 1; dir <= STAY; dir *= 2) {
            p2 = neighbor(p, dir);
            if (map_has_land(p2) && !map_has_food(p2) && !enemy_could_occupy[p2.row][p2.col]) {
                available_moves[p.row][p.col] |= dir;
            }
        }
    }
}

// void bar_admission_to(point p) {
//     
// }

void available_move_take(point p, int dir) {
    available_moves[p.row][p.col] &= ~dir;
}

void directions_calculate() {
    point p;
    point to_p;
    float value;
    int dir;

    foreach_point(reset_at);

    do {
        // find any move
        for (p.row = 0; p.row < rows; p.row++) {
            for (p.col = 0; p.col < cols; p.col++) {
                if (available_moves[p.row][p.col]) {
                    for (dir = 1; dir <= STAY; dir *= 2) {
                        if (available_moves[p.row][p.col] & dir) {
                            best_p = p;
                            best_dir = dir;
                            best_value = move_value(p, dir);
                            goto moves_available;
                        }
                    }
                }
            }
        }

        goto no_moves_available;

moves_available:

        // find best move
        for (p.row = 0; p.row < rows; p.row++) {
            for (p.col = 0; p.col < cols; p.col++) {
                for (dir = 1; dir <= STAY; dir *= 2) {
                    if (available_moves[p.row][p.col] & dir) {
                        value = move_value(p, dir);
                        if (best_value < value) {
                            best_p = p;
                            best_dir = dir;
                            best_value = value;
                        }
                    }
                }
            }
        }

        directions[best_p.row][best_p.col] = best_dir;
        available_moves[best_p.row][best_p.col] = 0;
        to_p = neighbor(best_p, best_dir);
        available_move_take(to_p, STAY);
        available_move_take(neighbor(to_p, NORTH), SOUTH);
        available_move_take(neighbor(to_p, EAST), WEST);
        available_move_take(neighbor(to_p, SOUTH), NORTH);
        available_move_take(neighbor(to_p, WEST), EAST);
    } while (1);

no_moves_available: {}
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
    
    input = "..a...b.....*......................\n"
            "......b.....*......................";
    expected = "..X................................\n"
               "...................................";
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
    assert(strcmp(directions_to_string(), expected) == 0);

    // input = "%%%%%%%%%%%%%%%%%%%\n"
    //         "%aaaaaaaaaaaaaaaa.%\n"
    //         "%%%%%%%%%%%%%%%%%%%";
    // input = "%%%%%%%%%%%%%%%%%%%\n"
    //         "%b%%%%%%%%%%%%%%%%%\n"
    //         "%b..aaaaaaaaaaaaa.%\n"
    //         "%b%%%%%%%%%%%%%%%%%\n"
    //         "%%%%%%%%%%%%%%%%%%%";
    // expected = "..X................................\n"
    //            "...................................";
    // map_load_from_string(input);
    // puts(map_to_string());
    // holy_ground_calculate();
    // threat_calculate();
    // mystery_reset();
    // aroma_stabilize();
    // // army_aroma[1][1] = 100000000.0;
    // // aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate();
    // // aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate();
    // // aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate();
    // puts(aroma_to_string());
    // puts(army_aroma_to_string());
    // army_calculate();
    // directions_calculate();
    // puts(directions_to_string());
    // assert(strcmp(directions_to_string(), expected) == 0);

    // aroma_iterate();

    puts("ok");
    return 0;
}
#endif
