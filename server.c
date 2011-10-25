#include <assert.h>
#include <stdio.h>
// #include <string.h>

void server_go() {
    puts("go");
    fflush(stdout);
}

void server_order(int row, int col, char direction) {
    printf("o %i %i %c\n", row, col, direction);
}

#ifdef UNIT_TESTS
#undef UNIT_TESTS
#include "globals.c"
int main(int argc, char *argv[]) {
    puts("ok");
    return 0;
}
#endif
