#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "globals.h"
#include "map.h"
#include "bot.h"
#include "handler.h"

void send_go_command() {
    puts("go");
    fflush(stdout);
}

void read_command(char *command) {
    char *words[256];
    int word_count = 0;
    char *p = command;
    int looking_for_word = 1;
    while (*p) {
        if (*p == ' ') {
            looking_for_word = 1;
            *p = '\0';
        } else if (looking_for_word) {
            looking_for_word = 0;
            words[word_count++] = p;
        }
        p++;
    }

    switch (word_count) {
        case 1:
            if (0 == strcmp(words[0], "go")) {
                issue_orders();
                send_go_command();
            } else if (0 == strcmp(words[0], "ready")) {
                bot_init();
                send_go_command();
            } else if (0 == strcmp(words[0], "end")) {
                // game over
                map_new_turn();
            }
            break;

        case 2:
            if (0 == strcmp(words[0], "turn")) {
                turn = atoi(words[1]);
                fprintf(logfile, "start turn %i\n", turn);
                // if (turn == 0) {
                //     map_blank();
                // } else {
                    map_new_turn();
                // }
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
                map_discover_water(atoi(words[1]), atoi(words[2]));
            } else if (0 == strcmp(words[0], "f")) {
                map_discover_food(atoi(words[1]), atoi(words[2]));
            }
            break;

        case 4:
            if (0 == strcmp(words[0], "h")) {
                map_discover_hill(atoi(words[1]), atoi(words[2]), atoi(words[3]));
            } else if (0 == strcmp(words[0], "a")) {
                map_discover_ant(atoi(words[1]), atoi(words[2]), atoi(words[3]));
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
    map_blank();
    read_commands_forever();
    return 0;
}
