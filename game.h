#pragma once

#include <stddef.h>

void new_game(const int* pos, const size_t pos_size, const int w, const int h);

void set(int x, int y, char value);

void clear();

int next_iteration();

void stop();