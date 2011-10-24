#include <assert.h>
#include <string.h>
#include "map.h"

char map[MAX_ROWS][MAX_COLS];
char owner[MAX_ROWS][MAX_COLS];

int normalize_row(int row) {
  while (row < rows) row += rows;
  return row % rows;
}

int normalize_col(int col) {
  while (col < cols) col += cols;
  return col % cols;
}

void map_blank() {
  int row;
  int col;
  for (row = 0; row < rows; row++) {
    for (col = 0; col < cols; col++) {
      map[row][col] = 0;
      owner[row][col] = 0;
    }
  }
}

void map_new_turn() {
  int row;
  int col;
  for (row = 0; row < rows; row++) {
    for (col = 0; col < cols; col++) {
      map[row][col] &= SQUARE_SEEN_MASK | SQUARE_WATER_MASK | SQUARE_HILL_MASK;
    }
  }
}

void map_discover_land(int row, int col) {
    map[row][col] |= SQUARE_SEEN_MASK;
}

void map_discover_water(int row, int col) {
    map[row][col] |= SQUARE_SEEN_MASK | SQUARE_WATER_MASK;
}

void map_discover_food(int row, int col) {
    map[row][col] |= SQUARE_SEEN_MASK | SQUARE_FOOD_MASK;
}

void map_discover_ant(int row, int col, int player) {
    map[row][col] |= SQUARE_SEEN_MASK | SQUARE_ANT_MASK;
    owner[row][col] = player;
}

void map_discover_hill(int row, int col, int player) {
    map[row][col] |= SQUARE_SEEN_MASK | SQUARE_HILL_MASK;
    owner[row][col] = player;
}

//    ?   = Unknown
//    .   = Land
//    %   = Water
//    *   = Food
// [a..z] = Ant
// [0..9] = Hill
// [A..Z] = Ant on a hill

void read_map_from_string(char *input) {
    int i;
    int row, col;
    char c;

    map_blank();

    for (i = 0, row = 0, col = 0; row < rows; i++) {
        c = input[i];
        if (c == '.') {
            map_discover_land(row, col);
        }
        else if (c == '%') {
            map_discover_water(row, col);
        }
        else if (c == '*') {
            map_discover_food(row, col);
        }
        else if (c >= 'a' && c <= 'z') {
            map_discover_ant(row, col, c - 'a');
        }
        else if (c >= '0' && c <= '9') {;
            map_discover_hill(row, col, c - '0');
        }
        else if (c >= 'A' && c <= 'Z') {
            map_discover_ant(row, col, c - 'A');
            map_discover_hill(row, col, c - 'A');
        }

        if (c == '\n') {
            row += 1;
            col = 0;
        } else if (c == '\0') {
            row = rows;
        } else {
            assert(col < cols);
            col += 1;
        }
    }
}

void write_map_to_string(char *output) {
    int row, col;
    char square;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            square = map[row][col];
            if ((square & SQUARE_SEEN_MASK) == 0) {
                *output++ = '?';
            } else if (square & SQUARE_WATER_MASK) {
                *output++ = '%';
            } else if (square & SQUARE_FOOD_MASK) {
                *output++ = '*';
            } else if ((square & SQUARE_ANT_MASK) && (square & SQUARE_HILL_MASK)) {
                *output++ = 'A' + owner[row][col];
            } else if (square & SQUARE_ANT_MASK) {
                *output++ = 'a' + owner[row][col];
            } else if (square & SQUARE_HILL_MASK) {
                *output++ = '0' + owner[row][col];
            } else {
                *output++ = '.';
            }
        }
        *output++ = '\n';
    }
    *--output = '\0';
}

#ifdef UNIT_TESTS
int main(int argc, char *argv[]) {
    rows = 10;
    cols = 10;

    assert(normalize_row(0) == 0);
    assert(normalize_row(rows) == 0);
    assert(normalize_row(-1) == rows - 1);
    
    assert(normalize_col(0) == 0);
    assert(normalize_col(cols) == 0);
    assert(normalize_col(-1) == cols - 1);

    char input[MAX_ROWS * MAX_COLS + MAX_COLS];
    char output[MAX_ROWS * MAX_COLS + MAX_COLS];
    
    rows = 2;
    cols = 4;
    strcpy(input, "?.%*\naB2?");
    read_map_from_string(input);
    write_map_to_string(output);
    assert(strcmp(input, output) == 0);

    puts("ok");
    return 0;
}
#endif
