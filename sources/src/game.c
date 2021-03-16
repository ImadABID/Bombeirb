/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include <game.h>
#include <misc.h>
#include <window.h>
#include <sprite.h>
#include <bomb.h>

#include "str_formating.h"

struct game {
	struct map** maps;       // the game's map
	short levels;        // nb maps of the game
	short level;
	struct player* player;
};

struct game* game_new(void) {
	sprite_load(); // load sprites into process memory

	struct game* game = malloc(sizeof(*game));

	int nbr_levels, level, x, y;
	char *second_line = malloc(10 * sizeof(char));
	char *map_prefix = malloc(10 * sizeof(char));
	FILE *fpartie= fopen("data/partie","r");
		fscanf(fpartie,"%d",&nbr_levels);
		fscanf(fpartie,"%s",second_line);
		fscanf(fpartie,"%s",map_prefix);
	fclose(fpartie);

	int *data_int = str_format_to_int(3, second_line);
	level = data_int[0];
	x = data_int[1];
	y = data_int[2];
	free(data_int);
	free(second_line);

	game->maps = malloc(nbr_levels*sizeof(struct map*));
	for(int i=0; i<nbr_levels; i++){
		game->maps[i] = map_get(map_prefix, i);
	}
	free(map_prefix);

	game->levels = nbr_levels;				//game->levels = 1;
	game->level = level;					//game->level = 0;

	game->player = player_init(9);
	// Set default location of the player
	player_set_position(game->player, x, y);

	return game;
}

void game_free(struct game* game) {
	assert(game);

	player_free(game->player);
	for (int i = 0; i < game->levels; i++)
		map_free(game->maps[i]);
}

struct map* game_get_current_map(struct game* game) {
	assert(game);
	return game->maps[game->level];
}


struct player* game_get_player(struct game* game) {
	assert(game);
	return game->player;
}

void game_banner_display(struct game* game) {
	assert(game);

	struct map* map = game_get_current_map(game);

	int y = (map_get_height(map)) * SIZE_BLOC;
	for (int i = 0; i < map_get_width(map); i++)
		window_display_image(sprite_get_banner_line(), i * SIZE_BLOC, y);

	int white_bloc = ((map_get_width(map) * SIZE_BLOC) - 6 * SIZE_BLOC) / 4;
	int x = white_bloc;
	y = (map_get_height(map) * SIZE_BLOC) + LINE_HEIGHT;
	window_display_image(sprite_get_banner_life(), x, y);

	x = white_bloc + SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_life(game_get_player(game))), x, y);

	x = 2 * white_bloc + 2 * SIZE_BLOC;
	window_display_image(sprite_get_banner_bomb(), x, y);

	x = 2 * white_bloc + 3 * SIZE_BLOC;
	window_display_image(
			sprite_get_number(player_get_nb_bomb(game_get_player(game))), x, y);

	x = 3 * white_bloc + 4 * SIZE_BLOC;
	window_display_image(sprite_get_banner_range(), x, y);

	x = 3 * white_bloc + 5 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_range(game_get_player(game))), x, y);
}

void game_display(struct game* game) {
	assert(game);

	window_clear();
	game_banner_display(game);
	map_display(game_get_current_map(game));
	player_display(game->player);

	window_refresh();
}

static short input_keyboard(struct game* game) {
	SDL_Event event;
	struct player* player = game_get_player(game);
	struct map* map = game_get_current_map(game);


	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return 1;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return 1;
			case SDLK_UP:
				player_set_current_way(player, NORTH);
				switch(player_move(player, map)){
					case 10:
						game->level++;
						break;

					case 9:
						game->level--;
						break;
				}
				break;
			case SDLK_DOWN:
				player_set_current_way(player, SOUTH);
				switch (player_move(player, map)){
					case 10:
						game->level++;
						break;

					case 9:
						game->level--;
						break;
				}
				break;
			case SDLK_RIGHT:
				player_set_current_way(player, EAST);
				switch (player_move(player, map)){
				case 10:
					game->level++;
					break;

				case 9:
					game->level--;
					break;
				}
				break;
			case SDLK_LEFT:
				player_set_current_way(player, WEST);
				switch (player_move(player, map)){
				case 10:
					game->level++;
					break;

				case 9:
					game->level--;
					break;
				}
				break;
			case SDLK_SPACE:
				bomb_place(player, map);
				break;
			default:
				break;
			}

			break;
		}
	}
	return 0;
}

int game_update(struct game* game) {
	if (input_keyboard(game))
		return 1; // exit game

	return 0;
}
