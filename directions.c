#include <assert.h>
#include <string.h>
#include "map.h"
#include "holy_ground.h"
#include "army.h"
#include "threat.h"
#include "aroma.h"
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

float move_value(row, col, move) {
    int row2, col2;

    switch (move) {
        case MOVE_NORTH:
            neighbor(row, col, NORTH, &row2, &col2);
            break;
        case MOVE_EAST:
            neighbor(row, col, EAST, &row2, &col2);
            break;
        case MOVE_SOUTH:
            neighbor(row, col, SOUTH, &row2, &col2);
            break;
        case MOVE_WEST:
            neighbor(row, col, WEST, &row2, &col2);
            break;
        case MOVE_STAY:
            return 0;
        default:
            assert(0);
    }

    if (army[row][col]) {
        return (army_aroma[row2][col2] - threat[row2][col2] * 1000.0) - (army_aroma[row][col] - threat[row][col] * 1000.0);
    } else {
        return (aroma[row2][col2] - threat[row2][col2] * 1000.0) - (aroma[row][col] - threat[row][col] * 1000.0);
    }

    // return aroma[row2][col2] - aroma[row][col];
}

void directions_calculate() {
    int row, col;
    int row2, col2;
    int best_row, best_col;
    int to_row, to_col;
    char best_move;
    float best_value;
    float value;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if ((map[row][col] & SQUARE_ANT) && (owner[row][col] == 0)) {
                directions[row][col] = '-';
                available_moves[row][col] = MOVE_STAY;
                neighbor(row, col, NORTH, &row2, &col2);
                if ((map[row2][col2] & SQUARE_LAND) && !(map[row2][col2] & SQUARE_FOOD) && !((map[row2][col2] & SQUARE_ANT) && owner[row2][col2])) {
                    available_moves[row][col] |= MOVE_NORTH;
                }
                neighbor(row, col, EAST, &row2, &col2);
                if ((map[row2][col2] & SQUARE_LAND) && !(map[row2][col2] & SQUARE_FOOD) && !((map[row2][col2] & SQUARE_ANT) && owner[row2][col2])) {
                    available_moves[row][col] |= MOVE_EAST;
                }
                neighbor(row, col, SOUTH, &row2, &col2);
                if ((map[row2][col2] & SQUARE_LAND) && !(map[row2][col2] & SQUARE_FOOD) && !((map[row2][col2] & SQUARE_ANT) && owner[row2][col2])) {
                    available_moves[row][col] |= MOVE_SOUTH;
                }
                neighbor(row, col, WEST, &row2, &col2);
                if ((map[row2][col2] & SQUARE_LAND) && !(map[row2][col2] & SQUARE_FOOD) && !((map[row2][col2] & SQUARE_ANT) && owner[row2][col2])) {
                    available_moves[row][col] |= MOVE_WEST;
                }
            } else {
                directions[row][col] = '.';
                available_moves[row][col] = 0;
            }
         }
    }

    do {
        // find first move
        for (row = 0; row < rows; row++) {
            for (col = 0; col < cols; col++) {
                if (available_moves[row][col] & MOVE_NORTH) {
                    best_move = MOVE_NORTH;
                    goto moves_available;
                }
                if (available_moves[row][col] & MOVE_EAST) {
                    best_move = MOVE_EAST;
                    goto moves_available;
                }
                if (available_moves[row][col] & MOVE_SOUTH) {
                    best_move = MOVE_SOUTH;
                    goto moves_available;
                }
                if (available_moves[row][col] & MOVE_WEST) {
                    best_move = MOVE_WEST;
                    goto moves_available;
                }
                if (available_moves[row][col] & MOVE_STAY) {
                    best_move = MOVE_STAY;
                    goto moves_available;
                }
            }
        }

        // no moves available
        goto no_moves_available;

    moves_available:

        best_row = row;
        best_col = col;
        best_value = move_value(best_row, best_col, best_move);

        // find best move
        for (row = 0; row < rows; row++) {
            for (col = 0; col < cols; col++) {
                if (available_moves[row][col] & MOVE_NORTH) {
                    value = move_value(row, col, MOVE_NORTH);
                    if (best_value < value) {
                        best_move = MOVE_NORTH;
                        best_value = value;
                        best_row = row;
                        best_col = col;
                    }
                }
                if (available_moves[row][col] & MOVE_EAST) {
                    value = move_value(row, col, MOVE_EAST);
                    if (best_value < value) {
                        best_move = MOVE_EAST;
                        best_value = value;
                        best_row = row;
                        best_col = col;
                    }
                }
                if (available_moves[row][col] & MOVE_SOUTH) {
                    value = move_value(row, col, MOVE_SOUTH);
                    if (best_value < value) {
                        best_move = MOVE_SOUTH;
                        best_value = value;
                        best_row = row;
                        best_col = col;
                    }
                }
                if (available_moves[row][col] & MOVE_WEST) {
                    value = move_value(row, col, MOVE_WEST);
                    if (best_value < value) {
                        best_move = MOVE_WEST;
                        best_value = value;
                        best_row = row;
                        best_col = col;
                    }
                }
                if (available_moves[row][col] & MOVE_STAY) {
                    value = move_value(row, col, MOVE_STAY);
                    if (best_value < value) {
                        best_move = MOVE_STAY;
                        best_value = value;
                        best_row = row;
                        best_col = col;
                    }
                }
            }
        }

        switch (best_move) {
            case MOVE_NORTH:
                directions[best_row][best_col] = 'N';
                available_moves[best_row][best_col] = 0;
                neighbor(best_row, best_col, NORTH, &to_row, &to_col);
                available_moves[to_row][to_col] &= ~MOVE_STAY;
                neighbor(to_row, to_col, WEST,  &row2, &col2); available_moves[row2][col2] &= ~MOVE_EAST;
                neighbor(to_row, to_col, NORTH, &row2, &col2); available_moves[row2][col2] &= ~MOVE_SOUTH;
                neighbor(to_row, to_col, EAST,  &row2, &col2); available_moves[row2][col2] &= ~MOVE_WEST;
                break;
            case MOVE_EAST:
                directions[best_row][best_col] = 'E';
                available_moves[best_row][best_col] = 0;
                neighbor(best_row, best_col, EAST, &to_row, &to_col);
                available_moves[to_row][to_col] &= ~MOVE_STAY;
                neighbor(to_row, to_col, NORTH, &row2, &col2); available_moves[row2][col2] &= ~MOVE_SOUTH;
                neighbor(to_row, to_col, EAST,  &row2, &col2); available_moves[row2][col2] &= ~MOVE_WEST;
                neighbor(to_row, to_col, SOUTH, &row2, &col2); available_moves[row2][col2] &= ~MOVE_NORTH;
                break;
            case MOVE_SOUTH:
                directions[best_row][best_col] = 'S';
                available_moves[best_row][best_col] = 0;
                neighbor(best_row, best_col, SOUTH, &to_row, &to_col);
                available_moves[to_row][to_col] &= ~MOVE_STAY;
                neighbor(to_row, to_col, EAST,  &row2, &col2); available_moves[row2][col2] &= ~MOVE_WEST;
                neighbor(to_row, to_col, SOUTH, &row2, &col2); available_moves[row2][col2] &= ~MOVE_NORTH;
                neighbor(to_row, to_col, WEST,  &row2, &col2); available_moves[row2][col2] &= ~MOVE_EAST;
                break;
            case MOVE_WEST:
                directions[best_row][best_col] = 'W';
                available_moves[best_row][best_col] = 0;
                neighbor(best_row, best_col, WEST, &to_row, &to_col);
                available_moves[to_row][to_col] &= ~MOVE_STAY;
                neighbor(to_row, to_col, SOUTH, &row2, &col2); available_moves[row2][col2] &= ~MOVE_NORTH;
                neighbor(to_row, to_col, WEST,  &row2, &col2); available_moves[row2][col2] &= ~MOVE_EAST;
                neighbor(to_row, to_col, NORTH, &row2, &col2); available_moves[row2][col2] &= ~MOVE_SOUTH;
                break;
            case MOVE_STAY:
                directions[best_row][best_col] = 'X';
                available_moves[best_row][best_col] = 0;
                neighbor(best_row, best_col, NORTH, &row2, &col2); available_moves[row2][col2] &= ~MOVE_SOUTH;
                neighbor(best_row, best_col, EAST,  &row2, &col2); available_moves[row2][col2] &= ~MOVE_WEST;
                neighbor(best_row, best_col, SOUTH, &row2, &col2); available_moves[row2][col2] &= ~MOVE_NORTH;
                neighbor(best_row, best_col, WEST,  &row2, &col2); available_moves[row2][col2] &= ~MOVE_EAST;
                break;
        }
    } while (1);

no_moves_available: {}
}







// float directions_score() {
//     int row, col;
//     struct square_t *square;
//     float score = 0;
//     for (row = 0; row < rows; row++) {
//         for (col = 0; col < cols; col++) {
//             square = &squares[row][col];
//             switch (square->direction) {
//                 case 'N':
//                     score += square->north->aroma;
//                     break;
//                 case 'E':
//                     score += square->east->aroma;
//                     break;
//                 case 'S':
//                     score += square->south->aroma;
//                     break;
//                 case 'W':
//                     score += square->west->aroma;
//                     break;
//                 case 'X':
//                     score += square->aroma;
//                     break;
//             }
//         }
//     }
//     return score;
// }
// 
// void directions_save() {
//     int row, col;
//     struct square_t *square;
//     for (row = 0; row < rows; row++) {
//         for (col = 0; col < cols; col++) {
//             square = &squares[row][col];
//             directions[row][col] = square->direction;
//         }
//     }
// }
// 
// void directions_calculate() {
//     int row, col;
//     int row2, col2;
//     int forward, backward;
//     struct square_t *square;
//     float score, max_score;
// 
//     struct square_t *first_square = NULL;
//     struct square_t *last_square = NULL;
// 
//     for (row = 0; row < rows; row++) {
//         for (col = 0; col < cols; col++) {
//             square = &squares[row][col];
//             square->row = row;
//             square->col = col;
// 
//             neighbor(row, col, NORTH, &row2, &col2);
//             square->north = &squares[row2][col2];
//             neighbor(row, col, EAST, &row2, &col2);
//             square->east = &squares[row2][col2];
//             neighbor(row, col, SOUTH, &row2, &col2);
//             square->south = &squares[row2][col2];
//             neighbor(row, col, WEST, &row2, &col2);
//             square->west = &squares[row2][col2];
// 
//             square->contents = map[row][col];
//             square->owner = owner[row][col];
//             square->aroma = aroma[row][col];
// 
//             if (square->contents & SQUARE_WATER) {
//                 square->direction = '%';
//             } else if (square->contents & SQUARE_LAND) {
//                 if (square->contents & SQUARE_FOOD) {
//                     square->direction = '*';
//                 } else if ((square->contents & SQUARE_ANT) && (square->owner == 0)) {
//                     square->direction = '-';
//                     square->previous = last_square;
//                     last_square = square;
//                 } else {
//                     square->direction = '.';
//                 }
//             } else {
//                 square->direction = '?';
//             }
//         }
//     }
// 
//     square = last_square;
//     while (square) {
//         square->next = first_square;
//         first_square = square;
//         square = square->previous;
//     }
// 
//     forward = 1;
//     backward = 0;
//     row = 0;
//     col = 0;
//     max_score = 0.0;
// 
//     square = first_square;
// 
//     printf("first %li, last %li\n", (long int) first_square, (long int) last_square);
// 
//     int i = 0;
//     while (square) {
//         printf("%i iterations\n", ++i);
//         // square = &squares[row][col];
// 
//         // printf("%i:%i = \n", row, col, square->direction);
// 
//         switch (square->direction) {
//             case '-':
//                 if (square->north->contents & SQUARE_LAND
//                     && square->north->direction != 'X'
//                     && square->north->north->direction != 'S'
//                     && square->north->east->direction != 'W'
//                     && square->north->west->direction != 'E'
//                 ) {
//                     square->direction = 'N';
//                     forward = 1;
//                     backward = 0;
//                     break;
//                 }
//             case 'N':
//                 if (square->east->contents & SQUARE_LAND
//                     && square->east->direction != 'X'
//                     && square->east->north->direction != 'S'
//                     && square->east->east->direction != 'W'
//                     && square->east->south->direction != 'N'
//                 ) {
//                     square->direction = 'E';
//                     forward = 1;
//                     backward = 0;
//                     break;
//                 }
//             case 'E':
//                 if (square->south->contents & SQUARE_LAND
//                     && square->south->direction != 'X'
//                     && square->south->east->direction != 'W'
//                     && square->south->south->direction != 'N'
//                     && square->south->west->direction != 'E'
//                 ) {
//                     square->direction = 'S';
//                     forward = 1;
//                     backward = 0;
//                     break;
//                 }
//             case 'S':
//                 if (square->west->contents & SQUARE_LAND
//                     && square->west->direction != 'X'
//                     && square->west->north->direction != 'S'
//                     && square->west->south->direction != 'N'
//                     && square->west->west->direction != 'E'
//                 ) {
//                     square->direction = 'W';
//                     forward = 1;
//                     backward = 0;
//                     break;
//                 }
//             case 'W':
//                 if (square->north->direction != 'S'
//                     && square->east->direction != 'W'
//                     && square->south->direction != 'N'
//                     && square->west->direction != 'E'
//                 ) {
//                     square->direction = 'X';
//                     forward = 1;
//                     backward = 0;
//                     break;
//                 }
//             case 'X':
//                 square->direction = '-';
//                 forward = 0;
//                 backward = 1;
//                 break;
//         }
// 
//         if (forward) {
//             printf("forward\n");
//             if (square->next) {
//                 printf("next\n");
//                 square = square->next;
//             } else {
//                 score = directions_score();
//                 printf("score %f\n", score);
//                 if (max_score < score) {
//                     max_score = score;
//                     directions_save();
//                 }
// 
//                 forward = 0;
//                 backward = 0;
//             }
//         }
// 
//         if (backward) {
//             printf("backward\n");
//             if (square->previous) {
//                 printf("previous\n");
//                 square = square->previous;
//             } else {
//                 printf("done\n");
//                 break;
//             }
//         }
//     }
// }

// void directions_calculate() {
//     int row, col;
//     char c;
//     int row2, col2;
//     int direction;
//     float max_aroma;
//     float min_threat;
//     for (row = 0; row < rows; row++) {
//         for (col = 0; col < cols; col++) {
//             if (map[row][col] & SQUARE_LAND) {
//                 if (map[row][col] & SQUARE_FOOD) {
//                     c = '*';
//                 } else if (enemy_can_attack[row][col]) {
//                     c = '!';
//                 } else {
//                     c = '+';
// 
//                     min_threat = threat[row][col];
//                     max_aroma = aroma[row][col];
// 
//                     for (direction = 0; direction < 4; direction++) {
//                         neighbor(row, col, direction, &row2, &col2);
// 
//                         if (map[row2][col2] & SQUARE_FOOD) {
//                             c = '+';
//                             break;
//                         }
// 
//                         if ((min_threat > threat[row2][col2]) || ((min_threat == threat[row2][col2]) && (max_aroma < aroma[row2][col2]))) {
//                             min_threat = threat[row2][col2];
//                             max_aroma = aroma[row2][col2];
//                             c = direction_symbols[direction];
//                         }
//                     }
//                 }
//             } else if (map[row][col] & SQUARE_WATER) {
//                 c = '-';
//             } else {
//                 c = '?';
//             }
// 
//             directions[row][col] = c;
//         }
//     }
// }

char *directions_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    int row, col;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            // *output++ = squares[row][col].direction;
            *output++ = directions[row][col];
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
int main(int argc, char *argv[]) {
    char *expected;
    attackradius2 = 5;
    viewradius2 = 55;
    // puts(aroma_to_string());
    // printf("%i %i %i %i\n", map[0][0], map[0][1], map[0][2], map[0][3]);

    map_load_from_string("a.*%");
    puts(map_to_string());
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    puts(aroma_to_string());
    directions_calculate();
    puts(directions_to_string());
    assert(directions[0][0] == 'E');

    map_load_from_string("a%...%*%\n"
                         "...%...%\n"
                         "%%%%%%%%");
    puts(map_to_string());
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();

    puts(aroma_to_string());
    directions_calculate();
    puts(directions_to_string());
    
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
    
    map_load_from_string(".aa...b.....*......................\n"
                         ".aa.........*......................");
    puts(map_to_string());
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    puts(aroma_to_string());
    directions_calculate();
    puts(directions_to_string());
    assert(directions[0][1] == 'E');
    assert(directions[0][2] == 'E');
    assert(directions[1][1] == 'E');
    assert(directions[1][2] == 'E');
    
    map_load_from_string("..a...b.....*......................\n"
                         "......b.....*......................");
    expected = "..X................................\n"
               "...................................";
    puts(map_to_string());
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    puts(aroma_to_string());
    directions_calculate();
    puts(directions_to_string());
    assert(strcmp(directions_to_string(), expected) == 0);

    // aroma_iterate();

    puts("ok");
    return 0;
}
#endif
