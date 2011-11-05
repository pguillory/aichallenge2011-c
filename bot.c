#include <assert.h>
#include <string.h>
#include "globals.h"
#include "map.h"
#include "directions.h"
#include "server.h"
#include "bot.h"

void bot_init() {
    
}

void issue_order_at(point p) {
    if (map_has_friendly_ant(p)) {
        switch (grid(directions, p)) {
            case NORTH:
                server_order(p, 'N');
                break;
            case EAST:
                server_order(p, 'E');
                break;
            case SOUTH:
                server_order(p, 'S');
                break;
            case WEST:
                server_order(p, 'W');
                break;
        }
    }
}

void bot_issue_orders() {
    foreach_point(issue_order_at);
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
