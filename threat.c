#include <math.h>
#include <string.h>
#include "map.h"
#include "holy_ground.h"
#include "threat.h"

#define grid(g, p) g[p.row][p.col]

void reset_threat_at(point p) {
    int side_count;
    int dir;

    threat[p.row][p.col] = 0;
    if (map_has_land(p)) {
        side_count = 0;
        for (dir = 1; dir < STAY; dir *= 2) {
            if (map_has_water(neighbor(p, dir))) {
                side_count += 1;
            }
        }
        if (side_count >= 3) {
            threat[p.row][p.col] += 9;
        }
    }
    grid(enemy_could_occupy, p) = 0;
    enemy_can_attack[p.row][p.col] = 0;
    enemy_could_attack[p.row][p.col] = 0;
}

void add_threat_for_each_enemy_that_could_attack(point p) {
    // add 1 threat for every enemy that could attack it next turn

    point pp, p2, d;
    int dir;
    int attackradius = ceil(sqrt(attackradius2) + sqrt(2));
    int rows_scanned, cols_scanned;

    if (map_has_enemy_ant(p)) {
        enemy_could_occupy[p.row][p.col] = 1;

        for (d.row = -attackradius, rows_scanned = 0; d.row <= attackradius && rows_scanned < rows; d.row++, rows_scanned++) {
            for (d.col = -attackradius, cols_scanned = 0; d.col <= attackradius && cols_scanned < cols; d.col++, cols_scanned++) {
                p2 = add_points(p, d);

                if (distance2(p, p2) <= attackradius2) {
                    enemy_can_attack[p2.row][p2.col] = 1;
                    enemy_could_attack[p2.row][p2.col] = 1;
                }

                for (dir = 1; dir < STAY; dir *= 2) {
                    pp = neighbor(p, dir);

                    if (map_has_land(pp) && !map_has_food(pp)) {
                        enemy_could_occupy[pp.row][pp.col] = 1;
                        if (distance2(pp, p2) <= attackradius2) {
                            threat[p2.row][p2.col] += 1;
                            enemy_could_attack[p2.row][p2.col] += 1;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void boost_threat(point p) {
    // add 1 threat to a square if it could be attacked by an enemy, to discourage even trades
    // exclude holy ground, where even trades are acceptable

    if ((threat[p.row][p.col] > 0) && (holy_ground[p.row][p.col] == 0)) {
        threat[p.row][p.col] += 1;
    }
}

void decrement_threat_to_zero(point p) {
    if (threat[p.row][p.col] > 0) {
        threat[p.row][p.col] -= 1;
    }
}

void decrement_threat_around_friendly_ant(point p) {
    // this doesn't really work.  it allows even trades sometimes
    if (map_has_friendly_ant(p)) {
        decrement_threat_to_zero(p);
        decrement_threat_to_zero(neighbor(p, NORTH));
        decrement_threat_to_zero(neighbor(p, EAST));
        decrement_threat_to_zero(neighbor(p, SOUTH));
        decrement_threat_to_zero(neighbor(p, WEST));
        // foreach_point_within_manhattan_distance(p, 1, decrement_threat_to_zero);
    }
}

// void decrement_threat_at_friendly_ants(point p) {
//     // this doesn't really work.  it allows even trades sometimes
//     if (map_has_friendly_ant(p)) {
//         foreach_point_within_manhattan_distance(p, 1, decrement_threat_to_zero);
//     }
// }

static char available_ants[MAX_ROWS][MAX_COLS];

void reset_available_ant_at(point p) {
    available_ants[p.row][p.col] = map_has_friendly_ant(p);
}

int get_available_ant_at(point p) {
    if (available_ants[p.row][p.col]) {
        available_ants[p.row][p.col] = 0;
        return 1;
    } else {
        return 0;
    }
}

void scan_for_potential_line_attacks_toward(point startp, int forward, int right, int left, int backward) {
    foreach_point(reset_available_ant_at);

    point endp = neighbor(startp, forward);
    int is_an_attack = 0;

    if (enemy_could_attack[endp.row][endp.col] == 0) {
        return;
    }

    // printf("%i:%i %c has potential\n", startp.row, startp.col, dir2char(forward));

    while (enemy_could_attack[endp.row][endp.col] > 0) {
        if (map_has_water(endp)) {
            return;
        }

        if (!get_available_ant_at(endp) &&
            !get_available_ant_at(neighbor(endp, right)) &&
            !get_available_ant_at(neighbor(endp, left)) &&
            !get_available_ant_at(neighbor(endp, forward))
        ) {
            // printf("%i:%i lacks support\n", endp.row, endp.col);
            return;
        }

        // printf("%i:%i has a friendly ant nearby\n", endp.row, endp.col);

        if (enemy_can_attack[endp.row][endp.col]) {
            // printf("%i:%i is in the danger zone\n", endp.row, endp.col);
            is_an_attack = 1;
        }

        endp = neighbor(endp, forward);
    }

    // printf("scanline from %i:%i to %i:%i\n", startp.row, startp.col, endp.row, endp.col);

    endp = neighbor(startp, forward);

    while (enemy_could_attack[endp.row][endp.col] > 0) {
        if (threat[endp.row][endp.col] >= 0) {
            threat[endp.row][endp.col] = is_an_attack ? -2 : -1;
        }

        endp = neighbor(endp, forward);
    }
}

void scan_for_potential_line_attacks(point startp) {
    if (enemy_could_attack[startp.row][startp.col] == 0) {
        // printf("scan_for_potential_line_attacks at %i:%i\n", startp.row, startp.col);
        scan_for_potential_line_attacks_toward(startp, EAST, NORTH, SOUTH, WEST);
        scan_for_potential_line_attacks_toward(startp, SOUTH, EAST, WEST, NORTH);
    }
}

void threat_calculate() {
    foreach_point(reset_threat_at);

    foreach_point(add_threat_for_each_enemy_that_could_attack);

    foreach_point(boost_threat);

    foreach_point(scan_for_potential_line_attacks);

    // foreach_point(decrement_threat_around_friendly_ant);
}

char *threat_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    point p;
    int value;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            if (map_has_land(p)) {
                if (enemy_could_attack[p.row][p.col]) {
                    value = (int) threat[p.row][p.col];
                    if (value < 0) {
                        if (value == -1) {
                            *output++ = '!';
                        } else if (value == -2) {
                            *output++ = '#';
                        } else {
                            *output++ = '-';
                        }
                    } else if (value < 10) {
                        *output++ = '0' + value;
                    } else if (value < 36) {
                        *output++ = 'a' + (value - 10);
                    } else {
                        *output++ = '+';
                    }
                } else {
                    *output++ = '.';
                }
            } else if (map_has_water(p)) {
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
#include "map.c"
#include "holy_ground.c"
int main(int argc, char *argv[]) {
    char *map_string, *threat_string;
    attackradius2 = 5;
    viewradius2 = 55;

    map_string = "...b....";
    threat_string = "2222222.";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);
    
    map_string = "........\n"
                 "........\n"
                 "........\n"
                 "...b....\n"
                 "........\n"
                 "........\n"
                 "........\n"
                 "........";
    
    threat_string = "..222...\n"
                    ".22222..\n"
                    "2222222.\n"
                    "2222222.\n"
                    "2222222.\n"
                    ".22222..\n"
                    "..222...\n"
                    "........";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);
    
    map_string = "........\n"
                 "........\n"
                 "........\n"
                 "...b....\n"
                 "...%....\n"
                 "........\n"
                 "........\n"
                 "........";
    
    threat_string = "..222...\n"
                    ".22222..\n"
                    "2222222.\n"
                    "2222222.\n"
                    "222%222.\n"
                    ".22222..\n"
                    "........\n"
                    "........";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);
    
    map_string = "b.......\n"
                 "........\n"
                 "........\n"
                 "........\n"
                 "........\n"
                 "........\n"
                 "........\n"
                 "........";
    
    threat_string = "2222.222\n"
                    "2222.222\n"
                    "222...22\n"
                    "22.....2\n"
                    "........\n"
                    "22.....2\n"
                    "222...22\n"
                    "2222.222";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);
    
    map_string = "........\n"
                 "........\n"
                 "........\n"
                 "...b....\n"
                 "...b....\n"
                 "........\n"
                 "........\n"
                 "........";
    threat_string = "..222...\n"
                    ".23332..\n"
                    "2333332.\n"
                    "3333333.\n"
                    "3333333.\n"
                    "2333332.\n"
                    ".23332..\n"
                    "..222...";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);
    
    map_string = ".........\n"
                 ".........\n"
                 ".........\n"
                 "a...b....\n"
                 ".........\n"
                 ".........\n"
                 ".........\n"
                 ".........";
    threat_string = "...222...\n"
                    "..22222..\n"
                    ".2222222.\n"
                    ".2222222.\n"
                    ".2222222.\n"
                    "..22222..\n"
                    "...222...\n"
                    ".........";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);
    
    map_string = ".........\n"
                 ".........\n"
                 ".........\n"
                 "a...b....\n"
                 "a........\n"
                 ".........\n"
                 ".........\n"
                 ".........";
    threat_string = "...222...\n"
                    "..22222..\n"
                    ".2222222.\n"
                    ".2222222.\n"
                    ".2222222.\n"
                    "..22222..\n"
                    "...222...\n"
                    ".........";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = ".........\n"
                 ".........\n"
                 "a........\n"
                 "a...b....\n"
                 "a........\n"
                 ".........\n"
                 ".........\n"
                 ".........";
    threat_string = "...222...\n"
                    "..22222..\n"
                    ".!222222.\n"
                    ".!222222.\n"
                    ".!222222.\n"
                    "..22222..\n"
                    "...222...\n"
                    ".........";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = ".........\n"
                 ".........\n"
                 ".a.......\n"
                 "a...b....\n"
                 "a........\n"
                 ".........\n"
                 ".........\n"
                 ".........";
    threat_string = "...222...\n"
                    "..22222..\n"
                    ".!222222.\n"
                    ".!222222.\n"
                    ".!222222.\n"
                    "..22222..\n"
                    "...222...\n"
                    ".........";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = ".........\n"
                 ".........\n"
                 ".a.......\n"
                 ".a..b....\n"
                 "a........\n"
                 ".........\n"
                 ".........\n"
                 ".........";
    threat_string = "...222...\n"
                    "..22222..\n"
                    ".!222222.\n"
                    ".!222222.\n"
                    ".!222222.\n"
                    "..22222..\n"
                    "...222...\n"
                    ".........";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = ".........\n"
                 ".........\n"
                 ".a.......\n"
                 ".a..b....\n"
                 ".a.......\n"
                 ".........\n"
                 ".........\n"
                 ".........";
    threat_string = "...222...\n"
                    "..22222..\n"
                    ".!222222.\n"
                    ".!222222.\n"
                    ".!222222.\n"
                    "..22222..\n"
                    "...222...\n"
                    ".........";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = ".........\n"
                 ".........\n"
                 "a........\n"
                 "a...b....\n"
                 "a%.......\n"
                 ".........\n"
                 ".........\n"
                 ".........";
    threat_string = "...222...\n"
                    "..22222..\n"
                    ".2222222.\n"
                    ".2222222.\n"
                    ".%222222.\n"
                    "..22222..\n"
                    "...222...\n"
                    ".........";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = ".........\n"
                 ".........\n"
                 "a........\n"
                 "a...b....\n"
                 ".........\n"
                 ".a.......\n"
                 ".........\n"
                 ".........";
    threat_string = "...222...\n"
                    "..22222..\n"
                    ".!222222.\n"
                    ".!222222.\n"
                    ".!222222.\n"
                    "..22222..\n"
                    "...222...\n"
                    ".........";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = ".........\n"
                 ".a.......\n"
                 ".a.......\n"
                 ".a..b....\n"
                 ".a.......\n"
                 ".a.......\n"
                 ".........\n"
                 ".........";
    threat_string = "...222...\n"
                    "..#2222..\n"
                    ".!#22222.\n"
                    ".!#22222.\n"
                    ".!#22222.\n"
                    "..#2222..\n"
                    "...222...\n"
                    ".........";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    map_string = ".........\n"
                 ".........\n"
                 "a........\n"
                 "a...b....\n"
                 "....b....\n"
                 ".........\n"
                 ".........\n"
                 ".........";
    threat_string = "...222...\n"
                    "..23332..\n"
                    ".2333332.\n"
                    ".3333333.\n"
                    ".3333333.\n"
                    ".2333332.\n"
                    "..23332..\n"
                    "...222...";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    puts("ok");
    return 0;
}
#endif
