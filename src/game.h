#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct _game Game;

Game *game_create(uint32_t width, uint32_t height);

void game_process_input(Game *game);
void game_update(Game *game);
void game_draw(Game *game);
