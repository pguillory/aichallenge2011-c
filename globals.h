#ifndef __GLOBALS_H
#define __GLOBALS_H

typedef struct
{
    int row, col;
} point;

typedef struct
{
    point point;
    int dir;
} move;

#define grid(g, p) g[p.row][p.col]

#define NORTH 1
#define EAST  2
#define SOUTH 4
#define WEST  8
#define STAY 16

#define MAX_PLAYERS 10
#define MAX_ROWS 200
#define MAX_COLS 200

typedef unsigned char byte;
typedef byte bytegrid[MAX_ROWS][MAX_COLS];

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

long int now();
point normalize(point);
point add_points(point, point);
int points_equal(point, point);
int point_is_origin(point p);
void foreach_point(void (*f)(point));
void foreach_point_within_manhattan_distance(point, int distance, void (*)(point));
void foreach_point_within_radius2(point, int radius2, void (*)(point));
void foreach_neighbor(point p, void (*)(point, int, point));
char *point_callback_to_string(char (*)(point));
int distance2(point, point);
point neighbor(point, char direction);
int left(int dir);
int right(int dir);
int backward(int dir);
char dir2char(int dir);

#include <stdio.h>
#ifdef DEBUG
FILE *logfile;
#endif
void init_log();
void logs(char *s);

#endif
