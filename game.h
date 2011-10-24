#ifndef __GAME_H
#define __GAME_H

#define MAX_ROWS 200
#define MAX_COLS 200

int turn;           /* = 0 */
int loadtime;       /* = 3000 */
int turntime;       /* = 1000 */
int rows;           /* = MAX_ROWS */
int cols;           /* = MAX_COLS */
int turns;          /* = 500 */
int viewradius2;    /* = 55 */
int attackradius2;  /* = 5 */
int spawnradius2;   /* = 1 */
int player_seed;    /* = 42 */

void bot_init();
void bot_turn();

#endif
