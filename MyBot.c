#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "bot.h"
#include "handler.h"

void read_split_command(int word_count, char *words[256]) {
    switch (word_count) {
        case 1:
            if (0 == strcmp(words[0], "go")) {
                bot_end_turn();
            } else if (0 == strcmp(words[0], "ready")) {
                bot_init();
            } else if (0 == strcmp(words[0], "end")) {
                exit(0);
            }
            break;

        case 2:
            if (0 == strcmp(words[0], "turn")) {
                turn = atoi(words[1]);
                if (turn > 0) {
                    bot_begin_turn();
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
                bot_see_water((point) {atoi(words[1]), atoi(words[2])});
            } else if (0 == strcmp(words[0], "f")) {
                bot_see_food((point) {atoi(words[1]), atoi(words[2])});
            }
            break;

        case 4:
            if (0 == strcmp(words[0], "a")) {
                bot_see_ant((point) {atoi(words[1]), atoi(words[2])}, atoi(words[3]));
            } else if (0 == strcmp(words[0], "h")) {
                bot_see_hill((point) {atoi(words[1]), atoi(words[2])}, atoi(words[3]));
            } else if (0 == strcmp(words[0], "d")) {
                bot_see_dead_ant((point) {atoi(words[1]), atoi(words[2])}, atoi(words[3]));
            }
            break;
    }
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
            if (word_count == 256) return;
        }
        read_head++;
    }

    read_split_command(word_count, words);
}

void read_commands_forever() {
    char command[256 * 1024];
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

void default_settings() {
    turn = 0;
    loadtime = 3000;
    turntime = 1000;
    rows = MAX_ROWS;
    cols = MAX_COLS;
    turns = 500;
    viewradius2 = 55;
    attackradius2 = 5;
    spawnradius2 = 1;
    player_seed = 42;
}

int main(int argc, char *argv[]) {
    install_handlers();
    init_log();
    read_commands_forever();
    return 0;
}
