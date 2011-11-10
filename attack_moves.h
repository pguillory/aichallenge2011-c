#ifndef __ATTACK_MOVES_H
#define __ATTACK_MOVES_H

#include "globals.h"

char attack_moves[MAX_ROWS][MAX_COLS];

void attack_moves_calculate();
char *attack_moves_to_string();

#endif
