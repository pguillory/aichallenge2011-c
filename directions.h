#ifndef __DIRECTIONS_H
#define __DIRECTIONS_H

#include "globals.h"

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3

char directions[MAX_ROWS][MAX_COLS];

void directions_calculate();
char *directions_to_string();

#endif
