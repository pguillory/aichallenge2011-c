#include <assert.h>
#include <string.h>
#include "map.h"
#include "holy_ground.h"
#include "threat.h"
#include "mystery.h"
#include "aroma.h"

#define AROMA_NATURAL_DISSIPATION  0.95
#define AROMA_ANT_DISSIPATION      0.0
#define AROMA_ARMY_ANT_DISSIPATION 0.05

#define AROMA_FOOD               100.0
#define AROMA_ENEMY                5.0
#define AROMA_INTRUDER           200.0
#define AROMA_ENEMY_HILL         500.0
#define AROMA_MYSTERY             10.0
#define AROMA_CONFLICT             0.0
#define AROMA_CONFLICT_FRONT      20.0
#define AROMA_CONFLICT_SIDE        0.0
#define AROMA_CONFLICT_REAR        0.0

void reset_aroma_at(point p) {
    grid(aroma, p) = 0.0;
    grid(army_aroma, p) = 0.0;
}

void aroma_reset() {
    foreach_point(reset_aroma_at);
}

void dissipate_aroma(point p) {
    if (map_has_friendly_ant(p)) {
        grid(aroma, p) *= AROMA_ANT_DISSIPATION;
        grid(army_aroma, p) *= AROMA_ARMY_ANT_DISSIPATION;
    } else {
        grid(aroma, p) *= AROMA_NATURAL_DISSIPATION;
        grid(army_aroma, p) *= AROMA_NATURAL_DISSIPATION;
    }
}

float aroma1[MAX_ROWS][MAX_COLS];
float aroma2[MAX_ROWS][MAX_COLS];
float *channels[MAX_ROWS][MAX_COLS][5];
int channel_counts[MAX_ROWS][MAX_COLS];

void add_channel(point p, float *channel) {
    int count = channel_counts[p.row][p.col]++;
    channels[p.row][p.col][count] = channel;
}

void calculate_channels_at(point p) {
    int dir;
    point p2;

    if (!map_has_water(p)) {
        add_channel(p, &grid(aroma1, p));
        for (dir = 1; dir < STAY; dir *= 2) {
            p2 = neighbor(p, dir);
            if (!map_has_water(p2)) {
                add_channel(p, &grid(aroma1, p2));
            }
        }
    }
}

void calculate_channels() {
    memset(channels, 0, sizeof(channels));
    memset(channel_counts, 0, sizeof(channel_counts));

    foreach_point(calculate_channels_at);
}

void spread_aroma_at(point p) {
    int i;
    int count = channel_counts[p.row][p.col];
    float sum = 0;

    if (count) {
        for (i = 0; i < count; i++) {
            sum += *channels[p.row][p.col][i];
        }

        aroma2[p.row][p.col] = sum / count;
    } else {
        aroma2[p.row][p.col] = 0;
    }
}

void spread_aroma() {
    memcpy(aroma1, aroma, sizeof(aroma));
    foreach_point(spread_aroma_at);
    memcpy(aroma, aroma2, sizeof(aroma));
    
    memcpy(aroma1, army_aroma, sizeof(aroma));
    foreach_point(spread_aroma_at);
    memcpy(army_aroma, aroma2, sizeof(aroma));
}

void add_aroma(point p) {
    point p2;
    int dir;

    if (map_has_food(p)) {
        grid(aroma, p) += AROMA_FOOD;
    }
    if (map_has_enemy_hill(p)) {
        grid(aroma, p) += AROMA_ENEMY_HILL;
        grid(army_aroma, p) += AROMA_ENEMY_HILL;
    }
    grid(aroma, p) += AROMA_MYSTERY * grid(mystery, p) / MYSTERY_MAX;

    if (map_has_ant(p)) {
        if (map_is_enemy(p)) {
            if (grid(holy_ground, p)) {
                grid(aroma, p) += AROMA_INTRUDER;
                grid(army_aroma, p) += AROMA_INTRUDER;
            } else {
                // grid(aroma, p) += AROMA_ENEMY;
                grid(army_aroma, p) += AROMA_ENEMY;
            }
        } else {
            if (grid(visible_ally_count, p) >= 3) {
                for (dir = 1; dir < STAY; dir *= 2) {
                    p2 = neighbor(p, dir);
                    if (map_has_land(p2) &&
                        (grid(enemy_could_attack, p2) > grid(enemy_could_attack, p))
                    ) {
                        // grid(aroma, p) += AROMA_CONFLICT;
                        grid(aroma, p2) += AROMA_CONFLICT_FRONT;
                        grid(army_aroma, p2) += AROMA_CONFLICT_FRONT;
                    }
                }
            }
        }
    }
}

void aroma_iterate(int times) {
    // long int start_time;
    int i;
    // long int channels_time = 0;
    // long int dissipate_time = 0;
    // long int spread_time = 0;
    // long int add_time = 0;

    // start_time = now();
    calculate_channels();
    // channels_time += now() - start_time;

    for (i = 0; i < times; i++) {
        // start_time = now();
        foreach_point(dissipate_aroma);
        // dissipate_time += now() - start_time;

        // start_time = now();
        spread_aroma();
        // spread_time += now() - start_time;

        // start_time = now();
        foreach_point(add_aroma);
        // add_time += now() - start_time;
    }

    // fprintf(logfile, "aroma_iterate(1): channels=%li, dissipate=%li, spread=%li, add=%li\n",
    //         channels_time, dissipate_time, spread_time, add_time);
}

void aroma_stabilize() {
    aroma_reset();
    aroma_iterate(20);
}

char *aroma_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    point p;
    char c;
    float v;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            v = grid(aroma, p);
            if (v <= 0) c = '-';
            else if (v < 1) c = '0';
            else if (v < 2) c = '1';
            else if (v < 4) c = '2';
            else if (v < 8) c = '3';
            else if (v < 16) c = '4';
            else if (v < 32) c = '5';
            else if (v < 64) c = '6';
            else if (v < 128) c = '7';
            else if (v < 256) c = '8';
            else if (v < 512) c = '9';
            else if (v < 1024) c = 'a';
            else if (v < 2048) c = 'b';
            else if (v < 4096) c = 'c';
            else if (v < 8192) c = 'd';
            else if (v < 16384) c = 'e';
            else if (v < 32768) c = 'f';
            else c = '+';

            *output++ = c;
        }
        *output++ = '\n';
    }
    *--output = '\0';
    return buffer;
}

char *army_aroma_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    point p;
    char c;
    float v;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            v = grid(army_aroma, p);
            if (v <= 0) c = '-';
            else if (v < 1) c = '0';
            else if (v < 2) c = '1';
            else if (v < 4) c = '2';
            else if (v < 8) c = '3';
            else if (v < 16) c = '4';
            else if (v < 32) c = '5';
            else if (v < 64) c = '6';
            else if (v < 128) c = '7';
            else if (v < 256) c = '8';
            else if (v < 512) c = '9';
            else if (v < 1024) c = 'a';
            else if (v < 2048) c = 'b';
            else if (v < 4096) c = 'c';
            else if (v < 8192) c = 'd';
            else if (v < 16384) c = 'e';
            else if (v < 32768) c = 'f';
            else c = '+';

            *output++ = c;
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
int main(int argc, char *argv[]) {
    init_log();
    attackradius2 = 5;
    viewradius2 = 55;
    // puts(aroma_to_string());
    // printf("%f %f %f %f\n", aroma[0][0], aroma[0][1], aroma[0][2], aroma[0][3]);

    map_load_from_string(".%*%");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    // puts(aroma_to_string());
    assert(aroma[0][0] == 0);
    assert(army_aroma[0][0] == 0);

    map_load_from_string("..*%");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    // puts(aroma_to_string());
    assert(aroma[0][0] > 0);
    assert(army_aroma[0][0] == 0);

    map_load_from_string(".%*.");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    // puts(aroma_to_string());
    assert(aroma[0][0] > 0);
    assert(army_aroma[0][0] == 0);

    // map_load_from_string(".%?%");
    // holy_ground_calculate();
    // threat_calculate();
    // mystery_reset();
    // aroma_stabilize();
    // assert(aroma[0][0] == 0);
    // 
    // map_load_from_string("..?%");
    // holy_ground_calculate();
    // threat_calculate();
    // mystery_reset();
    // aroma_stabilize();
    // puts(aroma_to_string());
    // assert(aroma[0][0] > 0);

    map_load_from_string(".%...%*%\n"
                         "...%...%\n"
                         "%%%%%%%%");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_stabilize();
    // puts(aroma_to_string());
    assert(aroma[0][0] > 0);
    assert(aroma[0][1] == 0);
    assert(aroma[0][2] > aroma[0][0]);
    assert(aroma[0][3] > aroma[0][2]);
    assert(aroma[0][4] > aroma[0][3]);
    assert(aroma[0][5] == 0);

    map_load_from_string("*1");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_reset();
    aroma_iterate(1);
    // puts(aroma_to_string());
    assert(aroma[0][0] == AROMA_FOOD);
    assert(aroma[0][1] == AROMA_ENEMY_HILL);

    map_load_from_string("0...........bb............................");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_reset();
    aroma_iterate(1);
    // puts(aroma_to_string());
    assert(HOLY_GROUND_RANGE == 12);
    // printf("%f %f %f %f\n", aroma[0][11], aroma[0][12], aroma[0][13], aroma[0][14]);
    assert(aroma[0][11] == 0.0);
    assert(aroma[0][12] == AROMA_INTRUDER);
    // assert(aroma[0][13] == AROMA_ENEMY);
    assert(aroma[0][14] == 0.0);
    assert(army_aroma[0][11] == 0.0);
    assert(army_aroma[0][12] == AROMA_INTRUDER);
    // assert(army_aroma[0][13] == AROMA_ENEMY);
    assert(army_aroma[0][14] == 0.0);

    map_load_from_string("?.");
    holy_ground_calculate();
    threat_calculate();
    mystery_reset();
    aroma_reset();
    aroma_iterate(1);
    // puts(aroma_to_string());
    assert(aroma[0][0] >= AROMA_MYSTERY * MYSTERY_INITIAL / MYSTERY_MAX - 0.00001);
    assert(aroma[0][0] <= AROMA_MYSTERY * MYSTERY_INITIAL / MYSTERY_MAX + 0.00001);
    mystery[0][0] = MYSTERY_MAX;
    aroma_reset();
    aroma_iterate(1);
    assert(aroma[0][0] == AROMA_MYSTERY);

    map_load_from_string("..................\n"
                         "..................\n"
                         "..................\n"
                         "..................\n"
                         "..a...b...........\n"
                         "..................\n"
                         "..................\n"
                         "..................\n"
                         "..................\n"
                         "..................\n"
                         "..................");
    // puts(map_to_string());
    holy_ground_calculate();
    // puts(holy_ground_to_string());
    threat_calculate();
    mystery_reset();
    aroma_reset();
    aroma_iterate(1);
    // puts(aroma_to_string());
    // puts(army_aroma_to_string());
    // printf("%f %f %f %f %f\n", aroma[4][2], aroma[4][3], aroma[5][2], aroma[3][2], aroma[4][1]);
    assert(aroma[4][2] == AROMA_CONFLICT);
    // assert(aroma[4][3] == AROMA_CONFLICT_FRONT);
    // assert(aroma[5][2] == AROMA_CONFLICT_SIDE);
    // assert(aroma[3][2] == AROMA_CONFLICT_SIDE);
    // assert(aroma[4][1] == AROMA_CONFLICT_REAR);

    puts("ok");
    return 0;
}
#endif
