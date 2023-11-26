#pragma once

#include <stddef.h>
#include <SDL.h>

typedef struct game {
    char map[100][100];
    int height;
    int width;

    SDL_Renderer* ctx;

} game;

int new_game(game*, SDL_Renderer*, int*, size_t, int h, int w);

void set(game* gm, int x, int y, char value);

void clear(game* gm);

int next_iteration(game*);