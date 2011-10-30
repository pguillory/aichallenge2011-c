#ifndef __THREAT_H
#define __THREAT_H

#include "globals.h"

unsigned char threat[MAX_ROWS][MAX_COLS];
unsigned char conflict[MAX_ROWS][MAX_COLS];
unsigned char killzone[MAX_ROWS][MAX_COLS];

void threat_calculate();
char *threat_to_string();

#endif
