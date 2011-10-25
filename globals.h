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
// float viewradius;
int attackradius2;  /* = 5 */
int spawnradius2;   /* = 1 */
int player_seed;    /* = 42 */

int distance2(int row1, int col1, int row2, int col2);
int normalize_row(int row);
int normalize_col(int col);

#include <stdio.h>
FILE *logfile;
void init_log();
void logs(char *s);

#endif
