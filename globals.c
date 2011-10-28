#include <assert.h>
#include "globals.h"

int abs(int x) {
    return (x >= 0) ? x : -x;
}

int distance2(int row1, int col1, int row2, int col2) {
    int dr, dc;
    int abs1, abs2;

    abs1 = abs(row1 - row2);
    abs2 = rows - abs(row1 - row2);

    dr = (abs1 < abs2) ? abs1 : abs2;

    abs1 = abs(col1 - col2);
    abs2 = cols - abs(col1 - col2);

    dc = (abs1 < abs2) ? abs1 : abs2;

    return dr * dr + dc * dc;
}

void neighbor(int row, int col, int direction, int *row2, int *col2) {
    switch (direction) {
        case NORTH:
            *row2 = normalize_row(row - 1);
            *col2 = col;
            break;
        case SOUTH:
            *row2 = normalize_row(row + 1);
            *col2 = col;
            break;
        case EAST:
            *row2 = row;
            *col2 = normalize_col(col + 1);
            break;
        case WEST:
            *row2 = row;
            *col2 = normalize_col(col - 1);
            break;
    }
}

int normalize_row(int row) {
    while (row < rows) row += rows;
    return row % rows;
}

int normalize_col(int col) {
    while (col < cols) col += cols;
    return col % cols;
}

void init_log() {
    logfile = fopen("log.txt", "w");
}

void logs(char *s) {
    fputs(s, logfile);
    fputs("\n", logfile);
}

#ifdef UNIT_TESTS
int main(int argc, char *argv[]) {
    rows = 10;
    cols = 10;

    assert(distance2(0, 0, -1, -1) == 2);
    assert(distance2(0, 0,  0,  0) == 0);
    assert(distance2(0, 0,  0,  1) == 1);
    assert(distance2(0, 0,  0,  2) == 4);
    assert(distance2(0, 0,  2,  2) == 8);
    assert(distance2(0, 0,  9,  9) == 2);

    rows = 10;
    cols = 10;

    assert(normalize_row(-2) == 8);
    assert(normalize_row(-1) == 9);
    assert(normalize_row(0) == 0);
    assert(normalize_row(1) == 1);
    assert(normalize_row(9) == 9);
    assert(normalize_row(11) == 1);
    
    assert(normalize_col(-2) == 8);
    assert(normalize_col(-1) == 9);
    assert(normalize_col(0) == 0);
    assert(normalize_col(1) == 1);
    assert(normalize_col(9) == 9);
    assert(normalize_col(10) == 0);
    assert(normalize_col(11) == 1);

    puts("ok");
    return 0;
}
#endif
