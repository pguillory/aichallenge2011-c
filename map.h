#ifndef __MAP_H
#define __MAP_H

#include "globals.h"

#define SQUARE_VISIBLE     1
#define SQUARE_LAND        2
#define SQUARE_WATER       4
#define SQUARE_FOOD        8
#define SQUARE_ANT        16
#define SQUARE_HILL       32

unsigned char map[MAX_ROWS][MAX_COLS];
unsigned char update[MAX_ROWS][MAX_COLS];
unsigned char owner[MAX_ROWS][MAX_COLS];
int initial_friendly_ant_count, friendly_ant_count, visible_enemy_ant_count, food_consumed;
int friendly_dead_ant_count, enemy_dead_ant_count;
unsigned char visible_ally_count[MAX_ROWS][MAX_COLS];

int map_is_visible(point);
int map_has_land(point);
int map_has_water(point);
int map_has_food(point);
int map_has_ant(point);
int map_has_hill(point);
int map_is_friendly(point);
int map_is_enemy(point);
int map_has_friendly_ant(point);
int map_has_enemy_ant(point);
int map_has_friendly_hill(point);
int map_has_enemy_hill(point);
void map_reset();
void map_begin_update();
void map_see_water(point);
void map_see_food(point);
void map_see_ant(point, int player);
void map_see_hill(point, int player);
void map_finish_update();
void map_load_from_string(char *);
char *map_to_string();
char *map_to_color_string();
// char *army_to_string();

#endif
