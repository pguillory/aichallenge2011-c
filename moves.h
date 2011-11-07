#ifndef __DIRECTIONS_H
#define __DIRECTIONS_H

#include "globals.h"

char moves[MAX_ROWS][MAX_COLS];

void moves_calculate();
char *moves_to_string();

#endif
