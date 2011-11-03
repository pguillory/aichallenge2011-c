#ifndef __GAME_H
#define __GAME_H

typedef struct
{
    int row, col;
} point;

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

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

point normalize(point);
point add_points(point, point);
int points_equal(point, point);
// int point_is_origin(point p);
int distance2(point, point);
point neighbor(point, char direction);

#include <stdio.h>
FILE *logfile;
void init_log();
void logs(char *s);

#endif
