#ifndef __SUPPORT_H
#define __SUPPORT_H

#include "globals.h"

char support[MAX_ROWS][MAX_COLS];

void support_calculate();
char *support_to_string();

#endif
