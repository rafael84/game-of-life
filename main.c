#include <assert.h>
#include <raylib.h>
#include <stdio.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960

#define ROWS 32
#define COLS 32

#define CELL_WIDTH (SCREEN_WIDTH / COLS)
#define CELL_HEIGHT (SCREEN_HEIGHT / ROWS)

#define DEAD 0
#define ALIVE 1

#define MOD(x, n) ((x % n + n) % n)

#define CELL(board, row, col) (board[row * COLS + col])
#define SAFE_CELL(board, row, col) (CELL(board, MOD(row, ROWS), MOD(col, COLS)))

int count_neighbors(int board[], int row, int col) {
    int count = 0;
    count += SAFE_CELL(board, row - 1, col - 1);
    count += SAFE_CELL(board, row - 1, col + 0);
    count += SAFE_CELL(board, row - 1, col + 1);
    count += SAFE_CELL(board, row + 0, col + 1);
    count += SAFE_CELL(board, row + 0, col - 1);
    count += SAFE_CELL(board, row + 1, col - 1);
    count += SAFE_CELL(board, row + 1, col + 0);
    count += SAFE_CELL(board, row + 1, col + 1);
    return count;
}

static const int glider[][2] = {{0, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}};
static const int blinker[][2] = {{0, 0}, {0, 1}, {0, 2}};
static const int r_pentomino[][2] = {{0, 1}, {0, 2}, {1, 0}, {1, 1}, {2, 1}};
static const int toad[][2] = {{0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1}, {1, 2}};

void place_pattern(int board[], int base_row, int base_col, const int pattern[][2], int count, int* population) {
    for (int i = 0; i < count; ++i) {
        int r = base_row + pattern[i][0];
        int c = base_col + pattern[i][1];
        if (CELL(board, r, c) == DEAD) {
            CELL(board, r, c) = ALIVE;
            (*population)++;
        }
    }
}

int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game of Life");

    int generation = 0;
    int population = 0;

    char text_buffer[100];

    int board_a[ROWS * COLS] = {DEAD};
    int board_b[ROWS * COLS] = {DEAD};

    int* current_board = board_a;
    int* next_board = board_b;

    place_pattern(current_board, 11, 2, glider, 5, &population);
    place_pattern(current_board, 4, 20, blinker, 3, &population);
    place_pattern(current_board, 22, 18, r_pentomino, 5, &population);
    place_pattern(current_board, 11, 9, toad, 6, &population);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_Q)) { break; }

        if (IsKeyDown(KEY_SPACE)) {
            generation++;
            for (int row = 0; row < ROWS; ++row) {
                for (int col = 0; col < COLS; ++col) {
                    int state = CELL(current_board, row, col);
                    CELL(next_board, row, col) = state;
                    int neighbors = count_neighbors(current_board, row, col);
                    switch (state) {
                        case DEAD: {
                            if (neighbors == 3) {
                                CELL(next_board, row, col) = ALIVE;
                                population += 1;
                            }
                        } break;
                        case ALIVE: {
                            if (neighbors < 2) {
                                CELL(next_board, row, col) = DEAD;
                                population -= 1;
                            } else if (neighbors == 2 || neighbors == 3) {
                                CELL(next_board, row, col) = ALIVE;
                            } else {
                                CELL(next_board, row, col) = DEAD;
                                population -= 1;
                            }
                        } break;
                        default: {
                            assert(0 && "unreachable");
                        }
                    }
                }
            }
            //
            // swap current & next board
            //
            int* tmp = current_board;
            current_board = next_board;
            next_board = tmp;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS; ++col) {
                int state = CELL(current_board, row, col);
                int x = col * CELL_WIDTH + 1;
                int y = row * CELL_HEIGHT + 1;
                int w = CELL_WIDTH - 1;
                int h = CELL_HEIGHT - 1;

                DrawLine(x, y, x, SCREEN_HEIGHT, GRAY);
                DrawLine(x, y, SCREEN_WIDTH, y, GRAY);

                if (state == ALIVE) {
                    DrawRectangle(x, y, w, h, WHITE);
                } else {
                    DrawRectangle(x, y, w, h, BLACK);
                }
            }
        }

        snprintf(text_buffer, 100, "Generation: %d", generation);
        DrawText(text_buffer, 20, 20, 24, YELLOW);

        snprintf(text_buffer, 100, "Population: %d", population);
        DrawText(text_buffer, 20, 40, 24, YELLOW);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
