#ifndef __MYSTERY_H
#define __MYSTERY_H

#include "globals.h"

#define MYSTERY_MAX 255
#define MYSTERY_INITIAL 50

unsigned char mystery[MAX_ROWS][MAX_COLS];

void mystery_reset();
void mystery_init();
void mystery_iterate();

#endif
