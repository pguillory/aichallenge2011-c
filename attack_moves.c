#include <assert.h>
#include <string.h>
#include "map.h"
#include "threat.h"
#include "attack_moves.h"

// unsigned char directional_threat[MAX_ROWS][MAX_COLS][5];
// unsigned char directional_support[MAX_ROWS][MAX_COLS][5];

// int considered_direction;

// #define directional_threat_at(p, dir) grid(directional_threat, p)[dir2index(dir)]
// #define directional_support_at(p, dir) grid(directional_support, p)[dir2index(dir)]

// int dir2index(int dir) {
//     switch (dir) {
//         case NORTH:
//             return 0;
//         case SOUTH:
//             return 1;
//         case EAST:
//             return 2;
//         case WEST:
//             return 3;
//         case STAY:
//             return 4;
//     }
//     assert(0);
// }

// void accumulate_directional_threat_at(point p) {
//     directional_threat_at(p, considered_direction) += 1;
// }
// 
// void accumulate_directional_threat_around(point p, int dir, point p2) {
//     if (map_has_land(p2)) {
//         considered_direction = dir;
//         foreach_point_within_radius2(p2, attackradius2, accumulate_directional_threat_at);
//     }
// }

void reset_at(point p) {
    if (map_has_friendly_ant(p)) {
        grid(attack_moves, p) = NORTH | EAST | SOUTH | WEST | STAY;
    } else {
        grid(attack_moves, p) = 0;
    }
    // if (map_has_enemy_ant(p)) {
    //     foreach_neighbor(p, accumulate_directional_threat_around);
    // }
}

void reset() {
    // memset(directional_threat, 0, sizeof(directional_threat));
    // memset(directional_support, 0, sizeof(directional_support));
    foreach_point(reset_at);
}

void exclude_move(point p, int dir) {
    grid(attack_moves, p) &= ~dir;
}

void exclude_moves_to(point p) {
    exclude_move(neighbor(p, SOUTH), NORTH);
    exclude_move(neighbor(p, WEST),  EAST);
    exclude_move(neighbor(p, NORTH), SOUTH);
    exclude_move(neighbor(p, EAST),  WEST);
    exclude_move(p, STAY);
}

void prune_impossible_moves_into(point p) {
    if (map_has_water(p) || map_has_food(p) || grid(enemy_could_occupy, p) || !grid(threat, p)) {
        exclude_moves_to(p);
    }
}

void prune_impossible_moves() {
    foreach_point(prune_impossible_moves_into);
}

// void add_support_at(point p) {
//     int i;
//     point p2;
// 
//     for (i = 0; i < 5; i++) {
//         if (grid(directional_threat, p)[i] > 0) {
//             grid(directional_support, p)[i] += 1;
//             p2 = neighbor(p, NORTH);   grid(directional_support, p2)[i] += 1;
//             p2 = neighbor(p2, EAST);   grid(directional_support, p2)[i] += 1;
//             p2 = neighbor(p, EAST);    grid(directional_support, p2)[i] += 1;
//             p2 = neighbor(p2, SOUTH);  grid(directional_support, p2)[i] += 1;
//             p2 = neighbor(p, SOUTH);   grid(directional_support, p2)[i] += 1;
//             p2 = neighbor(p2, WEST);   grid(directional_support, p2)[i] += 1;
//             p2 = neighbor(p, WEST);    grid(directional_support, p2)[i] += 1;
//             p2 = neighbor(p2, NORTH);  grid(directional_support, p2)[i] += 1;
//         }
//     }
// }
// 
// void add_support_for_single_moves_at(point p) {
//     int dir = grid(attack_moves, p);
// 
//     switch (dir) {
//         case NORTH:
//         case EAST:
//         case SOUTH:
//         case WEST:
//         case STAY:
//             add_support_at(p);
//             break;
//     }
// }
// 
// void add_support_for_single_moves() {
//     foreach_point(add_support_for_single_moves_at);
// }

// void prune_outnumbered_moves_at(point p) {
//     point p2;
//     int i;
//     int dir;
// 
//     for (dir = 1; dir <= STAY; dir *= 2) {
//         if (grid(attack_moves, p) & dir) {
//             p2 = neighbor(p, dir);
//             for (i = 0; i < 5; i++) {
//                 if (grid(directional_support, p2)[i] <= grid(directional_threat, p2)[i]) {
//                     grid(attack_moves, p) = 0;
//                     break;
//                 }
//             }
//             break;
//         }
//     }
// }
// 
// void prune_outnumbered_moves() {
//     foreach_point(prune_outnumbered_moves_at);
// }

void attack_moves_calculate() {
    reset();
    prune_impossible_moves();
    // add_support_for_single_moves();
    // prune_outnumbered_moves();
}

char attack_moves_to_string_at(point p) {
    if (map_has_water(p)) {
        return '%';
    } else if (map_has_friendly_ant(p)) {
        switch (grid(attack_moves, p)) {
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
}

char *attack_moves_to_string() {
    return point_callback_to_string(attack_moves_to_string_at);
}

// char *directional_threat_to_string() {
//     static char buffer[24 * MAX_ROWS * MAX_COLS + MAX_ROWS];
//     char *output = buffer;
//     point p;
// 
//     for (p.row = 0; p.row < rows; p.row++) {
//         for (p.col = 0; p.col < cols; p.col++) {
//             sprintf(output, "  %i  |", grid(directional_threat, p)[0]);
//             output += 6;
//         }
//         *output++ = '\n';
//         for (p.col = 0; p.col < cols; p.col++) {
//             sprintf(output, "%i %i %i|", grid(directional_threat, p)[3], grid(directional_threat, p)[4], grid(directional_threat, p)[1]);
//             output += 6;
//         }
//         *output++ = '\n';
//         for (p.col = 0; p.col < cols; p.col++) {
//             sprintf(output, "  %i  |", grid(directional_threat, p)[2]);
//             output += 6;
//         }
//         *output++ = '\n';
//         for (p.col = 0; p.col < cols; p.col++) {
//             sprintf(output, "-----|");
//             output += 6;
//         }
//         *output++ = '\n';
//     }
//     *--output = '\0';
//     return buffer;
// }
// 
// char *directional_support_to_string() {
//     static char buffer[24 * MAX_ROWS * MAX_COLS + MAX_ROWS];
//     char *output = buffer;
//     point p;
// 
//     for (p.row = 0; p.row < rows; p.row++) {
//         for (p.col = 0; p.col < cols; p.col++) {
//             sprintf(output, "  %i  |", grid(directional_support, p)[0]);
//             output += 6;
//         }
//         *output++ = '\n';
//         for (p.col = 0; p.col < cols; p.col++) {
//             sprintf(output, "%i %i %i|", grid(directional_support, p)[3], grid(directional_support, p)[4], grid(directional_support, p)[1]);
//             output += 6;
//         }
//         *output++ = '\n';
//         for (p.col = 0; p.col < cols; p.col++) {
//             sprintf(output, "  %i  |", grid(directional_support, p)[2]);
//             output += 6;
//         }
//         *output++ = '\n';
//         for (p.col = 0; p.col < cols; p.col++) {
//             sprintf(output, "-----|");
//             output += 6;
//         }
//         *output++ = '\n';
//     }
//     *--output = '\0';
//     return buffer;
// }

#ifdef UNIT_TESTS
#undef UNIT_TESTS
#include "globals.c"
#include "map.c"
// #include "holy_ground.c"
#include "threat.c"
// #include "mystery.c"
// #include "aroma.c"
// #include "army.c"
int main(int argc, char *argv[]) {
    char *input;

    attackradius2 = 5;

    input = "..........\n"
            "..........\n"
            "..........\n"
            "..........\n"
            "..........";
    map_load_from_string(input);
    threat_calculate();
    attack_moves_calculate();
    puts(attack_moves_to_string());
    assert(attack_moves[1][0] == 0);

    input = "..........\n"
            "a.........\n"
            "..........\n"
            "..........\n"
            "..........";
    map_load_from_string(input);
    threat_calculate();
    attack_moves_calculate();
    puts(attack_moves_to_string());
    assert(attack_moves[1][0] == 0);

    input = "..........\n"
            "a...b.....\n"
            "..........\n"
            "..........\n"
            "..........";
    map_load_from_string(input);
    threat_calculate();
    attack_moves_calculate();
    puts(attack_moves_to_string());
    assert(attack_moves[1][0] == 0);

    input = "..........\n"
            "a..b......\n"
            "..........\n"
            "..........\n"
            "..........";
    map_load_from_string(input);
    threat_calculate();
    attack_moves_calculate();
    puts(attack_moves_to_string());
    assert(attack_moves[1][0] == 0); //(EAST | NORTH | SOUTH | STAY));

    input = "%.........\n"
            "a..b......\n"
            "%.........\n"
            "..........\n"
            "..........";
    map_load_from_string(input);
    threat_calculate();
    attack_moves_calculate();
    puts(attack_moves_to_string());
    assert(attack_moves[1][0] == 0);

    input = "a%........\n"
            "..........\n"
            "...b......\n"
            "..........\n"
            "..........";
    map_load_from_string(input);
    threat_calculate();
    attack_moves_calculate();
    puts(attack_moves_to_string());
    assert(attack_moves[0][0] == 0);

    input = "..........\n"
            "..........\n"
            "a.........\n"
            "a...b.....\n"
            "a.........\n"
            "..........\n"
            "..........\n"
            "..........";
    map_load_from_string(input);
    threat_calculate();

    // attack_moves_calculate();

    puts("ok");
    return 0;
}
#endif
