#include <map.h>
#include <player.h>
#include <window.h>
#include <game.h>
#include <constant.h>
#include <stdio.h>
#include <stdlib.h>


//La map ou chaque case represente
//le bonus présent sur la case
int bonusMap[12][12];

//C'est pas très propre comme maniere de coder
//ça augmente la complexité en mémoire
//mais ça baisse la complexité en temps
//et l'accès est + pratique que faire une boucle
//jusqu'a trouver le match des coords x y
//voila


//Les lifetime en tick
int explosion_time = 15;
int bomb_time = 10;


int bomb_get_sprite(struct map* map, int x, int y){
	x = x/40;
	y = y/40;

	int time = map_get_cell(map, x, y) & 0x0f;
	return 4*time/(bomb_time+1);
}


void bomb_place(struct player* player, struct map* map){
  //La on pose une bombe

  //Verifier que c pas deja une bombe
	if(map_get_cell_type(map, player_get_x(player),player_get_y(player)) == CELL_EMPTY && player_get_nb_bomb(player) > 0){

		player_dec_nb_bomb(player);
		map_set_cell_type(map, player_get_x(player),player_get_y(player), CELL_BOMB + bomb_time);
	}

}

//Explosion d'une case
int explose_cell(struct map* map, int x, int y, int face){
	if(map_is_inside(map, x, y)){

		if(map_get_cell_type(map,x, y) == CELL_MONSTER){
			map_kill_the_monster_at(map, x, y);
		}


		if(map_get_cell_type(map,x, y) != CELL_EMPTY){
			face = 0;
		}

		if(map_get_cell_type(map,x, y) == CELL_BOX){
			//dropper caisse dans joueur


			bonusMap[x][y] = rand()%6 + 1; //avec random

			//bonusMap[x][y] = map_get_cell(map,x,y)-CELL_BOX; //sans random


			map_set_cell(map,x, y, CELL_EXPLOSION + explosion_time);
		}

	//si caisse alors face = 0
	//donc si vide:
	if(face){
		bonusMap[x][y] = 0;
		map_set_cell(map,x, y, CELL_EXPLOSION + explosion_time);
	}
	}
	return face;
}

//gere l'explosion d'une bombe
void bomb_explode(int x, int y, struct game* game){
	int r = player_get_range(game_get_player(game));
	struct map* map = game_get_current_map(game);

	map_set_cell(map, x, y, CELL_EXPLOSION + explosion_time);
	int north = 1;
	int south = 1;
	int east = 1;
	int west = 1;

	for (int i = 1; i < r; i++) {
		north = explose_cell(map, x, y-i, north);
		south = explose_cell(map, x, y+i, south);
		east = explose_cell(map, x+i, y, east);
		west = explose_cell(map, x-i, y, west);
	}

}

//Fonction degueu mais ca fait le taff
int explosion_tick(int x, int y, struct map* map){
	x = x/SIZE_BLOC;
	y = y/SIZE_BLOC;
	int time = map_get_cell(map, x, y) & 0x0f;
	time--;
	map_set_cell(map, x, y, CELL_EXPLOSION + time);
	return time;
}

int box_bonus(int x, int y){
	return bonusMap[x][y];
}


//tick tte les bombes
void bomb_tick(struct game* game){
	struct map* map = game_get_current_map(game);
	for (int x = 0; x < 12; x++) {
		for (int y = 0; y < 12; y++) {
			if(map_get_cell_type(map,x,y) == CELL_BOMB){
				int time = map_get_cell(map, x, y) & 0x0f;

				if(time>0 && SDL_GetTicks()%5 == 0){time--;}
				map_set_cell(map,x,y,CELL_BOMB + time);
				if(time == 0){
					time--;
					bomb_explode(x,y,game);
				}

			}
		}
	}
}
