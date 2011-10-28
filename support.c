#include <math.h>
#include "map.h"
#include "support.h"

void support_calculate() {
    int row, col;
    int row2, col2;
    int direction;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            support[row][col] = 0;
        }
    }

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if ((map[row][col] & SQUARE_ANT) && (owner[row][col] == 0)) {
                support[row][col] += 1;
                for (direction = 0; direction < 4; direction++) {
                    neighbor(row, col, direction , &row2, &col2);
                    support[row2][col2] += 1;
                }
            }
        }
    }
}

char *support_to_string() {
    static char buffer[MAX_ROWS * MAX_COLS + MAX_COLS];
    char *output = buffer;
    int row, col;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            *output++ = '0' + support[row][col];
        }
        *output++ = '\n';
    }
    *--output = '\0';
    return buffer;
}

#ifdef UNIT_TESTS
#undef UNIT_TESTS
#include "globals.c"
#include "map.c"
int main(int argc, char *argv[]) {
    char *map_string, *expected_output;

    map_string = ".....\n"
                 ".a...\n"
                 ".....\n"
                 ".....";
    expected_output = "01000\n"
                      "11100\n"
                      "01000\n"
                      "00000";
    map_load_from_string(map_string);
    support_calculate();
    // puts(support_to_string());
    assert(strcmp(support_to_string(), expected_output) == 0);

    map_string = ".....\n"
                 ".aa..\n"
                 ".....\n"
                 ".....";
    expected_output = "01100\n"
                      "12210\n"
                      "01100\n"
                      "00000";
    map_load_from_string(map_string);
    support_calculate();
    // puts(support_to_string());
    assert(strcmp(support_to_string(), expected_output) == 0);

    puts("ok");
    return 0;
}
#endif