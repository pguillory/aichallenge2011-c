#ifndef __MAP_H
#define __MAP_H

void map_blank();
void map_new_turn();
void map_end_turn();
void map_discover_land(int row, int col);
void map_discover_water(int row, int col);
void map_discover_food(int row, int col);
void map_discover_ant(int row, int col, int player);
void map_discover_hill(int row, int col, int player);
void map_discover_ant_on_hill(int row, int col, int player);
void map_load_from_string(char *input);
char *map_to_string();

#endif
