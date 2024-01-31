#include "game.h"
#include "sim.h"

char map[100][100];
int height;
int width;

static int stop_flag = 0;

void new_game(const int* pos, const size_t pos_size, const int w, const int h) {
    width = w;
    height = h;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            map[x][y] = 0;
        }
    }

    for (size_t i = 0; i < pos_size; i += 2) {
        map[pos[i]][pos[i + 1]] = 1;
    }
}

void set(int x, int y, char value) {
    if (x >= 0 && y >= 0 && x < width && y < height) {
        map[x][y] = value;
        simSetPixel(x, y, map[x][y]);
    }
}

void clear() {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            map[x][y] = 0;
            simSetPixel(x, y, map[x][y]);
        }
    }
}

int next_iteration() {
    char new_map[100][100];
    int result = 0;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int alive = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    const int nx = (SIM_X_SIZE + x + dx) % SIM_X_SIZE, ny = (y + SIM_Y_SIZE + dy) % SIM_Y_SIZE;
                    if (nx != x || ny != y) {
                        if (map[nx][ny] == 1) {
                            alive++;
                        }
                    }
                }
            }
            if (alive == 3 && map[x][y] == 0) {
                new_map[x][y] = 1;
                result++;
            } else if (map[x][y] == 1 && (alive == 2 || alive == 3)) {
                new_map[x][y] = 1;
                result++;
            } else {
                new_map[x][y] = 0;
            }
            simSetPixel(x, y, new_map[x][y]);
        }
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            map[x][y] = new_map[x][y];
        }
    }
    return result;
}

void stop() {
    stop_flag = 1;
}

int app(void* data) {
    while (!stop_flag) {
        next_iteration();
        simFlush();
    }
    return 0;
}