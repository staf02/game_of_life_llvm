#include "sim.h"

#include <stdlib.h>

void simSetPixel(int x, int y, int argb, SDL_Renderer* renderer) {
    SDL_Rect fillRect = { x * 20, y * 20, 20, 20 };
    if (argb == 1) {
        const uint32_t red = rand() % (0xFA);
        const uint32_t blue = rand() % (0xFA);
        const uint32_t green = rand() % (0xFA);
        SDL_SetRenderDrawColor(renderer, red, green, blue, 0xFF);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    }
    SDL_RenderFillRect(renderer, &fillRect);
    SDL_SetRenderDrawColor(renderer, 0x1F, 0x1F, 0x1F, 0xFF);
    SDL_RenderDrawRect(renderer, &fillRect);
}

void simFlush(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
}
