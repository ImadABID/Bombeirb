/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef PLAYER_H_
#define PLAYER_H_

#include <stdio.h>

#include <map.h>
#include <constant.h>

struct player;
struct game;

// Creates a new player with a given number of available bombs
struct player* player_init(int bomb_number);
void   player_free(struct player* player);

//Sauvegarde / Chargement partie
void player_save(struct player *player, FILE *f);
struct player *player_load(FILE *f);

// Set the position of the player
void player_set_position(struct player *player, int x, int y);

// Returns the current position of the player
int player_get_x(struct player* player);
int player_get_y(struct player* player);

int player_get_life(struct player* player);
void player_inc_life(struct player * player);
void player_dec_life(struct player * player);

int player_get_keys(struct player* player);
void player_dec_keys(struct player* player);
void player_inc_keys(struct player* player);

int player_get_range(struct player* player);

void player_inc_range(struct player * player);
void player_dec_range(struct player * player);

// Set the direction of the next move of the player
void player_set_current_way(struct player * player, enum direction direction);

void player_hurt(struct game* game);

// Set, Increase, Decrease the number of bomb that player can put
int  player_get_nb_bomb(struct player * player);
void player_inc_nb_bomb(struct player * player);
void player_dec_nb_bomb(struct player * player);

char player_won(struct player *player);

// Move the player according to the current direction
int player_move(struct player* player, struct map* map);

// Display the player on the screen
void player_display(struct player* player);

void player_open_door(struct player *player, struct map *map);

#endif /* PLAYER_H_ */
