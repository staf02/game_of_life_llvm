#include "game.h"
#include "sim.h"

#include <stdio.h>

int new_game(game* g, SDL_Renderer* context, const int* pos, const size_t pos_size, const int w, const int h) {
    g->ctx = context;
    g->width = w;
    g->height = h;

    for (size_t x = 0; x < w; x++) {
        for (size_t y = 0; y < h; y++) {
            g->map[x][y] = 0;
        }
    }

    for (size_t i = 0; i < pos_size; i += 2) {
        g->map[pos[i]][pos[i + 1]] = 1;
    }
    return 1;
}

void set(game* gm, int x, int y, char value) {
    if (x >= 0 && y >= 0 && x < gm->width && y < gm->height) {
        gm->map[x][y] = value;
        simSetPixel(x, y, gm->map[x][y], gm->ctx);
    }
}

void clear(game* gm) {
    for (size_t x = 0; x < gm->width; x++) {
        for (size_t y = 0; y < gm->height; y++) {
            gm->map[x][y] = 0;
            simSetPixel(x, y, gm->map[x][y], gm->ctx);
        }
    }
}

int next_iteration(game* g) {
    char new_map[100][100];
    int result = 0;
    for (size_t x = 0; x < g->width; x++) {
        for (size_t y = 0; y < g->height; y++) {
            int alive = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    const size_t nx = (SIM_X_SIZE + x + dx) % SIM_X_SIZE, ny = (y + SIM_Y_SIZE + dy) % SIM_Y_SIZE;
                    if (nx != x || ny != y) {
                        if (g->map[nx][ny] == 1) {
                            alive++;
                        }
                    }
                }
            }
            if (alive == 3 && g->map[x][y] == 0) {
                new_map[x][y] = 1;
                result++;
            }
            else if (g->map[x][y] == 1 && (alive == 2 || alive == 3)) {
                new_map[x][y] = 1;
                result++;
            }
            else {
                new_map[x][y] = 0;
            }
            simSetPixel(x, y, new_map[x][y], g->ctx);
        }
    }

    for (size_t x = 0; x < g->width; x++) {
        for (size_t y = 0; y < g->height; y++) {
            g->map[x][y] = new_map[x][y];
        }
    }
    return result;
}