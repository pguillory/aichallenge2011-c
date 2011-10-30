#include <assert.h>
#include <string.h>
#include "globals.h"
#include "map.h"
#include "holy_ground.h"
#include "threat.h"
#include "mystery.h"
#include "aroma.h"
#include "directions.h"
#include "server.h"
#include "bot.h"

void bot_init() {
    
}

void bot_issue_orders() {
    int row, col;
    int row2, col2;
    char direction;

    static char available[MAX_ROWS][MAX_COLS];

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            available[row][col] = !(map[row][col] & SQUARE_ANT);
        }
    }

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if ((map[row][col] & SQUARE_ANT) && (owner[row][col] == 0)) {
                direction = directions[row][col];
                switch (direction) {
                    case 'N':
                        neighbor(row, col, NORTH, &row2, &col2);
                        if (available[row2][col2]) {
                            available[row][col] = 1;
                            available[row2][col2] = 0;
                            server_order(row, col, direction);
                        }
                        break;
                    case 'S':
                        neighbor(row, col, SOUTH, &row2, &col2);
                        if (available[row2][col2]) {
                            available[row][col] = 1;
                            available[row2][col2] = 0;
                            server_order(row, col, direction);
                        }
                        break;
                    case 'E':
                        neighbor(row, col, EAST, &row2, &col2);
                        if (available[row2][col2]) {
                            available[row][col] = 1;
                            available[row2][col2] = 0;
                            server_order(row, col, direction);
                        }
                        break;
                    case 'W':
                        neighbor(row, col, WEST, &row2, &col2);
                        if (available[row2][col2]) {
                            available[row][col] = 1;
                            available[row2][col2] = 0;
                            server_order(row, col, direction);
                        }
                        break;
                    default:
                        // available[row][col] = 0;
                        break;
                }
                // if (c == 'N' || c == 'S' || c == 'E' || c == 'W') {
                //     server_order(row, col, c);
                // }
            }
        }
    }
}

#ifdef UNIT_TESTS
#undef UNIT_TESTS
#include "globals.c"
#include "map.c"
#include "aroma.c"
#include "directions.c"
#include "server.c"
int main(int argc, char *argv[]) {

    puts("ok");
    return 0;
}
#endif
