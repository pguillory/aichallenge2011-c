#ifndef __MAP_H
#define __MAP_H

#include "globals.h"

#define SQUARE_VISIBLE     1
#define SQUARE_LAND        2
#define SQUARE_WATER       4
#define SQUARE_FOOD        8
#define SQUARE_ANT         16
#define SQUARE_HILL        32

unsigned char map[MAX_ROWS][MAX_COLS];
unsigned char update[MAX_ROWS][MAX_COLS];
unsigned char owner[MAX_ROWS][MAX_COLS];

void map_reset();
void map_begin_update();
void map_finish_update();
void map_load_from_string(char *input);
char *map_to_string();

#endif
