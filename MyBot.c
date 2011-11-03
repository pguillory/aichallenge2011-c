#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "globals.h"
#include "map.h"
#include "holy_ground.h"
#include "threat.h"
#include "mystery.h"
#include "aroma.h"
#include "army.h"
#include "directions.h"
#include "bot.h"
#include "server.h"
#include "handler.h"

void dump_state() {
    // logs("map:");
    logs(map_to_string());
    // logs("holy ground:");
    // logs(holy_ground_to_string());
    // logs("threat:");
    // logs(threat_to_string());
    // // logs(mystery_to_string());
    // logs("aroma:");
    // logs(aroma_to_string());
    // logs("directions:");
    // logs(directions_to_string());
}

long int start_time, end_time;
long int now() {
    return clock() / (CLOCKS_PER_SEC / 1000);
}

void read_command(char *command) {
    char *words[256];
    int word_count = 0;
    char *read_head = command;
    int looking_for_word = 1;
    while (*read_head) {
        if (*read_head == ' ') {
            looking_for_word = 1;
            *read_head = '\0';
        } else if (looking_for_word) {
            looking_for_word = 0;
            words[word_count++] = read_head;
        }
        read_head++;
    }

    point p;
    int player;
    switch (word_count) {
        case 1:
            if (0 == strcmp(words[0], "go")) {
                fprintf(logfile, "end turn %i\n", turn);
                start_time = now();
                map_finish_update();
                fprintf(logfile, "map_finish_update(): %li ms elapsed\n", now() - start_time);
                start_time = now();
                holy_ground_calculate();
                fprintf(logfile, "holy_ground_calculate(): %li ms elapsed\n", now() - start_time);
                start_time = now();
                threat_calculate();
                fprintf(logfile, "threat_calculate(): %li ms elapsed\n", now() - start_time);
                start_time = now();
                mystery_iterate();
                fprintf(logfile, "mystery_iterate(): %li ms elapsed\n", now() - start_time);
                start_time = now();
                aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate();
                aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate();
                // aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate();
                // aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate();
                // aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate();
                // aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate(); aroma_iterate();
                fprintf(logfile, "aroma_iterate(): %li ms elapsed\n", now() - start_time);
                start_time = now();
                army_calculate();
                fprintf(logfile, "army_calculate(): %li ms elapsed\n", now() - start_time);
                start_time = now();
                directions_calculate();
                fprintf(logfile, "directions_calculate(): %li ms elapsed\n", now() - start_time);
                start_time = now();
                // dump_state();
                bot_issue_orders();
                fprintf(logfile, "bot_issue_orders(): %li ms elapsed\n", now() - start_time);
                server_go();
            } else if (0 == strcmp(words[0], "ready")) {
                bot_init();
                server_go();
            } else if (0 == strcmp(words[0], "end")) {
                exit(0);
            }
            break;

        case 2:
            if (0 == strcmp(words[0], "turn")) {
                turn = atoi(words[1]);
                if (turn > 0) {
                    fprintf(logfile, "start turn %i\n", turn);
                    map_begin_update();
                }
            } else if (0 == strcmp(words[0], "loadtime")) {
                loadtime = atoi(words[1]);
            } else if (0 == strcmp(words[0], "turntime")) {
                turntime = atoi(words[1]);
            } else if (0 == strcmp(words[0], "rows")) {
                rows = atoi(words[1]);
            } else if (0 == strcmp(words[0], "cols")) {
                cols = atoi(words[1]);
            } else if (0 == strcmp(words[0], "turns")) {
                turns = atoi(words[1]);
            } else if (0 == strcmp(words[0], "viewradius2")) {
                viewradius2 = atoi(words[1]);
                // viewradius = sqrt(viewradius2);
            } else if (0 == strcmp(words[0], "attackradius2")) {
                attackradius2 = atoi(words[1]);
            } else if (0 == strcmp(words[0], "spawnradius2")) {
                spawnradius2 = atoi(words[1]);
            } else if (0 == strcmp(words[0], "player_seed")) {
                player_seed = atoi(words[1]);
            }
            break;

        case 3:
            if (0 == strcmp(words[0], "w")) {
                p.row = atoi(words[1]);
                p.col = atoi(words[2]);
                update[p.row][p.col] |= SQUARE_WATER;
            } else if (0 == strcmp(words[0], "f")) {
                p.row = atoi(words[1]);
                p.col = atoi(words[2]);
                update[p.row][p.col] |= SQUARE_FOOD;
            }
            break;

        case 4:
            if (0 == strcmp(words[0], "h")) {
                p.row = atoi(words[1]);
                p.col = atoi(words[2]);
                player = atoi(words[3]);
                update[p.row][p.col] |= SQUARE_HILL;
                owner[p.row][p.col] = player;
            } else if (0 == strcmp(words[0], "a")) {
                p.row = atoi(words[1]);
                p.col = atoi(words[2]);
                player = atoi(words[3]);
                update[p.row][p.col] |= SQUARE_ANT;
                owner[p.row][p.col] = player;
            } else if (0 == strcmp(words[0], "d")) {
                // dead ant
            }
            break;
    }
}

void read_commands_forever() {
    char command[10 * 1024];
    char *write_ptr;
    char c;

    do {
        write_ptr = command;
        while (1) {
            c = getchar();
            if (c == '\n' || c == EOF) break;
            *write_ptr++ = c;
        }
        *write_ptr = '\0';

        read_command(command);
    } while (c != EOF);
}

int main(int argc, char *argv[]) {
    install_handlers();
    init_log();
    map_reset();
    mystery_reset();
    aroma_reset();
    read_commands_forever();
    return 0;
}
