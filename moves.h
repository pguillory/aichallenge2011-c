#ifndef __ATTACK_MOVES_H
#define __ATTACK_MOVES_H

#include "globals.h"

bytegrid moves[2];

void moves_calculate();
char *attack_moves_to_string();
char *focus_to_string();
char *max_focus_to_string();

#endif
