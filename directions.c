#include <assert.h>
#include <string.h>
#include "map.h"
#include "holy_ground.h"
#include "threat.h"
#include "aroma.h"
#include "army.h"
#include "directions.h"

char direction_symbols[] = {'N', 'E', 'S', 'W'};

struct square_t {
    int row;
    int col;
    struct square_t *next, *previous;
    struct square_t *north, *east, *south, *west;
    char contents;
    char owner;
    char direction;
    float aroma;
};

struct square_t squares[MAX_ROWS][MAX_COLS];

#define MOVE_NORTH 1
#define MOVE_EAST  2
#define MOVE_SOUTH 4
#define MOVE_WEST  8
#define MOVE_STAY 16

static char available_moves[MAX_ROWS][MAX_COLS];

float value_at(point p) {
    return aroma[p.row][p.col] - threat[p.row][p.col] * 1000.0;
}

float army_value_at(point p) {
    return army_aroma[p.row][p.col] - threat[p.row][p.col] * 1000.0;
}

float move_value(point p, int move) {
    point p2;

    switch (move) {
        case MOVE_NORTH:
            p2 = neighbor(p, NORTH);
            break;
        case MOVE_EAST:
            p2 = neighbor(p, EAST);
            break;
        case MOVE_SOUTH:
            p2 = neighbor(p, SOUTH);
            break;
        case MOVE_WEST:
            p2 = neighbor(p, WEST);
            break;
        case MOVE_STAY:
            return 0;
        default:
            assert(0);
    }

    if (army[p.row][p.col]) {
        return army_value_at(p2) - army_value_at(p);
        // return (army_aroma[p2.row][p2.col] - threat[p2.row][p2.col] * 1000.0) - (army_aroma[p.row][p.col] - threat[p.row][p.col] * 1000.0);
    } else {
        return value_at(p2) - value_at(p);
        // return (aroma[p2.row][p2.col] - threat[p2.row][p2.col] * 1000.0) - (aroma[p.row][p.col] - threat[p.row][p.col] * 1000.0);
    }

    // return aroma[p2.row][p2.col] - aroma[p.row][p.col];
}

void directions_calculate() {
    point p;
    point p2;
    point best_p, to_p;
    // int best_p.row, best_p.col;
    // int to_p.row, to_p.col;
    char best_move;
    float best_value;
    float value;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            if (friendly_ant_exists_at(p)) {
                directions[p.row][p.col] = '-';
                available_moves[p.row][p.col] = MOVE_STAY;
                p2 = neighbor(p, NORTH);
                if ((map[p2.row][p2.col] & SQUARE_LAND) && !(map[p2.row][p2.col] & SQUARE_FOOD) && !((map[p2.row][p2.col] & SQUARE_ANT) && owner[p2.row][p2.col])) {
                    available_moves[p.row][p.col] |= MOVE_NORTH;
                }
                p2 = neighbor(p, EAST);
                if ((map[p2.row][p2.col] & SQUARE_LAND) && !(map[p2.row][p2.col] & SQUARE_FOOD) && !((map[p2.row][p2.col] & SQUARE_ANT) && owner[p2.row][p2.col])) {
                    available_moves[p.row][p.col] |= MOVE_EAST;
                }
                p2 = neighbor(p, SOUTH);
                if ((map[p2.row][p2.col] & SQUARE_LAND) && !(map[p2.row][p2.col] & SQUARE_FOOD) && !((map[p2.row][p2.col] & SQUARE_ANT) && owner[p2.row][p2.col])) {
                    available_moves[p.row][p.col] |= MOVE_SOUTH;
                }
                p2 = neighbor(p, WEST);
                if ((map[p2.row][p2.col] & SQUARE_LAND) && !(map[p2.row][p2.col] & SQUARE_FOOD) && !((map[p2.row][p2.col] & SQUARE_ANT) && owner[p2.row][p2.col])) {
                    available_moves[p.row][p.col] |= MOVE_WEST;
                }
            } else {
                directions[p.row][p.col] = '.';
                available_moves[p.row][p.col] = 0;
            }
         }
    }

    do {
        // find first move
        for (p.row = 0; p.row < rows; p.row++) {
            for (p.col = 0; p.col < cols; p.col++) {
                if (available_moves[p.row][p.col] & MOVE_NORTH) {
                    best_move = MOVE_NORTH;
                    goto moves_available;
                }
                if (available_moves[p.row][p.col] & MOVE_EAST) {
                    best_move = MOVE_EAST;
                    goto moves_available;
                }
                if (available_moves[p.row][p.col] & MOVE_SOUTH) {
                    best_move = MOVE_SOUTH;
                    goto moves_available;
                }
                if (available_moves[p.row][p.col] & MOVE_WEST) {
                    best_move = MOVE_WEST;
                    goto moves_available;
                }
                if (available_moves[p.row][p.col] & MOVE_STAY) {
                    best_move = MOVE_STAY;
                    goto moves_available;
                }
            }
        }

        // no moves available
        goto no_moves_available;

    moves_available:

        best_p = p;
        best_value = move_value(best_p, best_move);

        // find best move
        for (p.row = 0; p.row < rows; p.row++) {
            for (p.col = 0; p.col < cols; p.col++) {
                if (available_moves[p.row][p.col] & MOVE_NORTH) {
                    value = move_value(p, MOVE_NORTH);
                    if (best_value < value) {
                        best_move = MOVE_NORTH;
                        best_value = value;
                        best_p = p;
                    }
                }
                if (available_moves[p.row][p.col] & MOVE_EAST) {
                    value = move_value(p, MOVE_EAST);
                    if (best_value < value) {
                        best_move = MOVE_EAST;
                        best_value = value;
                        best_p = p;
                    }
                }
                if (available_moves[p.row][p.col] & MOVE_SOUTH) {
                    value = move_value(p, MOVE_SOUTH);
                    if (best_value < value) {
                        best_move = MOVE_SOUTH;
                        best_value = value;
                        best_p = p;
                    }
                }
                if (available_moves[p.row][p.col] & MOVE_WEST) {
                    value = move_value(p, MOVE_WEST);
                    if (best_value < value) {
                        best_move = MOVE_WEST;
                        best_value = value;
                        best_p = p;
                    }
                }
                if (available_moves[p.row][p.col] & MOVE_STAY) {
                    value = move_value(p, MOVE_STAY);
                    if (best_value < value) {
                        best_move = MOVE_STAY;
                        best_value = value;
                        best_p = p;
                    }
                }
            }
        }

        switch (best_move) {
            case MOVE_NORTH:
                directions[best_p.row][best_p.col] = 'N';
                available_moves[best_p.row][best_p.col] = 0;
                to_p = neighbor(best_p, NORTH);
                available_moves[to_p.row][to_p.col] &= ~MOVE_STAY;
                p2 = neighbor(to_p, WEST);  available_moves[p2.row][p2.col] &= ~MOVE_EAST;
                p2 = neighbor(to_p, NORTH); available_moves[p2.row][p2.col] &= ~MOVE_SOUTH;
                p2 = neighbor(to_p, EAST);  available_moves[p2.row][p2.col] &= ~MOVE_WEST;
                break;
            case MOVE_EAST:
                directions[best_p.row][best_p.col] = 'E';
                available_moves[best_p.row][best_p.col] = 0;
                to_p = neighbor(best_p, EAST);
                available_moves[to_p.row][to_p.col] &= ~MOVE_STAY;
                p2 = neighbor(to_p, NORTH); available_moves[p2.row][p2.col] &= ~MOVE_SOUTH;
                p2 = neighbor(to_p, EAST);  available_moves[p2.row][p2.col] &= ~MOVE_WEST;
                p2 = neighbor(to_p, SOUTH); available_moves[p2.row][p2.col] &= ~MOVE_NORTH;
                break;
            case MOVE_SOUTH:
                directions[best_p.row][best_p.col] = 'S';
                available_moves[best_p.row][best_p.col] = 0;
                to_p = neighbor(best_p, SOUTH);
                available_moves[to_p.row][to_p.col] &= ~MOVE_STAY;
                p2 = neighbor(to_p, EAST);  available_moves[p2.row][p2.col] &= ~MOVE_WEST;
                p2 = neighbor(to_p, SOUTH); available_moves[p2.row][p2.col] &= ~MOVE_NORTH;
                p2 = neighbor(to_p, WEST);  available_moves[p2.row][p2.col] &= ~MOVE_EAST;
                break;
            case MOVE_WEST:
                directions[best_p.row][best_p.col] = 'W';
                available_moves[best_p.row][best_p.col] = 0;
                to_p = neighbor(best_p, WEST);
                available_moves[to_p.row][to_p.col] &= ~MOVE_STAY;
                p2 = neighbor(to_p, SOUTH); available_moves[p2.row][p2.col] &= ~MOVE_NORTH;
                p2 = neighbor(to_p, WEST);  available_moves[p2.row][p2.col] &= ~MOVE_EAST;
                p2 = neighbor(to_p, NORTH); available_moves[p2.row][p2.col] &= ~MOVE_SOUTH;
                break;
            case MOVE_STAY:
                directions[best_p.row][best_p.col] = 'X';
                available_moves[best_p.row][best_p.col] = 0;
                p2 = neighbor(best_p, NORTH); available_moves[p2.row][p2.col] &= ~MOVE_SOUTH;
                p2 = neighbor(best_p, EAST);  available_moves[p2.row][p2.col] &= ~MOVE_WEST;
                p2 = neighbor(best_p, SOUTH); available_moves[p2.row][p2.col] &= ~MOVE_NORTH;
                p2 = neighbor(best_p, WEST);  available_moves[p2.row][p2.col] &= ~MOVE_EAST;
                break;
        }
    } while (1);

no_moves_available: {}
}


char *directions_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    point p;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            // *output++ = squares[p.row][p.col].direction;
            *output++ = directions[p.row][p.col];
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
    assert(directions[0][0] == 'E');

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
    
    assert(directions[0][0] == 'S');

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
    assert(directions[0][1] == 'E');
    assert(directions[0][2] == 'E');
    assert(directions[1][1] == 'E');
    assert(directions[1][2] == 'E');
    
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
