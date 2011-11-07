#ifndef __BOT_H
#define __BOT_H

void bot_init();
void bot_begin_turn();
void bot_see_water(point);
void bot_see_food(point);
void bot_see_ant(point, int player);
void bot_see_hill(point, int player);
void bot_see_dead_ant(point, int player);
void bot_end_turn();

#endif
