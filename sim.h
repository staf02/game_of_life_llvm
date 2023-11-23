#ifndef SIM_H
#define SIM_H

#include <SDL.h>


#define SIM_X_SIZE 40
#define SIM_Y_SIZE 30
#define SIM_DISPLAY_MEM_ADDR 128*256


void simSetPixel(int x, int y, int argb, SDL_Renderer*);
void simFlush(SDL_Renderer*);
void simBkpt(SDL_Renderer*);
int simRand(SDL_Renderer*);

#endif // SIM_H
