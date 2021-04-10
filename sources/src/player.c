/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>

#include <game.h>
#include <player.h>
#include <sprite.h>
#include <window.h>
#include <menu.h>
#include <misc.h>
#include <constant.h>

struct player {
	int x, y;
	enum direction direction;
	int bombs;
	int life;
	int range;
	int blink;
	int keys;
};

struct player* player_init(int bombs) {
	struct player* player = malloc(sizeof(*player));
	if (!player)
		error("Memory error");

	player->direction = NORTH;
	player->bombs = bombs;
	player->life = 8;
	player->keys = 1; //
	player->range = 3;
	player->blink = 0;

	return player;
}

//Sauvegarde / Chargement partie
void player_save(struct player *player, FILE *f){
	fwrite(&player->x, sizeof(player->x), 1, f);
    fwrite(&player->y, sizeof(player->y), 1, f);
    fwrite(&player->direction, sizeof(player->direction), 1, f);
    fwrite(&player->bombs, sizeof(player->bombs), 1, f);
	fwrite(&player->life, sizeof(player->life), 1, f);
	fwrite(&player->range, sizeof(player->range), 1, f);
}

struct player *player_load(FILE *f){
	struct player *player = malloc(sizeof(struct player));
	fread(&player->x, sizeof(player->x), 1, f);
	fread(&player->y, sizeof(player->y), 1, f);
	fread(&player->direction, sizeof(player->direction), 1, f);
    fread(&player->bombs, sizeof(player->bombs), 1, f);
	fread(&player->life, sizeof(player->life), 1, f);
	fread(&player->range, sizeof(player->range), 1, f);

	return player;
}

int i = 0;
void player_hurt(struct game* game){
	struct map* map = game_get_current_map(game);
	struct player* player = game_get_player(game);
	char cell = map_get_cell_type(map, player_get_x(player), player_get_y(player));

	if(cell == CELL_MONSTER || cell == CELL_EXPLOSION){
		player_dec_life(player);
	}
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

int player_get_range(struct player* player) {
	assert(player);
	return player->range;
}

int player_get_life(struct player* player) {
	assert(player);
	return player->life;
}

void player_dec_life(struct player* player) {
	assert(player);
	if(player->blink==0){
		if(player->life>1){
			player->life -= 1;
		} else {
		menu_show_msg_with_default_opts("GAME OVER");
		}
		player->blink = 45;
	}
}

void player_inc_life(struct player* player) {
	assert(player);
	if(player->life<9){
	player->life += 1;}
}

int player_get_keys(struct player* player) {
	assert(player);
	return player->keys;
}

void player_inc_keys(struct player* player) {
	assert(player);
	player->keys++;
}

void player_dec_keys(struct player* player) {
	assert(player);
	player->keys--;
}

void player_dec_range(struct player* player) {
	assert(player);
	if(player->range>0){
	player->range -= 1;}
}

void player_inc_range(struct player* player) {
	assert(player);
	if(player->range<9){
	player->range += 1;}
}

void player_inc_nb_bomb(struct player* player) {
	assert(player);
	if(player->bombs<9){
	player->bombs += 1;}
}

void player_dec_nb_bomb(struct player* player) {
	assert(player);
	if(player->bombs>0){
	player->bombs -= 1;}
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
		if(map_get_cell(map, x, y) == CELL_PRINCESS){
			menu_show_msg_with_default_opts("GAGNee!");
		}
		return 0;
		break;

	case CELL_DOOR:
		if(map_is_door_closed(map, x, y))
			return 0;
		
		if(map_is_next_level_door(map, x, y))
			return 10;
		
		return 9;

	case CELL_BOX:
		//Est ce qu'on peut bouger la caisse ?
		switch (player->direction) {
				case NORTH:
					if (map_is_inside(map, x, y - 1) && map_get_cell_type(map, x, y - 1) == CELL_EMPTY) {
						map_set_cell_type(map, x, y - 1, map_get_cell(map, x, y));
						map_set_cell_type(map, x, y, CELL_EMPTY);
						return 1;
					}
					break;

				case SOUTH:
				if (map_is_inside(map, x, y + 1) && map_get_cell_type(map, x, y + 1) == CELL_EMPTY) {
					map_set_cell_type(map, x, y + 1, map_get_cell(map, x, y));
					map_set_cell_type(map, x, y, CELL_EMPTY);
					return 1;
				}
					break;

				case WEST:
				if (map_is_inside(map, x - 1, y) && map_get_cell_type(map, x - 1, y) == CELL_EMPTY) {
					map_set_cell_type(map, x - 1, y, map_get_cell(map, x, y));
					map_set_cell_type(map, x, y, CELL_EMPTY);
					return 1;
				}
					break;

				case EAST:
				if (map_is_inside(map, x + 1, y) && map_get_cell_type(map, x + 1,y) == CELL_EMPTY) {
					map_set_cell_type(map, x + 1, y, map_get_cell(map, x, y));
					map_set_cell_type(map, x, y, CELL_EMPTY);
					return 1;
				}
					break;
				}
		return 0;
		break;

	case CELL_BONUS:
		switch (map_get_cell(map, x, y) - CELL_BONUS) {
			case 1:
				player_dec_range(player);
				break;
			case 2:
				player_inc_range(player);
				break;
			case 3:
				player_dec_nb_bomb(player);
				break;
			case 4:
				player_inc_nb_bomb(player);
				break;
			case 5:
				break;
			case 6:
				player_inc_life(player);
				break;
		}
		map_set_cell_type(map,x,y,CELL_EMPTY);
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

	player_open_door(player, map);

	return move;
}

int blink_tick = 40;
void player_display(struct player* player) {
	assert(player);


	if(player->blink>0){player->blink--;}


	if(player->blink%10 < 5){
		window_display_image(sprite_get_player(player->direction),
			player->x * SIZE_BLOC, player->y * SIZE_BLOC);
		}
}

void player_open_door(struct player *player, struct map *map){
	if(player->keys <= 0)
		return;

	int x=player->x, y=player->y;

	for(int dx = -1; dx <=1; dx++){
		for(int dy = -1; dy <=1; dy++){
			
			if(dx==0 && dy==0)
				continue;
			
			if(!map_is_inside(map, x+dx, y+dy))
				continue;
			
			if(map_get_cell_type(map, x+dx, y+dy) == CELL_DOOR && map_is_door_closed(map, x+dx, y+dy)){
				player->keys--;
				map_open_door(map, x+dx, y+dy);
			}
		}
	}
}