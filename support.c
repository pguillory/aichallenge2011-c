#include <math.h>
#include "map.h"
#include "support.h"

void support_calculate() {
    int row, col;
    int dr, dc;
    int row2, col2;
    // int direction;

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            grid(support, p) = 0;
        }
    }

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if (map_has_friendly_ant(p)) {
                for (dr = -1; dr <= +1; dr++) {
                    row2 = normalize_row(row + dr);
                    for (dc = -1; dc <= +1; dc++) {
                        col2 = normalize_col(col + dc);
                        grid(support, p2) += 1;
                    }
                }
                // for (direction = 1; direction < STAY; direction *= 2) {
                //     neighbor(row, col, direction , &row2, &col2);
                //     grid(support, p2) += 1;
                // }
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
            *output++ = '0' + grid(support, p);
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
    expected_output = "11100\n"
                      "11100\n"
                      "11100\n"
                      "00000";
    map_load_from_string(map_string);
    support_calculate();
    // puts(support_to_string());
    assert(strcmp(support_to_string(), expected_output) == 0);

    map_string = ".....\n"
                 ".aa..\n"
                 ".....\n"
                 ".....";
    expected_output = "12210\n"
                      "12210\n"
                      "12210\n"
                      "00000";
    map_load_from_string(map_string);
    support_calculate();
    // puts(support_to_string());
    assert(strcmp(support_to_string(), expected_output) == 0);

    puts("ok");
    return 0;
}
#endif
