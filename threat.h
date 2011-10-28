#ifndef __THREAT_H
#define __THREAT_H

#include "globals.h"

char threat[MAX_ROWS][MAX_COLS];
char support[MAX_ROWS][MAX_COLS];

void threat_calculate();
char *threat_to_string();

#endif
