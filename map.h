#ifndef __MAP_H
#define __MAP_H

#include "globals.h"

#define SQUARE_SEEN_MASK     1
#define SQUARE_VISIBLE_MASK  2
#define SQUARE_WATER_MASK    4
#define SQUARE_FOOD_MASK     8
#define SQUARE_ANT_MASK      16
#define SQUARE_HILL_MASK     32

char map[MAX_ROWS][MAX_COLS];
char owner[MAX_ROWS][MAX_COLS];

void map_reset();
void map_start_turn();
void map_end_turn();
void map_load_from_string(char *input);
char *map_to_string();

#endif
