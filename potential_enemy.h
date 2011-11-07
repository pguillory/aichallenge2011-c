#ifndef __POTENTIAL_ENEMY_H
#define __POTENTIAL_ENEMY_H

#include "globals.h"

unsigned char potential_enemy[MAX_ROWS][MAX_COLS];
int potential_enemy_ant_count;

void potential_enemy_reset();
void potential_enemy_iterate();
char *potential_enemy_to_string();

#endif
