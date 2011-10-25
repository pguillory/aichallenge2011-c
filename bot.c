#include <assert.h>
#include <string.h>
#include "globals.h"
#include "map.h"
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

    // calculate_sight();
    // 
    logs(map_to_string());
    logs(aroma_to_string());
    logs(directions_to_string());
    // puts_aroma2();
    // puts_direction();
    // puts_aroma();

    static char available[MAX_ROWS][MAX_COLS];

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            available[row][col] = !(map[row][col] & SQUARE_ANT_MASK);
        }
    }

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if ((map[row][col] & SQUARE_ANT_MASK) && (owner[row][col] == 0)) {
                direction = directions[row][col];
                switch (direction) {
                    case 'N':
                        row2 = normalize_row(row - 1);
                        col2 = col;
                        if (available[row2][col2]) {
                            available[row][col] = 1;
                            available[row2][col2] = 0;
                            server_order(row, col, direction);
                        }
                        break;
                    case 'S':
                        row2 = normalize_row(row + 1);
                        col2 = col;
                        if (available[row2][col2]) {
                            available[row][col] = 1;
                            available[row2][col2] = 0;
                            server_order(row, col, direction);
                        }
                        break;
                    case 'E':
                        row2 = row;
                        col2 = normalize_col(col + 1);
                        if (available[row2][col2]) {
                            available[row][col] = 1;
                            available[row2][col2] = 0;
                            server_order(row, col, direction);
                        }
                        break;
                    case 'W':
                        row2 = row;
                        col2 = normalize_col(col - 1);
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
