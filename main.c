#include <raylib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define ROWS (2*90)
#define COLS (2*160)

#define ALIVE_COLOR (GetColor(0x101010ff))
#define DEAD_COLOR GRAY

typedef enum {
    EIGHT_SUM = 8,
    NINE_SUM = 9,
} CountType;

typedef struct {
    unsigned map[2];
    CountType type;
} Rule;

#define wrap(x, MAX) ((((x)%(MAX))+(MAX))%(MAX))

Rule life_rule = {
    .map = {
        [0] = 0b0000001000,
        [1] = 0b0000001100,
    },
    .type = EIGHT_SUM,
};

Rule minor_rule = {
    .map = {
        [0] = 0b1101110000,
        [1] = 0b1101110000,
    },
    .type = EIGHT_SUM,
};

Rule major_rule = {
    .map = {
        [0] = 0b1111100000,
        [1] = 0b1111100000,
    },
    .type = NINE_SUM,
};

Rule vote_rule = {
    .map = {
        [0] = 0b1111010000,
        [1] = 0b1111010000,
    },
    .type = NINE_SUM,
};

unsigned count_neighbors(bool* grid, const size_t rows, const size_t cols, const int r, const int c, CountType type) {
    bool (*grid2d)[cols] = (bool(*)[cols])grid;
    unsigned count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (type == EIGHT_SUM && i == 0 && j == 0) continue;
            count += grid2d[wrap(r+i, rows)][wrap(c+j, cols)];
        }
    }
    return count;
}

void update(bool* grid, const size_t rows, const size_t cols, Rule rule) {
    bool temp_grid[rows*cols] = {};
    bool (*temp_grid2d)[cols] = (bool(*)[cols])temp_grid;
    for (size_t r = 0; r < ROWS; r++) {
        for (size_t c = 0; c < COLS; c++) {
            unsigned neighbors = count_neighbors(grid, rows, cols, r, c, rule.type);
            temp_grid2d[r][c] = (rule.map[grid[r*cols + c]] >> neighbors)&1;
        }
    }
    memcpy(grid, temp_grid, rows*cols*sizeof(*grid));
}

void display(bool* grid, const size_t rows, const size_t cols) {
    const float cell_size = fmin(GetScreenWidth()*1.0f/COLS, GetScreenHeight()*1.0f/ROWS);
    bool (*grid2d)[cols] = (bool(*)[cols])grid;
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++) {
            Vector2 start = {
                .x = GetScreenWidth()/2.0f-cell_size*COLS/2 + c*cell_size,
                .y = GetScreenHeight()/2.0f-cell_size*ROWS/2 + r*cell_size,
            };
            if (grid2d[r][c]) {
                DrawRectangleV(start, CLITERAL(Vector2){cell_size, cell_size}, ALIVE_COLOR);
            } else {
                DrawRectangleV(start, CLITERAL(Vector2){cell_size, cell_size}, DEAD_COLOR);
            }
        }
    }
}

void randomize(bool* grid, size_t n) {
    for (size_t i = 0; i < n; i++) {
        grid[i] = rand()%2;
    }
}

int main() {
    const int factor = 100;
    InitWindow(16*factor, 9*factor, "Cellular Automata");
    SetTargetFPS(30);
    bool grid[ROWS * COLS] = {};
    randomize(grid, ROWS*COLS);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        // update(grid, ROWS, COLS, major_rule);
        // update(grid, ROWS, COLS, life_rule);
        update(grid, ROWS, COLS, minor_rule);
        if (IsKeyPressed(KEY_SPACE)){ 
            randomize(grid, ROWS*COLS);
        }
        display(grid, ROWS, COLS);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
