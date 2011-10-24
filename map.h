#ifndef __MAP_H
#define __MAP_H

#include "game.h"

#define SQUARE_SEEN_MASK     1
#define SQUARE_VISIBLE_MASK  2
#define SQUARE_WATER_MASK    4
#define SQUARE_FOOD_MASK     8
#define SQUARE_ANT_MASK      16
#define SQUARE_HILL_MASK     32

void map_blank();
void map_new_turn();
void map_discover_land(int row, int col);
void map_discover_water(int row, int col);
void map_discover_food(int row, int col);
void map_discover_ant(int row, int col, int player);
void map_discover_hill(int row, int col, int player);
void map_discover_ant_on_hill(int row, int col, int player);

#endif
