/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>

#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>

struct player {
	int x, y;
	enum direction direction;
	int bombs;
	int life;
	int range;
};

struct player* player_init(int bombs) {
	struct player* player = malloc(sizeof(*player));
	if (!player)
		error("Memory error");

	player->direction = NORTH;
	player->bombs = bombs;
	player->life = 10;
	player->range = 1;

	return player;
}


void player_set_position(struct player *player, int x, int y) {
	assert(player);
	player->x = x;
	player->y = y;
}


void player_free(struct player* player) {
	assert(player);
	free(player);
}

int player_get_x(struct player* player) {
	assert(player != NULL);
	return player->x;
}

int player_get_y(struct player* player) {
	assert(player != NULL);
	return player->y;
}

void player_set_current_way(struct player* player, enum direction way) {
	assert(player);
	player->direction = way;
}

int player_get_nb_bomb(struct player* player) {
	assert(player);
	return player->bombs;
}

void player_inc_nb_bomb(struct player* player) {
	assert(player);
	player->bombs += 1;
}

void player_dec_nb_bomb(struct player* player) {
	assert(player);
	player->bombs -= 1;
}

static int player_move_aux(struct player* player, struct map* map, int x, int y) {
	/*
	return 1 	if the player is allowed to move
	return 10	if the player has enter into a next level door
	return 9	if the player has enter into a pervious level door
	*/

	if (!map_is_inside(map, x, y))
		return 0;

	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY:
		return 0;
		break;

	case CELL_DOOR:
		switch (map_get_cell(map, x, y)){
			case CELL_DOOR + DOOR_OPENED_NEXT:
				return 10;
			
			case CELL_DOOR + DOOR_OPENED_PREV:
				return 9;

			default:
				return 0; //return 0
		}

	case CELL_BOX:
		//Est ce qu'on peut bouger la caisse ?
		switch (player->direction) {
				case NORTH:
					if (map_is_inside(map, x, y - 1) && map_get_cell_type(map, x, y - 1) == CELL_EMPTY) {
						map_set_cell_type(map, x, y - 1, CELL_BOX);
						map_set_cell_type(map, x, y, CELL_EMPTY);
						return 1;
					}
					break;

				case SOUTH:
				if (map_is_inside(map, x, y + 1) && map_get_cell_type(map, x, y + 1) == CELL_EMPTY) {
					map_set_cell_type(map, x, y + 1, CELL_BOX);
					map_set_cell_type(map, x, y, CELL_EMPTY);
					return 1;
				}
					break;

				case WEST:
				if (map_is_inside(map, x - 1, y) && map_get_cell_type(map, x - 1, y) == CELL_EMPTY) {
					map_set_cell_type(map, x - 1, y, CELL_BOX);
					map_set_cell_type(map, x, y, CELL_EMPTY);
					return 1;
				}
					break;

				case EAST:
				if (map_is_inside(map, x + 1, y) && map_get_cell_type(map, x + 1,y) == CELL_EMPTY) {
					map_set_cell_type(map, x + 1, y, CELL_BOX);
					map_set_cell_type(map, x, y, CELL_EMPTY);
					return 1;
				}
					break;
				}
		return 0;
		break;

	case CELL_BONUS:
		break;

	case CELL_MONSTER:
		break;

	default:
		break;
	}

	// Player has moved
	return 1;
}

int player_move(struct player* player, struct map* map) {
	int x = player->x;
	int y = player->y;
	int move;
	/*
	move = 1 	if the player is allowed to move
	move = 10	if the player has enter into a next level door
	move = 9	if the player has enter into a pervious level door
	*/

	switch (player->direction) {
	case NORTH:
		move = player_move_aux(player, map, x, y - 1);
		if(move){
			player->y--;
		}
		break;

	case SOUTH:
		move = player_move_aux(player, map, x, y + 1);
		if (move) {
			player->y++;
		}
		break;

	case WEST:
		move = player_move_aux(player, map, x - 1, y);
		if (move) {
			player->x--;
		}
		break;

	case EAST:
		move = player_move_aux(player, map, x + 1, y);
		if (move) {
			player->x++;
		}
		break;
	}

	if (move) {
		//bon bah on bouge la
		//map_set_cell_type(map, x, y, CELL_EMPTY);
	}
	return move;
}

void player_display(struct player* player) {
	assert(player);
	window_display_image(sprite_get_player(player->direction),
			player->x * SIZE_BLOC, player->y * SIZE_BLOC);
}
