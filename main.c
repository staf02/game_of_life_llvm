#define _CRT_SECURE_NO_WARNINGS

#include "game.h"
#include "sim.h"

#include <SDL.h>
#include <stdio.h>
#include <Windows.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

typedef struct render_data {
    SDL_Renderer* renderer;
    game* gm;
    int* stop_flag;
    int* paused;

} render_data;

void close(SDL_Window* window, game* gm, SDL_Thread* worker, int* stop_flag) {
    if (worker != NULL) {
        *stop_flag = 1;
        SDL_WaitThread(worker, NULL);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}

int render(void* data) {
    render_data* r_data = (render_data*)(data);
    SDL_Renderer* renderer = r_data->renderer;
    game* gm = r_data->gm;
    int* stop_flag = r_data->stop_flag;
    int* paused = r_data->paused;
    while (!(*stop_flag)) {
        if (!(*paused)) {
            simFlush(renderer);
            next_iteration(gm);
            SDL_RenderPresent(renderer);
        }
        Sleep(500);
    }
    return 0;
}

int read_positon(char* file, int* position) {
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

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    else {

        window = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return 1;
        }
        else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

            if (renderer == NULL) {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                close(window, NULL, NULL, NULL);
                return 1;
            }

            int stop_flag = 0;
            int paused = 0;

            int height = 30, width = 40;
            int pos[SIM_X_SIZE * SIM_Y_SIZE * 2];
            int size = 0;

            if (argc > 1) {
                size = read_positon(argv[1], pos);
            }

            game gm;
            if (!new_game(&gm, renderer, pos, size, SIM_X_SIZE, SIM_Y_SIZE)) {
                close(window, NULL, NULL, NULL);
                return 1;
            }

            simFlush(renderer);
            SDL_RenderPresent(renderer);

            render_data r_data = {renderer, &gm, &stop_flag, &paused};
            SDL_Thread* worker = SDL_CreateThread(render, "LazyThread", &r_data);

            SDL_Event e;

            while (1) {
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) {
                        close(window, &gm, worker, &stop_flag);
                        return 0;
                    }
                    else if (e.type == SDL_KEYDOWN) {
                        if (e.key.keysym.sym == SDLK_SPACE) {
                            paused = !paused;
                        }
                    }
                    else if (e.type == SDL_MOUSEBUTTONDOWN && paused) {
                        int x, y;
                        SDL_GetMouseState(&x, &y);
                        x /= 20;
                        y /= 20;
                        invert(&gm, x, y);
                        SDL_RenderPresent(renderer);
                    }
                }
            }
        }
    }
    return 0;
}
