#include <assert.h>
#include <string.h>
#include "globals.h"
#include "map.h"
#include "directions.h"
#include "server.h"
#include "bot.h"

void bot_init() {
    
}

void bot_issue_orders() {
    point p;
    // int row2, col2;
    char direction;

    for (p.row = 0; p.row < rows; p.row++) {
        for (p.col = 0; p.col < cols; p.col++) {
            if (friendly_ant_exists_at(p)) {
                direction = directions[p.row][p.col];
                switch (direction) {
                    case 'N':
                    case 'S':
                    case 'E':
                    case 'W':
                        server_order(p, direction);
                        break;
                }
            }
        }
    }
}

#ifdef UNIT_TESTS
#undef UNIT_TESTS
#include "globals.c"
#include "map.c"
#include "directions.c"
#include "server.c"
int main(int argc, char *argv[]) {

    puts("ok");
    return 0;
}
#endif
