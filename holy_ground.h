#ifndef __HOLY_GROUND_H
#define __HOLY_GROUND_H

#include "globals.h"

#define HOLY_GROUND_RANGE 12

unsigned char holy_ground[MAX_ROWS][MAX_COLS];

void holy_ground_calculate();
char *holy_ground_to_string();

#endif
