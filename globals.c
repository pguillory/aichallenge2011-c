#include <assert.h>
#include "globals.h"

point normalize(point p) {
    while (p.row < rows) p.row += rows;
    p.row %= rows;
    while (p.col < cols) p.col += cols;
    p.col %= cols;
    return p;
}

point add_points(point p1, point p2) {
    p1.row += p2.row;
    p1.col += p2.col;
    return normalize(p1);
}

int points_equal(point p1, point p2) {
    return ((p1.row == p2.row) && (p1.col == p2.col));
}

// int point_is_origin(point p) {
//     return ((p.row == 0) && (p.col == 0));
// }

int abs(int x) {
    return (x >= 0) ? x : -x;
}

// int distance2(int row1, int col1, int row2, int col2) {
//     int dr, dc;
//     int abs1, abs2;
// 
//     abs1 = abs(row1 - row2);
//     abs2 = rows - abs(row1 - row2);
// 
//     dr = (abs1 < abs2) ? abs1 : abs2;
// 
//     abs1 = abs(col1 - col2);
//     abs2 = cols - abs(col1 - col2);
// 
//     dc = (abs1 < abs2) ? abs1 : abs2;
// 
//     return dr * dr + dc * dc;
// }

int distance2(point p1, point p2) {
    int dr, dc;
    int abs1, abs2;

    abs1 = abs(p1.row - p2.row);
    abs2 = rows - abs(p1.row - p2.row);

    dr = (abs1 < abs2) ? abs1 : abs2;

    abs1 = abs(p1.col - p2.col);
    abs2 = cols - abs(p1.col - p2.col);

    dc = (abs1 < abs2) ? abs1 : abs2;

    return dr * dr + dc * dc;
}

point neighbor(point p, char dir) {
    switch (dir) {
        case NORTH:
            p.row -= 1;
            return normalize(p);
        case SOUTH:
            p.row += 1;
            return normalize(p);
        case EAST:
            p.col += 1;
            return normalize(p);
        case WEST:
            p.col -= 1;
            return normalize(p);
    }
    assert(0);
}

// int normalize_row(int row) {
//     while (row < rows) row += rows;
//     return row % rows;
// }
// 
// int normalize_col(int col) {
//     while (col < cols) col += cols;
//     return col % cols;
// }

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

    assert(normalize((point){0, -2}).col == 8);
    assert(normalize((point){0, -1}).col == 9);
    assert(normalize((point){0,  0}).col == 0);
    assert(normalize((point){0,  1}).col == 1);
    assert(normalize((point){0,  9}).col == 9);
    assert(normalize((point){0, 11}).col == 1);

    assert(distance2((point){0, 0}, (point){-1, -1}) == 2);
    assert(distance2((point){0, 0}, (point){ 0,  0}) == 0);
    assert(distance2((point){0, 0}, (point){ 0,  1}) == 1);
    assert(distance2((point){0, 0}, (point){ 0,  2}) == 4);
    assert(distance2((point){0, 0}, (point){ 2,  2}) == 8);
    assert(distance2((point){0, 0}, (point){ 9,  9}) == 2);

    point p;
    p = neighbor((point){0, 0}, NORTH);
    assert(p.row == 9 && p.col == 0);
    p = neighbor((point){0, 0}, EAST);
    assert(p.row == 0 && p.col == 1);
    p = neighbor((point){0, 0}, SOUTH);
    assert(p.row == 1 && p.col == 0);
    p = neighbor((point){0, 0}, WEST);
    assert(p.row == 0 && p.col == 9);

    puts("ok");
    return 0;
}
#endif
