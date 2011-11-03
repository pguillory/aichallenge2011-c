#ifndef __AROMA_H
#define __AROMA_H

#include "globals.h"

float aroma[MAX_ROWS][MAX_COLS];
float army_aroma[MAX_ROWS][MAX_COLS];

void aroma_reset();
void aroma_iterate();
char *aroma_to_string();

#endif
