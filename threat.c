#include <math.h>
#include "map.h"
#include "holy_ground.h"
#include "threat.h"

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
    enemy_could_occupy[p.row][p.col] = 0;
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
    // add 1 threat to every square that could be attacked by any enemy
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
        foreach_point_within_manhattan_distance(p, 1, decrement_threat_to_zero);
    }
}

void threat_calculate() {
    foreach_point(reset_threat_at);

    foreach_point(add_threat_for_each_enemy_that_could_attack);

    foreach_point(boost_threat);

    foreach_point(decrement_threat_around_friendly_ant);

    // foreach_point(scan_for_potential_line_attacks);
}

char *threat_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    point p;
    int value;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            value = (int) threat[p.row][p.col];
            if (value < 0) {
                *output++ = '-';
            } else if (value < 10) {
                *output++ = '0' + value;
            } else if (value < 36) {
                *output++ = 'a' + (value - 10);
            } else {
                *output++ = '+';
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
    threat_string = "22222220";
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

    threat_string = "00222000\n"
                    "02222200\n"
                    "22222220\n"
                    "22222220\n"
                    "22222220\n"
                    "02222200\n"
                    "00222000\n"
                    "00000000";
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

    threat_string = "00222000\n"
                    "02222200\n"
                    "22222220\n"
                    "22222220\n"
                    "22222220\n"
                    "02222200\n"
                    "00000000\n"
                    "00000000";
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

    threat_string = "22220222\n"
                    "22220222\n"
                    "22200022\n"
                    "22000002\n"
                    "00000000\n"
                    "22000002\n"
                    "22200022\n"
                    "22220222";
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
    threat_string = "00222000\n"
                    "02333200\n"
                    "23333320\n"
                    "33333330\n"
                    "33333330\n"
                    "23333320\n"
                    "02333200\n"
                    "00222000";
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
    threat_string = "000222000\n"
                    "002222200\n"
                    "012222220\n"
                    "012222220\n"
                    "012222220\n"
                    "002222200\n"
                    "000222000\n"
                    "000000000";
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
    threat_string = "000222000\n"
                    "002222200\n"
                    "012222220\n"
                    "002222220\n"
                    "002222220\n"
                    "002222200\n"
                    "000222000\n"
                    "000000000";
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
    threat_string = "000222000\n"
                    "002333200\n"
                    "003333320\n"
                    "013333330\n"
                    "023333330\n"
                    "023333320\n"
                    "002333200\n"
                    "000222000";
    map_load_from_string(map_string);
    threat_calculate();
    // puts(threat_to_string());
    assert(strcmp(threat_to_string(), threat_string) == 0);

    puts("ok");
    return 0;
}
#endif
