/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include <constant.h>
#include <game.h>
#include <window.h>
#include <misc.h>
#include <bomb.h>
#include <menu.h>


int main(int argc, char *argv[]) {

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		error("Can't init SDL:  %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	TTF_Init();

	window_create(SIZE_BLOC * STATIC_MAP_WIDTH,
	SIZE_BLOC * STATIC_MAP_HEIGHT + BANNER_HEIGHT + LINE_HEIGHT);

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	menu_load();

	struct game *game;

	int done = 0;
	char game_was_played = 0;
	if(game_already_saved()){
		char *options[] = {"Continue your progress", "New game", "Quit"};
		switch(menu_display(options, 3)){
			case 0:
				game = game_load();
				game_was_played = 1;
				break;

			case 1:
				game = game_new();
				game_was_played = 1;
				break;

			default :
				done = 1;
		}
	}else{
		char *options[] = {"New game", "Quit"};
		switch(menu_display(options, 2)){
			case 0:
				game = game_new();
				game_was_played = 1;
				break;
			default :
				done = 1;
		}
	}

	// to obtain the DEFAULT_GAME_FPS, we have to reach a loop duration of (1000 / DEFAULT_GAME_FPS) ms
	int ideal_speed = 1000 / DEFAULT_GAME_FPS;
	int timer, execution_speed;

	// game loop
	// static time rate implementation

	while (!done) {
		timer = SDL_GetTicks();

		done = game_update(game);
		bomb_tick(game);
		game_display(game);

		execution_speed = SDL_GetTicks() - timer;
		if (execution_speed < ideal_speed)
			SDL_Delay(ideal_speed - execution_speed); // we are ahead of ideal time. let's wait.
	}

	menu_unload();

	if(game_was_played){
		game_save(game);
		game_free(game);
	}

	SDL_Quit();

	return EXIT_SUCCESS;
}
