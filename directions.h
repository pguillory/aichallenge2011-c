#ifndef __DIRECTIONS_H
#define __DIRECTIONS_H

#include "globals.h"

char directions[MAX_ROWS][MAX_COLS];

void directions_calculate();
char *directions_to_string();

#endif
