#include "game.h"
#include "sim.h"

#include <SDL.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

bool paused = false;
int pos[SIM_X_SIZE * SIM_Y_SIZE * 2];
int size = 0;

void simSetPixel(int x, int y, int argb) {
    const SDL_Rect fillRect = { x * 20, y * 20, 20, 20 };
    if (argb == 1) {
        const uint8_t red = rand() % (0xFA);
        const uint8_t blue = rand() % (0xFA);
        const uint8_t green = rand() % (0xFA);
        SDL_SetRenderDrawColor(renderer, red, green, blue, 0xFF);
    } else {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    }
    SDL_RenderFillRect(renderer, &fillRect);
    SDL_SetRenderDrawColor(renderer, 0x1F, 0x1F, 0x1F, 0xFF);
    SDL_RenderDrawRect(renderer, &fillRect);
}

void simFlush() {
    SDL_RenderPresent(renderer);
    while (paused) {
        SDL_Delay(50);
    }
    SDL_Delay(50);
}

void close(SDL_Thread* worker) {
    if (worker != NULL) {
        SDL_WaitThread(worker, NULL);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}

extern int app(void* data);

static int read_position(const char* file, int* position) {
    FILE* f = fopen(file, "r");
    if (!f) {
        return 0;
    }
    int size = 0;
    int x, y;
    while (fscanf(f, "%d %d\n", &x, &y) != EOF) {
        position[size++] = x;
        position[size++] = y;
    }
    fclose(f);
    return size;
}

static void simInit(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    window = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    if (argc > 1) {
        size = read_position(argv[1], pos);
    }

    new_game(pos, size, SIM_X_SIZE, SIM_Y_SIZE);

    srand(time(NULL));
}

static void simProcess() {
    SDL_Event e;

    bool mouse_flag = 0;

    while (1) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                paused = !paused;
                stop();
                return;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_SPACE) {
                    paused = !paused;
                    SDL_RenderPresent(renderer);
                }
            } else if ((e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEMOTION && mouse_flag) && paused) {
                mouse_flag = true;
                int x, y;
                const uint32_t res = SDL_GetMouseState(&x, &y);
                x /= 20;
                y /= 20;
                if (res == 2) {
                    clear();
                } else {
                    set(x, y, (char)res & 1);
                }
                SDL_RenderPresent(renderer);
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                mouse_flag = false;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    simInit(argc, argv);

    SDL_Thread* worker = SDL_CreateThread(app, "LazyThread", NULL);

    simProcess();
    close(worker);
    return 0;
}