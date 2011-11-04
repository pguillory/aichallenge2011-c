#include <assert.h>
#include <string.h>
#include "map.h"
#include "aroma.h"
#include "army.h"

#define MIN_ARMY_SIZE 3

void enlist(point p) {
    army[p.row][p.col] = 0;
}

void promote(point p) {
    if (map_has_friendly_ant(p)) {
        army[p.row][p.col] += 1;
    }
}

void initial_inspection(point p) {
    if (army_aroma[p.row][p.col] == 0.0) return;

    if (map_has_friendly_ant(p)) {
        foreach_point_within_manhattan_distance(p, 1, promote);
    }
}

void officer_promote_neighbors(point p) {
    if (army[p.row][p.col] < MIN_ARMY_SIZE) return;

    if (map_has_friendly_ant(p)) {
        foreach_point_within_manhattan_distance(p, 1, promote);
    }
}

void final_inspection(point p) {
    if (army[p.row][p.col] < MIN_ARMY_SIZE) {
        army[p.row][p.col] = 0;
    } else {
        army[p.row][p.col] = 1;
    }
}

void army_calculate() {
    foreach_point(enlist);

    foreach_point(initial_inspection);

    foreach_point(officer_promote_neighbors);

    foreach_point(final_inspection);
}

char *army_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    point p;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            if (map_has_land(p)) {
                if (map_has_food(p)) {
                    *output++ = '*';
                } else if (map_has_ant(p)) {
                    // *output++ = '0' + army[p.row][p.col];
                    if (army[p.row][p.col]) {
                        *output++ = 'A' + owner[p.row][p.col];
                    } else {
                        *output++ = 'a' + owner[p.row][p.col];
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
#include "aroma.c"
int main(int argc, char *argv[]) {
    char *input, *expected;

    input = "..aa..b.....*.............\n"
            "...aa.b............a......\n"
            "...........a..........a...\n"
            "....a.......a.........a...\n"
            ".................aa...a...\n"
            "..........................\n"
            "....a..a.......aaaa.......\n"
            "....aa.a..........a.......\n"
            "................a.a.......\n"
            "..................a.......\n"
            "..........................";
    expected = "..AA..b.....*.............\n"
               "...AA.b............a......\n"
               "...........a..........A...\n"
               "....a.......a.........A...\n"
               ".................aa...A...\n"
               "..........................\n"
               "....A..a.......AAAA.......\n"
               "....AA.a..........A.......\n"
               "................a.A.......\n"
               "..................A.......\n"
               "..........................";
    map_load_from_string(input);
    aroma_stabilize();
    army_calculate();
    // puts(map_to_string());
    // puts(army_to_string());
    assert(strcmp(army_to_string(), expected) == 0);

    puts("ok");
    return 0;
}
#endif
