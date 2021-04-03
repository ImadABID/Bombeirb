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
#include <menu.h>
#include <sprite.h>
#include <bomb.h>
#include <str_formating.h>

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
		game->maps[i] = map_get(map_prefix, i, nbr_levels);
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

	char *pause_options[] = {"Resume", "Quit"};


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
			case SDLK_p:
				switch(menu_display(pause_options, 2)){
					case 0:
						break;

					default :
						return 1;
				}
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

void game_save(struct game *game){
	FILE *f = fopen(SAVE_FILE, "wb");
	fwrite(&(game->level), sizeof(game->level), 1, f);
	fwrite(&(game->levels), sizeof(game->levels), 1, f);
	player_save(game->player, f);
	struct map *map;
	for(int i=0; i<game->levels; i++){
		map = map_get_by_index(game->maps, i);
		map_save(map, f);
	}
	fclose(f);
}

struct game *game_load(){
	FILE *f = fopen(SAVE_FILE, "rb");
	if(f==NULL)
		return game_new();

	sprite_load();

	struct game *game = malloc(sizeof(struct game));

	fread(&game->level, sizeof(game->level), 1, f);
	fread(&(game->levels), sizeof(game->levels), 1, f);

	game->player = player_load(f);


	game->maps = malloc(game->levels * sizeof(struct map*));
	for(int i=0; i<game->levels; i++){
		game->maps[i] = map_load_progress(f);
	}

	fclose(f);

	return game;
}

char game_already_saved(){
	FILE *f = fopen(SAVE_FILE,"rb");
	return f!=NULL;
}